// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QWidget>
#include <QLabel>
#include <QSlider>

namespace skybolt {

class TimelineWidget : public QWidget
{
	Q_OBJECT
public:
	TimelineWidget(QWidget* parent = 0);

	double getTime() const { return mTimeValue; }
	void setTime(double time);

	struct TimeRange
	{
		double start;
		double end;
	};

	TimeRange getRange() const { return mRange; }
	void setRange(const TimeRange& range);

	//! Allows the user to change the current time, e.g. by dragging the time slider
	void setUserTimeChangeAllowed(bool allowed);
	
	//! Allows visualization of a "marked" part of the timeline.
	//! For example, this can be used to show the part of a video that has been buffered.
	void setMarkedRange(const TimeRange& range);
	void setMarkedRangeBrush(const QBrush& brush);

	Q_SIGNAL void timeChanged(double time);

private:
	int toSliderValue(double time) const;

private:
	double mTimeValue = 0;
	TimeRange mRange{ 0, 60};
	bool mUserTimeChangeAllowed = true;

	class TimelineSlider* mSlider;
	TimeRange mMarkedRange;
};

} // namespace skybolt