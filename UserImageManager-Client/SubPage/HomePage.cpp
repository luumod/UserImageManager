#include "HomePage.h"
#include "SImageShowWidget.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QGridLayout>
#include <QStackedWidget>
#include <QDir>

HomePage::HomePage(QWidget *parent) 
	: QWidget(parent)
{
	this->setContentsMargins(0, 0, 0, 0);
	this->setStyleSheet("background-color:green;");
	init();

	onSearch();
}

HomePage::~HomePage()
{

}

void HomePage::init()
{
	m_stkedWidget = new QStackedWidget;

	connect(this, &HomePage::parseFinised, this, &HomePage::test_images_4);
}

void HomePage::onSearch()
{
	QVariantMap params;
	auto filter = QString("1173012900"); 
	if (!filter.isEmpty()) {
		params.insert("user_id", filter);
	}
	SHttpClient(URL("/api/user/get_image"))
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.debug(true)
		.params(params)
		.success([=](const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
			qWarning() << "Json解析错误: " << error.errorString();
			return;
		}
		parseJson(jdom.object()); // 解析json数据
			})
		.get(); // 发送get请求
}

void HomePage::parseJson(const QJsonObject& obj)
{
	int w = m_stkedWidget->width() / 2;
	int h = m_stkedWidget->height() / 2;
	auto jArray = obj["data"]["images"].toArray();
	for (int i = 0; i < jArray.size(); i++) { //6
		auto jUser = jArray[i].toObject(); 

		m_images.push_back(new SImageShowWidget(w, h,
			QDir::currentPath() + "/" +  jUser["image_path"].toVariant().toString()
		, jUser["image_name"].toVariant().toString()
		, jUser["description"].toVariant().toString()));
	} //(F:/code/UserImageManager/bin)  /  (../images/upload/1/1173012900_20250109145406_test1.jpg)
	emit parseFinised();
}

void HomePage::test_images_4()
{
	qDebug() << "触发";
	QGridLayout* layout = new QGridLayout;
	
	for (int i = 0; i < m_images.size(); i++) { //6
		layout->addWidget(m_images[i], i / 2, i % 2);
	}

	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(1);
	QWidget* widget = new QWidget;
	widget->setLayout(layout);
	widget->setStyleSheet("background-color:#E0DDEB;");
	m_stkedWidget->addWidget(widget);
	m_stkedWidget->setCurrentIndex(0);
	this->setLayout(new QVBoxLayout);
	this->layout()->addWidget(m_stkedWidget);
	this->layout()->setContentsMargins(0, 0, 0, 0);
}

