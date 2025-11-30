// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "ErrorLogModel.h"
#include <QWidget>

class QTableWidget;

namespace skybolt {

class ErrorLogWidget : public QWidget
{
public:
    ErrorLogWidget(ErrorLogModel* model, QWidget* parent = nullptr);

private:
    void addItemToTable(const ErrorLogModel::Item& item);

private:
    QTableWidget* mTableWidget;
};

} // namespace skybolt