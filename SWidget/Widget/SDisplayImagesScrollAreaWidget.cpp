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
	scrollArea->setFrameShape(QFrame::NoFrame); // 无边框
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 不显示水平滚动条
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 不显示垂直滚动条
	scrollArea->setWidgetResizable(true);

	auto content_widget = new QWidget;
	QHBoxLayout* carouselLayout = new QHBoxLayout;
	carouselLayout->setContentsMargins(0, 0, 0, 0);
	carouselLayout->setSpacing(0);
	SDisplayImageWidget* imageLabel = nullptr;
	for (int i = 1; i <= MAX_SHOW_IMAGE_COUNT; ++i) {
		 imageLabel = new SDisplayImageWidget;
		imageLabel->setImagePath(QString("F:\\code\\GP\\homepage_topImages\\%1.png").arg(i));
		carouselLayout->addWidget(imageLabel);
		carouselLayout->addSpacing(30);
	}

	for (int i = 1; i <= MAX_SHOW_IMAGE_COUNT; ++i) {
		 imageLabel = new SDisplayImageWidget;
		imageLabel->setImagePath(QString("F:\\code\\GP\\homepage_topImages\\%1.png").arg(i));
		carouselLayout->addWidget(imageLabel);
		carouselLayout->addSpacing(30);
	}

	for (int i = 1; i <= MAX_SHOW_IMAGE_COUNT; ++i) {
		 imageLabel = new SDisplayImageWidget;
		imageLabel->setImagePath(QString("F:\\code\\GP\\homepage_topImages\\%1.png").arg(i));
		carouselLayout->addWidget(imageLabel);
		carouselLayout->addSpacing(30);
	}
	content_widget->setLayout(carouselLayout);
	scrollArea->setWidget(content_widget);
	// ---------------------------------------------
	content_layout->addLayout(title_layout); //上
	content_layout->addWidget(scrollArea);   //下

	this->setLayout(content_layout);

	content_widget->update();
	scrollArea->update();

	// 初始化时将滚动条设置到中间位置
    QTimer::singleShot(100, this, [=]() {
        auto hScrollBar = scrollArea->horizontalScrollBar();
        hScrollBar->setValue(hScrollBar->maximum() / 2);
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
	// 鼠标移动时，计算滑动量并调整位置
	if (dragging) {
		int deltaX = event->pos().x() - lastMousePos.x();
		lastMousePos = event->pos();
		if (scrollArea) {
            auto hScrollBar = scrollArea->horizontalScrollBar();
            hScrollBar->setValue(hScrollBar->value() - deltaX);


            // 检查是否到达末尾并重置到中间位置
            if (hScrollBar->value() == hScrollBar->maximum()) {
                hScrollBar->setValue(hScrollBar->maximum() / 2);
            } else if (hScrollBar->value() == hScrollBar->minimum()) {
                hScrollBar->setValue(hScrollBar->maximum() / 2);
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