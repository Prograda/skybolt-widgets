// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "QtLayoutUtil.h"
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

namespace skybolt {

void clearLayout(QLayout& layout)
{
	while (QLayoutItem* item = layout.takeAt(0))
	{
		delete item->widget();
		if (item->layout())
		{
			clearLayout(*item->layout());
		}
		delete item;
	}
}

void addWidgetWithLabel(QGridLayout& layout, QWidget* widget, const QString& label)
{
	int row = layout.rowCount();
	layout.addWidget(new QLabel(label), row, 0);
	layout.addWidget(widget, row, 1);
}

QBoxLayout* createBoxLayoutWithWidgets(const std::vector<QWidget*>& widgets, QWidget* parent, QBoxLayout::Direction direction)
{
	auto layout = new QBoxLayout(direction, parent);
	for (QWidget* widget : widgets)
	{
		layout->addWidget(widget);
	}
	return layout;
}

} // namespace skybolt