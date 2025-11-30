// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <memory>

namespace skybolt {

class CollapsiblePanelWidget;
class ErrorLogModel;
class ErrorLogWidget;
class ItemEditorWidget;
class LastErrorWidget;
class ListEditorWidget;
class PropertyEditor;
class PropertiesModel;
struct QtProperty;
class RecentFilesMenuPopulator;
class TreeItem;
class TreeItemModel;

using PropertiesModelPtr = std::shared_ptr<PropertiesModel>;
using QtPropertyPtr = std::shared_ptr<QtProperty>;
using TreeItemPtr = std::shared_ptr<TreeItem>;

} // namespace skybolt