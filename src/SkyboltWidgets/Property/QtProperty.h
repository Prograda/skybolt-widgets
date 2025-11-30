// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "SkyboltWidgets/SkyboltWidgetsFwd.h"

#include <QObject>
#include <QVariant>
#include <memory>

namespace skybolt {

struct QtProperty : public QObject
{
	Q_OBJECT
public:
	QString name;
	bool enabled = true;

	void setEnabled(bool e)
	{
		if (enabled != e)
		{
			enabled = e;
			emit enabledChanged(enabled);
		}
	}

	const QVariant& value() const { return mValue; }

	void setValue(const QVariant& v)
	{
		if (mValue != v)
		{
			mValue = v;
			emit valueChanged();
		}
	}


signals:
	void valueChanged();
	void enabledChanged(bool enabled);

private:
	QVariant mValue;
};

QtPropertyPtr createQtProperty(const QString& name, const QVariant& value);

class PropertiesModel : public QObject
{
	Q_OBJECT
public:
	using Properties = std::vector<QtPropertyPtr>;
	using SectionProperties = std::map<std::string, Properties>;

	PropertiesModel();
	PropertiesModel(const SectionProperties& properties);
	~PropertiesModel() {}

	void update();

	virtual SectionProperties getProperties() const { return mProperties; }

	using QtPropertyUpdater = std::function<void(QtProperty&)> ;
	using QtPropertyApplier = std::function<void(const QtProperty&)>;

	static const std::string& getDefaultSectionName()
	{
		static std::string s;
		return s;
	}

	//! @param updater is regularly called update the value of QtProperty from an external model
	//! @param applier is called when a QtProperty value should be applied to an external model (e.g. if the user pressent 'Enter' key in a text box
	void addProperty(const QtPropertyPtr& property, QtPropertyUpdater updater = nullptr, QtPropertyApplier applier = nullptr, const std::string& sectionName = getDefaultSectionName());

signals:
	void modelReset(PropertiesModel*);

protected:
	SectionProperties mProperties;
	std::map<QtPropertyPtr, QtPropertyUpdater> mPropertyUpdaters;
	bool mCurrentlyUpdating = false;
};

} // namespace skybolt