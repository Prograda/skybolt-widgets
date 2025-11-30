// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "QtTimerUtil.h"
#include <QTimer>

namespace skybolt {

QTimer* createAndStartIntervalTimer(int intervalMilliseconds, QObject* parent, std::function<void()> updateAction)
{
	auto timer = new QTimer(parent);
	timer->setInterval(intervalMilliseconds);

	QObject::connect(timer, &QTimer::timeout, updateAction);
	
	timer->start();
	return timer;
}

} // namespace skybolt