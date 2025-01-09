#include "RoundedImageWidget.h"
#include <QPainter>
#include <QPainterPath>

RoundedImageWidget::RoundedImageWidget(const QPixmap& pixmap,int w, int h, QWidget* parent)
	: QWidget(parent)
    , m_pixmap(pixmap)
    , m_w(w)
    , m_h(h)
{
}

RoundedImageWidget::~RoundedImageWidget()
{	
}

void RoundedImageWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect(), 20,20);
    painter.setClipPath(path);
    painter.drawPixmap(rect(), m_pixmap);
}