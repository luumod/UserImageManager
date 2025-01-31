#include "SDisplayImageWidget.h"
#include "SImage.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>

SDisplayImageWidget::SDisplayImageWidget(int image_id,const QString& imagePath, QWidget* parent)
	: QWidget(parent)
	, m_imagePath(imagePath)
{
	init();
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
	m_pixmapLab->setAlignment(Qt::AlignCenter);
	QPainterPath path;
	path.addRoundedRect(m_pixmapLab->rect(), 30, 30);
	m_pixmapLab->setMask(QRegion(path.toFillPolygon().toPolygon()));
	//---------------------------

	//---------图片信息----------
	auto infoLayout = new QHBoxLayout;
	infoLayout->setContentsMargins(10,5,10,5);
	auto nameLab = new QLabel(QString("Candi Pidun"));
	nameLab->setObjectName("nameLab");
	nameLab->setAlignment(Qt::AlignCenter);

	m_heatLab = new QLabel(QString("♨️112"));
	m_heatLab->setAlignment(Qt::AlignCenter);

	infoLayout->addSpacing(10);
	infoLayout->addWidget(nameLab);
	infoLayout->addStretch();
	infoLayout->addWidget(m_heatLab);
	infoLayout->addSpacing(5);
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

void SDisplayImageWidget::setData(const QString& path,int heat)
{
	m_imagePath = path;
	setCursor(Qt::PointingHandCursor);
	m_heatLab->setText(QString("♨️%1").arg(heat));

	//异步加载图片
	SImage::loadAndCropImage(m_imagePath, m_pixmapLab);
}

void SDisplayImageWidget::mousePressEvent(QMouseEvent* event)
{
	//鼠标左键双击
	if (event->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonDblClick) {
		emit doubleClickedToOpenImageDetail(m_imagePath);
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
