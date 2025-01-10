#include "SImageShowWidget.h"
#include "RoundedImageWidget.h"
#include <QGridLayout>
#include <QPainter>

SImageShowWidget::SImageShowWidget(int id,const QString& path, const QString& name, const QString& desc, QWidget* parent)
	: QWidget(parent)
	, m_id(id)
	, m_imagePath(path)
	, m_imageName(name)
	, m_imageDesc(desc)
{
	setMouseTracking(true);
	this->setObjectName("SImageShowWidget");
	init(); 
	this->update();

}

SImageShowWidget::SImageShowWidget(int id,QWidget* parent)
	:SImageShowWidget(id, "", "", "", parent) {
}

SImageShowWidget::~SImageShowWidget()
{
}

void SImageShowWidget::init()
{
	//左边
	m_wid_labImage = new RoundedImageWidget(m_id);
	if (!m_imagePath.isEmpty()){
		m_wid_labImage->setPixmap(m_imagePath);
	}
	m_wid_labImage->update();

	//标题
	m_lab_imageName = new QLabel(m_imageName);

	//内容
	m_area_textArea = new QTextEdit;
	m_area_textArea->setReadOnly(true);
	m_area_textArea->setPlainText(m_imageDesc);
	
	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(m_wid_labImage, 0, 0, 3, 3);
	layout->addWidget(m_lab_imageName, 0, 3, 1, 7);
	layout->addWidget(m_area_textArea, 1, 3, 2, 7);
	layout->setHorizontalSpacing(0);
	layout->setVerticalSpacing(0);
	layout->setContentsMargins(0, 5, 0, 5);
}

void SImageShowWidget::loadImage(const QString& path, const QString& name, const QString& desc)
{
	m_imagePath = path;
	m_imageName = name;
	m_imageDesc = desc;
	m_wid_labImage->setPixmap(m_imagePath);
	m_lab_imageName->setText(m_imageName);
	m_area_textArea->setPlainText(m_imageDesc);

	update();
}

void SImageShowWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
}
