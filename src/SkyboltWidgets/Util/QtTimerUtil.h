// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <functional>

class QObject;
class QTimer;

namespace skybolt {

QTimer* createAndStartIntervalTimer(int intervalMilliseconds, QObject* parent, std::function<void()> updateAction);

} // namespace skybolt