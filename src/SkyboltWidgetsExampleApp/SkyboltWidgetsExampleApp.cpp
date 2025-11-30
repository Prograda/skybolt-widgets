// Copyright (c) Prograda Pty. Ltd.
// Licensed under the MIT license. See LICENSE file in the project root for details.

#include <SkyboltWidgets/CollapsiblePanel/CollapsiblePanelWidget.h>
#include <SkyboltWidgets/Property/PropertyEditor.h>
#include <SkyboltWidgets/Property/EditorWidgets.h>
#include <SkyboltWidgets/Property/DefaultEditorWidgets.h>
#include <SkyboltWidgets/List/ListEditorWidget.h>
#include <SkyboltWidgets/Property/QtProperty.h>
#include <SkyboltWidgets/Property/QtPropertyMetadata.h>
#include <SkyboltWidgets/Property/QtMetaTypes.h>
#include <SkyboltWidgets/ErrorLog/ErrorLogModel.h>
#include <SkyboltWidgets/ErrorLog/LatestErrorWidget.h>
#include <SkyboltWidgets/Timeline/TimelineWidget.h>
#include <SkyboltWidgets/Timeline/TimeControlWidget.h>
#include <SkyboltWidgets/Timeline/TimeRateDialog.h>
#include <SkyboltWidgets/Util/RecentFilesMenuPopulator.h>

#include <QApplication>
#include <QMainWindow>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <qicon.h>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QDateTime>
#include <QVector3D>
#include <QTreeView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QTimer>

#include <memory>

using namespace std;
using namespace skybolt;

static QWidget* makeLabeledWidget(const QString& title, QWidget* widget, QWidget* parent = nullptr)
{
	QGroupBox* gb = new QGroupBox(title, parent);
	QVBoxLayout* l = new QVBoxLayout(gb);
	l->setContentsMargins(6,6,6,6);
	if (widget) l->addWidget(widget);
	gb->setLayout(l);
	return gb;
}

static QWidget* createPropertyEditorPanel(QWidget* parent = nullptr)
{
	// Prepare the editor factory map
	DefaultEditorWidgetFactoryMapConfig config;
	config.listEditorIcons = createDefaultListEditorIcons();
	PropertyEditorWidgetFactoryMapPtr factoryMap = createDefaultEditorWidgetFactoryMap(config);

	// Create a PropertiesModel and some example QtProperty instances
	auto model = make_shared<PropertiesModel>();

	// Basic properties
	auto stringProp = createQtProperty("String", QVariant(QString("Hello World")));
	auto intProp = createQtProperty("Integer", QVariant(42));
	auto doubleProp = createQtProperty("Double", QVariant(3.14));
	auto boolProp = createQtProperty("Boolean", QVariant(false));
	auto dateTimeProp = createQtProperty("DateTime", QVariant(QDateTime::currentDateTime()));
	auto vec3Prop = createQtProperty("Vector3D", QVariant::fromValue(QVector3D(1.0f, 2.0f, 3.0f)));

	// Bool property with toggle button representation
	auto boolToggleProp = createQtProperty("Boolean", QVariant(true));
	boolToggleProp->setProperty(QtPropertyMetadataKeys::representation, QtPropertyRepresentations::toggleButton);

	// OptionalProperty example
	OptionalProperty opt;
	opt.property = createQtProperty("InnerDouble", QVariant(12.345));
	opt.present = true;
	auto optionalProp = createQtProperty("OptionalDouble", QVariant::fromValue(opt));

	// PropertyVector example
	PropertyVector pv;
	{
		pv.itemDefaultValue = QVariant::fromValue(QString("new item"));
		auto itemA = createQtProperty("itemA", QVariant(QString("Item A")));
		auto itemB = createQtProperty("itemB", QVariant(QString("Item B")));
		pv.items.push_back(itemA);
		pv.items.push_back(itemB);
	}
	auto propertyVectorProp = createQtProperty("StringList", QVariant::fromValue(pv));

	// Add properties to the model
	model->addProperty(stringProp);
	model->addProperty(intProp);
	model->addProperty(doubleProp);
	model->addProperty(boolProp);
	model->addProperty(boolToggleProp);
	model->addProperty(dateTimeProp);
	model->addProperty(vec3Prop);
	model->addProperty(optionalProp, nullptr, nullptr, "TestCategory");
	model->addProperty(propertyVectorProp, nullptr, nullptr, "TestCategory");

	auto propertyEditor = new PropertyEditor(factoryMap, parent);
	propertyEditor->setModel(model);
	return makeLabeledWidget("PropertyEditor", propertyEditor, parent);
}

static QWidget* createTreeViewPanel(QWidget* parent = nullptr)
{
	using TreeItemModel = QStandardItemModel;
	auto treeModel = new TreeItemModel(parent);
	treeModel->setHorizontalHeaderLabels({ "Name", "Value" });
	{
		auto root = treeModel->invisibleRootItem();
		auto parentA = new QStandardItem("Parent A");
		parentA->appendRow({ new QStandardItem("Child A1"), new QStandardItem("Value A1") });
		parentA->appendRow({ new QStandardItem("Child A2"), new QStandardItem("Value A2") });
		root->appendRow(parentA);

		auto parentB = new QStandardItem("Parent B");
		parentB->appendRow({ new QStandardItem("Child B1"), new QStandardItem("Value B1") });
		root->appendRow(parentB);
	}

	auto treeView = new QTreeView(parent);
	treeView->setModel(treeModel);
	treeView->expandAll();
	treeView->setAlternatingRowColors(true);
	treeView->header()->setStretchLastSection(false);
	treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	return makeLabeledWidget("TreeView (TreeItemModel)", treeView, parent);
}

