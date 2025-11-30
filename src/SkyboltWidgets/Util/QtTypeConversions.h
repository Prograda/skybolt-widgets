// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QStringList>
#include <string>
#include <vector>

namespace skybolt {

inline QStringList toQStringList(const std::vector<std::string>& strs)
{
	QStringList r;
	r.reserve((int)strs.size());
	for (const std::string& str : strs)
	{
		r.push_back(QString::fromStdString(str));
	}
	return r;
}

} // namespace skybolt