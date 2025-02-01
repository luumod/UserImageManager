#include "SPersonalImagePage.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "SRoundedImageWidget.h"
#include "SImageShowWidget.h"
#include "SImageDetailView.h"
#include <QDir>
#include <QFileDialog>
#include <QStackedWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>

SPersonalImagePage::SPersonalImagePage(QWidget* parent)
	: QWidget(parent)
	, m_model(new QStandardItemModel(this))
{
	setMouseTracking(true);
	init();
}

SPersonalImagePage::~SPersonalImagePage()
{
}

void SPersonalImagePage::init()
{
	main_layout = new QVBoxLayout;
	this->setLayout(main_layout);

	images_layout_1 = new QHBoxLayout; 
	images_layout_2 = new QHBoxLayout;
	images_layout_3 = new QHBoxLayout;

	main_layout->addLayout(images_layout_1,2);
	main_layout->addSpacing(20);
	main_layout->addLayout(images_layout_2,2);
	main_layout->addSpacing(20);
	main_layout->addLayout(images_layout_3,2);


	//-----------------底部：页码、切换页数---------------------
	auto bottomLayout = new QHBoxLayout;
	m_totalLab = new QLabel;
	auto preBtn = new  QPushButton;
	auto nextBtn = new  QPushButton;
	preBtn->setFixedHeight(30);
	nextBtn->setFixedHeight(30);
	preBtn->setIcon(QIcon(":/ResourceClient/pre.png"));
	nextBtn->setIcon(QIcon(":/ResourceClient/next.png"));
	auto pageLab = new QLabel;
	pageLab->setText(QString::number(m_currentPage)); //从1开始
	connect(nextBtn, &QPushButton::clicked, this, [=]() {
		onChangeNextPage();
		pageLab->setText(QString::number(m_currentPage));
		});
	connect(preBtn, &QPushButton::clicked, this, [=]() {
		onChangePrePage();
		pageLab->setText(QString::number(m_currentPage));
		});
	auto text1 = new QLabel("前往");
	m_pageEdit = new QLineEdit;
	m_pageEdit->setFixedWidth(30);
	m_pageEdit->setAlignment(Qt::AlignCenter);
	m_pageEdit->setText(QString::number(m_currentPage));
	auto text2 = new QLabel("页");
	m_totalPageLab = new QLabel;

	bottomLayout->addWidget(m_totalLab);
	bottomLayout->addWidget(preBtn);
	bottomLayout->addWidget(pageLab);
	bottomLayout->addWidget(nextBtn);
	bottomLayout->addWidget(text1);
	bottomLayout->addWidget(m_pageEdit);
	bottomLayout->addWidget(text2);
	bottomLayout->addWidget(m_totalPageLab);
	bottomLayout->addStretch();
	//----------------------------------------------------

	main_layout->addLayout(bottomLayout,1);
	
	initImageShowWidgets();

	connect(m_pageEdit, &QLineEdit::returnPressed, this, [=]() {
		m_currentPage = m_pageEdit->text().toInt();
		if (m_currentPage < 1 || m_currentPage > m_totalPage) {
			m_currentPage = 1;
			m_pageEdit->setText(QString::number(m_currentPage));
		}
		pageLab->setText(QString::number(m_currentPage));
		onSearch();
		});
}

void SPersonalImagePage::resizeEvent(QResizeEvent* e)
{
	if (m_imageDetailPage) {
		m_imageDetailPage->resize(this->size());
	}
}

void SPersonalImagePage::onChangePrePage()
{
	if (m_currentPage <= 1) {
		return;
	}
	m_currentPage--;//  3 2 1 0
	onSearch();
}

void SPersonalImagePage::onChangeNextPage()
{
	if (m_currentPage == m_totalPage) {
		return;
	}
	m_currentPage++; // 0 1 2 3
	onSearch();
}

