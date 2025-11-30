// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "SyncedListModel.h"
#include "Util/QtTimerUtil.h"

namespace skybolt {

SyncedListModel::SyncedListModel(ItemsGetter itemsGetter, std::optional<int> autoUpdateIntervalMilliseconds) :
	mItemsGetter(itemsGetter)
{
	updateItems();

	if (autoUpdateIntervalMilliseconds)
	{
		createAndStartIntervalTimer(*autoUpdateIntervalMilliseconds, this, [this]()
		{
			updateItems();
		});
	}
}

SyncedListModel::~SyncedListModel() = default;

void SyncedListModel::updateItems()
{
	// Make a complete list of items we want in the list
	std::set<QString> newItems = mItemsGetter();

	// Add new items
	for (const QString& item : newItems)
	{
		if (mItems.find(item) == mItems.end())
		{
			insertRow(rowCount(), new QStandardItem(item));
		}
	}

	// Remove old items
	for (const QString& item : mItems)
	{
		if (newItems.find(item) == newItems.end())
		{
			if (auto items = findItems(item); !items.empty())
			{
				removeRow(items.front()->row());
			}
		}
	}
	std::swap(mItems, newItems);
}

} // namespace skybolt