#include "RoundedImageWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>

RoundedImageWidget::RoundedImageWidget(int id, const QString& filePath, QWidget* parent)
	: QWidget(parent)
	, m_id(id)
	, m_filePath(filePath)
{
	init();
	setImagePath(filePath);
	setMouseTracking(true);
}

RoundedImageWidget::RoundedImageWidget(int id, QWidget* parent)
	:RoundedImageWidget(id, QString(), parent) {
}

RoundedImageWidget::RoundedImageWidget(QWidget* parent)
	:RoundedImageWidget(0, parent)
{
}

RoundedImageWidget::~RoundedImageWidget()
{	
}

void RoundedImageWidget::init()
{
	auto mainLayout = new QVBoxLayout;

	
	m_pixmapLab = new QLabel;
	m_pixmapLab->setMargin(0);
	m_pixmapLab->setFixedSize(250, 220);
	QPainterPath path;
	path.addRoundedRect(m_pixmapLab->rect(), 30, 30);
	m_pixmapLab->setMask(QRegion(path.toFillPolygon().toPolygon()));
	m_pixmapLab->setScaledContents(true);

	mainLayout->addWidget(m_pixmapLab);

	this->setLayout(mainLayout);
}

void RoundedImageWidget::setStyle()
{
}

bool RoundedImageWidget::isNull() const
{
	return m_filePath.isNull();
}

void RoundedImageWidget::setImagePath(const QString& filePath)
{
	if (filePath.isEmpty()) { //仅当有图片时，光标才会变成手型
		setCursor(Qt::ArrowCursor);
		m_pixmapLab->clear();
		return;
	}
	setCursor(Qt::PointingHandCursor);
	m_filePath = filePath;
	m_pixmapLab->setPixmap(QPixmap(m_filePath));
}

void RoundedImageWidget::mouseMoveEvent(QMouseEvent* event)
{	
	QWidget::mouseMoveEvent(event);
}

void RoundedImageWidget::mousePressEvent(QMouseEvent* event)
{
	//鼠标左键点击事件
	if (event->button() == Qt::LeftButton && !m_pixmapLab->pixmap().isNull()) {
		emit clickedImage(m_id);
	}
}