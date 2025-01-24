#include "SSliderBarItem.h"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QGraphicsColorizeEffect>

SSliderBarItem::SSliderBarItem(int id, const QString& iconPath, const QString& title, QWidget* parent)
	:QWidget(parent)
	,m_id(id)
	,m_iconPath(iconPath)
	,m_title(title)
{
	init();
	this->setFixedSize(200, 60);

	this->setStyleSheet("\
		QLabel#lab_title{font-size:14px; font-weight:bold;}\
		QLabel#lab_title:hover{color:rgb(67,176,42);}");
}

SSliderBarItem::~SSliderBarItem()
{
}

void SSliderBarItem::init()
{
	auto main_layout = new QHBoxLayout;
	main_layout->setContentsMargins(30, 5, 0, 5);

	m_labIcon = new QLabel;
	m_labIcon->setFixedSize(20, 20);
	m_labIcon->setPixmap(QPixmap(m_iconPath));
	m_labIcon->setScaledContents(true);

	auto colorizeEffect = new QGraphicsColorizeEffect;
	colorizeEffect->setColor(Qt::transparent); 
	m_labIcon->setGraphicsEffect(colorizeEffect);

	m_labTitle = new QLabel;
	m_labTitle->setObjectName("lab_title");
	m_labTitle->setText(m_title);

	main_layout->addWidget(m_labIcon);
	main_layout->addWidget(m_labTitle);


	this->setLayout(main_layout);
}


void SSliderBarItem::enterEvent(QEnterEvent* event)
{
	QPalette palette;
	palette.setColor(QPalette::Window, QColor(245, 245, 245));
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	if (m_labTitle) {
		m_labTitle->setText(QString("<u>%1</u>").arg(m_title));
	}
	if (m_labIcon) {
		auto colorizeEffect = static_cast<QGraphicsColorizeEffect*>(m_labIcon->graphicsEffect());
		if (colorizeEffect) {
			colorizeEffect->setColor(QColor(67, 176, 42));
		}
	}
	QWidget::enterEvent(event);
}

void SSliderBarItem::leaveEvent(QEvent* event)
{
	QPalette defaultPalette;
	this->setPalette(defaultPalette);
	this->setAutoFillBackground(false);

	if (m_labTitle) {
		m_labTitle->setText(m_title);
	}
	if (m_labIcon) {
		auto colorizeEffect = static_cast<QGraphicsColorizeEffect*>(m_labIcon->graphicsEffect());
		if (colorizeEffect) {
			colorizeEffect->setColor(Qt::black);
		}
	}
	QWidget::leaveEvent(event);
}

void SSliderBarItem::mousePressEvent(QMouseEvent* event)
{
	emit clickedOnePage(m_id);
}
