#include "SSliderBar.h"
#include "SSliderBarItem.h"
#include "SSliderBarBottomInfoItem.h"
#include "SApp.h"
#include <QBoxLayout>

SSliderBar::SSliderBar(QWidget* parent)
{
	init();
	this->setFixedWidth(200);

	QPalette palette;
	palette.setColor(QPalette::Window, QColor(224, 224, 224));
	this->setPalette(palette);
	this->setAutoFillBackground(true);
}

SSliderBar::~SSliderBar()
{
}

void SSliderBar::init()
{
	auto main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);

	m_sliderBarItems.append(new SSliderBarItem(0,":/ResourceClient/home.png","首页"));
	m_sliderBarItems.append(new SSliderBarItem(1, ":/ResourceClient/upload.png", "上传图片"));
	m_sliderBarItems.append(new SSliderBarItem(2,":/ResourceClient/cangku.png","我的图片"));
	m_sliderBarItems.append(new SSliderBarItem(3,":/ResourceClient/imageManager.png","图片管理"));
	m_sliderBarItems.append(new SSliderBarItem(4, ":/ResourceClient/jiagong.png", "用户加工"));
	m_sliderBarItems.append(new SSliderBarItem(5, ":/ResourceClient/community.png", "共享社区"));
	m_sliderBarItems.append(new SSliderBarItem(6, ":/ResourceClient/userManager.png", "用户管理"));
	m_sliderBarItems.append(new SSliderBarItem(7, ":/ResourceClient/user.png", "个人信息"));
	//TODO：添加更多的item

	for (const auto& item : m_sliderBarItems) {
		main_layout->addWidget(item);
		connect(item, &SSliderBarItem::clickedOnePage, this, [=](int id) {
			emit clickedOneItem(id);
			});
	}
	main_layout->addStretch();

	auto bottom_info = new SSliderBarBottomInfoItem("Yuleo", "编程爱好者\n互联网从业者\n知识分享博主", sApp->userData("user/avatar_path").toString());
	main_layout->addWidget(bottom_info);

	this->setLayout(main_layout);
	
}
