// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QMenu>
#include <QPointer>
#include <QSettings>
#include <functional>

namespace skybolt {

class RecentFilesMenuPopulator
{
public:
	typedef std::function<void(const QString&)> FileOpener;

	RecentFilesMenuPopulator(QMenu& menu, QPointer<QSettings> settings, FileOpener fileOpener, int maxRecentFiles = 10);

	void addRecentFilename(const QString &fileName);

private:
	void updateRecentFileActions(const QSettings& settings);

private:
	QPointer<QSettings> mSettings;
	FileOpener mFileOpener;
	std::vector<QAction*> mRecentFileActs;
	const int mMaxRecentFiles = 10;
};

} // namespace skybolt