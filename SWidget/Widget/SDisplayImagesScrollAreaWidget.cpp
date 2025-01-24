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

SDisplayImagesScrollAreaWidget::SDisplayImagesScrollAreaWidget(QWidget* parent)
	: QWidget(parent) 
{
	init();
	this->setMinimumWidth(1200);
	this->setFixedHeight(290);
	this->setMouseTracking(true); // 开启鼠标跟踪
}

SDisplayImagesScrollAreaWidget::~SDisplayImagesScrollAreaWidget()
{
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
	for (int i = 1; i <= 10; ++i) {
		auto imageLabel = new SDisplayImageWidget;
		imageLabel->setImagePath(QString("F:\\code\\UserImageManager\\test\\homePage\\%1.png").arg(i));
		carouselLayout->addWidget(imageLabel);
		carouselLayout->addSpacing(30);
	}
	content_widget->setLayout(carouselLayout);
	scrollArea->setWidget(content_widget);
	// ---------------------------------------------
	content_layout->addLayout(title_layout); //上
	content_layout->addWidget(scrollArea);   //下

	this->setLayout(content_layout);
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
			scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value() - deltaX);
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
	qDebug() << "moveLeft: value=" << scrollArea->horizontalScrollBar()->value() << '\n';
}

void  SDisplayImagesScrollAreaWidget::moveRight() {
	// 右箭头点击事件，向右移动
	scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value() + 300);
	qDebug() << "moveLeft: value=" << scrollArea->horizontalScrollBar()->value() << '\n';
}
