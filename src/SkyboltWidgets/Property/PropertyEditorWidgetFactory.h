// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "SkyboltWidgets/SkyboltWidgetsFwd.h"

#include <functional>
#include <memory>
#include <QWidget>

namespace skybolt {

using PropertyEditorWidgetFactory = std::function<QWidget*(QtProperty* property, QWidget* parent)>;
using QtMetaTypeId = int;
using PropertyEditorWidgetFactoryMap = std::map<QtMetaTypeId, PropertyEditorWidgetFactory>;
using PropertyEditorWidgetFactoryMapPtr = std::shared_ptr<PropertyEditorWidgetFactoryMap>;

} // namespace skybolt