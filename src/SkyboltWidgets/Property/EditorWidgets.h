// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "PropertyEditorWidgetFactory.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QStringList>
#include <QVector3D>
#include <QWidget>

namespace skybolt {

struct ListEditorIcons;
class QtProperty;

class DoubleVectorEditor : public QWidget
{
public:
	DoubleVectorEditor(const QStringList& componentLabels, QWidget* parent = nullptr);

protected:
	void setValue(int index, double value);

	virtual void componentEdited(int index, double value) = 0;

private:
	std::vector<QLineEdit*> mEditors;
};

class QVector3PropertyEditor : public DoubleVectorEditor
{
public:
	QVector3PropertyEditor(QtProperty* property, const QStringList& componentLabels, QWidget* parent = nullptr);

protected:
	void setValue(const QVector3D& value);

	void componentEdited(int index, double value) override;

private:
	QtProperty* mProperty;
};

QLineEdit* createDoubleLineEdit(QWidget* parent = nullptr);

QLineEdit* addDoubleEditor(QGridLayout& layout, const QString& name);

QWidget* createComboStringEditor(QtProperty* property, QWidget* parent);

QWidget* createSingleLineStringEditor(QtProperty* property, QWidget* parent);

QWidget* createMultiLineStringEditor(QtProperty* property, QWidget* parent);

QWidget* createStringEditor(QtProperty* property, QWidget* parent);

QWidget* createIntEditor(QtProperty* property, QWidget* parent);

QWidget* createEnumEditor(QtProperty* property, QWidget* parent);

bool shouldUseEnumEditor(const QtProperty& property);

QWidget* createIntOrEnumEditor(QtProperty* property, QWidget* parent);

QWidget* createDoubleEditor(QtProperty* property, QWidget* parent);

QWidget* createBoolEditor(QtProperty* property, QWidget* parent);

QWidget* createDateTimeEditor(QtProperty* property, QWidget* parent);

QWidget* createVector3DEditor(QtProperty* property, QWidget* parent);

QWidget* createOptionalVariantEditor(const PropertyEditorWidgetFactoryMap& factories, QtProperty* property, QWidget* parent);

QWidget* createPropertyVectorEditor(const PropertyEditorWidgetFactoryMap& factories, QtProperty* property, const ListEditorIcons& listEditorIcons, QWidget* parent);

} // namespace skybolt