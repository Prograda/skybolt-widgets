// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QWidget>
#include <QAction>

class QDialog;
class QLabel;
class QToolBar;

namespace skybolt {

struct TimeControlWidgetIcons
{
	QIcon play;
	QIcon pause;
	QIcon forward;
	QIcon backward;
};

TimeControlWidgetIcons createDefaultTimeControlWidgetIcons();

class TimeControlWidget : public QWidget
{
	Q_OBJECT
public:
	TimeControlWidget(const TimeControlWidgetIcons& icons, QWidget* parent = nullptr);
	~TimeControlWidget();

	bool getRequestedPlayState() const { return mRequestedPlay; }

	void setPlaying(bool playing);

	void setForwardEnabled(bool enabled);

	void setBackwardEnabled(bool enabled);

	QToolBar* getToolBar() const { return mToolBar; }

	Q_SIGNAL void requestedPlayStateChanged(bool playing);

	Q_SIGNAL void requestedTimeRateChanged(double rate);

	Q_SIGNAL void requestedTimeBackward();

	Q_SIGNAL void requestedTimeForward();

private:
	const QIcon mPlayIcon;
	const QIcon mPauseIcon;
	QToolBar* mToolBar;

	bool mRequestedPlay = false;
	bool mPlaying = false;

	QAction* mPlayAction;
	QAction* mForwardAction;
	QAction* mBackwardAction;
	QLabel* mTimeRateLabel;
};

} // namespace skybolt