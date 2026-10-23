// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#ifdef BUILD_WITH_SKYBOLT_REFLECT

#include <catch2/catch_test_macros.hpp>
#include <SkyboltReflect/Reflection.h>
#include <SkyboltWidgets/Property/ContainerProperties.h>
#include <SkyboltWidgets/Property/QtPropertyReflection.h>
#include <SkyboltWidgets/Property/QtPropertyReflectionConversion.h>

using namespace skybolt;
namespace refl = skybolt::refl;

struct TestObject
{
	int intProperty;
};

SKYBOLT_REFLECT(TestObject) {
	registry.type<TestObject>("TestObject")
		.property("intProperty", &TestObject::intProperty);
}

static bool updateValueFromModel(refl::TypeRegistry& typeRegistry, const ReflValueTranslatorMap& valueTranslators, const refl::Instance& instance, const refl::TypePtr& type, QtValue& value)
{
	ReflQtValueUpdaterContext context{
		.typeRegistry = typeRegistry,
		.instance = instance
	};
	return reflValueToQt(type, valueTranslators, context, value);
}

static bool applyValueToModel(refl::TypeRegistry& typeRegistry, const ReflValueTranslatorMap& valueTranslators, refl::Instance& instance, const ReflInstanceSetter& instanceSetter, const refl::TypePtr& type, const QtValue& value)
{
	ReflQtValueApplierContext context{
		.typeRegistry = typeRegistry,
		.instance = instance,
		.instanceSetter = instanceSetter
	};
	return reflValueFromQt(type, valueTranslators, context, value);
}

TEST_CASE("Reflect basic property to Qt")
{
	refl::TypeRegistry typeRegistry;
	ReflValueTranslatorMap valueTranslators = createDefaultReflValueTranslators(typeRegistry);

	// Create model property
	TestObject object;
	auto objectInstance = refl::makeRefInstance(typeRegistry, &object);
	refl::PropertyPtr property = typeRegistry.getTypeRequired<TestObject>()->getProperty("intProperty");

	auto childInstance = property->getValue(objectInstance);
	ReflInstanceSetter instanceSetter = [&](const refl::Instance& newValue) { property->setValue(objectInstance, newValue); };

	// Create reflected UI property
	auto value = createQtValue(QVariant{});
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	// Listen to property value change events
	int valueChangeCount = 0;
	QObject::connect(value.get(), &QtValue::valueChanged, [&]() { valueChangeCount++; });

	// Set model property and check that its value is reflected to Qt
	object.intProperty = 123;
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	CHECK(value->value() == 123);
	CHECK(valueChangeCount == 1);

	// Set Qt property and check that its value is reflected to model
	value->setValue(456);
	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));

	CHECK(object.intProperty == 456);
	CHECK(valueChangeCount == 2);
}

struct TestObjectContainingOptional
{
	std::optional<double> value;

	void propertyValueChanged(const std::string& name) { valueChangeCount++; }
	int valueChangeCount = 0;
};

SKYBOLT_REFLECT(TestObjectContainingOptional) {
	registry.type<TestObjectContainingOptional>("TestObjectContainingOptional")
		.property("value", &TestObjectContainingOptional::value, {}, &TestObjectContainingOptional::propertyValueChanged);
}

