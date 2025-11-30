// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "TimeRateDialog.h"
#include "TimeControlWidget.h"

#include <QDialog>
#include <QDoubleValidator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QToolBar>

namespace skybolt {

TimeRateDialog::TimeRateDialog(double initialRate, QWidget* parent) :
	QDialog(parent, Qt::Popup | Qt::FramelessWindowHint),
	mRate(initialRate)
{
	setFocusPolicy(Qt::ClickFocus);

	QHBoxLayout* layout = new QHBoxLayout(this);

	const int buttonHeight = 25;

	auto toolBar = new QToolBar(this);

	QPushButton* realtimeButton = new QPushButton("1x");
	realtimeButton->setFixedWidth(buttonHeight);
	realtimeButton->setDefault(false);
	realtimeButton->setAutoDefault(false);

	QPushButton* slowDownButton = new QPushButton("-");
	slowDownButton->setFixedWidth(buttonHeight);
	slowDownButton->setDefault(false);
	slowDownButton->setAutoDefault(false);

	QPushButton* speedUpButton = new QPushButton("+");
	speedUpButton->setFixedWidth(buttonHeight);
	speedUpButton->setDefault(false);
	speedUpButton->setAutoDefault(false);

	QDoubleValidator* validator = new QDoubleValidator(this);
	validator->setNotation(QDoubleValidator::StandardNotation);
	validator->setBottom(0);

	mRateLineEdit = new QLineEdit(this);
	mRateLineEdit->setFocus();
	mRateLineEdit->setText(QString::number(mRate));
	mRateLineEdit->setMaximumWidth(50);
	mRateLineEdit->setValidator(validator);

	toolBar->addWidget(realtimeButton);
	toolBar->addWidget(slowDownButton);
	toolBar->addWidget(speedUpButton);
	toolBar->addWidget(mRateLineEdit);
	toolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	setMaximumWidth(toolBar->width());

	connect(realtimeButton, &QPushButton::clicked, this, [this] {
		setRate(1);
	});

	connect(slowDownButton, &QPushButton::clicked, this, [this] {
		setRate(mRate / 2);
	});

	connect(speedUpButton, &QPushButton::clicked, this, [this] {
		setRate(mRate * 2);
	});

	connect(mRateLineEdit, &QLineEdit::editingFinished, this, [this] {
		mRate = mRateLineEdit->text().toDouble();
		emit rateChanged(mRate);
	});

	layout->addWidget(toolBar);
}

void TimeRateDialog::closeEvent(QCloseEvent* event)
{
	emit closed();
}

void TimeRateDialog::setRate(double rate)
{
	mRate = rate;
	mRateLineEdit->setText(QString::number(mRate));
	emit rateChanged(mRate);
}

void addTimeRateDialogToToolBar(TimeRateDialog* timeRateDialog, QToolBar* toolBar, const QIcon& icon)
{
	assert(timeRateDialog);
	assert(timeControlWidget);

	auto rateAction = new QAction(icon, QObject::tr("Rate"), toolBar);
	toolBar->addAction(rateAction);

	QWidget* rateActionWidget = toolBar->widgetForAction(rateAction);
	QObject::connect(rateAction, &QAction::triggered, timeRateDialog, [timeRateDialog, rateActionWidget] () mutable {
		if (!timeRateDialog->isVisible())
		{
			timeRateDialog->move(rateActionWidget->mapToGlobal(QPoint(20, -50)));
			timeRateDialog->show();

			QObject::connect(timeRateDialog, &TimeRateDialog::closed, timeRateDialog, [timeRateDialog] {
				timeRateDialog->hide();
			});
		}		
	});
}

} // namespace skybolt