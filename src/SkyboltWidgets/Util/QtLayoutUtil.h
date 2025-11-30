// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QLayout>

namespace skybolt {

//! Deletes all items in a QLayout
void clearLayout(QLayout& layout);

void addWidgetWithLabel(QGridLayout& layout, QWidget* widget, const QString& label);

QBoxLayout* createBoxLayoutWithWidgets(const std::vector<QWidget*>& widgets, QWidget* parent = nullptr, QBoxLayout::Direction direction = QBoxLayout::Direction::LeftToRight);

} // namespace skybolt