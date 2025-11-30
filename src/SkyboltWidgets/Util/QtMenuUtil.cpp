// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "QtMenuUtil.h"

#include <QMenuBar>

namespace skybolt {

QAction* findMenuWithText(const QMenuBar& bar, const QString& text)
{
	QAction* action = nullptr;
	for (QAction* action : bar.actions())
	{
		if (action->text() == text)
		{
			return action;
		}
	}
	return nullptr;
}

void insertMenuBefore(QMenuBar& bar, const QString& before, QMenu& menu)
{
	QAction* action = findMenuWithText(bar, before);
	action ? bar.insertMenu(action, &menu) : bar.addMenu(&menu);
}

QMenu* findSubMenuByName(const QMenu& menu, const QString& subMenuName)
{
    for (QAction* action : menu.actions())
	{
        if (QMenu* submenu = action->menu())
		{
            if (submenu->title() == subMenuName)
			{
                return submenu;
            }
        }
    }
    return nullptr;
}

} // namespace skybolt