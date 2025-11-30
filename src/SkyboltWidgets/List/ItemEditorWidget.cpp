// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "ItemEditorWidget.h"

#include <QBoxLayout>
#include <QDialogButtonBox>

namespace skybolt {

ItemEditorWidget::ItemEditorWidget(QWidget* contentWidget, QWidget* parent) :
	QWidget(parent)
{
	assert(contentWidget);

	auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	layout->addWidget(contentWidget);

	mCreateItemButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mCreateItemButtonBox->setVisible(false);
	layout->addWidget(mCreateItemButtonBox);

	connect(mCreateItemButtonBox, &QDialogButtonBox::accepted, this, [this] {
		setCreateItemModeEnabled(false);
		Q_EMIT createItemAccepted();
		});

	connect(mCreateItemButtonBox, &QDialogButtonBox::rejected, this, [this] {
		setCreateItemModeEnabled(false);
		Q_EMIT createItemCancelled();
		});
}

bool ItemEditorWidget::isCreateitemModelEnalbled() const
{
	return mCreateItemButtonBox->isVisible();
}

void ItemEditorWidget::setCreateItemModeEnabled(bool enabled)
{
	mCreateItemButtonBox->setVisible(enabled);
	Q_EMIT setCreateItemModeEnabledChanged(enabled);
}

} // namespace skybolt