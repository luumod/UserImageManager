#include "SDisplayImageWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>

SDisplayImageWidget::SDisplayImageWidget(int image_id,const QString& imagePath, QWidget* parent)
	: QWidget(parent)
	, m_image_id(image_id)
	, m_imagePath(imagePath)
{
	init();
	setImagePath(imagePath);
	setMouseTracking(true);

	this->setStyleSheet("QLabel#nameLab{font-size:13px;font-family: 微软雅黑; font-weight: 550;}");
}

SDisplayImageWidget::SDisplayImageWidget(int image_id,QWidget* parent)
	:SDisplayImageWidget(image_id, QString(), parent) {
}

SDisplayImageWidget::SDisplayImageWidget(QWidget* parent)
	: SDisplayImageWidget(0, QString(), parent)
{
}

SDisplayImageWidget::~SDisplayImageWidget()
{	
}

void SDisplayImageWidget::init()
{
	auto mainLayout = new QVBoxLayout;
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	
	//--------图片显示-------------
	m_pixmapLab = new QLabel;
	m_pixmapLab->setMargin(0);
	m_pixmapLab->setFixedSize(250, 220);
	QPainterPath path;
	path.addRoundedRect(m_pixmapLab->rect(), 30, 30);
	m_pixmapLab->setMask(QRegion(path.toFillPolygon().toPolygon()));
	m_pixmapLab->setScaledContents(true);
	//---------------------------

	//---------图片信息----------
	auto infoLayout = new QHBoxLayout;
	infoLayout->setContentsMargins(10,5,10,5);
	auto nameLab = new QLabel(QString("Candi Pidun"));
	nameLab->setObjectName("nameLab");
	nameLab->setAlignment(Qt::AlignCenter);

	auto likeLab = new QLabel(QString("❤112"));
	likeLab->setAlignment(Qt::AlignCenter);

	auto starLab = new QLabel(QString("⭐43"));
	nameLab->setAlignment(Qt::AlignCenter);

	infoLayout->addSpacing(10);
	infoLayout->addWidget(nameLab);
	infoLayout->addStretch();
	infoLayout->addWidget(likeLab);
	infoLayout->addSpacing(5);
	infoLayout->addWidget(starLab);
	infoLayout->addSpacing(10);
	//---------------------------

	mainLayout->addStretch();
	mainLayout->addWidget(m_pixmapLab);
	mainLayout->addLayout(infoLayout);
	mainLayout->addStretch();

	this->setLayout(mainLayout);
}

bool SDisplayImageWidget::isNull() const
{
	return m_imagePath.isNull();
}

void SDisplayImageWidget::setImagePath(const QString& filePath)
{
	if (filePath.isEmpty()) { //仅当有图片时，光标才会变成手型
		setCursor(Qt::ArrowCursor);
		m_pixmapLab->clear();
		return;
	}
	setCursor(Qt::PointingHandCursor);
	m_imagePath = filePath;
	QPixmap pixmap(m_imagePath);
	pixmap.scaled(m_pixmapLab->size(), Qt::KeepAspectRatio);
	m_pixmapLab->setPixmap(pixmap);
}

void SDisplayImageWidget::mousePressEvent(QMouseEvent* event)
{
	//鼠标左键点击事件
	if (event->button() == Qt::LeftButton && !m_pixmapLab->pixmap().isNull()) {
		emit clickedImage(m_image_id);
	}
	QWidget::mousePressEvent(event);
}

void SDisplayImageWidget::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);
}

void SDisplayImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);
}
