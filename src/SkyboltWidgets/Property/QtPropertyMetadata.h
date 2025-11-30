// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

namespace skybolt {

struct QtPropertyRepresentations
{
	static constexpr const char* toggleButton = "ToggleButton"; //!< Representation for a boolean property indicating it should be represented as a toggle button.
};

struct QtPropertyMetadataKeys
{
	static constexpr const char* representation = "Representation"; //!< Property value is a QString defining how the property should be represented in the UI. E.g. "Slider", "ColorPicker", etc.
	static constexpr const char* multiLine = "MultiLine"; //!< Property value is a bool indicating whether text should be rendered as multiple lines
	static constexpr const char* optionNames = "OptionNames"; //!< Property value is a QStringList containing the names of the possible values the property can hold. Useful for representing enum types.
	static constexpr const char* allowCustomOptions = "AllowCustomOptions"; //!< Value is a bool indicating whether the "OptionNames" are exhaustive, or whether the user can set a custom value not contained in "OptionNames". False by default.
};

} // namespace skybolt