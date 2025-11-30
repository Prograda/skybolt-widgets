// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "DefaultEditorWidgets.h"
#include "EditorWidgets.h"
#include "PropertyEditor.h"
#include "QtPropertyMetadata.h"
#include "QtPropertyReflection.h"
#include "QtMetaTypes.h"

namespace skybolt {

std::unique_ptr<PropertyEditorWidgetFactoryMap> createDefaultEditorWidgetFactoryMap(const DefaultEditorWidgetFactoryMapConfig& config)
{
	auto m = std::make_unique<PropertyEditorWidgetFactoryMap>(PropertyEditorWidgetFactoryMap{
		{ QMetaType::Type::QString, &createStringEditor },
		{ QMetaType::Type::Int, &createIntOrEnumEditor },
		{ QMetaType::Type::Float, &createDoubleEditor },
		{ QMetaType::Type::Double, &createDoubleEditor },
		{ QMetaType::Type::Bool, &createBoolEditor },
		{ QMetaType::Type::QDateTime, &createDateTimeEditor },
		{ QMetaType::Type::QVector3D, &createVector3DEditor }
	});

	(*m)[qMetaTypeId<OptionalProperty>()] = [factories = m.get()] (QtProperty* property, QWidget* parent) {
		return createOptionalVariantEditor(*factories, property, parent);
	};
	(*m)[qMetaTypeId<PropertyVector>()] = [factories = m.get(), listEditorIcons = config.listEditorIcons](QtProperty* property, QWidget* parent) {
		return createPropertyVectorEditor(*factories, property, listEditorIcons, parent);
	};

	return m;
}

} // namespace skybolt