TEST_CASE("Reflect optional property to Qt")
{
	refl::TypeRegistry typeRegistry;
	ReflValueTranslatorMap valueTranslators = createDefaultReflValueTranslators(typeRegistry);

	// Create model property
	TestObjectContainingOptional object;
	auto objectInstance = refl::makeRefInstance(typeRegistry, &object);
	refl::PropertyPtr property = typeRegistry.getTypeRequired<TestObjectContainingOptional>()->getProperty("value");
	REQUIRE(property);

	auto childInstance = property->getValue(objectInstance);
	ReflInstanceSetter instanceSetter = [&](const refl::Instance& newValue) { property->setValue(objectInstance, newValue); };

	// Create reflected UI property
	auto value = createQtValue(QVariant{});
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));
	CHECK(value->value().userType() == qMetaTypeId<QtOptionalValue>());

	// Listen to property value change events
	int qtValueChangeCount = 0;
	QObject::connect(value.get(), &QtValue::valueChanged, [&]() { qtValueChangeCount++; });

	// Check that property value is initially absent
	auto optionalValue = value->value().value<QtOptionalValue>();
	REQUIRE(optionalValue.value);
	CHECK(!optionalValue.present);
	CHECK(qtValueChangeCount == 0);

	// Check that the default value is of the correct type.
	// Even though the optional is initially absent, the default value is still created and should be of the correct type so that the UI can show the correct editor for the optional value type.
	CHECK(optionalValue.value->value().userType() == qMetaTypeId<double>());

	// Set model property and check that its value is reflected to Qt
	object.value = 123.0;
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	optionalValue = value->value().value<QtOptionalValue>();
	CHECK(optionalValue.value->value() == 123.0);
	CHECK(optionalValue.present);
	CHECK(qtValueChangeCount == 1);

	// Set model property to absent and check that absent state is reflected to Qt,
	// but the Qt child property value is unchanged so that the last entry is preserved
	// for next time the optional switched to present.
	object.value = std::nullopt;
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	optionalValue = value->value().value<QtOptionalValue>();
	CHECK(optionalValue.value->value() == 123.0);
	CHECK(!optionalValue.present);
	CHECK(qtValueChangeCount == 2);

	// Set Qt property value and check that its value is reflected to model
	optionalValue.value->setValue(456.0);
	CHECK(qtValueChangeCount == 3);

	optionalValue.present = true;
	value->setValue(QVariant::fromValue(optionalValue));
	CHECK(qtValueChangeCount == 4);

	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));
	CHECK(object.value == 456.0);
	CHECK(object.valueChangeCount == 1);

	// Set Qt property to be absent and check that it's reflected in model
	optionalValue.present = false;
	value->setValue(QVariant::fromValue(optionalValue));
	CHECK(qtValueChangeCount == 5);

	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));
	CHECK(!object.value.has_value());
	CHECK(object.valueChangeCount == 2);
}

struct TestObjectContainingVector
{
	std::vector<int> values;

	void propertyValueChanged(const std::string& name) { valueChangeCount++; }
	int valueChangeCount = 0;
};

SKYBOLT_REFLECT(TestObjectContainingVector) {
	registry.type<TestObjectContainingVector>("TestObjectContainingVector")
		.property("values", &TestObjectContainingVector::values, {}, &TestObjectContainingVector::propertyValueChanged);
}

TEST_CASE("Reflect vector-of-POD-type property to Qt")
{
	refl::TypeRegistry typeRegistry;
	ReflValueTranslatorMap valueTranslators = createDefaultReflValueTranslators(typeRegistry);

	// Create model property
	TestObjectContainingVector object;
	auto objectInstance = refl::makeRefInstance(typeRegistry, &object);
	refl::PropertyPtr property = typeRegistry.getTypeRequired<TestObjectContainingVector>()->getProperty("values");
	REQUIRE(property);

	auto childInstance = property->getValue(objectInstance);
	ReflInstanceSetter instanceSetter = [&](const refl::Instance& newValue) { property->setValue(objectInstance, newValue); };

	// Create reflected UI property
	auto value = createQtValue(QVariant{});
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));
	CHECK(value->value().userType() == qMetaTypeId<QtVectorValue>());

	// Listen to property value change events
	int qtValueChangeCount = 0;
	QObject::connect(value.get(), &QtValue::valueChanged, [&]() { qtValueChangeCount++; });

	// Assert that vector is initially empty
	auto vectorValue = value->value().value<QtVectorValue>();
	CHECK(vectorValue.items.empty());
	CHECK(qtValueChangeCount == 0);

	// Assert vector has correct default item value
	CHECK(vectorValue.itemFactory()->value().userType() == qMetaTypeId<int>());

	// Set model property and check that its value is reflected to Qt
	object.values = {1, 2};
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	vectorValue = value->value().value<QtVectorValue>();
	REQUIRE(vectorValue.items.size() == 2);
	CHECK(vectorValue.items.front()->value().toInt() == 1);
	CHECK(vectorValue.items.back()->value().toInt() == 2);
	CHECK(qtValueChangeCount == 1);

	// Modify vector property and check that its value is reflected to model
	vectorValue.items.resize(1);
	value->setValue(QVariant::fromValue(vectorValue));
	CHECK(qtValueChangeCount == 2);

	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));
	CHECK(object.values == std::vector<int>{1});
	CHECK(object.valueChangeCount == 1);

	// Modify vector item property and check that its value is reflected to model
	vectorValue.items.front()->setValue(2);
	CHECK(qtValueChangeCount == 3);

	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));
	CHECK(object.values == std::vector<int>{2});
	CHECK(object.valueChangeCount == 2);
}

struct TestObjectWithChild
{
	TestObject child;

