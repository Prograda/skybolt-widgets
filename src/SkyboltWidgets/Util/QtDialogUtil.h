// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QDialog>
#include <QString>
#include <memory>

namespace skybolt {

std::shared_ptr<QDialog> createDialogModal(QWidget* content, const QString& title);

QDialog* createDialogNonModal(QWidget* content, const QString& title, QWidget* parent = nullptr);

} // namespace skybolt