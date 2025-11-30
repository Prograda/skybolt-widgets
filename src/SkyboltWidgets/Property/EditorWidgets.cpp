// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "EditorWidgets.h"
#include "PropertyEditor.h"
#include "QtPropertyMetadata.h"
#include "QtPropertyReflection.h"
#include "QtMetaTypes.h"
#include "Util/QtLayoutUtil.h"
#include "List/ItemEditorWidget.h"
#include "List/ListEditorWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QItemEditorFactory>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVector3D>

namespace skybolt {

DoubleVectorEditor::DoubleVectorEditor(const QStringList& componentLabels, QWidget* parent) :
	QWidget(parent)
{
	QGridLayout* layout = new QGridLayout;
	layout->setContentsMargins(0,0,0,0);
	setLayout(layout);
		
	for (const QString& label : componentLabels)
	{
		int index = (int)mEditors.size();
		mEditors.push_back(addDoubleEditor(*layout, label));
		connect(mEditors.back(), &QLineEdit::editingFinished, this, [this, index]() {
			componentEdited(index, mEditors[index]->text().toDouble());
		});
	}
}

void DoubleVectorEditor::setValue(int index, double value)
{
	mEditors[index]->blockSignals(true);
	mEditors[index]->setText(QString::number(value));
	mEditors[index]->blockSignals(false);
}


QVector3PropertyEditor::QVector3PropertyEditor(QtProperty* property, const QStringList& componentLabels, QWidget* parent) :
	mProperty(property),
	DoubleVectorEditor(componentLabels, parent)
{
	setValue(mProperty->value().value<QVector3D>());

	connect(property, &QtProperty::valueChanged, this, [=]() {
		setValue(mProperty->value().value<QVector3D>());
	});
}

void QVector3PropertyEditor::setValue(const QVector3D& value)
{
	for (int i = 0; i < 3; ++i)
	{
		DoubleVectorEditor::setValue(i, value[i]);
	}
}

void QVector3PropertyEditor::componentEdited(int index, double value)
{
	QVector3D vec = mProperty->value().value<QVector3D>();
	vec[index] = value;
	mProperty->setValue(vec);
}


QLineEdit* createDoubleLineEdit(QWidget* parent)
{
	QLineEdit* editor = new QLineEdit(parent);

	QDoubleValidator* validator = new QDoubleValidator();
	validator->setNotation(QDoubleValidator::StandardNotation);
	editor->setValidator(validator);

	return editor;
}

QLineEdit* addDoubleEditor(QGridLayout& layout, const QString& name)
{
	int row = layout.rowCount();
	layout.addWidget(new QLabel(name), row, 0);

	QLineEdit* editor = createDoubleLineEdit();
	layout.addWidget(editor, row, 1);

	return editor;
}


QWidget* createComboStringEditor(QtProperty* property, QWidget* parent)
{
	QStringList optionNames = property->property(QtPropertyMetadataKeys::optionNames).toStringList();
	auto widget = new QComboBox(parent);
	widget->addItems(optionNames);
	widget->setCurrentText(property->value().toString());
	widget->setEditable(property->property(QtPropertyMetadataKeys::allowCustomOptions).toBool());

	QObject::connect(property, &QtProperty::valueChanged, widget, [widget, property]() {
		widget->blockSignals(true);
		widget->setCurrentText(property->value().toString());
		widget->blockSignals(false);
	});

	QObject::connect(widget, &QComboBox::currentTextChanged, property, [=](const QString& newValue) {
		property->setValue(newValue);
	});

	return widget;
}

QWidget* createSingleLineStringEditor(QtProperty* property, QWidget* parent)
{
	QLineEdit* widget = new QLineEdit(parent);
	widget->setText(property->value().toString());

	QObject::connect(property, &QtProperty::valueChanged, widget, [widget, property]() {
		widget->blockSignals(true);
		widget->setText(property->value().toString());
		widget->blockSignals(false);
	});

	QObject::connect(widget, &QLineEdit::editingFinished, property, [=]() {
		property->setValue(widget->text());
	});

	return widget;
}

QWidget* createMultiLineStringEditor(QtProperty* property, QWidget* parent)
{
	QTextEdit* widget = new QTextEdit(parent);
	widget->setText(property->value().toString());

	QObject::connect(property, &QtProperty::valueChanged, widget, [widget, property]() {
		widget->blockSignals(true);
		widget->setText(property->value().toString());
		widget->blockSignals(false);
	});

	QObject::connect(widget, &QTextEdit::textChanged, property, [=]() {
		property->setValue(widget->toPlainText());
	});

	return widget;
}


QWidget* createStringEditor(QtProperty* property, QWidget* parent)
{
	if (auto value = property->property(QtPropertyMetadataKeys::optionNames); value.isValid())
	{
		return createComboStringEditor(property, parent);
	}

	if (auto value = property->property(QtPropertyMetadataKeys::multiLine); value.isValid() && value.toBool())
	{
		return createMultiLineStringEditor(property, parent);
	}
	else
	{
		return createSingleLineStringEditor(property, parent);
	}
}

QWidget* createIntEditor(QtProperty* property, QWidget* parent)
{
	QSpinBox* widget = new QSpinBox(parent);
	widget->setMaximum(999999);
	widget->setValue(property->value().toInt());

	QObject::connect(property, &QtProperty::valueChanged, widget, [widget, property]() {
		widget->blockSignals(true);
		widget->setValue(property->value().toInt());
		widget->blockSignals(false);
	});

	QObject::connect(widget, QOverload<int>::of(&QSpinBox::valueChanged), property, [=](int newValue) {
		property->setValue(newValue);
	});

	return widget;
}

QWidget* createEnumEditor(QtProperty* property, QWidget* parent)
{
	QStringList optionNames = property->property(QtPropertyMetadataKeys::optionNames).toStringList();
	auto widget = new QComboBox(parent);
	widget->addItems(optionNames);
	widget->setCurrentIndex(property->value().toInt());

	QObject::connect(property, &QtProperty::valueChanged, widget, [widget, property]() {
		widget->blockSignals(true);
		widget->setCurrentIndex(property->value().toInt());
		widget->blockSignals(false);
	});

	QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), property, [=](int newValue) {
		property->setValue(newValue);
	});

	return widget;
}