	void propertyValueChanged(const std::string& name) { valueChangeCount++; }
	int valueChangeCount = 0;
};

SKYBOLT_REFLECT(TestObjectWithChild) {
	registry.type<TestObjectWithChild>("TestObjectWithChild")
		.property("child", &TestObjectWithChild::child, {}, &TestObjectWithChild::propertyValueChanged);
}

TEST_CASE("Reflect struct property to Qt")
{
	refl::TypeRegistry typeRegistry;
	ReflValueTranslatorMap valueTranslators = createDefaultReflValueTranslators(typeRegistry);

	// Create model property
	TestObjectWithChild object;
	auto objectInstance = refl::makeRefInstance(typeRegistry, &object);
	refl::PropertyPtr property = typeRegistry.getTypeRequired<TestObjectWithChild>()->getProperty("child");
	REQUIRE(property);

	auto childInstance = property->getValue(objectInstance);
	ReflInstanceSetter instanceSetter = [&](const refl::Instance& newValue) { property->setValue(objectInstance, newValue); };

	// Create reflected UI property
	auto value = createQtValue(QVariant{});
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));
	REQUIRE(value->value().userType() == qMetaTypeId<QtStructValue>());

	// Listen to property value change events
	int qtValueChangeCount = 0;
	QObject::connect(value.get(), &QtValue::valueChanged, [&]() { qtValueChangeCount++; });

	// Assert that struct has all the expected child properties
	auto structValue = value->value().value<QtStructValue>();
	CHECK(structValue.items.size() == 1);
	CHECK(qtValueChangeCount == 0);

	// Get the item property value for later use.
	// We expect the item object to persist across changes to the struct value so that references to the item are stable
	// (i.e we want the reflection to modify the existing object rather than creating a new object).
	QtValuePtr itemValue = structValue.items.front()->value();

	// Set model property and check that its value is reflected to UI
	object.child.intProperty = 1;
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	structValue = value->value().value<QtStructValue>();
	REQUIRE(structValue.items.size() == 1);
	CHECK(itemValue->value().toInt() == 1);
	CHECK(qtValueChangeCount == 1);

	// Modify UI property and check that its value is reflected to model
	itemValue->setValue(2);
	CHECK(qtValueChangeCount == 2);

	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));
	CHECK(object.child.intProperty == 2);
	CHECK(object.valueChangeCount == 1);
}

struct TestObjectContainingVectorOfStruct
{
	std::vector<TestObject> values;
};

SKYBOLT_REFLECT(TestObjectContainingVectorOfStruct) {
	registry.type<TestObjectContainingVectorOfStruct>("TestObjectContainingVectorOfStruct")
		.property("values", &TestObjectContainingVectorOfStruct::values);
}

TEST_CASE("Reflect vector-of-structs property to Qt")
{
	refl::TypeRegistry typeRegistry;
	ReflValueTranslatorMap valueTranslators = createDefaultReflValueTranslators(typeRegistry);

	// Create model property
	TestObjectContainingVectorOfStruct object;
	auto objectInstance = refl::makeRefInstance(typeRegistry, &object);
	refl::PropertyPtr property = typeRegistry.getTypeRequired<TestObjectContainingVectorOfStruct>()->getProperty("values");
	REQUIRE(property);

	auto childInstance = property->getValue(objectInstance);
	ReflInstanceSetter instanceSetter = [&](const refl::Instance& newValue) { property->setValue(objectInstance, newValue); };

	// Create reflected UI property
	auto value = createQtValue(QVariant{});
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));
	REQUIRE(value->value().userType() == qMetaTypeId<QtVectorValue>());

	// Listen to property value change events
	int valueChangeCount = 0;
	QObject::connect(value.get(), &QtValue::valueChanged, [&]() { valueChangeCount++; });

	// Assert that vector is initially empty
	auto vectorValue = value->value().value<QtVectorValue>();
	CHECK(vectorValue.items.empty());
	CHECK(valueChangeCount == 0);

	// Set model property and check that its value is reflected to Qt
	object.values = { {1}, {2} };
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	vectorValue = value->value().value<QtVectorValue>();
	REQUIRE(vectorValue.items.size() == 2);

	// Each item should be a QtStructValue containing the intProperty
	auto struct0 = vectorValue.items[0]->value().value<QtStructValue>();
	auto struct1 = vectorValue.items[1]->value().value<QtStructValue>();
	REQUIRE(struct0.items.size() == 1);
	REQUIRE(struct1.items.size() == 1);
	CHECK(struct0.items.front()->value()->value().toInt() == 1);
	CHECK(struct1.items.front()->value()->value().toInt() == 2);
	CHECK(valueChangeCount == 1);

	// Modify vector property (resize down) and check that its value is reflected to model
	vectorValue.items.resize(1);
	value->setValue(QVariant::fromValue(vectorValue));
	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));

	REQUIRE(object.values.size() == 1);
	CHECK(object.values[0].intProperty == 1);
	CHECK(valueChangeCount == 2);

	// Modify vector item child property and check that its value is reflected to model
	struct0.items.front()->value()->setValue(3);
	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));

	CHECK(object.values[0].intProperty == 3);
	CHECK(valueChangeCount == 3);
}

