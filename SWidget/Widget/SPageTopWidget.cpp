#include "SPageTopWidget.h"
#include "sApp.h"
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>

SPageTopWidget::SPageTopWidget(QWidget* parent)
	: QWidget(parent)
{
	init();

	QPalette palette;
	palette.setColor(QPalette::Window, QColor(224, 224, 224));
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	this->setStyleSheet("QLabel#program_name{font-size: 26px; font-weight: bold; color: #000000;}");
}

SPageTopWidget::~SPageTopWidget()
{
}

void SPageTopWidget::init()
{
	auto main_layout = new QHBoxLayout;

	//程序名：用户图像管理系统
	auto program_name = new QLabel("用户图像管理系统");
	program_name->setObjectName("program_name");

	//我的图片总数，总图片数，普通用户/管理员，版本号
	auto my_image_count = new QLabel("我的图片总数：");
	my_image_count->setObjectName("my_image_count");
	auto my_image_count_value = new QLabel(QString::number(sApp->userData("user/has_image_count").toInt()));
	auto total_image_count = new QLabel("全站图片数：");
	auto total_image_count_value = new QLabel(QString::number(sApp->globalConfig()->value("program/total_image_count").toInt()));
	total_image_count->setObjectName("total_image_count");	
	auto user_type = new QLabel("管理员");
	user_type->setObjectName("user_type");
	auto version_number = new QLabel("版本号：V1.0.0");
	version_number->setObjectName("version_number");

	main_layout->addWidget(program_name);
	main_layout->addStretch();
	main_layout->addWidget(my_image_count);
	main_layout->addWidget(my_image_count_value);
	main_layout->addSpacing(20);
	main_layout->addWidget(total_image_count);
	main_layout->addWidget(total_image_count_value);
	main_layout->addSpacing(20);
	main_layout->addWidget(user_type);
	main_layout->addSpacing(20);
	main_layout->addWidget(version_number);

	this->setLayout(main_layout);
}