bool shouldUseEnumEditor(const QtProperty& property)
{
	if (property.property(QtPropertyMetadataKeys::optionNames).isValid())
	{
		if (auto allowCustomOptions = property.property(QtPropertyMetadataKeys::allowCustomOptions); allowCustomOptions.isValid())
		{
			if (allowCustomOptions.toBool())
			{
				// Can't use enum editor if custom options are allowed.
				return false;
			}
		}
		return true;
	}
	return false;
}

QWidget* createIntOrEnumEditor(QtProperty* property, QWidget* parent)
{
	if (shouldUseEnumEditor(*property))
	{
		return createEnumEditor(property, parent);
	}
	else
	{
		return createIntEditor(property, parent);
	}
}

QWidget* createDoubleEditor(QtProperty* property, QWidget* parent)
{
	QLineEdit* widget = createDoubleLineEdit(parent);

	auto widgetTextSetter = [widget](double value) {
		widget->setText(QString::number(value, 'f', 4));
	};
	widgetTextSetter(property->value().toDouble());

	QObject::connect(property, &QtProperty::valueChanged, widget, [widget, property, widgetTextSetter]() {
		widget->blockSignals(true);
		widgetTextSetter(property->value().toDouble());
		widget->blockSignals(false);
	});

	QObject::connect(widget, &QLineEdit::editingFinished, property, [=]() {
		property->setValue(widget->text().toDouble());
	});

	return widget;
}

QWidget* createBoolEditor(QtProperty* property, QWidget* parent)
{
	QAbstractButton* button;
	if (auto attributeType = property->property(QtPropertyMetadataKeys::representation); attributeType.isValid() && attributeType.toString() == QtPropertyRepresentations::toggleButton)
	{
		button = new QPushButton(property->name, parent);
		button->setCheckable(true);
	}
	else
	{
		button = new QCheckBox(parent);
	}
	button->setChecked(property->value().toBool());

	QObject::connect(property, &QtProperty::valueChanged, button, [button, property]() {
		button->blockSignals(true);
		button->setChecked(property->value().toBool());
		button->blockSignals(false);
	});

	QObject::connect(button, &QAbstractButton::toggled, property, [=](int state) {
		property->setValue((bool)state);
	});

	return button;
}

