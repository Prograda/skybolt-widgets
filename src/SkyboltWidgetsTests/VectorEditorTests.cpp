// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <catch2/catch_test_macros.hpp>
#include <SkyboltWidgets/Property/ContainerProperties.h>
#include <SkyboltWidgets/Property/EditorWidgets.h>
#include <SkyboltWidgets/Property/QtProperty.h>
#include <SkyboltWidgets/List/ListEditorWidget.h>
#include <SkyboltWidgets/List/ItemEditorWidget.h>
#include <SkyboltWidgets/Property/QtPropertyMetadata.h>

#include <QApplication>
#include <QListWidget>
#include <QSignalSpy>

using namespace skybolt;

namespace {

struct VectorEditorFixture
{
	VectorEditorFixture()
	{
		// Create a PropertyVector with int items
		QtVectorValue vectorValue;
		vectorValue.itemFactory = [] { return createQtValue(QVariant(0)); };
		value = createQtValue(QVariant::fromValue(vectorValue));

		// Create factory map with int editor
		PropertyEditorWidgetFactoryMap factories;
		factories[qMetaTypeId<int>()] = [](QtValue* value, QWidget* parent) -> QWidget* {
			return createIntEditor(value, parent);
		};

		ListEditorIcons icons = createDefaultListEditorIcons();
		widget.reset(createVectorEditor(factories, value.get(), icons, nullptr));
		REQUIRE(widget != nullptr);

		listWidget = widget->findChild<QListWidget*>();
		REQUIRE(listWidget != nullptr);

		listEditorWidget = widget->findChild<ListEditorWidget*>();
		REQUIRE(listEditorWidget != nullptr);
	}

	void addItem(int v)
	{
		auto vectorValue = value->value().value<QtVectorValue>();
		vectorValue.items.push_back(createQtValue(QVariant(v)));
		value->setValue(QVariant::fromValue(vectorValue));
	}

	QtValuePtr value;
	std::unique_ptr<QWidget> widget;
	QListWidget* listWidget = nullptr;
	ListEditorWidget* listEditorWidget = nullptr;
};

} // namespace

TEST_CASE_METHOD(VectorEditorFixture, "PropertyVectorEditor reflects property items to list widget")
{
	CHECK(listWidget->count() == 0);

	addItem(10);
	CHECK(listWidget->count() == 1);
	CHECK(listWidget->item(0)->text() == "1: 10");

	addItem(20);
	CHECK(listWidget->count() == 2);
	CHECK(listWidget->item(1)->text() == "2: 20");
}

TEST_CASE_METHOD(VectorEditorFixture, "PropertyVectorEditor updates list widget when item value changes")
{
	addItem(10);
	CHECK(listWidget->item(0)->text() == "1: 10");

	auto vectorValue = value->value().value<QtVectorValue>();
	vectorValue.items[0]->setValue(42);

	// The item property valueChanged signal triggers updateListWidget
	CHECK(listWidget->item(0)->text() == "1: 42");
}

TEST_CASE_METHOD(VectorEditorFixture, "item added to PropertyVector when ListEditorWidget accepts item creation")
{
	// Emit itemAddRequested to trigger add flow
	emit listEditorWidget->itemAddRequested();

	// At this point the item editor widget should be in create mode
	// Accept the item creation
	auto itemEditorWidget = widget->findChild<ItemEditorWidget*>();
	REQUIRE(itemEditorWidget != nullptr);
	REQUIRE(itemEditorWidget->isCreateItemModeEnabled());

	emit itemEditorWidget->createItemAccepted();

	auto vectorValue = value->value().value<QtVectorValue>();
	CHECK(vectorValue.items.size() == 1);
	CHECK(vectorValue.items[0]->value().toInt() == 0); // default value
}

TEST_CASE_METHOD(VectorEditorFixture, "Item removed from PropertyVector when ListEditorWidget requests item removal")
{
	addItem(10);
	addItem(20);
	CHECK(listWidget->count() == 2);

	listWidget->setCurrentRow(0);
	emit listEditorWidget->itemRemoveRequested();

	auto vectorValue = value->value().value<QtVectorValue>();
	REQUIRE(vectorValue.items.size() == 1);
	CHECK(vectorValue.items[0]->value().toInt() == 20);
}

TEST_CASE_METHOD(VectorEditorFixture, "Item moved up in PropertyVector when ListEditorWidget move up requested")
{
	addItem(10);
	addItem(20);
	addItem(30);

	listWidget->setCurrentRow(2);
	emit listEditorWidget->itemMoveUpRequested();

	auto vectorValue = value->value().value<QtVectorValue>();
	REQUIRE(vectorValue.items.size() == 3);
	CHECK(vectorValue.items[0]->value().toInt() == 10);
	CHECK(vectorValue.items[1]->value().toInt() == 30);
	CHECK(vectorValue.items[2]->value().toInt() == 20);
	CHECK(listWidget->currentRow() == 1);
}

TEST_CASE_METHOD(VectorEditorFixture, "Item moved down in PropertyVector when ListEditorWidget move down requested")
{
	addItem(10);
	addItem(20);
	addItem(30);

	listWidget->setCurrentRow(0);
	emit listEditorWidget->itemMoveDownRequested();

	auto vectorValue = value->value().value<QtVectorValue>();
	REQUIRE(vectorValue.items.size() == 3);
	CHECK(vectorValue.items[0]->value().toInt() == 20);
	CHECK(vectorValue.items[1]->value().toInt() == 10);
	CHECK(vectorValue.items[2]->value().toInt() == 30);
	CHECK(listWidget->currentRow() == 1);
}

TEST_CASE_METHOD(VectorEditorFixture, "ListEditorWidget move up does nothing for first item")
{
	addItem(10);
	addItem(20);

	listWidget->setCurrentRow(0);
	emit listEditorWidget->itemMoveUpRequested();

	auto vectorValue = value->value().value<QtVectorValue>();
	CHECK(vectorValue.items[0]->value().toInt() == 10);
	CHECK(vectorValue.items[1]->value().toInt() == 20);
}

TEST_CASE_METHOD(VectorEditorFixture, "ListEditorWidget move down does nothing for last item")
{
	addItem(10);
	addItem(20);

	listWidget->setCurrentRow(1);
	emit listEditorWidget->itemMoveDownRequested();

	auto vectorValue = value->value().value<QtVectorValue>();
	CHECK(vectorValue.items[0]->value().toInt() == 10);
	CHECK(vectorValue.items[1]->value().toInt() == 20);
}

TEST_CASE_METHOD(VectorEditorFixture, "List items with custom display name appear with that text in list widget")
{
	// Add items with custom display names set via the DisplayName metadata property
	auto vectorValue = value->value().value<QtVectorValue>();

	auto item1 = createQtValue(QVariant(10));
	item1->setProperty(QtPropertyMetadataKeys::displayName, "Custom Name A");
	vectorValue.items.push_back(item1);

	auto item2 = createQtValue(QVariant(20));
	item2->setProperty(QtPropertyMetadataKeys::displayName, "Custom Name B");
	vectorValue.items.push_back(item2);

	value->setValue(QVariant::fromValue(vectorValue));

	REQUIRE(listWidget->count() == 2);
	CHECK(listWidget->item(0)->text() == "1: Custom Name A");
	CHECK(listWidget->item(1)->text() == "2: Custom Name B");
}
