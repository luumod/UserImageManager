#include "PersonalSpace.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "RoundedImageWidget.h"
#include "SImageShowWidget.h"
#include "ImageDetailPage.h"
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
	if (m_imageDetailPage) {
		m_imageDetailPage->resize(this->size());
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
	if (id == -1 && m_currentPage == 0) { //第一页的第一张---->最后一页的最后一张
		m_currentPage = m_totalPage - 1;
		id = m_lastPageImageCount - 1;
	}
	else if (id == 5 && m_currentPage > 0) { // 第二，三，....页的第一张---->上一页的最后一张
		m_currentPage--;
	}
	else if (id >= m_images.size()) { //当前页的最后一张---->下一页的第一张，或者第一页的第一张
		id = 0;
		m_currentPage++;
		if (m_currentPage >= m_totalPage){
			m_currentPage = 0;
		}
	}

#if _DEBUG
	qDebug() << "---------- 当前页：" << m_currentPage << "  第" << id << "张图片 ----------------";
#endif

	if (!m_imageDetailPage) {
		m_imageDetailPage = new ImageDetailPage(this);
		connect(m_imageDetailPage, &ImageDetailPage::imageLiked, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_likeCount++;
			});
		connect(m_imageDetailPage, &ImageDetailPage::imageUnLiked, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_likeCount--;
			});
		connect(m_imageDetailPage, &ImageDetailPage::imageStared, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_starCount++;
			});
		connect(m_imageDetailPage, &ImageDetailPage::imageUnStared, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_starCount--;
			});
		connect(m_imageDetailPage, &ImageDetailPage::imageDownloaded, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_downloadCount++;
			});
		connect(m_imageDetailPage, &ImageDetailPage::nextImage, this, &PersonalSpace::onClickedOneImage);
		connect(m_imageDetailPage, &ImageDetailPage::prevImage, this, &PersonalSpace::onClickedOneImage);
	}
	auto image_real_index = m_currentPage * m_images.size() + id;
	m_imageDetailPage->setData(m_imagesInfoMap[image_real_index], image_real_index);
	m_imageDetailPage->resize(this->size());
	m_imageDetailPage->show();
}

void PersonalSpace::onSearch()
{
	QVariantMap params;
	auto filter = sApp->userData("user/user_account").toString();
	if (!filter.isEmpty()) {
		params.insert("user_account", filter);
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
	m_totalPage = std::ceil(jArray.size() * 1.0 / m_images.size()); //总页数从1开始（上取整）
	m_lastPageImageCount = jArray.size() % m_images.size(); //最后一页的图片数量
	loadImage(jArray);
}

void PersonalSpace::loadImage(const QJsonArray& imagesArray)
{
	//首先保存全部图片信息
	for (int i = 0; i < imagesArray.size(); i++) {
		auto image = imagesArray[i].toObject();
		m_imagesInfoMap.insert(i,
			ImageInfo(image["image_id"].toInt() //image_id
				, image["owner_id"].toInt()     // id
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
				, image["description"].toString()
				, image["like_count"].toInt()
				, image["star_count"].toInt()
				, image["download_count"].toInt()
				, image["comment_count"].toInt()));
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

