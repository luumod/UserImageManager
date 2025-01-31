#include "SImagesJointWidget.h"
#include "SImage.h"
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QFile>
#include <QStylePainter>
#include <QList>
#include <QStyleOption>

// 图片路径
QStringList imagePaths = {
	"F:/code/GP/uploadImages/动漫/暗夜的神秘力量.png",
	"F:/code/GP/uploadImages/动漫/幻彩精灵之韵.png",
	"F:/code/GP/uploadImages/动漫/暮色中的遐思.png",
	"F:/code/GP/uploadImages/动漫/烟雨江南梦.jpg",
	"F:/code/GP/uploadImages/风景/wallhaven-3ldlx3_1920x1080.png",
	"F:/code/GP/uploadImages/风景/wallhaven-6dvd3w_1600x1200.png",
	"F:/code/GP/uploadImages/动漫/时尚先锋魅力.png",
};

SImagesJointWidget::SImagesJointWidget(QWidget* parent)
	: QWidget(parent) 
{
	init();
	//this->setMinimumSize(1200, 800);
	//this->setFixedSize(960, 720);
	this->setObjectName("SImagesJointWidget");

	QString qss_file = ":/ResourceClient/joint.css";
	QFile qss(qss_file);
	if (qss.open(QFile::ReadOnly))
	{
		QString qss_style = qss.readAll();
		this->setStyleSheet(qss_style);
	}
}

SImagesJointWidget::~SImagesJointWidget()
{
}

void SImagesJointWidget::init()
{
	auto main_layout = new QVBoxLayout;
	//main_layout->setSpacing(10);
	main_layout->setContentsMargins(10, 10, 10, 10);


	//---------------标题----------------
	auto title_layout = new QHBoxLayout;
	title_layout->setContentsMargins(0, 0, 0, 0);
	auto title = new QLabel("热门图片");
	title->setFixedHeight(50);
	title->setObjectName("title");
	title->setAlignment(Qt::AlignCenter);
	title_layout->addStretch();
	title_layout->addWidget(title);
	title_layout->addStretch();
	//----------------------------------

	

	//----------------------------------
	grid_layout = new QGridLayout;
	grid_layout->setContentsMargins(0,0,0,0);
	//grid_layout->setSpacing(20);

	m_image_labels.resize(7);
	for (int i = 0; i < 7; i++) {
		m_image_labels[i] = new QLabel;
		m_image_labels[i]->setPixmap(QPixmap(imagePaths[i]));
	}

	grid_layout->addWidget(m_image_labels[0], 0, 0, 5, 6);
	grid_layout->addWidget(m_image_labels[1], 0, 6, 3, 3);
	grid_layout->addWidget(m_image_labels[2], 0, 9, 3, 3);
	grid_layout->addWidget(m_image_labels[3], 3, 6, 2, 6);
	grid_layout->addWidget(m_image_labels[4], 5, 0, 4, 4);
	grid_layout->addWidget(m_image_labels[5], 5, 4, 4, 4);
	grid_layout->addWidget(m_image_labels[6], 5, 8, 4, 4);

	for (int i = 0; i < 7; i++) {
		SImage::loadAndCropImage(imagePaths[i], m_image_labels[i],SImage::LoadInWhere::DontClear);
	}
	//-----------------------------------

	main_layout->addLayout(title_layout);
	main_layout->addLayout(grid_layout);

	this->setLayout(main_layout);
}

void SImagesJointWidget::paintEvent(QPaintEvent* event)
{
	QStylePainter painter(this);
	QStyleOption opt;
	opt.initFrom(this);
	opt.rect = rect();
	painter.drawPrimitive(QStyle::PE_Widget, opt);

	QWidget::paintEvent(event);
}

void SImagesJointWidget::resizeEvent(QResizeEvent* event)
{
	grid_layout->update();
	this->update();
	layout()->activate();
	for (int i = 0; i < 7; i++) {
		SImage::loadAndCropImage(imagePaths[i], m_image_labels[i], SImage::LoadInWhere::DontClear);
	}
}
