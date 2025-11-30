// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QIcon>
#include <QWidget>


class QDialogButtonBox;
class QListWidget;
class QToolBar;
class QToolButton;

namespace skybolt {

struct ListEditorIcons
{
	QIcon add;
	QIcon remove;
	QIcon moveUp;
	QIcon moveDown;	
};

ListEditorIcons createDefaultListEditorIcons();

class ListEditorWidget : public QWidget
{
	Q_OBJECT
public:
	ListEditorWidget(const ListEditorIcons& icons, QWidget* parent = nullptr);

public Q_SLOTS:
	void setRemoveEnabled(bool enabled);
	void setMoveUpEnabled(bool enabled);
	void setMoveDownEnabled(bool enabled);

Q_SIGNALS:
	void itemAddRequested();
	void itemRemoveRequested();
	void itemMoveUpRequested();
	void itemMoveDownRequested();

private:
	QToolBar* mListControlsWidget;
	QToolButton* mAddButton;
	QToolButton* mRemoveButton;
	QToolButton* mMoveUpButton;
	QToolButton* mMoveDownButton;
};

//! Connect ListEditorWidget to a QListWidget
void connectListEditorWithListWidget(ListEditorWidget* listEditor, QListWidget* listWidget);

} // namespace skybolt