struct TestObjectContainingOptionalStruct
{
	std::optional<TestObject> value;

	void propertyValueChanged(const std::string& name) { valueChangeCount++; }
	int valueChangeCount = 0;
};

SKYBOLT_REFLECT(TestObjectContainingOptionalStruct) {
	registry.type<TestObjectContainingOptionalStruct>("TestObjectContainingOptionalStruct")
		.property("value", &TestObjectContainingOptionalStruct::value, {}, &TestObjectContainingOptionalStruct::propertyValueChanged);
}

TEST_CASE("Reflect optional struct property to Qt")
{
	refl::TypeRegistry typeRegistry;
	ReflValueTranslatorMap valueTranslators = createDefaultReflValueTranslators(typeRegistry);

	// Create model property
	TestObjectContainingOptionalStruct object;
	auto objectInstance = refl::makeRefInstance(typeRegistry, &object);
	refl::PropertyPtr property = typeRegistry.getTypeRequired<TestObjectContainingOptionalStruct>()->getProperty("value");
	REQUIRE(property);

	auto childInstance = property->getValue(objectInstance);
	ReflInstanceSetter instanceSetter = [&](const refl::Instance& newValue) { property->setValue(objectInstance, newValue); };

	// Create reflected UI property
	auto value = createQtValue(QVariant{});
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));
	CHECK(value->value().userType() == qMetaTypeId<QtOptionalValue>());

	// Listen to property value change events
	int qtValueChangeCount = 0;
	QObject::connect(value.get(), &QtValue::valueChanged, [&]() { qtValueChangeCount++; });

	// Check that property value is initially absent
	auto optionalValue = value->value().value<QtOptionalValue>();
	REQUIRE(optionalValue.value);
	CHECK(!optionalValue.present);
	CHECK(qtValueChangeCount == 0);

	// Check that the default value is of the correct type (QtStructValue), even when the optional is absent
	CHECK(optionalValue.value->value().userType() == qMetaTypeId<QtStructValue>());

	// Check default struct has the expected child property
	auto defaultStruct = optionalValue.value->value().value<QtStructValue>();
	REQUIRE(defaultStruct.items.size() == 1);

	// Set model property to a present struct and check that its value is reflected to Qt
	object.value = TestObject{ 123 };
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	optionalValue = value->value().value<QtOptionalValue>();
	CHECK(optionalValue.present);
	CHECK(qtValueChangeCount == 1);

	// Check the inner struct value is correct
	auto structValue = optionalValue.value->value().value<QtStructValue>();
	REQUIRE(structValue.items.size() == 1);
	CHECK(structValue.items.front()->value()->value().toInt() == 123);

	// Set model property to absent and check that absent state is reflected to Qt,
	// but the Qt child struct value is unchanged so that the last entry is preserved
	// for next time the optional switched to present.
	object.value = std::nullopt;
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	optionalValue = value->value().value<QtOptionalValue>();
	CHECK(!optionalValue.present);
	CHECK(qtValueChangeCount == 2);

	// Inner struct value should still be preserved
	structValue = optionalValue.value->value().value<QtStructValue>();
	REQUIRE(structValue.items.size() == 1);
	CHECK(structValue.items.front()->value()->value().toInt() == 123);

	// Set Qt property value (present with modified struct) and check reflected to model
	optionalValue.value->value().value<QtStructValue>().items.front()->value()->setValue(456);
	CHECK(qtValueChangeCount == 3);

	optionalValue.present = true;
	value->setValue(QVariant::fromValue(optionalValue));
	CHECK(qtValueChangeCount == 4);

	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));
	REQUIRE(object.value.has_value());
	CHECK(object.value->intProperty == 456);
	CHECK(object.valueChangeCount == 1);

	// Set Qt property to absent and check reflected to model
	optionalValue.present = false;
	value->setValue(QVariant::fromValue(optionalValue));
	CHECK(qtValueChangeCount == 5);

	CHECK(applyValueToModel(typeRegistry, valueTranslators, childInstance, instanceSetter, property->getType(), *value));
	CHECK(!object.value.has_value());
	CHECK(object.valueChangeCount == 2);
}

