// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "QtScrollAreaUtil.h"
#include <QScrollArea>


namespace skybolt {

QWidget* wrapWithVerticalScrollBar(QWidget* content, QWidget* parent)
{
	auto scrollArea = new QScrollArea(parent);
	scrollArea->setWidget(content);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	return scrollArea;
}

} // namespace skybolt