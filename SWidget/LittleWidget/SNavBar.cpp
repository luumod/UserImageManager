#include "SNavBar.h"
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include <QBoxLayout>

SNavBar::SNavBar(QWidget* parent)
	:QWidget(parent)
{
	init();
}

void SNavBar::addNav(const QString& text, int id)
{
	addNav(QIcon(), text, id);
}

void SNavBar::addNav(const QIcon& icon, const QString& text, int id)
{
	if (id == -1)
		id = m_lastId++;
	auto btn = new QPushButton(icon,text);
	btn->setFixedHeight(30);
	btn->setCheckable(true);
	m_buttonGroup->addButton(btn, id);
	m_vlayout->insertWidget(m_vlayout->count() - 1, btn);
}

void SNavBar::setNavHeader(const QString& text)
{
	m_header->setText(text);
}

void SNavBar::init()
{
	m_buttonGroup = new QButtonGroup(this);
	m_header = new QLabel("标题");
	m_vlayout = new QVBoxLayout(this);

	m_vlayout->setContentsMargins(0, 0, 0, 0);
	m_vlayout->setSpacing(0);
	m_vlayout->addWidget(m_header);
	m_vlayout->addStretch();

	m_buttonGroup->setExclusive(true);

	m_header->setAlignment(Qt::AlignCenter);
	auto fnt = m_header->font();
	fnt.setPointSize(11);
	m_header->setFont(fnt);

	setAttribute(Qt::WA_StyledBackground);
	setStyleSheet(R"(QWidget{background-color:rgb(57,61,72)}QPushButton,QLabel{border:none;color:white}
QPushButton:checked{background-color:rgb(81,177,249)}QPushButton:hover{background-color:rgb(64,210,186)})");


	connect(m_buttonGroup, &QButtonGroup::idClicked,this, &SNavBar::idClicked);
	connect(m_buttonGroup, &QButtonGroup::buttonClicked,this, &SNavBar::buttonClicked);

}
