// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

class QWidget;

namespace skybolt {

QWidget* wrapWithVerticalScrollBar(QWidget* content, QWidget* parent = nullptr);

} // namespace skybolt