// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "QtMetaTypes.h"
#include "SkyboltWidgets/Property/QtPropertyMetadata.h"
#include "SkyboltWidgets/Util/QtTypeConversions.h"

#ifdef BUILD_WITH_SKYBOLT_REFLECT

namespace skybolt {

template <typename ReflValueT>
inline QVariant reflValueToQt(const refl::Property& property, const ReflValueT& value)
{
	return value;
}

template <typename ReflValueT>
inline ReflValueT qtValueToRefl(const refl::Property& property, const QVariant& value)
{
	return value.value<ReflValueT>();
}

template <>
inline QVariant reflValueToQt(const refl::Property& property, const std::string& value)
{
	return QString::fromStdString(value);
}

template <>
inline std::string qtValueToRefl(const refl::Property& property, const QVariant& value)
{
	return value.toString().toStdString();
}

template <typename ReflValueT>
QVariant valueInstanceToQt(const refl::Property& property, const refl::Instance& valueInstance)
{
	if constexpr (std::is_same_v<ReflValueT, ReflPropertyInstanceVariant>)
	{
		return QVariant::fromValue(ReflPropertyInstanceVariant{ valueInstance });
	}
	else
	{
		const ReflValueT& value = valueInstance.cast<ReflValueT>();
		return reflValueToQt(property, value);
	}
}

template <typename ReflValueT>
std::optional<refl::Instance> qtToValueInstance(const refl::Property& property, refl::TypeRegistry& typeRegistry, const QVariant value, const QVariant& defaultValue)
{
	if constexpr (std::is_same_v<ReflValueT, ReflPropertyInstanceVariant>)
	{
		std::optional<refl::Instance> optionalValueInstance = value.value<ReflPropertyInstanceVariant>().instance;
		if (!optionalValueInstance)
		{
			optionalValueInstance = defaultValue.value<ReflPropertyInstanceVariant>().instance;
		}
		return optionalValueInstance;
	}
	else
	{
		ReflValueT reflValue = qtValueToRefl<ReflValueT>(property, value);
		return refl::makeValueInstance(typeRegistry, reflValue);
	}
}

template <typename T>
void setQtPropertyMetadata(QtProperty& qtProperty, const refl::Property& reflProperty, const std::string& key)
{
	if (auto value = reflProperty.getMetadata(key); value.has_value())
	{
		T qtValue;
		if constexpr (std::is_same_v<T, QStringList>)
		{
			// Special case for std::vector<std::string> to convert to QStringList
			qtValue = toQStringList(std::any_cast<std::vector<std::string>>(value));
		}
		else
		{
			qtValue = std::any_cast<T>(value);
		}
		qtProperty.setProperty(key.c_str(), qtValue);
	}
}

inline void addMetadata(QtProperty& qtProperty, const refl::Property& reflProperty)
{
	setQtPropertyMetadata<const char*>(qtProperty, reflProperty, QtPropertyMetadataKeys::representation);
	setQtPropertyMetadata<bool>(qtProperty, reflProperty, QtPropertyMetadataKeys::multiLine);
	setQtPropertyMetadata<QStringList>(qtProperty, reflProperty, QtPropertyMetadataKeys::optionNames);
	setQtPropertyMetadata<bool>(qtProperty, reflProperty, QtPropertyMetadataKeys::allowCustomOptions);
}

template <typename ReflValueT, typename QtValueT>
QtPropertyUpdaterApplier createValueProperty(refl::TypeRegistry& typeRegistry, const ReflInstanceGetter& instanceGetter, const refl::PropertyPtr& property, const QtValueT& defaultValue)
{
	QString name = QString::fromStdString(property->getName());
	QVariant defaultValueVariant = QVariant::fromValue(defaultValue);

	QtPropertyUpdaterApplier r;
	r.property = createQtProperty(name, defaultValueVariant);
	r.property->enabled = !property->isReadOnly();
	addMetadata(*r.property, *property);
		
	r.updater = [instanceGetter, property] (QtProperty& qtProperty) {
		if (const auto& objectInstance = instanceGetter(); objectInstance)
		{
			refl::Instance valueInstance = property->getValue(*objectInstance);
			qtProperty.setValue(valueInstanceToQt<ReflValueT>(*property, valueInstance));
		}
	};
	r.updater(*r.property);

	if (!property->isReadOnly())
	{
		r.applier = [typeRegistry = &typeRegistry, instanceGetter, property, defaultValueVariant] (const QtProperty& qtProperty) {
			if (auto objectInstance = instanceGetter(); objectInstance)
			{
				std::optional<refl::Instance> valueInstance = qtToValueInstance<ReflValueT>(*property, *typeRegistry, qtProperty.value(), defaultValueVariant);
				if (valueInstance)
				{
					property->setValue(*objectInstance, *valueInstance);
				}
			}
		};
	}

	return r;
}

inline std::optional<refl::Instance> getOptionalValue(refl::TypeRegistry& registry, const refl::Instance& optionalInstance)
{
	auto accessor = optionalInstance.getType()->getContainerValueAccessor();
	if (!accessor) { return std::nullopt; }

	auto values = accessor->getValues(registry, optionalInstance);
	return values.empty() ? std::optional<refl::Instance>() : values.front();
}

inline void setOptionalValue(refl::Instance& optionalInstance, const std::optional<refl::Instance>& value)
{
	auto accessor = optionalInstance.getType()->getContainerValueAccessor();
	if (!accessor) { return; }

	accessor->setValues(optionalInstance, value ? std::vector<refl::Instance>({*value}) : std::vector<refl::Instance>());
}

template <typename ReflValueT, typename QtValueT>
QtPropertyUpdaterApplier createOptionalValueProperty(refl::TypeRegistry& typeRegistry, const ReflInstanceGetter& instanceGetter, const refl::PropertyPtr& property, const QtValueT& defaultValue)
{
	QString name = QString::fromStdString(property->getName());
	QVariant defaultValueVariant = QVariant::fromValue(defaultValue);
	QtPropertyPtr childProperty = createQtProperty(name, defaultValueVariant);
	addMetadata(*childProperty, *property);

	OptionalProperty optionalProperty;
	optionalProperty.property = childProperty;

	QtPropertyUpdaterApplier r;
	r.property = createQtProperty(name, QVariant::fromValue(optionalProperty));
	r.property->enabled = !property->isReadOnly();

	auto propagateChangeSignalToParent = std::make_shared<bool>(true);
	QObject::connect(childProperty.get(), &QtProperty::valueChanged, r.property.get(), [propagateChangeSignalToParent, parentProperty = r.property.get()] {
		if (*propagateChangeSignalToParent) { parentProperty->valueChanged(); }
		});

	r.updater = [typeRegistry = &typeRegistry, instanceGetter, property, propagateChangeSignalToParent] (QtProperty& qtProperty) {
		if (const auto& objectInstance = instanceGetter(); objectInstance)
		{
			refl::Instance optionalInstance = property->getValue(*objectInstance);
			std::optional<refl::Instance> valueInstance = getOptionalValue(*typeRegistry, optionalInstance);

			OptionalProperty optionalProperty = qtProperty.value().value<OptionalProperty>();

			if (valueInstance)
			{
				// Update the child property witout propagating update signal to parent
				// to avoid unnecessary update, as we will be updating the parent after.
				*propagateChangeSignalToParent = false;
				QVariant qtValue = valueInstanceToQt<ReflValueT>(*property, *valueInstance);
				optionalProperty.property->setValue(qtValue);
				*propagateChangeSignalToParent = true;
			}

			optionalProperty.present = valueInstance.has_value();
			qtProperty.setValue(QVariant::fromValue(optionalProperty));
		}
	};
	r.updater(*r.property);

	if (!property->isReadOnly())
	{
		r.applier = [typeRegistry = &typeRegistry, instanceGetter, property, defaultValue] (const QtProperty& qtProperty) {
			if (auto objectInstance = instanceGetter(); objectInstance)
			{
				std::optional<refl::Instance> valueInstance;

				OptionalProperty optionalProperty = qtProperty.value().value<OptionalProperty>();
				if (optionalProperty.present)
				{
					assert(optionalProperty.property);
					valueInstance = qtToValueInstance<ReflValueT>(*property, *typeRegistry, optionalProperty.property->value(), defaultValue);
				}
					
				refl::Instance optionalInstance = property->getValue(*objectInstance);
				setOptionalValue(optionalInstance, valueInstance);
				property->setValue(*objectInstance, optionalInstance);
			}
		};
	}

	return r;
}

inline std::vector<refl::Instance> getVectorValues(refl::TypeRegistry& registry, const refl::Instance& vectorInstance)
{
	auto accessor = vectorInstance.getType()->getContainerValueAccessor();
	if (!accessor) { return {}; }

	return accessor->getValues(registry, vectorInstance);
}

inline void setVectorValues(refl::Instance& vectorInstance, const std::vector<refl::Instance>& values)
{
	auto accessor = vectorInstance.getType()->getContainerValueAccessor();
	if (!accessor) { return; }

	accessor->setValues(vectorInstance, values);
}

template <typename KeyT, typename ValueT>
std::vector<ValueT> toValuesVector(const std::map<KeyT, ValueT>& m)
{
	std::vector<ValueT> r;
	for (const auto& i : m)
	{
		r.push_back(i.second);
	}
	return r;
}

template <typename ReflValueT, typename QtValueT>
QtPropertyUpdaterApplier createVectorValueProperty(refl::TypeRegistry& typeRegistry, const ReflInstanceGetter& instanceGetter, const refl::PropertyPtr& property, const QtValueT& defaultValue)
{
	QString name = QString::fromStdString(property->getName());
	QVariant defaultValueVariant = QVariant::fromValue(defaultValue);

	PropertyVector propertyVector;
	propertyVector.itemDefaultValue = defaultValueVariant;

	QtPropertyUpdaterApplier r;
	r.property = createQtProperty(name, QVariant::fromValue(propertyVector));
	r.property->enabled = !property->isReadOnly();

	r.updater = [typeRegistry = &typeRegistry, instanceGetter, property](QtProperty& qtProperty) {
		if (const auto& objectInstance = instanceGetter(); objectInstance)
		{
			refl::Instance vectorInstance = property->getValue(*objectInstance);
			std::vector<refl::Instance> valueInstances = getVectorValues(*typeRegistry, vectorInstance);

			PropertyVector propertyVector = qtProperty.value().value<PropertyVector>();
			propertyVector.items.reserve(valueInstances.size());
			propertyVector.items.clear();
			
			int i = 0;
			for (const auto& valueInstance : valueInstances)
			{
				QString name = QString::number(i);
				QVariant qtValue = valueInstanceToQt<ReflValueT>(*property, valueInstance);

				QtPropertyPtr itemProperty = createQtProperty(name, qtValue);
				addMetadata(*itemProperty, *property);
				QObject::connect(itemProperty.get(), &QtProperty::valueChanged, &qtProperty, &QtProperty::valueChanged);
				propertyVector.items.push_back(itemProperty);
				++i;
			}

			qtProperty.setValue(QVariant::fromValue(propertyVector));
		}
	};
	r.updater(*r.property);

	if (!property->isReadOnly())
	{
		r.applier = [typeRegistry = &typeRegistry, instanceGetter, property, defaultValueVariant](const QtProperty& qtProperty) {
			if (auto objectInstance = instanceGetter(); objectInstance)
			{
				PropertyVector propertyVector = qtProperty.value().value<PropertyVector>();

				std::vector<refl::Instance> valueInstances;
				valueInstances.reserve(propertyVector.items.size());

				for (const auto& valueProperty : propertyVector.items)
				{
					assert(valueProperty);
					std::optional<refl::Instance> valueInstance = qtToValueInstance<ReflValueT>(*property, *typeRegistry, valueProperty->value(), defaultValueVariant);
					if (valueInstance)
					{
						valueInstances.push_back(*valueInstance);
					}
				}

				refl::Instance vectorInstance = property->getValue(*objectInstance);
				setVectorValues(vectorInstance, valueInstances);
				property->setValue(*objectInstance, vectorInstance);
			}
		};
	}

	return r;
}

template <typename ReflValueT, typename QtValueT>
PropertyFactory createPropertyFactory(const QtValueT& defaultValue)
{
	return [defaultValue] (refl::TypeRegistry& typeRegistry, const ReflInstanceGetter& instanceGetter, const refl::PropertyPtr& property) {	
		refl::TypePtr type = property->getType();
		auto accessor = type->getContainerValueAccessor();
		
		if (accessor)
		{
			if (auto optionalValueAccessor = dynamic_cast<refl::StdOptionalValueAccessor*>(accessor.get()); optionalValueAccessor)
			{
				 return createOptionalValueProperty<ReflValueT, QtValueT>(typeRegistry, instanceGetter, property, defaultValue);
			}
			else if (auto vectorValueAccessor = dynamic_cast<refl::StdVectorValueAccessor*>(accessor.get()); vectorValueAccessor)
			{
				return createVectorValueProperty<ReflValueT, QtValueT>(typeRegistry, instanceGetter, property, defaultValue);
			}
		}

		return createValueProperty<ReflValueT, QtValueT>(typeRegistry, instanceGetter, property, defaultValue);
	};
}

} // namespace skybolt

#endif // BUILD_WITH_SKYBOLT_REFLECT