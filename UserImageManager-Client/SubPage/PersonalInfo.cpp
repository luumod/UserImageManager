#include "PersonalInfo.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "SImageShowWidget.h"
#include <QDir>
#include <QFileDialog>
#include <QStackedWidget>
#include <QGridLayout>
#include <QPushButton>

PersonalInfo::PersonalInfo(QWidget* parent)
	: QWidget(parent)
	, m_model(new QStandardItemModel(this))
{
	init();
}

PersonalInfo::~PersonalInfo()
{
}

void PersonalInfo::init()
{
	main_layout = new QVBoxLayout;
	this->setLayout(main_layout);

	images_layout = new QGridLayout; //显示3*2的图片
	//images_layout->widget()->setStyleSheet("background-color:#E0DDEB;");
	main_layout->addLayout(images_layout);

	auto preBtn = new  QPushButton;
	auto nextBtn = new  QPushButton;
	preBtn->setIcon(QIcon(":/ResourceClient/pre.png"));
	nextBtn->setIcon(QIcon(":/ResourceClient/next.png"));
	connect(preBtn, &QPushButton::clicked, this, &PersonalInfo::onChangePrePage);
	connect(nextBtn, &QPushButton::clicked, this, &PersonalInfo::onChangeNextPage);
	QHBoxLayout* changeLayout = new QHBoxLayout;
	changeLayout->addStretch();
	changeLayout->addWidget(preBtn);
	changeLayout->addWidget(nextBtn);
	changeLayout->addStretch();
	main_layout->addLayout(changeLayout);

	onSearch(); 
}

void PersonalInfo::loadImage(const QJsonArray& imagesArray)
{
	//首先保存全部图片信息
	for (int i = 0; i < imagesArray.size(); i++) {
		auto image = imagesArray[i].toObject();
		m_imagesInfoMap.insert(i,
			{ image["image_id"].toVariant().toInt(),
			image["ower_user_id"].toVariant().toInt(),
			image["image_path"].toVariant().toString(),
			image["image_name"].toVariant().toString(),
			image["description"].toVariant().toString() });
		//qDebug()<<i<<": "<<m_imagesInfoMap[i].path<<","<< m_imagesInfoMap[i].name<<","<<m_imagesInfoMap[i].desc;
	}
}
void PersonalInfo::onChangePrePage()
{
	if (m_firstImageIndex - 6 < 0) {
		return;
	}
	m_firstImageIndex -= 6;
	m_lastImageIndex -= 6;
	parseJson();
}

void PersonalInfo::onChangeNextPage()
{
	if (m_firstImageIndex + 6 >= m_imagesInfoMap.size()) {
		return;
	}
	m_firstImageIndex += 6;
	m_lastImageIndex += 6;
	parseJson();
}

void PersonalInfo::onSearch()
{
	QVariantMap params;
	auto filter = sApp->globalConfig()->value("user/user_id").toString();
	if (!filter.isEmpty()) {
		params.insert("user_id", filter);
	}
	SHttpClient(URL("/api/user/get_image")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.params(params)
		.success([=](const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
			qWarning() << "Json解析错误: " << error.errorString();
			return;
		}
		if (jdom["code"].toInt() < 1000) {
			initParase(jdom.object()); // 初始化解析 + 预加载图片信息
			parseJson();  // 解析json数据
		}
			})
		.get(); // 发送get请求
}

void PersonalInfo::initParase(const QJsonObject& obj) //只初始化一次
{
	m_images.resize(6); //固定每页6张图片 3*2显示，换页则重新加载6张图片
	for (int j = 0; j < m_images.size(); j++) {
		m_images[j] = new SImageShowWidget; //预初始化
	}
	for (int i = 0; i < m_images.size(); i++) { //0-5
		images_layout->addWidget(m_images[i], i / 2, i % 2);
	}
	images_layout->setContentsMargins(0, 0, 0, 0);
	images_layout->setSpacing(1);

	auto jArray = obj["data"]["images"].toArray();
	loadImage(jArray);
}

void PersonalInfo::parseJson()
{
	for (int i = m_firstImageIndex, j = 0; i <= m_lastImageIndex; i++, j++) { //[0-5] [6-11] [...]
		//加载每一张图片
		m_images[j]->loadImage(QDir::currentPath() + "/" + m_imagesInfoMap[i].path
			, m_imagesInfoMap[i].name
			, m_imagesInfoMap[i].desc);
	} //(F:/code/UserImageManager/bin)  /  (../images/upload/1/1173012900_20250109145406_test1.jpg)
}

