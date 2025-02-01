#include "SUploadorDragImageWidget.h"
#include <QBoxLayout>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOption>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

SUploadorDragImageWidget::SUploadorDragImageWidget(QWidget* parent)
	: QWidget(parent)
{
	init();
	isMouseInside = false;

	this->setAcceptDrops(true);
	this->setMouseTracking(true);
	this->setObjectName("SUploadorDragImageWidget");

	QPalette palette;
	palette.setColor(QPalette::Window, Qt::yellow);
	this->setPalette(palette);
	this->setAutoFillBackground(true);
	this->setMinimumSize(500, 350);

	//读取css文件
	QFile file(":/ResourceClient/upload.css");
	if (file.open(QIODevice::ReadOnly))
	{	
		QString style = file.readAll();
		this->setStyleSheet(style);
	}
}

SUploadorDragImageWidget::~SUploadorDragImageWidget()
{
}

void SUploadorDragImageWidget::init()
{
	auto main_layout = new QVBoxLayout;
	main_layout->setContentsMargins(0, 0, 0, 0);

	//------------------图标----------------
	auto image_layout = new QHBoxLayout;
	image_layout->setContentsMargins(0, 0, 0, 0);
	auto imageLab = new QLabel;
	imageLab->setPixmap(QPixmap("F:/upload.png"));
	imageLab->setFixedSize(100, 100);
	imageLab->setScaledContents(true);
	image_layout->addStretch();
	image_layout->addWidget(imageLab);
	image_layout->addStretch();
	//---------------------------------------

	//------------------提示----------------
	auto tipLab_layout = new QHBoxLayout;
	tipLab_layout->setContentsMargins(0, 0, 0, 0);
	auto tipLab = new QLabel("Drag and drop image here!");
	tipLab_layout->addStretch();
	tipLab_layout->addWidget(tipLab);
	tipLab_layout->addStretch();
	//-------------------------------------

	//------------------警告----------------
	auto warningLab_layout = new QHBoxLayout;
	warningLab_layout->setContentsMargins(0, 0, 0, 0);
	auto warningLab = new QLabel("Up to 1 image, max 10MB.");
	warningLab_layout->addStretch();
	warningLab_layout->addWidget(warningLab);
	warningLab_layout->addStretch();
	//-------------------------------------

	main_layout->addStretch();
	main_layout->addLayout(image_layout);
	main_layout->addLayout(tipLab_layout);
	main_layout->addLayout(warningLab_layout);
	main_layout->addStretch();
	this->setLayout(main_layout);
}

bool SUploadorDragImageWidget::isImageFile(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	QString suffix = fileInfo.suffix().toLower();
	return (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "bmp" || suffix == "gif");
}


void SUploadorDragImageWidget::paintEvent(QPaintEvent* event)
{
	m_InsideRect = QRect(this->rect().topLeft() + QPoint(20, 20), QSize(this->width() - 40, this->height() - 40));

	QStylePainter painter(this);
	QStyleOption opt;
	opt.initFrom(this);
	opt.rect = rect();
	painter.drawPrimitive(QStyle::PE_Widget, opt);

	painter.setPen(Qt::NoPen); 
	painter.setBrush(QColor("#EEFFE2"));
	painter.drawRoundedRect(this->rect(), 20, 20);

	painter.setPen(QPen(QColor("#000000"), 2, Qt::DashLine)); 
	if (isMouseInside) {
		painter.setBrush(QColor("#FFD8A5"));
	}
	else {
		painter.setBrush(QColor("#FFF7F2"));
	}
	painter.drawRoundedRect(m_InsideRect, 20, 20); // 绘制圆角矩形

	QWidget::paintEvent(event);
}

void SUploadorDragImageWidget::mouseMoveEvent(QMouseEvent* event) {
	
	if (isDraging) {
		return;
	}
	isMouseInside = m_InsideRect.contains(event->pos());
	if (isMouseInside) {
		setCursor(Qt::OpenHandCursor);
	}
	else {
		setCursor(Qt::ArrowCursor);
	}
	update();
	QWidget::mouseMoveEvent(event);
}

void SUploadorDragImageWidget::leaveEvent(QEvent* event)
{
	qDebug()<<"leave";
	isMouseInside = false;
	update();

	QWidget::leaveEvent(event);
}

void SUploadorDragImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (!m_InsideRect.contains(event->pos())) {
		QWidget::mouseReleaseEvent(event);
		return;
	}

	//打开文件选择框
	if (event->button() == Qt::LeftButton) {
		emit openFileDialog();
	}
	QWidget::mouseReleaseEvent(event);
}

void SUploadorDragImageWidget::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls()) {
		// 检查是否为图片文件
		const QList<QUrl> urls = event->mimeData()->urls();
		for (const QUrl& url : urls) {
			qDebug() << url.toLocalFile();
		}

		if (!urls.isEmpty()) {
			QString filePath = urls.first().toLocalFile();
			if (isImageFile(filePath)) {
				event->acceptProposedAction();
			}
		}
	}
}

void SUploadorDragImageWidget::dropEvent(QDropEvent* event)
{
	const QList<QUrl> urls = event->mimeData()->urls();
	if (!urls.isEmpty()) {
		QString filePath = urls.first().toLocalFile(); //单张
		emit dragImageFile(filePath);
	}
}
