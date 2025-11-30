// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "DefaultEditorWidgets.h"
#include "SkyboltWidgets/Property/QtProperty.h"
#include "SkyboltWidgets/SkyboltWidgetsFwd.h"

#include <QWidget>
#include <memory>
#include <typeindex>

class QVBoxLayout;

namespace skybolt {

class PropertyEditor : public QWidget
{
	Q_OBJECT
public:
	PropertyEditor(const PropertyEditorWidgetFactoryMapPtr& factoryMap, QWidget* parent = nullptr);

	void setModel(const PropertiesModelPtr& model);
	PropertiesModelPtr getModel() const { return mModel; }

private slots:
	void modelReset(PropertiesModel* model);

private:
	QWidget* createEditor(QtProperty* property);

private:
	PropertiesModelPtr mModel;
	QVBoxLayout* mLayout;
	PropertyEditorWidgetFactoryMapPtr mFactoryMap;

	std::map<std::string, bool> mDefaultSectionExpandedState;
};

} // namespace skybolt