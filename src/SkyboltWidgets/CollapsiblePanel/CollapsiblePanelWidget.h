// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QToolButton>

namespace skybolt {

class CollapsiblePanelWidget : public QWidget
{
    Q_OBJECT

public:
    CollapsiblePanelWidget(const QString& title, QWidget* contentWidget, QWidget* parent = nullptr);

    QWidget* getContentWidget() const { return mContentWidget; }

    void setContentMargins(int left, int top, int right, int bottom);

    QToolButton* getToggleButton() const { return mToggleButton; }

public slots:
    void setExpanded(bool expanded);

signals:
    void expandedStateChanged(bool expanded);

private:
    QToolButton* mToggleButton;
    QWidget* mContentWidget;
    QVBoxLayout* mContentLayout;
};

} // namespace skybolt