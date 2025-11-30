// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include "LatestErrorWidget.h"
#include "ErrorLogModel.h"
#include "ErrorLogWidget.h"
#include "Util/QtDialogUtil.h"

#include <QApplication>
#include <QBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QToolButton>
#include <QStatusBar>
#include <QStyle>

#include <assert.h>

namespace skybolt {

LatestErrorWidget::LatestErrorWidget(ErrorLogModel* model, QWidget* parent) :
	QWidget(parent)
{
	assert(model);

	auto widget = new QWidget(this);
	auto layout = new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	auto label = new QLabel(this);
	layout->addWidget(label);

	QFontMetrics fm(label->font());

	QStyle* style = QApplication::style();

	mExpandButton = new QToolButton(this);
	mExpandButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::WindowNew));
	mExpandButton->setToolTip("Expand");
	mExpandButton->setFixedHeight(fm.height());
	mExpandButton->setVisible(false);
	layout->addWidget(mExpandButton);

	QObject::connect(mExpandButton, &QToolButton::clicked, model, [this, model] {
		auto logWidget = new ErrorLogWidget(model, this);
		QDialog* dialog = createDialogNonModal(logWidget, "Error Log");
		dialog->resize(800, 500);
		dialog->show();
	});

    auto sink = [label, this] (const ErrorLogModel::Item& item) {
		QString singleLineMessage = item.message;
		singleLineMessage = singleLineMessage.replace('\n', ' ');
		singleLineMessage.resize(std::min(singleLineMessage.size(), qsizetype(100)));
		singleLineMessage += "...";

		label->setText(singleLineMessage);
		mExpandButton->setVisible(!item.message.isEmpty());
	};

	QObject::connect(model, &ErrorLogModel::itemAppended, this, sink);
	if (!model->getItems().empty())
	{
		sink(model->getItems().back());
	}

	QObject::connect(model, &ErrorLogModel::cleared, this, [label, this] {
		label->setText("");
		mExpandButton->setVisible(false);
	});
}

} // namespace skybolt