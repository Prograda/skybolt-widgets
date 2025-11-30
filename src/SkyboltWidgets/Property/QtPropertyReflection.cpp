// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#ifdef BUILD_WITH_SKYBOLT_REFLECT

#include "QtPropertyReflection.h"
#include "QtPropertyReflectionConversion.h"
#include "PropertyEditor.h"

#include <SkyboltReflect/Reflection.h>
#include <QVector3D>

namespace skybolt {

std::optional<QtPropertyUpdaterApplier> reflPropertyToQt(refl::TypeRegistry& typeRegistry, const ReflInstanceGetter& instanceGetter, const refl::PropertyPtr& property, const ReflTypePropertyFactoryMap& typePropertyFactories)
{
	auto type = property->getType();
	if (auto accessor = type->getContainerValueAccessor(); accessor)
	{
		type = typeRegistry.getTypeByName(accessor->valueTypeName);
		if (!type)
		{
			return std::nullopt;
		}
	}

	if (const auto& i = typePropertyFactories.find(type); i != typePropertyFactories.end())
	{
		return i->second(typeRegistry, instanceGetter, property); 
	}
	return std::nullopt;
}

void addReflPropertiesToModel(refl::TypeRegistry& typeRegistry, PropertiesModel& model, const std::vector<refl::PropertyPtr>& properties, const ReflInstanceGetter& instanceGetter, const ReflTypePropertyFactoryMap& typePropertyFactories)
{
	for (const refl::PropertyPtr& property : properties)
	{
		std::optional<QtPropertyUpdaterApplier> qtProperty = reflPropertyToQt(typeRegistry, instanceGetter, property, typePropertyFactories);
		if (qtProperty)
		{
			model.addProperty(qtProperty->property, qtProperty->updater, qtProperty->applier, property->getCategory());
		}
	}
}

QWidget* createReflPropertyInstanceEditor(QtProperty* property, QWidget* parent, refl::TypeRegistry* typeRegistry, const ReflTypePropertyFactoryMap& typePropertyFactories, const PropertyEditorWidgetFactoryMapPtr& factoryMap)
{
	auto instanceGetter = [property] () -> std::optional<refl::Instance> {
		auto instanceProperty = property->value().value<ReflPropertyInstanceVariant>();
		return instanceProperty.instance;
	};

	std::optional<refl::Instance> instance = instanceGetter();
	if (!instance) { return nullptr; }

	refl::TypePtr type = instance->getType();

	auto model = std::make_shared<PropertiesModel>();
	addReflPropertiesToModel(*typeRegistry, *model, toValuesVector(type->getProperties()), instanceGetter, typePropertyFactories);

	auto editor = new PropertyEditor(factoryMap, parent);
	editor->setModel(model);
	return editor;
}

void addReflEditorsToFactoryMap(PropertyEditorWidgetFactoryMap& m, refl::TypeRegistry* typeRegistry, const ReflTypePropertyFactoryMapPtr& typePropertyFactories, const PropertyEditorWidgetFactoryMapPtr& factoryMap)
{
	assert(typeRegistry);

	PropertyEditorWidgetFactoryMap result = m;
	result[qMetaTypeId<ReflPropertyInstanceVariant>()] = [typeRegistry, typePropertyFactories, factoryMap] (QtProperty* property, QWidget* parent) {
		return createReflPropertyInstanceEditor(property, parent, typeRegistry, *typePropertyFactories, factoryMap);
	};
}

ReflTypePropertyFactoryMap createDefaultReflTypePropertyFactories(refl::TypeRegistry& typeRegistry)
{
	std::map<refl::TypePtr, PropertyFactory> typePropertyFactories = {
		{ typeRegistry.getOrCreateType<std::string>(), createPropertyFactory<std::string>(QString()) },
		{ typeRegistry.getOrCreateType<bool>(), createPropertyFactory<bool>(false) },
		{ typeRegistry.getOrCreateType<int>(), createPropertyFactory<int>(0) },
		{ typeRegistry.getOrCreateType<float>(), createPropertyFactory<float>(0.f) },
		{ typeRegistry.getOrCreateType<double>(), createPropertyFactory<double>(0.0) }
	};
	return typePropertyFactories;
}

} // namespace skybolt

#endif // BUILD_WITH_SKYBOLT_REFLECT