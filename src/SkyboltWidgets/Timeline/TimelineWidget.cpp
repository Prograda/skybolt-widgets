// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "TimelineWidget.h"

#include <QLayout>
#include <QStyle>
#include <QToolBar>
#include <QDateTime>
#include <QMouseEvent>
#include <QPainter>

namespace skybolt {

struct IntRangeInclusive
{
	int first;
	int last;
};

class TimelineSlider : public QSlider
{
public:
	TimelineSlider(QWidget* parent = nullptr) :
		QSlider(Qt::Horizontal, parent),
		mMarkedRangeBrush(qApp->palette().brush(QPalette::Midlight))
	{}

	void setMarkedRange(const IntRangeInclusive& range)
	{
		mMarkedRange = range;
		update();
	}

	void setMarkedRangeBrush(const QBrush& brush)
	{
		mMarkedRangeBrush = brush;
		update();
	}

protected:
	void mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
		{
			setValue(minimum() + ((maximum() - minimum()) * event->x()) / width());
			event->accept();
		}
		QSlider::mousePressEvent(event);
	}

	void paintEvent(QPaintEvent *ev)
	{
		// Draw Marked time range
		int start = QStyle::sliderPositionFromValue(minimum(), maximum(), mMarkedRange.first, width());
		int end = QStyle::sliderPositionFromValue(minimum(), maximum(), mMarkedRange.last, width());

		QPainter painter(this);
		painter.setBrush(mMarkedRangeBrush);
		painter.setPen(Qt::NoPen);
		painter.drawRect(start, 0, end - start, height());
		QSlider::paintEvent(ev);
	}

private:
	IntRangeInclusive mMarkedRange;
	QBrush mMarkedRangeBrush;
};

TimelineWidget::TimelineWidget(QWidget* parent) :
	QWidget(parent),
	mMarkedRange(0,0)
{
	mSlider = new TimelineSlider(this);
	mSlider->setMaximum(1000);

	setLayout(new QHBoxLayout);
	layout()->setContentsMargins(0,0,0,0);
	layout()->addWidget(mSlider);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	connect(mSlider, &QSlider::valueChanged, [&](int valueInt) {
		if (mUserTimeChangeAllowed)
		{
			double value = double(valueInt) / double(mSlider->maximum() - mSlider->minimum());
			value = std::lerp(mRange.start, mRange.end, value);
			setTime(value);
		}
	});
}

void TimelineWidget::setMarkedRange(const TimeRange& range)
{
	mMarkedRange = range;

	IntRangeInclusive intRange(toSliderValue(range.start), toSliderValue(range.end));
	mSlider->setMarkedRange(intRange);
}

void TimelineWidget::setMarkedRangeBrush(const QBrush& brush)
{
	mSlider->setMarkedRangeBrush(brush);
}

void TimelineWidget::setTime(double time)
{
	if (time != mTimeValue)
	{
		Q_EMIT timeChanged(time);
		mTimeValue = time;
	}

	mSlider->blockSignals(true);
	mSlider->setValue(toSliderValue(time));
	mSlider->blockSignals(false);
}

void TimelineWidget::setRange(const TimeRange& range)
{
	mRange = range;

	mSlider->setValue(toSliderValue(mTimeValue));
	IntRangeInclusive intRange(toSliderValue(mMarkedRange.start), toSliderValue(mMarkedRange.end));
	mSlider->setMarkedRange(intRange);
}

void TimelineWidget::setUserTimeChangeAllowed(bool allowed)
{
	mUserTimeChangeAllowed = allowed;
	mSlider->setEnabled(allowed);
}

int TimelineWidget::toSliderValue(double time) const
{
	double duration = mRange.end - mRange.start;
	return (duration > 0) ? (time - mRange.start) / duration * mSlider->maximum() : 0;
}

} // namespace skybolt