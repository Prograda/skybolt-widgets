// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "TimeControlWidget.h"

#include <QApplication.h>
#include <QLabel>
#include <QLayout>
#include <QStyle>
#include <QToolBar>

namespace skybolt {

TimeControlWidgetIcons createDefaultTimeControlWidgetIcons()
{
	TimeControlWidgetIcons icons;
	icons.play = QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart);
	icons.pause = QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackPause);
	icons.forward = QIcon::fromTheme(QIcon::ThemeIcon::MediaSkipForward);
	icons.backward = QIcon::fromTheme(QIcon::ThemeIcon::MediaSkipBackward);

	return icons;
}

TimeControlWidget::TimeControlWidget(const TimeControlWidgetIcons& icons, QWidget* parent) :
	QWidget(parent),
	mPlayIcon(icons.play),
	mPauseIcon(icons.pause),
	mToolBar(new QToolBar(this))
{
	mPlayAction = new QAction(icons.play, tr("Play"), this);
	mPlayAction->setShortcut(tr("Ctrl+P"));

	mForwardAction = new QAction(icons.forward, tr("Forward to end"), this);
	mForwardAction->setShortcut(tr("Ctrl+F"));
	
	mBackwardAction = new QAction(icons.backward, tr("Back to start"), this);
	mBackwardAction->setShortcut(tr("Ctrl+B"));

	mTimeRateLabel = new QLabel("", this);
	mTimeRateLabel->setVisible(false);

	mToolBar->addAction(mBackwardAction);
	mToolBar->addAction(mPlayAction);
	mToolBar->addAction(mForwardAction);

	connect(mPlayAction, &QAction::triggered, this, [this] {
		mRequestedPlay = !mPlaying;
		Q_EMIT requestedPlayStateChanged(mRequestedPlay);
		});

	connect(mForwardAction, &QAction::triggered, this, [this] {
		Q_EMIT requestedTimeForward();
		});

	connect(mBackwardAction, &QAction::triggered, this, [this] {
		Q_EMIT requestedTimeBackward();
		});

	auto layout = new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(mToolBar);
	layout->addWidget(mTimeRateLabel, 0);
	layout->addStretch();
}

TimeControlWidget::~TimeControlWidget() = default;

void TimeControlWidget::setPlaying(bool playing)
{
	mPlayAction->setIcon(playing ? mPauseIcon : mPlayIcon);
	mPlaying = playing;
}

void TimeControlWidget::setForwardEnabled(bool enabled)
{
	mForwardAction->setEnabled(enabled);
}

void TimeControlWidget::setBackwardEnabled(bool enabled)
{
	mBackwardAction->setEnabled(enabled);
}

} // namespace skybolt