#include "STopBar.h"
#include <QHBoxLayout>

STopBar::STopBar(QWidget* parent)
	: QWidget(parent)
{
	init();
}

STopBar::~STopBar()
{
}

void STopBar::init()
{
	initTitle();
}

void STopBar::initTitle()
{
	QHBoxLayout* main_layout = new QHBoxLayout(this);
	title = new QLabel(this);
	title->setText("用户图像管理系统");
	//直接写css格式
	title->setStyleSheet("QLabel{font: normal 16pt '微软雅黑'; color:black;}");
	main_layout->addWidget(title);
	main_layout->addStretch(1);
}