QWidget* createDateTimeEditor(QtProperty* property, QWidget* parent)
{
	QDateTimeEdit* widget = new QDateTimeEdit(parent);
	widget->setDateTime(property->value().toDateTime());

	QObject::connect(property, &QtProperty::valueChanged, widget, [widget, property]() {
		widget->blockSignals(true);
		widget->setDateTime(property->value().toDateTime());
		widget->blockSignals(false);
	});

	QObject::connect(widget, &QDateTimeEdit::dateTimeChanged, property, [=](const QDateTime& dateTime) {
		property->setValue(dateTime);
	});

	return widget;
}

QWidget* createOptionalVariantEditor(const PropertyEditorWidgetFactoryMap& factories, QtProperty* property, QWidget* parent)
{
	auto optionalProperty = property->value().value<OptionalProperty>();
	if (auto i = factories.find(optionalProperty.property->value().userType()); i != factories.end())
	{
		auto widget = new QWidget(parent);
		auto layout = new QVBoxLayout(widget);
		layout->setContentsMargins(0,0,0,0);
		widget->setLayout(layout);

		auto activateCheckbox = new QCheckBox("Enable", widget);
		activateCheckbox->setChecked(optionalProperty.present);
		layout->addWidget(activateCheckbox);

		QWidget* valueEditorWidget = i->second(optionalProperty.property.get(), parent);
		if (valueEditorWidget)
		{
			valueEditorWidget->setEnabled(optionalProperty.present);
			layout->addWidget(valueEditorWidget);
		}

		QObject::connect(property, &QtProperty::valueChanged, activateCheckbox, [activateCheckbox, valueEditorWidget, property]() {
			bool present = property->value().value<OptionalProperty>().present;
			activateCheckbox->blockSignals(true);
			activateCheckbox->setChecked(present);
			activateCheckbox->blockSignals(false);

			if (valueEditorWidget)
			{
				valueEditorWidget->setEnabled(present && property->enabled);
			}
		});

		QObject::connect(activateCheckbox, &QCheckBox::stateChanged, property, [=](bool value) {
			auto optionalProperty = property->value().value<OptionalProperty>();
			optionalProperty.present = value;
			property->setValue(QVariant::fromValue(optionalProperty));

			if (valueEditorWidget)
			{
				valueEditorWidget->setEnabled(value && property->enabled);
			}
		});

		return widget;
	}
	return nullptr;
}

QWidget* createVector3DEditor(QtProperty* property, QWidget* parent)
{
	return new QVector3PropertyEditor(property, { "x", "y", "z" }, parent);
}