static QWidget* createListEditorPanel(QWidget* parent = nullptr)
{
	auto listEditor = new ListEditorWidget(createDefaultListEditorIcons(), parent);

	// Create the list widget that will be manipulated by the ListEditor
	auto listWidget = new QListWidget(parent);
	listWidget->addItems({ "Item 0", "Item 1", "Item 2" });

	connectListEditorWithListWidget(listEditor, listWidget);
	QObject::connect(listEditor, &ListEditorWidget::itemAddRequested, parent, [listWidget, counter = listWidget->count()]() mutable {
		QString newItemText = "Item " + QString::number(counter);
		++counter;
		listWidget->addItem(newItemText);
		});

	// Place the list editor under the list widget
	QWidget* listBox = new QWidget(parent);
	QVBoxLayout* listBoxLayout = new QVBoxLayout(listBox);
	listBoxLayout->setContentsMargins(6,6,6,6);
	listBoxLayout->addWidget(listWidget, 1);
	listBoxLayout->addWidget(listEditor, 0);
	listBox->setLayout(listBoxLayout);
	return makeLabeledWidget("ListEditor", listBox, parent);
}

static QWidget* createLatestErrorWidget(QWidget* parent)
{
	auto errorLogModel = new ErrorLogModel(parent);
	// Append some sample items
	errorLogModel->append({ QDateTime::currentDateTime(), ErrorLogModel::Severity::Warning, QString("This is a warning sample message.") });
	errorLogModel->append({ QDateTime::currentDateTime(), ErrorLogModel::Severity::Error, QString("This is an error sample message\nwith multiple lines to test single-line display.") });

	auto errorWidget = new LatestErrorWidget(errorLogModel, parent);
	return makeLabeledWidget("LatestErrorWidget", errorWidget, parent);
}

static QWidget* createCollapsiblePanelWidgets(QWidget* parent)
{
	auto widgets = new QWidget(parent);
	auto layout = new QVBoxLayout(widgets);

	for (int i = 0; i < 2; ++i)
	{
		auto content = new QLabel("Content", parent);
		auto widget = new CollapsiblePanelWidget("Panel" + QString::number(i+1), content, parent);
		layout->addWidget(widget);
	}

	return makeLabeledWidget("CollapsiblePanelWidget", widgets, parent);
}

static QWidget* createTimeControlWidgetPanel(QWidget* parent)
{
	auto widget = new QWidget(parent);
	auto layout = new QVBoxLayout(widget);

	auto timeControl = new TimeControlWidget(createDefaultTimeControlWidgetIcons(), parent);
	layout->addWidget(timeControl);

	auto timeline = new TimelineWidget(parent);
	timeline->setMarkedRange({20, 50});
	layout->addWidget(timeline);

	QObject::connect(timeControl, &TimeControlWidget::requestedTimeForward, widget, [timeline]() {
		timeline->setTime(timeline->getRange().end);
		});

	QObject::connect(timeControl, &TimeControlWidget::requestedTimeBackward, widget, [timeline]() {
		timeline->setTime(timeline->getRange().start);
		});

	auto timeRateWidget = new TimeRateDialog(1.0, widget);
	addTimeRateDialogToToolBar(timeRateWidget, timeControl->getToolBar(), QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));

	auto playbackTimer = new QTimer(widget);
	playbackTimer->setInterval(100); // 100 ms ticks
	QObject::connect(playbackTimer, &QTimer::timeout, widget, [timeline, timeControl, timeRateWidget]() {
		// advance time
		timeControl->setPlaying(timeControl->getRequestedPlayState());
		if (timeControl->getRequestedPlayState())
		{
			double t = timeline->getTime() + timeRateWidget->getRate() * 0.1;
			t = std::clamp(t, timeline->getRange().start, timeline->getRange().end);
			timeline->setTime(t);
		}
	});
	playbackTimer->start();

	return widget;
}

int main(int argc, char** argv)
{
	// Create application
	QApplication app(argc, argv);

	// Build main window
	QMainWindow mainWindow;
	auto central = new QWidget(&mainWindow);
	mainWindow.setCentralWidget(central);
	mainWindow.setWindowTitle("SkyboltWidgets - Widget Demo");
	mainWindow.resize(1200, 800);

	// Use a grid layout to place widgets in multiple columns and rows
	auto mainLayout = new QGridLayout(central);
	mainLayout->setContentsMargins(8,8,8,8);
	mainLayout->setHorizontalSpacing(12);
	mainLayout->setVerticalSpacing(12);
	central->setLayout(mainLayout);

	// Create Widgets
	mainLayout->addWidget(createPropertyEditorPanel(central), 0, 0);
	mainLayout->addWidget(createTreeViewPanel(central), 0, 1);
	mainLayout->addWidget(createListEditorPanel(central), 1, 0);
	mainLayout->addWidget(createLatestErrorWidget(central), 1, 1);
	mainLayout->addWidget(createCollapsiblePanelWidgets(central), 2, 1);
	mainLayout->addWidget(createTimeControlWidgetPanel(central), 3, 0);

	mainWindow.show();
	return app.exec();
}