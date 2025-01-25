#include "SImagesJointWidget.h"
#include "SImage.h"
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QFile>
#include <QStylePainter>
#include <QStyleOption>

SImagesJointWidget::SImagesJointWidget(QWidget* parent)
	: QWidget(parent) 
{
	init();
	//this->setMinimumSize(1200, 800);
	//this->setFixedSize(960, 720);
	this->setObjectName("SImagesJointWidget");

	QString qss_file = "F:\\code\\UserImageManager\\test\\homePage\\joint.css";
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
	main_layout->setSpacing(10);
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

	// 图片路径
	QStringList imagePaths = {
		"F:/code/UserImageManager/test/uploadImage/y/动漫/暗夜的神秘力量.png",
		"F:/code/UserImageManager/test/uploadImage/y/动漫/幻彩精灵之韵.png",
		"F:/code/UserImageManager/test/uploadImage/y/动漫/暮色中的遐思.png",
		"F:/code/UserImageManager/test/uploadImage/y/动漫/烟雨江南梦.jpg",
		"F:/code/UserImageManager/test/uploadImage/y/美女/wallhaven-5g2d95_400x800.png",
		"F:/code/UserImageManager/test/uploadImage/y/美女/wallhaven-jxme8q_500x1000.png",
		"F:/code/UserImageManager/test/uploadImage/y/动漫/时尚先锋魅力.png",
	};

	//----------------------------------
	QGridLayout* grid_layout = new QGridLayout;
	grid_layout->setContentsMargins(0,0,0,0);
	//grid_layout->setSpacing(20);

	auto lab1 = new QLabel;
	lab1->setPixmap(QPixmap(imagePaths[0]));

	auto lab2 = new QLabel;
	lab2->setPixmap(QPixmap(imagePaths[1]));

	auto lab3 = new QLabel;
	lab3->setPixmap(QPixmap(imagePaths[2]));

	auto lab4 = new QLabel;
	lab4->setPixmap(QPixmap(imagePaths[3]));

	auto lab5 = new QLabel;
	lab5->setPixmap(QPixmap(imagePaths[4]));

	auto lab6 = new QLabel;
	lab6->setPixmap(QPixmap(imagePaths[5]));

	auto lab7 = new QLabel;
	lab7->setPixmap(QPixmap(imagePaths[6]));

	grid_layout->addWidget(lab1, 0, 0, 5, 6);
	grid_layout->addWidget(lab2, 0, 6, 3, 3);
	grid_layout->addWidget(lab3, 0, 9, 3, 3);
	grid_layout->addWidget(lab4, 3, 6, 2, 6);
	grid_layout->addWidget(lab5, 5, 0, 4, 4);
	grid_layout->addWidget(lab6, 5, 4, 4, 4);
	grid_layout->addWidget(lab7, 5, 8, 4, 4);

	SImage::loadAndCropImage(imagePaths[0], lab1);
	SImage::loadAndCropImage(imagePaths[1], lab2);
	SImage::loadAndCropImage(imagePaths[2], lab3);
	SImage::loadAndCropImage(imagePaths[3], lab4);
	SImage::loadAndCropImage(imagePaths[4], lab5);
	SImage::loadAndCropImage(imagePaths[5], lab6);
	SImage::loadAndCropImage(imagePaths[6], lab7);
	//-----------------------------------

	main_layout->addLayout(title_layout);
	main_layout->addLayout(grid_layout);
	this->setLayout(main_layout);


	grid_layout->update();
	title_layout->update();
	main_layout->update();
	layout()->activate();
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