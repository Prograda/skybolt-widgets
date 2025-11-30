// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "PropertyEditorWidgetFactory.h"
#include "SkyboltWidgets/List/ListEditorWidget.h"

#ifdef BUILD_WITH_SKYBOLT_REFLECT
#include <SkyboltReflect/SkyboltReflectFwd.h>
#endif

namespace skybolt {

struct DefaultEditorWidgetFactoryMapConfig
{
	ListEditorIcons listEditorIcons;
};

std::unique_ptr<PropertyEditorWidgetFactoryMap> createDefaultEditorWidgetFactoryMap(const DefaultEditorWidgetFactoryMapConfig& config);

} // namespace skybolt