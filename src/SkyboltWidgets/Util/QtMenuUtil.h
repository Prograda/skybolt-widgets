// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

class QAction;
class QMenu;
class QMenuBar;
class QString;

namespace skybolt {

//! @returns nullptr if not found
QAction* findMenuWithText(const QMenuBar& bar, const QString& text);

//! Inserts menu before a named menu item, or adds to end of menu if the named item is not found.
void insertMenuBefore(QMenuBar& bar, const QString& before, QMenu& menu);

QMenu* findSubMenuByName(const QMenu& menu, const QString& subMenuName);

} // namespace skybolt