QWidget* createPropertyVectorEditor(const PropertyEditorWidgetFactoryMap& factories, QtProperty* property, const ListEditorIcons& listEditorIcons, QWidget* parent)
{
	auto propertyVector = property->value().value<PropertyVector>();
	if (auto i = factories.find(propertyVector.itemDefaultValue.userType()); i != factories.end())
	{
		auto widget = new QWidget(parent);
		auto layout = new QVBoxLayout(widget);
		layout->setContentsMargins(0,0,0,0);

		auto listWidget = new QListWidget(parent);
		layout->addWidget(listWidget);

		auto listEditorWidget = new ListEditorWidget(listEditorIcons, parent);
		layout->addWidget(listEditorWidget);

		auto itemEditorContentWidget = new QWidget(parent);
		auto itemEditorWidget = new ItemEditorWidget(itemEditorContentWidget, parent);
		auto itemEditorLayout = new QVBoxLayout(itemEditorContentWidget);
		itemEditorLayout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(itemEditorWidget);

		auto updateControlButtonsState = [listWidget, listEditorWidget]() {
			int index = listWidget->currentRow();
			listEditorWidget->setMoveUpEnabled(index > 0);
			listEditorWidget->setMoveDownEnabled((index >= 0) && (index + 1 < listWidget->count()));
			listEditorWidget->setRemoveEnabled(index >= 0);
		};

		auto updateListWidgetFunction = [property, listWidget, previousItems = QStringList(), updateControlButtonsState] () mutable {
			QStringList newItems;
			auto propertyVector = property->value().value<PropertyVector>();
			for (const auto& itemProperty : propertyVector.items)
			{
				newItems.push_back(itemProperty->value().toString());
			}

			if (newItems != previousItems)
			{
				listWidget->clear();
				listWidget->addItems(newItems);
				previousItems = newItems;
				updateControlButtonsState();
			}
		};

		updateListWidgetFunction();
		QObject::connect(property, &QtProperty::valueChanged, [listWidget, updateListWidgetFunction = std::move(updateListWidgetFunction)] () mutable {
			updateListWidgetFunction();
			});

		QObject::connect(listWidget, &QListWidget::itemSelectionChanged, listEditorWidget, updateControlButtonsState);

		auto newItemProperty = std::make_shared<QtPropertyPtr>();

		QObject::connect(itemEditorWidget,&ItemEditorWidget::setCreateItemModeEnabledChanged, listEditorWidget, [listEditorWidget](bool enabled) {
			listEditorWidget->setEnabled(!enabled);
			});

		QObject::connect(listEditorWidget, &ListEditorWidget::itemAddRequested, [
			newItemProperty,
			defaultValue = propertyVector.itemDefaultValue,
			factory = i->second,
			parent,
			itemEditorLayout,
			itemEditorWidget
		]() {
			(*newItemProperty) = createQtProperty("newItem", defaultValue);
			QWidget* valueEditorWidget = factory(newItemProperty->get(), parent);
			itemEditorLayout->addWidget(valueEditorWidget);
			itemEditorWidget->setCreateItemModeEnabled(true);
			});

		QObject::connect(itemEditorWidget, &ItemEditorWidget::createItemAccepted, [
			property,
			newItemProperty,
			itemEditorLayout
		]() {
			assert(newItemProperty);
			auto propertyVector = property->value().value<PropertyVector>();
			propertyVector.items.push_back(*newItemProperty);
			property->setValue(QVariant::fromValue(propertyVector));
			newItemProperty->reset();

			clearLayout(*itemEditorLayout);
			});

		QObject::connect(itemEditorWidget, &ItemEditorWidget::createItemCancelled, [
			property,
				newItemProperty,
				itemEditorLayout
		]() {
				assert(newItemProperty);
				newItemProperty->reset();

				clearLayout(*itemEditorLayout);
			});

		QObject::connect(listEditorWidget, &ListEditorWidget::itemRemoveRequested, [
			property,
			listWidget
		]() {
			auto propertyVector = property->value().value<PropertyVector>();
			int row = listWidget->currentRow();
			if (row >= 0 && row < propertyVector.items.size())
			{
				propertyVector.items.erase(propertyVector.items.begin() + row);
				property->setValue(QVariant::fromValue(propertyVector));
			}
		});

		QObject::connect(listEditorWidget, &ListEditorWidget::itemMoveUpRequested, [
			property,
			listWidget
		]() {
			auto propertyVector = property->value().value<PropertyVector>();
			int row = listWidget->currentRow();
			if (row >= 1 && row < propertyVector.items.size())
			{
				std::swap(propertyVector.items[row - 1], propertyVector.items[row]);
				property->setValue(QVariant::fromValue(propertyVector));
			}
		});


		QObject::connect(listEditorWidget, &ListEditorWidget::itemMoveDownRequested, [
			property,
			listWidget
		]() {
			auto propertyVector = property->value().value<PropertyVector>();
			int row = listWidget->currentRow();
			if (row >= 0 && row + 1 < propertyVector.items.size())
			{
				std::swap(propertyVector.items[row], propertyVector.items[row+1]);
				property->setValue(QVariant::fromValue(propertyVector));
			}
		});

		return widget;
	}
	return nullptr;
}

} // namespace skybolt