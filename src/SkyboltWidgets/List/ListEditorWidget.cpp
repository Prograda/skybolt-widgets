// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "ListEditorWidget.h"

#include <QApplication>
#include <QBoxLayout>
#include <QListWidget>
#include <QStyle>
#include <QToolBar>
#include <QToolButton>

namespace skybolt {

ListEditorIcons createDefaultListEditorIcons()
{
	ListEditorIcons icons;
	icons.add = QIcon::fromTheme(QIcon::ThemeIcon::ListAdd);
	icons.remove = QIcon::fromTheme(QIcon::ThemeIcon::ListRemove);
	icons.moveUp = QIcon::fromTheme(QIcon::ThemeIcon::GoUp);
	icons.moveDown = QIcon::fromTheme(QIcon::ThemeIcon::GoDown);
	return icons;
}

ListEditorWidget::ListEditorWidget(const ListEditorIcons& icons, QWidget* parent) :
	QWidget(parent)
{
	assert(itemEditorWidget);

	auto mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	// Create list controls
	{
		mListControlsWidget = new QToolBar(parent);
		mainLayout->addWidget(mListControlsWidget);

		mAddButton = new QToolButton(this);
		mAddButton->setIcon(icons.add);
		mAddButton->setText("Add");
		mAddButton->setToolTip("Add");
		mListControlsWidget->addWidget(mAddButton);
		connect(mAddButton, &QAbstractButton::clicked, this, [this, mainLayout] {
			Q_EMIT itemAddRequested();
			});

		mRemoveButton = new QToolButton(this);
		mRemoveButton->setIcon(icons.remove);
		mRemoveButton->setText("Remove");
		mRemoveButton->setToolTip("Remove");
		mRemoveButton->setEnabled(false);
		mListControlsWidget->addWidget(mRemoveButton);
		connect(mRemoveButton, &QAbstractButton::clicked, this, [this] {
			Q_EMIT itemRemoveRequested();
			});

		mMoveUpButton = new QToolButton(this);
		mMoveUpButton->setIcon(icons.moveUp);
		mMoveUpButton->setText("Move up");
		mMoveUpButton->setToolTip("Move up");
		mMoveUpButton->setEnabled(false);
		mListControlsWidget->addWidget(mMoveUpButton);
		connect(mMoveUpButton, &QAbstractButton::clicked, this, [this] {
			Q_EMIT itemMoveUpRequested();
			});

		mMoveDownButton = new QToolButton(this);
		mMoveDownButton->setIcon(icons.moveDown);
		mMoveDownButton->setText("Move down");
		mMoveDownButton->setToolTip("Move down");
		mMoveDownButton->setEnabled(false);
		mListControlsWidget->addWidget(mMoveDownButton);
		connect(mMoveDownButton, &QAbstractButton::clicked, this, [this] {
			Q_EMIT itemMoveDownRequested();
			});
	}
}

void ListEditorWidget::setRemoveEnabled(bool enabled)
{
	mRemoveButton->setEnabled(enabled);
}

void ListEditorWidget::setMoveUpEnabled(bool enabled)
{
	mMoveUpButton->setEnabled(enabled);
}

void ListEditorWidget::setMoveDownEnabled(bool enabled)
{
	mMoveDownButton->setEnabled(enabled);
}

void connectListEditorWithListWidget(ListEditorWidget* listEditor, QListWidget* listWidget)
{
	assert(listEditor);
	assert(listWidget);

	// Remove the current item
	QObject::connect(listEditor, &ListEditorWidget::itemRemoveRequested, listWidget, [listWidget] {
		int row = listWidget->currentRow();
		if (row >= 0)
		{
			delete listWidget->takeItem(row);
			// adjust selection
			const int newCount = listWidget->count();
			if (newCount > 0)
			{
				listWidget->setCurrentRow(qBound(0, row, newCount - 1));
			}
		}
	});

	// Move up
	QObject::connect(listEditor, &ListEditorWidget::itemMoveUpRequested, listWidget, [listWidget] {
		int row = listWidget->currentRow();
		if (row > 0)
		{
			QListWidgetItem* it = listWidget->takeItem(row);
			listWidget->insertItem(row - 1, it);
			listWidget->setCurrentRow(row - 1);
		}
	});

	// Move down
	QObject::connect(listEditor, &ListEditorWidget::itemMoveDownRequested, listWidget, [listWidget] {
		int row = listWidget->currentRow();
		if (row >= 0 && row < listWidget->count() - 1)
		{
			QListWidgetItem* it = listWidget->takeItem(row);
			listWidget->insertItem(row + 1, it);
			listWidget->setCurrentRow(row + 1);
		}
	});

	// Update buttons enabled state when selection changes.
	QObject::connect(listWidget, &QListWidget::currentRowChanged, listEditor, [listEditor, listWidget](int row) {
		const int count = listWidget->count();
		const bool hasSelection = (row >= 0);
		listEditor->setRemoveEnabled(hasSelection);
		listEditor->setMoveUpEnabled(hasSelection && row > 0);
		listEditor->setMoveDownEnabled(hasSelection && row < (count - 1));
	});

	// Ensure buttons reflect initial state
	{
		int row = listWidget->currentRow();
		const int count = listWidget->count();
		listEditor->setRemoveEnabled(row >= 0);
		listEditor->setMoveUpEnabled(row > 0);
		listEditor->setMoveDownEnabled(row >= 0 && row < (count - 1));
	}
}

} // namespace skybolt