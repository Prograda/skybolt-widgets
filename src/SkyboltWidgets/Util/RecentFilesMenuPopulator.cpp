// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "RecentFilesMenuPopulator.h"
#include <QFileInfo>
#include <QDir>

static const QString recentFileListName = "recentFileList";

namespace skybolt {

RecentFilesMenuPopulator::RecentFilesMenuPopulator(QMenu& menu, QPointer<QSettings> settings, FileOpener fileOpener, int maxRecentFiles) :
	mSettings(settings),
	mFileOpener(fileOpener),
	mMaxRecentFiles(maxRecentFiles)
{
	for (int i = 0; i < maxRecentFiles; ++i)
	{
		QAction* action = new QAction;
		action->setVisible(false);
		mRecentFileActs.push_back(action);
		menu.addAction(action);

		QObject::connect(action, &QAction::triggered, [fileOpener, action] {fileOpener(action->data().toString()); });
	}

	if (mSettings)
	{
		updateRecentFileActions(*mSettings);
	}
}

void RecentFilesMenuPopulator::addRecentFilename(const QString &filename)
{
	if (!mSettings)
	{
		return;
	}

	QString fixedFilename = QDir::fromNativeSeparators(filename);

	QStringList files = mSettings->value(recentFileListName).toStringList();
	files.removeAll(fixedFilename);
	files.prepend(fixedFilename);
	while (files.size() > mMaxRecentFiles)
	{
		files.removeLast();
	}
	mSettings->setValue(recentFileListName, files);

	updateRecentFileActions(*mSettings);
}

void RecentFilesMenuPopulator::updateRecentFileActions(const QSettings& settings)
{
	QStringList files = settings.value(recentFileListName).toStringList();

	int numRecentFiles = qMin(files.size(), (int)mMaxRecentFiles);

	for (int i = 0; i < numRecentFiles; ++i)
	{
		QString text = QObject::tr("&%1 %2").arg(i + 1).arg(files[i]);
		mRecentFileActs[i]->setText(text);
		mRecentFileActs[i]->setData(files[i]);
		mRecentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < mMaxRecentFiles; ++j)
	{
		mRecentFileActs[j]->setVisible(false);
	}

	if (numRecentFiles == 0)
	{
		mRecentFileActs[0]->setVisible(true);
		mRecentFileActs[0]->setText("No recent files");
		mRecentFileActs[0]->setEnabled(false);
	}
	else
	{
		mRecentFileActs[0]->setEnabled(true);
	}
}

} // namespace skybolt