void SPersonalImagePage::onClickedOneImage(int id) {
	if (!m_imageDetailPage) {
		m_imageDetailPage = new SImageDetailView(SImageDetailView::ImageDetailPageType::In_SPersonalImagePage,this);
		connect(m_imageDetailPage, &SImageDetailView::imageLiked, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_likeCount++;
			});
		connect(m_imageDetailPage, &SImageDetailView::imageUnLiked, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_likeCount--;
			});
		connect(m_imageDetailPage, &SImageDetailView::imageStared, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_starCount++;
			});
		connect(m_imageDetailPage, &SImageDetailView::imageUnStared, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_starCount--;
			});
		connect(m_imageDetailPage, &SImageDetailView::imageDownloaded, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_downloadCount++;
			});
		connect(m_imageDetailPage, &SImageDetailView::commentCountChange, this, [=](int image_index,int comment_count) {
			m_imagesInfoMap[image_index].m_commentCount = comment_count;
			});
		connect(m_imageDetailPage, &SImageDetailView::imageDownloaded, this, [=](int image_index) {
			m_imagesInfoMap[image_index].m_downloadCount++;
			});
		connect(m_imageDetailPage, &SImageDetailView::nextImage, this, &SPersonalImagePage::onChangeNextImage);
		connect(m_imageDetailPage, &SImageDetailView::prevImage, this, &SPersonalImagePage::onChangePrevImage);
	}
	auto image_real_index = id;
	m_imageDetailPage->setData(m_imagesInfoMap[image_real_index], image_real_index);
	m_imageDetailPage->resize(this->size());
	m_imageDetailPage->show();
}

void SPersonalImagePage::onChangeNextImage(int next_id)
{
	if (next_id >= m_imagesInfoMap.size()) {

	}
	m_imageDetailPage->setData(m_imagesInfoMap[next_id], next_id);
}

void SPersonalImagePage::onChangePrevImage(int prev_id)
{
	m_imageDetailPage->setData(m_imagesInfoMap[prev_id], prev_id);
}

void SPersonalImagePage::onSearch() //切换页面时查询一次
{
	QVariantMap params;
	auto user_account = sApp->userData("user/user_account").toString();
	if (!user_account.isEmpty()) {
		params.insert("user_account", user_account);
	}
	params.insert("page", m_currentPage);
	params.insert("pageSize", m_images.size());
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

void SPersonalImagePage::showPage()
{
	onSearch();

	updateLayouts();
}

void SPersonalImagePage::updateLayouts()
{// 强制更新布局，确保所有部件都已经被正确布局
	images_layout_1->update();
	images_layout_2->update();
	images_layout_3->update();

	// 确保窗口已经布局和显示
	layout()->activate();
	show();
	QApplication::processEvents(); // 处理所有未处理的事件，确保部件被正确绘制
}

void SPersonalImagePage::initImageShowWidgets()
{
	m_images.resize(6); //固定每页6张图片 3*2显示，换页则重新加载6张图片
	for (int j = 0; j < m_images.size(); j++) {
		m_images[j] = new SImageShowWidget(j); //预初始化 ,传入id，用于记录图片所处的位置
		connect(m_images[j]->getImageShowWidget(), &SRoundedImageWidget::clickedImage, this, &SPersonalImagePage::onClickedOneImage);
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
} 

void SPersonalImagePage::initParase(const QJsonObject& obj)
{
	auto jArray = obj["data"]["images"].toArray();
	sApp->setUserData("user/totla_image_count_in_one_page", jArray.size());
	m_totalPage = obj["data"]["last_page"].toInt();
	m_totalImages = obj["data"]["total_records"].toInt();

	m_totalLab->setText(QString("共%1个图片").arg(m_totalImages));
	m_totalPageLab->setText(QString("共%1页").arg(m_totalPage));

	loadImage(jArray);
}

void SPersonalImagePage::loadImage(const QJsonArray& imagesArray)
{
	m_imagesInfoMap.clear();
	//首先保存全部图片信息
	for (int i = 0; i < imagesArray.size(); i++) {
		auto image = imagesArray[i].toObject();
		m_imagesInfoMap.insert(i, image);
	}
}

void SPersonalImagePage::parseJson()
{
	for (int i = 0; i < m_images.size(); i++) { //[0-5] [6-11] [...]
		//加载每一张图片
		if (i>= m_imagesInfoMap.size()) {
			m_images[i]->loadImage("", "", "");
		}
		else {
			m_images[i]->loadImage(QDir::currentPath() + "/" + m_imagesInfoMap[i].m_path
				, m_imagesInfoMap[i].m_name
				, m_imagesInfoMap[i].m_desc);
		}
		
	} //(F:/code/UserImageManager/bin)  /  (../images/upload/1/1173012900_20250109145406_test1.jpg)
}

