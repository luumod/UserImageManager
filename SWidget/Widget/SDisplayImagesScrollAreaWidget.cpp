#include "SDisplayImagesScrollAreaWidget.h"
#include "SDisplayImageWidget.h"
#include <QScrollArea>
#include <QHBoxLayout>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QTimer>
#include <QIcon>
#include <QScrollBar>
#include <QtConcurrent/QtConcurrent>

SDisplayImagesScrollAreaWidget::SDisplayImagesScrollAreaWidget(QWidget* parent)
	: QWidget(parent) 
{
	init();
	this->setMinimumWidth(1200);
	this->setFixedHeight(290);
	this->setMouseTracking(true); // 开启鼠标跟踪

	startAutoScroll();
}

SDisplayImagesScrollAreaWidget::~SDisplayImagesScrollAreaWidget()
{
	stopScrolling = true;
}

void SDisplayImagesScrollAreaWidget::init()
{
	auto content_layout = new QVBoxLayout;
	content_layout->setContentsMargins(0, 0, 0, 0);
	content_layout->setSpacing(0);

	// -------------- 标题 -------------------------
	auto title_layout = new QHBoxLayout;
	title_layout->setContentsMargins(0, 0, 0, 0);
	auto titleLabel = new QLabel;
	titleLabel->setText("图片展示区");
	titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
	title_layout->addStretch();
	title_layout->addWidget(titleLabel);
	title_layout->addStretch();
	// ---------------------------------------------

	//--------------- 内容 ---------------------
	scrollArea = new QScrollArea;
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setWidgetResizable(false); // 关闭自动调整，确保内容足够宽

	content_widget = new QWidget;
	QHBoxLayout* carouselLayout = new QHBoxLayout;

	// 添加三组相同的图片以实现循环
	for (int group = 0; group < 3; ++group) { // 三组循环
		for (int i = 1; i <= MAX_SHOW_IMAGE_COUNT; ++i) {
			auto imageLabel = new SDisplayImageWidget;
			imageLabel->setImagePath(QString("F:\\code\\GP\\homepage_topImages\\%1.png").arg(i));
			carouselLayout->addWidget(imageLabel);
			carouselLayout->addSpacing(30);
		}
	}

	content_widget->setLayout(carouselLayout);
	content_widget->adjustSize(); // 调整内容大小以正确计算宽度
	scrollArea->setWidget(content_widget);
	// ---------------------------------------------
	content_layout->addLayout(title_layout); //上
	content_layout->addWidget(scrollArea);   //下

	this->setLayout(content_layout);

	content_widget->update();
	scrollArea->update();

	// 初始化时滚动到中间组
	QTimer::singleShot(10, this, [=]() {
		auto hScrollBar = scrollArea->horizontalScrollBar();
		int totalWidth = content_widget->width();
		hScrollBar->setValue(totalWidth / 3); // 定位到中间组起始位置
		});
}

void SDisplayImagesScrollAreaWidget::mousePressEvent(QMouseEvent* event)
{
	// 记录鼠标按下的位置
	if (event->button() == Qt::LeftButton) {
		dragging = true;
		lastMousePos = event->pos();
	}
}

void SDisplayImagesScrollAreaWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (dragging) {
		int deltaX = event->pos().x() - lastMousePos.x();
		lastMousePos = event->pos();
		if (scrollArea) {
			auto hScrollBar = scrollArea->horizontalScrollBar();
			int oldValue = hScrollBar->value();
			hScrollBar->setValue(oldValue - deltaX);

			// 动态调整循环逻辑
			int totalWidth = content_widget->width();
			if (totalWidth <= 0) return; // 避免除零

			int sectionWidth = totalWidth / 3; //每组的宽度
			int currentValue = hScrollBar->value();

			if (currentValue >= 2 * sectionWidth) { //滚动到了C组
				hScrollBar->setValue(currentValue - sectionWidth);
			}
			else if (currentValue < sectionWidth) { //滚动到了A组
				hScrollBar->setValue(currentValue + sectionWidth);
			}
		}
	}
}

void SDisplayImagesScrollAreaWidget::mouseReleaseEvent(QMouseEvent* event)
{
	dragging = false;
}

void  SDisplayImagesScrollAreaWidget::moveLeft() {
	// 左箭头点击事件，向左移动
	scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value() - 300);
}

void  SDisplayImagesScrollAreaWidget::moveRight() {
	// 右箭头点击事件，向右移动
	scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value() + 300);
}

void SDisplayImagesScrollAreaWidget::startAutoScroll()
{
    QtConcurrent::run([=]() {
        while (true) {
            QThread::msleep(50); // 每50毫秒滚动一次
			if (stopScrolling) {
				break;
			}
            if (!QMetaObject::invokeMethod(this, "autoScroll", Qt::QueuedConnection)){
				break;
			}
        }
    });
}

void SDisplayImagesScrollAreaWidget::autoScroll()
{
    auto hScrollBar = scrollArea->horizontalScrollBar();
    int newValue = hScrollBar->value() + 1;
    if (newValue >= hScrollBar->maximum()) {
        newValue = hScrollBar->minimum();
    }
    hScrollBar->setValue(newValue);
}