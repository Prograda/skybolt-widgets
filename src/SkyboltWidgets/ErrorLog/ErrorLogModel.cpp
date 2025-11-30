// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "ErrorLogModel.h"

#include <assert.h>

namespace skybolt {

ErrorLogModel::ErrorLogModel(QObject* parent) :
	QObject(parent)
{
}

void ErrorLogModel::append(const Item& item)
{
	mItems.push_back(item);
	Q_EMIT itemAppended(item);
}

void ErrorLogModel::clear()
{
	mItems.clear();
	Q_EMIT cleared();
}

} // namespace skybolt