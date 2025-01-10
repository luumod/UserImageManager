#include "RoundedImageWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

RoundedImageWidget::RoundedImageWidget(int id,const QPixmap& pixmap, QWidget* parent)
	: QWidget(parent)
	, m_id(id)
	, m_pixmap(pixmap)
{
	setMouseTracking(true);
}

RoundedImageWidget::RoundedImageWidget(int id, QWidget* parent)
	:RoundedImageWidget(id, QPixmap(QString()), parent) {
}

RoundedImageWidget::RoundedImageWidget(QWidget* parent)
	:RoundedImageWidget(0, parent)
{
}

RoundedImageWidget::~RoundedImageWidget()
{	
}

void RoundedImageWidget::setPixmap(const QString& path)
{
	m_pixmap = QPixmap(path);
	update();
}

bool RoundedImageWidget::isNull() const
{
	return m_pixmap.isNull();
}

void RoundedImageWidget::mouseMoveEvent(QMouseEvent* event)
{
	//鼠标光标变样式
	setCursor(Qt::PointingHandCursor);
  
	QWidget::mouseMoveEvent(event);
}

void RoundedImageWidget::mousePressEvent(QMouseEvent* event)
{
	//鼠标左键点击事件
	
	if (event->button() == Qt::LeftButton) {
		emit clickedImage(m_id);
	}
}

void RoundedImageWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QPainterPath path;
	path.addRoundedRect(rect(), 20,20);
	painter.setClipPath(path);
	if (!m_pixmap.isNull()) {
		painter.drawPixmap(rect(), m_pixmap);
	}
}