#include "SSliderBarBottomInfoItem.h"
#include <QBoxLayout>
#include <QLabel>
#include <QToolButton>

SSliderBarBottomInfoItem::SSliderBarBottomInfoItem(const QString& name, const QString& desc, const QString& avatarPath, QWidget* parent)
	: QWidget(parent)
	,m_name(name)
	,m_desc(desc)
	,m_avatarPath(avatarPath)
{
	init();
	this->setStyleSheet("QToolButton{background-color:transparent;border:none;}\
		QLabel#nameLabel{font-size:18px;font-family: 微软雅黑;}\
		QLabel#descLabel{font-size:13px;font-family: 微软雅黑;color:black;} \
		QLabel#sayLabel{font-size:12px;font-family: 微软雅黑;color:gray;}"
	);
}

SSliderBarBottomInfoItem::~SSliderBarBottomInfoItem()
{

}

void SSliderBarBottomInfoItem::init()
{
	auto main_layout = new QVBoxLayout;

	//-------------------
	auto topLayout = new QHBoxLayout;

	auto avatarLabel = new QLabel;
	avatarLabel->setPixmap(QPixmap(m_avatarPath));
	avatarLabel->setFixedSize(80, 80);
	avatarLabel->setScaledContents(true);
	avatarLabel->setMask(QRegion(avatarLabel->rect(), QRegion::RegionType::Ellipse));
	

	auto infoLayout = new QVBoxLayout;

	auto nameLabel = new QLabel(m_name);
	nameLabel->setObjectName("nameLabel");
	auto descLabel = new QLabel(m_desc);
	descLabel->setObjectName("descLabel");

	infoLayout->addWidget(nameLabel);
	infoLayout->addSpacing(10);
	infoLayout->addWidget(descLabel);


	topLayout->addWidget(avatarLabel);
	topLayout->addSpacing(10);
	topLayout->addLayout(infoLayout);

	//-------------------------------



	//---------媒体---------
	auto mediaLayout = new QHBoxLayout;
	mediaLayout->setSpacing(10);
	auto githubLabel = new QToolButton;
	githubLabel->setIcon(QPixmap(":/ResourceClient/github.png"));
	githubLabel->setFixedSize(20, 20);

	auto bbLabel = new QToolButton;
	bbLabel->setIcon(QPixmap(":/ResourceClient/bilibili.png"));
	bbLabel->setFixedSize(20, 20);

	auto emailLabel = new QToolButton;
	emailLabel->setIcon(QPixmap(":/ResourceClient/email.png"));
	emailLabel->setFixedSize(20, 20);

	mediaLayout->addStretch();
	mediaLayout->addWidget(githubLabel);
	mediaLayout->addWidget(bbLabel);
	mediaLayout->addWidget(emailLabel);
	mediaLayout->addStretch();
	//-----------------------

	//---------谚语----------
	auto sayLayout = new QHBoxLayout;
	auto sayLabel = new QLabel("奔赴山海，保持热爱");
	sayLabel->setObjectName("sayLabel");
	sayLabel->setAlignment(Qt::AlignCenter);
	
	sayLayout->addStretch();
	sayLayout->addWidget(sayLabel);
	sayLayout->addStretch();
	//------------------------

	main_layout->addLayout(topLayout);
	main_layout->addSpacing(20);
	main_layout->addLayout(mediaLayout);
	main_layout->addSpacing(20);
	main_layout->addLayout(sayLayout);

	this->setLayout(main_layout);
}

