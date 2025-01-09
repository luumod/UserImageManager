#include "SIODialog.h"
#include "SStandardPixmap.h"
#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>
#include <QApplication>

SIODialog::SIODialog(QWidget* parent)
	:QWidget(parent)
	,m_centraWidget(new QWidget)
{
	init();
	setTitle(QApplication::applicationDisplayName());
}

void SIODialog::setTitle(const QString& title)
{
	m_titleLab->setText(title);
}

QString SIODialog::title() const
{
	return m_titleLab->text();
}

QWidget* SIODialog::centralWidget() const
{
	return m_centraWidget;
}

void SIODialog::init()
{
	setStyleSheet("background-color:white");
	setAttribute(Qt::WA_StyledBackground);

	m_titleLab = new QLabel;
	m_closeBtn = new QPushButton;
	
	m_titleLab->setStyleSheet("font:bold 13px");
	m_closeBtn->setStyleSheet("border:none");
	m_closeBtn->setIcon(SStandardPixmap::pixmap(SStandardPixmap::SP_TitleBarCloseButton));
	connect(m_closeBtn, &QPushButton::clicked, this, &QWidget::close);

	QWidget* titleBar = new QWidget;
	titleBar->setStyleSheet(".QWidget{ border-bottom:1px solid rgb(194,195,201)}");
	auto titleLayout = new QHBoxLayout(titleBar);
	titleLayout->addWidget(m_titleLab);
	titleLayout->addStretch();
	titleLayout->addWidget(m_closeBtn);

	auto vlayout = new QVBoxLayout(this);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->addWidget(titleBar,0,Qt::AlignTop);
	vlayout->addWidget(centralWidget());

}
