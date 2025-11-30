// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#pragma once

#include <QDialog>

class QLineEdit;
class QToolBar;

namespace skybolt {

class TimeRateDialog : public QDialog
{
    Q_OBJECT
public:
    TimeRateDialog(double initialRate, QWidget* parent = nullptr);

	double getRate() const { return mRate; }
	void setRate(double rate);

Q_SIGNALS:
	void rateChanged(double newRate);
	void closed();

private:
    void closeEvent(QCloseEvent* event) override;

private:
	double mRate;
	QLineEdit* mRateLineEdit;
};

//! @param timeControlWidget is the TimeControlWidget to which the TimeRateDialog should be added. Never null.
//! @param icon is the icon to use for the TimeRateDialog action in the TimeControlWidget toolbar.
void addTimeRateDialogToToolBar(TimeRateDialog* timeRateDialog, QToolBar* toolBar, const QIcon& icon);

} // namespace skybolt