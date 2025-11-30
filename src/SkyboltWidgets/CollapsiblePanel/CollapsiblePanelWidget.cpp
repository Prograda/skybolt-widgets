// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "CollapsiblePanelWidget.h"

#include <QScrollArea>
#include <QStringLiteral>

namespace skybolt {

CollapsiblePanelWidget::CollapsiblePanelWidget(const QString& title, QWidget* contentWidget, QWidget* parent) :
    QWidget(parent),
    mContentWidget(contentWidget),
    mToggleButton(new QToolButton)
{
    mToggleButton->setText(title);
    mToggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mToggleButton->setCheckable(true);
    mToggleButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mToggleButton->setStyleSheet("QToolButton:checked { background-color: palette(mid); }");

    mContentLayout = new QVBoxLayout();
    mContentLayout->setContentsMargins(4, 0, 4, 0);
    mContentLayout->addWidget(mContentWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mToggleButton);
    mainLayout->addLayout(mContentLayout);

    connect(mToggleButton, &QAbstractButton::toggled, this, &CollapsiblePanelWidget::setExpanded);
    setExpanded(true);
}

void CollapsiblePanelWidget::setExpanded(bool expanded)
{
    mToggleButton->setChecked(expanded);
    mToggleButton->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
    mContentWidget->setVisible(expanded);

    Q_EMIT expandedStateChanged(expanded);
}

void CollapsiblePanelWidget::setContentMargins(int left, int top, int right, int bottom)
{
    mContentLayout->setContentsMargins(left, top, right, bottom);
}

} // namespace skybolt