#include "ImageDetailPage.h"
#include "SImageDetailDlg.h"
#include "SCommentDlg.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QScrollArea>

ImageDetailPage::ImageDetailPage(QWidget* parent)
	: QWidget(parent)
{
	init();
	this->setStyleSheet("QScrollArea{border: 0px solid;} \
		QPushButton#backBtn, QPushButton#prevBtn, QPushButton#nextBtn {background-color: #FFFFFF; border: 1px solid #000000; border-radius: 5px; color: #000000; font-size: 16px; padding: 5px 10px;}\
        QPushButton#backBtn:hover, QPushButton#prevBtn:hover, QPushButton#nextBtn:hover {background-color: #000000; color: #FFFFFF;} \
		 ");
	
}

ImageDetailPage::~ImageDetailPage()
{
}

void ImageDetailPage::init()
{
	auto mainLayout = new QVBoxLayout;

	auto scrollArea = new QScrollArea;
	scrollArea->setWidgetResizable(true);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	
	auto widget = new QWidget;
	auto layout1 = new QVBoxLayout;

	//顶端（返回 + 下一张，上一张）
	auto topLayout = new QHBoxLayout;
	QWidget* topWidget = new QWidget;
	auto backBtn = new QPushButton("返回");
	auto nextBtn = new QPushButton("下一张");
	auto prevBtn = new QPushButton("上一张");
	backBtn->setObjectName("backBtn");
	prevBtn->setObjectName("prevBtn");
	nextBtn->setObjectName("nextBtn");
	backBtn->setIcon(QIcon(":/ResourceClient/back.png"));
	prevBtn->setIcon(QIcon(":/ResourceClient/pre.png"));
	nextBtn->setIcon(QIcon(":/ResourceClient/next.png"));
	topLayout->addWidget(backBtn);
	topLayout->addStretch();
	topLayout->addWidget(prevBtn);
	topLayout->addWidget(nextBtn);
	topLayout->addStretch();
	topWidget->setObjectName("topWidget");
	topWidget->setLayout(topLayout);



	//中部（图片信息显示）
	auto middleLayout = new QHBoxLayout;
	m_imageDetailDlg  = new SImageDetailDlg;
	middleLayout->addStretch();
	middleLayout->addWidget(m_imageDetailDlg);
	middleLayout->addStretch();

	//底部（评论区）
	m_commentDlg = new SCommentDlg;

	layout1->addWidget(topWidget);
	layout1->addLayout(middleLayout);
	layout1->addWidget(m_commentDlg);
	
	widget->setLayout(layout1);

	//滚动
	scrollArea->setWidget(widget);

	mainLayout->addWidget(scrollArea);
	this->setLayout(mainLayout);
	





	connect(backBtn, &QPushButton::clicked, this, [=]() {
		this->close();
		});
	connect(nextBtn, &QPushButton::clicked, this, [=]() {
		m_imageDetailDlg->nextImage();
		emit nextImage(m_imageDetailDlg->currentImageIndex());
		});
	connect(prevBtn, &QPushButton::clicked, this, [=]() {
		m_imageDetailDlg->prevImage();
		emit prevImage(m_imageDetailDlg->currentImageIndex());
		});

	connect(m_imageDetailDlg, &SImageDetailDlg::imageLiked, this, [=](int image_index) {
		emit imageLiked(image_index);
	});
	connect(m_imageDetailDlg, &SImageDetailDlg::imageUnLiked, this, [=](int image_index) {
		emit imageUnLiked(image_index);
		});
	connect(m_imageDetailDlg, &SImageDetailDlg::imageStared, this, [=](int image_index) {
		emit imageStared(image_index);
		});
	connect(m_imageDetailDlg, &SImageDetailDlg::imageUnStared, this, [=](int image_index) {
		emit imageUnStared(image_index);
		});
	connect(m_imageDetailDlg, &SImageDetailDlg::imageDownloaded, this, [=](int image_index) {
		emit imageDownloaded(image_index);
		});
	
}

void ImageDetailPage::setData(ImageInfo info,int image_index)
{
	m_imageDetailDlg->setData(info, image_index);
}