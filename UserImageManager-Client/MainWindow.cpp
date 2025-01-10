#include "MainWindow.h"
#include "UserManagerPage.h"
#include "HomePage.h"
#include "PersonalSpace.h"
#include "STopBar.h"
#include "SMaskWidget.h"
#include "PersonalDetail.h"
#include <QBoxLayout>
#include <QFile>
#include <QStackedWidget>
#include <QTreeWidget>

MainWindow::MainWindow(QWidget* parent)
	:QWidget(parent)
{
	this->setMinimumSize(1200,600);
	this->resize(this->minimumSize());
	init();

	SMaskWidget::instance()->setMainWindow(this);
}

void MainWindow::init()
{
	//整体布局
	QVBoxLayout* sp = new QVBoxLayout;
	sp->addWidget(initTopBar()); //top
	sp->addWidget(initConetentArea()); //content
	sp->setContentsMargins(0, 0, 0, 0);
	sp->setSpacing(0);
	this->setLayout(sp);
}


QWidget* MainWindow::initTopBar()
{
	m_topBar = new STopBar;
	return m_topBar;
}

QWidget* MainWindow::initConetentArea()
{
	m_treeNavBar = new QTreeWidget;
	m_stkWidget = new QStackedWidget;

	m_homePage = new HomePage;
	m_personalInfoPage = new PersonalSpace;
	m_userManagerPage = new UserManagerPage;
	m_personalDetailPage = new PersonalDetail;
	m_stkWidget->addWidget(m_homePage); //首页
	m_stkWidget->addWidget(m_personalInfoPage); //个人仓库
	m_stkWidget->addWidget(m_userManagerPage); //用户管理
	m_stkWidget->addWidget(m_personalDetailPage); //个人中心
	m_stkWidget->setContentsMargins(0, 0, 0, 0);

	m_treeNavBar->setMaximumWidth(this->width() / 5);

	m_treeNavBar->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	QTreeWidgetItem* item = nullptr;
	//头
	item = new QTreeWidgetItem({ "操作" });
	item->setIcon(0,QIcon(":/ResourceClient/privilege_header.png"));
	m_treeNavBar->setHeaderItem(item);
	//item
	item = new QTreeWidgetItem(m_treeNavBar, { "首页" }, 0);
	item->setIcon(0,QIcon(":/ResourceClient/user.png"));

	item = new QTreeWidgetItem(m_treeNavBar, { "我的仓库" }, 1);
	item->setIcon(0,QIcon(":/ResourceClient/role.png"));

	item = new QTreeWidgetItem(m_treeNavBar, { "用户管理" }, 2);
	item->setIcon(0,QIcon(":/ResourceClient/privilege.png"));

	item = new QTreeWidgetItem(m_treeNavBar, { "个人中心" }, 3);
	item->setIcon(0,QIcon(":/ResourceClient/user.png"));

	//选择某一个项目，切换页面
	connect(m_treeNavBar, &QTreeWidget::itemClicked, [=](QTreeWidgetItem* item, int column){
			m_stkWidget->setCurrentIndex(item->type());
		});

	QFile file(":/ResourceClient/sytle.css");
	if (file.open(QIODevice::ReadOnly)) {
		setStyleSheet(file.readAll());
	}

	QWidget* content_widget = new QWidget;
	QHBoxLayout* layout = new QHBoxLayout(content_widget);
	layout->addWidget(m_treeNavBar);
	layout->addWidget(m_stkWidget);
	return content_widget;
}