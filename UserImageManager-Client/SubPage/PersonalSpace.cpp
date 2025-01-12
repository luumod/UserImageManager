#include "PersonalSpace.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "RoundedImageWidget.h"
#include "SImageShowWidget.h"
#include "SImageDetail.h"
#include <QDir>
#include <QFileDialog>
#include <QStackedWidget>
#include <QGridLayout>
#include <QPushButton>

PersonalSpace::PersonalSpace(QWidget* parent)
	: QWidget(parent)
	, m_model(new QStandardItemModel(this))
{
	setMouseTracking(true);
	init();
}

PersonalSpace::~PersonalSpace()
{
}

void PersonalSpace::init()
{
	main_layout = new QVBoxLayout;
	this->setLayout(main_layout);

	images_layout_1 = new QHBoxLayout; 
	images_layout_2 = new QHBoxLayout;
	images_layout_3 = new QHBoxLayout;

	main_layout->addLayout(images_layout_1,2);
	main_layout->addLayout(images_layout_2,2);
	main_layout->addLayout(images_layout_3,2);

	auto preBtn = new  QPushButton;
	auto nextBtn = new  QPushButton;
	preBtn->setFixedHeight(30);
	nextBtn->setFixedHeight(30);
	preBtn->setIcon(QIcon(":/ResourceClient/pre.png"));
	nextBtn->setIcon(QIcon(":/ResourceClient/next.png"));
	connect(preBtn, &QPushButton::clicked, this, &PersonalSpace::onChangePrePage);
	connect(nextBtn, &QPushButton::clicked, this, &PersonalSpace::onChangeNextPage);
	QHBoxLayout* changeLayout = new QHBoxLayout;
	changeLayout->addStretch();
	changeLayout->addWidget(preBtn);
	changeLayout->addWidget(nextBtn);
	changeLayout->addStretch();
	main_layout->addLayout(changeLayout,1);

	onSearch(); 
}

void PersonalSpace::resizeEvent(QResizeEvent* e)
{
	if (m_imageDetailDlg) {
		m_imageDetailDlg->resize(this->size());
	}
}

void PersonalSpace::onChangePrePage()
{
	if (m_firstImageIndex - 6 < 0) {
		return;
	}
	m_firstImageIndex -= 6;
	m_lastImageIndex -= 6;
	m_currentPage--;//  3 2 1 0
	parseJson();
}

void PersonalSpace::onChangeNextPage()
{
	if (m_firstImageIndex + 6 >= m_imagesInfoMap.size()) {
		return;
	}
	m_firstImageIndex += 6;
	m_lastImageIndex += 6;
	m_currentPage++; // 0 1 2 3
	parseJson();
}

void PersonalSpace::onClickedOneImage(int id) {
	if (!m_imageDetailDlg) {
		m_imageDetailDlg = new SImageDetailDlg(
			 this); //图片id
	}
	auto image_real_index = m_currentPage * m_images.size() + id;;
	m_imageDetailDlg->setData(m_imagesInfoMap[image_real_index]);
	/*m_imageDetailDlg->setData(ImageInfo(
		m_imagesInfoMap[image_real_index].m_id,
		m_imagesInfoMap[image_real_index].m_owner_id,
		m_imagesInfoMap[image_real_index].m_path,
		m_imagesInfoMap[image_real_index].m_name,
		m_imagesInfoMap[image_real_index].m_size,
		m_imagesInfoMap[image_real_index].m_format,
		m_imagesInfoMap[image_real_index].m_share,
		m_imagesInfoMap[image_real_index].m_type,
		m_imagesInfoMap[image_real_index].m_download,
		m_imagesInfoMap[image_real_index].m_ResolutionRatio,
		m_imagesInfoMap[image_real_index].m_quality,
		m_imagesInfoMap[image_real_index].m_upload_time,
		m_imagesInfoMap[image_real_index].m_desc));
		ImageInfo(image["image_id"].toInt()
				, image["owner_id"].toInt()
				, image["image_path"].toString()
				, image["image_name"].toString()
				, image["image_size"].toInt()
				, image["image_format"].toString()
				, image["image_share"].toInt()
				, image["image_type"].toString()
				, image["image_download"].toInt()
				, image["image_ResolutionRatio"].toString()
				, image["image_quality"].toString()
				, image["upload_time"].toString()
				, image["description"].toString())
	*/
	m_imageDetailDlg->resize(this->size());
	m_imageDetailDlg->show();
}



void PersonalSpace::onSearch()
{
	QVariantMap params;
	auto filter = sApp->userData("user/user_id").toString();
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

void PersonalSpace::initParase(const QJsonObject& obj) //只初始化一次
{
	m_images.resize(6); //固定每页6张图片 3*2显示，换页则重新加载6张图片
	for (int j = 0; j < m_images.size(); j++) {
		m_images[j] = new SImageShowWidget(j); //预初始化 ,传入id，用于记录图片所处的位置
		connect(m_images[j]->getImageShowWidget(), &RoundedImageWidget::clickedImage, this, &PersonalSpace::onClickedOneImage);
	}
	images_layout_1->addWidget(m_images[0]);
	images_layout_1->addSpacing(10);
	images_layout_1->addWidget(m_images[1]);

	images_layout_2->addWidget(m_images[2]);
	images_layout_2->addSpacing(10);
	images_layout_2->addWidget(m_images[3]);

	images_layout_3->addWidget(m_images[4]);
	images_layout_3->addSpacing(10);
	images_layout_3->addWidget(m_images[5]);

	auto jArray = obj["data"]["images"].toArray();
	loadImage(jArray);
}

void PersonalSpace::loadImage(const QJsonArray& imagesArray)
{
	//首先保存全部图片信息
	for (int i = 0; i < imagesArray.size(); i++) {
		auto image = imagesArray[i].toObject();
		m_imagesInfoMap.insert(i,
			ImageInfo(image["image_id"].toInt()
				, image["owner_id"].toInt()
				, image["image_path"].toString()
				, image["image_name"].toString()
				, image["image_size"].toInt()
				, image["image_format"].toString()
				, image["image_share"].toInt()
				, image["image_type"].toString()
				, image["image_download"].toInt()
				, image["image_ResolutionRatio"].toString()
				, image["image_quality"].toString()
				, image["upload_time"].toString()
				, image["description"].toString()));
	}
}

void PersonalSpace::parseJson()
{
	for (int i = m_firstImageIndex, j = 0; i <= m_lastImageIndex; i++, j++) { //[0-5] [6-11] [...]
		//加载每一张图片
		m_images[j]->loadImage(QDir::currentPath() + "/" + m_imagesInfoMap[i].m_path
			, m_imagesInfoMap[i].m_name
			, m_imagesInfoMap[i].m_desc);
	} //(F:/code/UserImageManager/bin)  /  (../images/upload/1/1173012900_20250109145406_test1.jpg)
}

