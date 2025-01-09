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

RoundedImageWidget::RoundedImageWidget(int w, int h, QWidget* parent)
    : QWidget(parent)
    , m_w(w)
    , m_h(h)
{
    m_pixmap = QPixmap(""); //空
}

RoundedImageWidget::~RoundedImageWidget()
{	
}

void RoundedImageWidget::setPixmap(const QString& path)
{
    m_pixmap = QPixmap(path);
    update();
}

void RoundedImageWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect(), 20,20);
    painter.setClipPath(path);
    if (!m_pixmap.isNull()) {
        painter.drawPixmap(rect(), m_pixmap);
    }
}