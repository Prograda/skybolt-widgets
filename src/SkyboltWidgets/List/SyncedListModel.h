// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QStandardItemModel>

#include <optional>
#include <set>

namespace skybolt {

class SyncedListModel : public QStandardItemModel
{
public:
	typedef std::function<std::set<QString>()> ItemsGetter;

	//! @param autoUpdateIntervalMilliseconds If set, the model will automatically call updateItems() at this interval
	SyncedListModel(ItemsGetter itemsGetter, std::optional<int> autoUpdateIntervalMilliseconds = 100);
	~SyncedListModel() override;

	void updateItems();

private:
	ItemsGetter mItemsGetter;
	std::set<QString> mItems;
};

} // namespace skybolt