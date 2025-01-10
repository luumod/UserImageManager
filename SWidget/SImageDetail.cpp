#include "SImageDetail.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QPushButton>
#include <QFile>
#include <QDir>

SImageDetailDlg::SImageDetailDlg(QWidget* parent)
	:QWidget(parent)
{
	init();
}

void SImageDetailDlg::init()
{
	setStyleSheet("background-color:white");
	setAttribute(Qt::WA_StyledBackground);

	QFont nameFont = QFont("Corbel", 12);

	auto mlayout = new QVBoxLayout;

	//图片
	m_imageLabel = new QLabel;
	m_imageLabel->setFixedSize(400, 400);
	m_imageLabel->setScaledContents(true);

	//---右侧布局
	auto hlayout0 = new QHBoxLayout;
	m_backBtn = new QPushButton("返回");
	hlayout0->addWidget(m_backBtn,1);
	hlayout0->addStretch(4);

	auto hlayout1 = new QHBoxLayout;
	auto flayout1 = new QFormLayout;
	m_imageName = new QLineEdit;
	m_imageName->setReadOnly(true);
	flayout1->addRow(new QLabel("图片名称："), m_imageName);
	//hlayout1->addStretch();
	hlayout1->addLayout(flayout1);
	hlayout1->addStretch();

	auto hlayout2 = new QHBoxLayout;
	auto flayout2 = new QFormLayout;
	m_imageSize = new QLineEdit;
	m_imageSize->setReadOnly(true);
	flayout2->addRow(new QLabel("图片尺寸："), m_imageSize);
	//hlayout2->addStretch();
	hlayout2->addLayout(flayout2);
	hlayout2->addStretch();

	auto hlayout3 = new QHBoxLayout;
	auto flayout3 = new QFormLayout;
	m_imageType = new QLineEdit;
	m_imageType->setReadOnly(true);
	flayout3->addRow(new QLabel("图片类型："), m_imageType);
	hlayout3->addLayout(flayout3);
	hlayout3->addStretch();

	auto hlayout4 = new QHBoxLayout;
	auto flayout4 = new QFormLayout;
	m_imageOwer = new QLineEdit;
	m_imageOwer->setReadOnly(true);
	flayout4->addRow(new QLabel("图片归属："), m_imageOwer);
	hlayout4->addLayout(flayout4);
	hlayout4->addStretch();

	auto hlayout5 = new QHBoxLayout;
	auto flayout5 = new QFormLayout;
	m_imageDesc = new QTextEdit;
	m_imageDesc->setReadOnly(true);
	flayout5->addRow(new QLabel("图片介绍："), m_imageDesc);
	hlayout5->addLayout(flayout5);
	hlayout5->addStretch();

	auto v_rightLayout = new QVBoxLayout;
	v_rightLayout->addLayout(hlayout0);
	v_rightLayout->addLayout(hlayout1);
	v_rightLayout->addLayout(hlayout2);
	v_rightLayout->addLayout(hlayout3);
	v_rightLayout->addLayout(hlayout4);
	v_rightLayout->addLayout(hlayout5);


	auto h_firstlayout = new QHBoxLayout;

	//左侧图片 + 右侧信息
	h_firstlayout->addWidget(m_imageLabel);
	h_firstlayout->addLayout(v_rightLayout);


	//主体布局
	mlayout->addLayout(h_firstlayout);


	this->setLayout(mlayout);


	connect(m_backBtn, &QPushButton::clicked, this, &SImageDetailDlg::close);
}

void SImageDetailDlg::setData(ImageInfo info)
{
	m_imageInfo = info;

	updateUi();
}

void SImageDetailDlg::updateUi()
{
	//获取图片
	auto path = QDir::currentPath() + "/" + m_imageInfo.image_path;
	QFile imageFile(path);
	if (!imageFile.open(QIODevice::ReadOnly)) {
		qWarning() << "Failed to open image file:" << imageFile.errorString();
		return;
	}

	//图片名称
	m_imageName->setText(m_imageInfo.image_name);
	//图片尺寸
	m_imageSize->setText(QString::number(imageFile.size() / 1024.0 / 1024.0) + "MB");
	//图片类型
	m_imageType->setText(QFileInfo(imageFile).suffix());
	//图片归属
	m_imageOwer->setText(sApp->userData("user/user_name").toString());
	//图片介绍
	m_imageDesc->setText(m_imageInfo.image_desc);
	//加载图片
	m_imageLabel->setPixmap(QPixmap::fromImage(QImage(imageFile.fileName())));
}
