#include "SImageDetailView.h"
#include "SImageInfoWidget.h"
#include "SUserCommentView.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QPushButton>
#include <QDir>
#include <QBoxLayout>
#include <QScrollArea>
#include <QMessageBox>

SImageDetailView::SImageDetailView(ImageDetailPageType pageType, QWidget* parent)
	: QWidget(parent)
	, m_pageType(pageType)

{
	init();
	this->setStyleSheet("QScrollArea{border: 0px solid;} \
		QPushButton#backBtn, QPushButton#prevBtn, QPushButton#nextBtn {background-color: #FFFFFF; border: 1px solid #000000; border-radius: 5px; color: #000000; font-size: 16px; padding: 5px 10px;}\
        QPushButton#backBtn:hover, QPushButton#prevBtn:hover, QPushButton#nextBtn:hover {background-color: #000000; color: #FFFFFF;} \
		 ");
	
}

SImageDetailView::~SImageDetailView()
{
}

void SImageDetailView::init()
{
	auto mainLayout = new QVBoxLayout;
	mainLayout->setContentsMargins(0, 0, 0, 0);

	auto scrollArea = new QScrollArea;
	scrollArea->setWidgetResizable(true);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	
	auto widget = new QWidget;
	auto layout1 = new QVBoxLayout;
	layout1->setContentsMargins(0, 0, 0, 0);

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
	if (m_pageType == ImageDetailPageType::In_SPersonalImagePage) {
		topLayout->addWidget(prevBtn);
		topLayout->addWidget(nextBtn);
	}
	topLayout->addStretch();
	topWidget->setObjectName("topWidget");
	topWidget->setLayout(topLayout);



	//中部（图片信息显示）
	auto middleLayout = new QHBoxLayout;
	m_imageDetailDlg  = new SImageInfoWidget;
	middleLayout->addStretch();
	middleLayout->addWidget(m_imageDetailDlg);
	middleLayout->addStretch();

	//底部（评论区）
	m_commentDlg = new SUserCommentView;

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
		if (!m_imageDetailDlg->nextImage()) {
			QMessageBox::information(this, "提示", "已经是当前页的最后一张了");
			return;
		}
		emit nextImage(m_imageDetailDlg->currentImageIndex());
		});
	connect(prevBtn, &QPushButton::clicked, this, [=]() {
		if (!m_imageDetailDlg->prevImage()) {
			QMessageBox::information(this, "提示", "已经是当前页的第一张了");
			return;
		}
		emit prevImage(m_imageDetailDlg->currentImageIndex());
		});

	connect(m_imageDetailDlg, &SImageInfoWidget::imageLiked, this, [=](int image_index) {
		emit imageLiked(image_index);
	});
	connect(m_imageDetailDlg, &SImageInfoWidget::imageUnLiked, this, [=](int image_index) {
		emit imageUnLiked(image_index);
		});
	connect(m_imageDetailDlg, &SImageInfoWidget::imageStared, this, [=](int image_index) {
		emit imageStared(image_index);
		});
	connect(m_imageDetailDlg, &SImageInfoWidget::imageUnStared, this, [=](int image_index) {
		emit imageUnStared(image_index);
		});
	connect(m_imageDetailDlg, &SImageInfoWidget::imageDownloaded, this, [=](int image_index) {
		emit imageDownloaded(image_index);
		});
	connect(m_commentDlg, &SUserCommentView::commentCountChange, this, [=](int image_index,int comment_count) {
		emit commentCountChange(image_index, comment_count);
		});
	
}

void SImageDetailView::setData(ImageInfo info,int image_index)
{
	//1. 图片基本信息
	m_imageDetailDlg->setData(info, image_index);

	//2. 图片评论信息
	SHttpClient(URL("/api/user/get_comment?image_id=" + QString::number(info.m_id))).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.success([=](const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
			qWarning() << "Json解析错误: " << error.errorString();
			return;
		}
		if (jdom["code"].toInt() < 1000) {
			auto jUsers = jdom["data"]["users"].toArray();
			m_commentDlg->clearData(info.m_id,info.m_commentCount, image_index);
			for (int i = 0; i < jUsers.size(); i++) {
				auto jUser = jUsers.at(i).toObject();
				m_commentDlg->addUserComment(
					jUser.value("comment_id").toInt(),
					jUser.value("user_name").toString(),
					jUser.value("avatar_path").toString().isEmpty() ? "" : QDir::currentPath() + "/" + jUser.value("avatar_path").toString(),
					jUser.value("comment_content").toString(),
					jUser.value("comment_time").toString(),
					jUser.value("priority").toInt() == 999);
			}
		}
		})
		.get(); // 发送get请求
}

void SImageDetailView::setData(ImageInfo info)
{
	setData(info, 0);
}
