// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include "QtProperty.h"

namespace skybolt {

struct OptionalProperty
{
	QtPropertyPtr property;
	bool present = false;
};

struct PropertyVector
{
	std::vector<QtPropertyPtr> items;
	QVariant itemDefaultValue;
};

} // namespace skybolt

Q_DECLARE_METATYPE(skybolt::OptionalProperty)
Q_DECLARE_METATYPE(skybolt::PropertyVector)