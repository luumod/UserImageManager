#include "MainWindow.h"
#include "SUserManagerPage.h"
#include "SHomePage.h"
#include "SUploadImagePage.h"
#include "SPersonalImagePage.h"
#include "SImageManagerPage.h"
#include "SPageTopWidget.h"
#include "SMaskWidget.h"
#include "SPersonalCenterPage.h"
#include "SHttpClient.h"
#include "SConfigFile.h"
#include "SApp.h"
#include "SSliderBar.h"
#include <QBoxLayout>
#include <QFile>
#include <QStackedWidget>


MainWindow::MainWindow(QWidget* parent)
	:QWidget(parent)
{
	this->setMinimumSize(1800,900);
	move(50, 50);
	init();

	SMaskWidget::instance()->setMainWindow(this);

	SHttpClient(URL("/api/user/get_image_count")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.fail([=](const QString& error, int code) {
#ifdef _DEBUG
		qCritical() << "error:" << error << "code:" << code;
#endif
			})
		.success([=](const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
#ifdef _DEBUG
			qWarning() << "json数据解析错误: " << error.errorString();
#endif
		}
		if (jdom["code"].toInt() < 1000) {
			auto count = jdom["data"].toObject()["total_image_count"].toInt();
			sApp->globalConfig()->setValue("program/total_image_count", count);
		}
			})
		.get();

}

MainWindow::~MainWindow()
{
}

void MainWindow::init()
{
	auto main_layout = new QVBoxLayout;
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);

	//---------------------------
	auto topWidget = new SPageTopWidget;
	//---------------------------


	///--------------------------
	auto contentLayout = new QHBoxLayout;
	contentLayout->setContentsMargins(0, 0, 0, 0);

	m_sliderBar = new SSliderBar;
	m_stkWidget = new QStackedWidget;

	m_homePage = new SHomePage;
	m_uploadImagePage = new SUploadImagePage;
	m_personalImagePage = new SPersonalImagePage;
	m_imageManagerPage = new SImageManagerPage;
	m_userManagerPage = new SUserManagerPage;
	m_personalDetailPage = new SPersonalCenterPage;

	m_stkWidget->addWidget(m_homePage); //首页
	m_stkWidget->addWidget(m_uploadImagePage); //上传图片
	m_stkWidget->addWidget(m_personalImagePage); //我的图片
	m_stkWidget->addWidget(m_imageManagerPage); //图片管理
	m_stkWidget->addWidget(new QWidget); //图片加工
	m_stkWidget->addWidget(new QWidget); //共享社区
	m_stkWidget->addWidget(m_userManagerPage); //用户管理
	m_stkWidget->addWidget(m_personalDetailPage); //个人中心

	connect(m_sliderBar, &SSliderBar::clickedOneItem, [=](int pageIdx) {
		m_stkWidget->setCurrentIndex(pageIdx);
		switch (pageIdx)
		{
			//保证在切换页面时，一定是最新的数据
		case 2:
			m_personalImagePage->showPage();
			break;
		case 3:
			m_imageManagerPage->showPage();
			break;
		case 6:
			m_userManagerPage->showPage();
			break;
		case 7:
			m_personalDetailPage->showPage();
			break;
		default:
			break;
		}
		});

	contentLayout->addWidget(m_sliderBar);
	contentLayout->addWidget(m_stkWidget);


	//-----------------------
	main_layout->addWidget(topWidget);
	main_layout->addLayout(contentLayout);
	this->setLayout(main_layout);
	//-----------------------
}