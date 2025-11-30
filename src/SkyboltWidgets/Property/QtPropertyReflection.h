// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#ifdef BUILD_WITH_SKYBOLT_REFLECT

#include "SkyboltWidgets/Property/PropertyEditorWidgetFactory.h"
#include "SkyboltWidgets/Property/QtProperty.h"
#include <SkyboltReflect/Reflection.h>

#include <functional>
#include <optional>

namespace skybolt {

struct QtPropertyUpdaterApplier
{
	QtPropertyPtr property; //!< Never null
	PropertiesModel::QtPropertyUpdater updater; //!< Never null
	PropertiesModel::QtPropertyApplier applier; //!< Null for read-only properties
};

struct ReflPropertyInstanceVariant
{
	std::optional<skybolt::refl::Instance> instance;
};

Q_DECLARE_METATYPE(ReflPropertyInstanceVariant);

using ReflInstanceGetter = std::function<std::optional<skybolt::refl::Instance>()>;

using PropertyFactory = std::function<QtPropertyUpdaterApplier(skybolt::refl::TypeRegistry& typeRegistry, const ReflInstanceGetter& instanceGetter, const skybolt::refl::PropertyPtr& property)>;
using ReflTypePropertyFactoryMap = std::map<skybolt::refl::TypePtr, PropertyFactory>;
using ReflTypePropertyFactoryMapPtr = std::shared_ptr<ReflTypePropertyFactoryMap>;

std::optional<QtPropertyUpdaterApplier> reflPropertyToQt(skybolt::refl::TypeRegistry& typeRegistry, const ReflInstanceGetter& instanceGetter, const skybolt::refl::PropertyPtr& property, const ReflTypePropertyFactoryMap& typePropertyFactories);

void addReflPropertiesToModel(skybolt::refl::TypeRegistry& typeRegistry, PropertiesModel& model, const std::vector<skybolt::refl::PropertyPtr>& properties, const ReflInstanceGetter& instanceGetter, const ReflTypePropertyFactoryMap& typePropertyFactories);

QWidget* createReflPropertyInstanceEditor(QtProperty* property, QWidget* parent, skybolt::refl::TypeRegistry* typeRegistry, const ReflTypePropertyFactoryMap& typePropertyFactories, const PropertyEditorWidgetFactoryMapPtr& factoryMap);

void addReflEditorsToFactoryMap(PropertyEditorWidgetFactoryMap& m, skybolt::refl::TypeRegistry* typeRegistry, const ReflTypePropertyFactoryMapPtr& typePropertyFactories, const PropertyEditorWidgetFactoryMapPtr& factoryMap);

ReflTypePropertyFactoryMap createDefaultReflTypePropertyFactories(skybolt::refl::TypeRegistry& typeRegistry);

} // namespace skybolt

#endif // BUILD_WITH_SKYBOLT_REFLECT