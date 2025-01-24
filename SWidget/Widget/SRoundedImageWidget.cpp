#include "SRoundedImageWidget.h"
#include "SImageShowWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>

SRoundedImageWidget::SRoundedImageWidget(int id, const QString& filePath, QWidget* parent)
	: QWidget(parent)
	, m_id(id)
	, m_filePath(filePath)
{
	init();
	setImagePath(filePath);
	setMouseTracking(true);
}

SRoundedImageWidget::SRoundedImageWidget(int id, QWidget* parent)
	:SRoundedImageWidget(id, QString(), parent) {
}

SRoundedImageWidget::SRoundedImageWidget(QWidget* parent)
	:SRoundedImageWidget(0, parent)
{
}

SRoundedImageWidget::~SRoundedImageWidget()
{	
}

void SRoundedImageWidget::init()
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

bool SRoundedImageWidget::isNull() const
{
	return m_filePath.isNull();
}

void SRoundedImageWidget::setImagePath(const QString& filePath)
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

void SRoundedImageWidget::mouseMoveEvent(QMouseEvent* event)
{	
	SImageShowWidget* parent = dynamic_cast<SImageShowWidget*>(parentWidget());
	if (parent) {
		parent->m_shadowEffect->setColor(QColor("#575FDE"));
	}
}

void SRoundedImageWidget::leaveEvent(QEvent* event)
{
	SImageShowWidget* parent = dynamic_cast<SImageShowWidget*>(parentWidget());
	if (parent) {
		parent->m_shadowEffect->setColor(QColor(100, 100, 100));
	}
}

void SRoundedImageWidget::mousePressEvent(QMouseEvent* event)
{
	//鼠标左键点击事件
	if (event->button() == Qt::LeftButton && !m_pixmapLab->pixmap().isNull()) {
		emit clickedImage(m_id);
	}
}