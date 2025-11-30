// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "QtProperty.h"

#include <QScopeGuard>
#include <assert.h>

namespace skybolt {

QtPropertyPtr createQtProperty(const QString& name, const QVariant& value)
{
	auto property = std::make_shared<QtProperty>();
	property->name = name;
	property->setValue(value);
	return property;
}

void PropertiesModel::update()
{
	mCurrentlyUpdating = true;
	QScopeGuard guard([this] {
        mCurrentlyUpdating = false;
    });

	for (const auto& entry : mPropertyUpdaters)
	{
		entry.second(*entry.first);
	}
}

void PropertiesModel::addProperty(const QtPropertyPtr& property, QtPropertyUpdater updater, QtPropertyApplier applier, const std::string& sectionName)
{
	mProperties[sectionName].push_back(property);
	if (updater)
	{
		mPropertyUpdaters[property] = updater;
	}

	if (applier)
	{
		connect(property.get(), &QtProperty::valueChanged, this, [=]() {
			if (!mCurrentlyUpdating)
			{
				applier(*property);
			}
		});
	}
}

PropertiesModel::PropertiesModel()
{
}

PropertiesModel::PropertiesModel(const SectionProperties& properties) :
	mProperties(properties)
{
}

} // namespace skybolt