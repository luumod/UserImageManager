#include "HomePage.h"
#include "SImageShowWidget.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QGridLayout>
#include <QStackedWidget>
#include <QDir>

HomePage::HomePage(QWidget *parent) 
	: QWidget(parent)
{
	this->setContentsMargins(0, 0, 0, 0);
	init();

}

HomePage::~HomePage()
{

}

void HomePage::init()
{	
	QLabel *titleLabel = new QLabel(tr("首页"));
	this->setLayout(new QHBoxLayout());
	auto mlayout =  this->layout();
	mlayout->addWidget(titleLabel);
	mlayout->setAlignment(Qt::AlignCenter);
}