TEST_CASE("DisplayNameRenderer sets display name on QtValue when updater is called")
{
	refl::TypeRegistry typeRegistry;

	// Create a custom DisplayNameRenderer that prefixes the value
	DisplayNameRenderer renderer = [](const refl::Instance& reflValue, const QVariant& qtValue) {
		return QString("Item: %1").arg(qtValue.toInt());
	};

	// Register int type with the custom display name renderer
	ReflValueTranslatorMap valueTranslators = {
		{ typeRegistry.getOrCreateType<int>(), createReflValueTranslatorWithDisplayName<int, int>(renderer) }
	};

	// Create model property
	TestObject object;
	object.intProperty = 42;
	auto objectInstance = refl::makeRefInstance(typeRegistry, &object);
	refl::PropertyPtr property = typeRegistry.getTypeRequired<TestObject>()->getProperty("intProperty");

	auto childInstance = property->getValue(objectInstance);
	ReflInstanceSetter instanceSetter = [&](const refl::Instance& newValue) { property->setValue(objectInstance, newValue); };

	// Create reflected UI property
	auto value = createQtValue(QVariant{});
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));

	// Check the value is correct
	CHECK(value->value().toInt() == 42);

	// Check that the DisplayName metadata was set by the renderer
	QVariant displayName = value->property(QtPropertyMetadataKeys::displayName);
	REQUIRE(displayName.isValid());
	CHECK(displayName.toString() == "Item: 42");

	// Update the model value and verify display name updates accordingly
	object.intProperty = 99;
	CHECK(updateValueFromModel(typeRegistry, valueTranslators, childInstance, property->getType(), *value));
	CHECK(value->value().toInt() == 99);
	displayName = value->property(QtPropertyMetadataKeys::displayName);
	CHECK(displayName.toString() == "Item: 99");
}

struct TestObjectWithDisplayNameMetadata
{
	int labeledProperty;
};

SKYBOLT_REFLECT(TestObjectWithDisplayNameMetadata) {
	registry.type<TestObjectWithDisplayNameMetadata>("TestObjectWithDisplayNameMetadata")
		.property("labeledProperty", &TestObjectWithDisplayNameMetadata::labeledProperty,
			{ { QtPropertyMetadataKeys::displayName, QString("My Label") } });
}

TEST_CASE("Refl property metadata is copied onto reflected QtValue")
{
	refl::TypeRegistry typeRegistry;
	ReflValueTranslatorMap valueTranslators = createDefaultReflValueTranslators(typeRegistry);

	// Create model property
	TestObjectWithDisplayNameMetadata object;
	object.labeledProperty = 7;
	auto objectInstance = refl::makeRefInstance(typeRegistry, &object);
	refl::PropertyPtr property = typeRegistry.getTypeRequired<TestObjectWithDisplayNameMetadata>()->getProperty("labeledProperty");
	REQUIRE(property);

	// Verify the refl property has the displayName metadata
	auto metadataValue = property->getMetadata(QtPropertyMetadataKeys::displayName);
	REQUIRE(metadataValue.has_value());
	CHECK(std::any_cast<QString>(metadataValue) == "My Label");

	// Create the parent struct instance so that addReflPropertiesToModel copies metadata onto the Qt property
	PropertiesModel model;
	ReflInstanceGetter instanceGetter = [&] { return refl::makeRefInstance(typeRegistry, &object); };
	addReflPropertiesToModel(typeRegistry, model, { property }, instanceGetter, valueTranslators);

	// Retrieve the created Qt property from the model
	const auto& sections = model.getProperties();
	REQUIRE(!sections.empty());
	const auto& properties = sections.begin()->second;
	REQUIRE(properties.size() == 1);
	QtValue* qtValue = properties.front()->value().get();
	REQUIRE(qtValue);

	// Check that the displayName metadata was copied from the refl property onto the QtValue
	QVariant displayName = qtValue->property(QtPropertyMetadataKeys::displayName);
	REQUIRE(displayName.isValid());
	CHECK(displayName.toString() == "My Label");
}

#endif // BUILD_WITH_SKYBOLT_REFLECT