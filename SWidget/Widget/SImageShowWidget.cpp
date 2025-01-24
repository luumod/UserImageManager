#include "SImageShowWidget.h"
#include "SRoundedImageWidget.h"
#include <QGridLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

SImageShowWidget::SImageShowWidget(int id,const QString& path, const QString& name, const QString& desc, QWidget* parent)
	: QWidget(parent)
	, m_id(id)
	, m_imagePath(path)
	, m_imageName(name)
	, m_imageDesc(desc)
{
	this->setMaximumSize(750, 250);
	init(); 
	this->setMouseTracking(true);
	//为此窗口的边缘添加阴影
	m_shadowEffect = new QGraphicsDropShadowEffect(this);
	m_shadowEffect->setBlurRadius(30); // 设置模糊半径
	m_shadowEffect->setColor(QColor(100, 100, 100)); // 设置阴影颜色（这里是半透明的黑色）
	m_shadowEffect->setOffset(0,0); // 设置阴影的偏移量
	this->setGraphicsEffect(m_shadowEffect);

	this->setStyleSheet("QLabel#lab_imageName{border-radius:10px; padding:5px; font-family:Roboto; font-size:20px; font: bold;}\
		QLabel#lab_imageDesc{border-radius:20px; padding:5px; font:16px; font-family: 微软雅黑;}");

}

SImageShowWidget::SImageShowWidget(int id,QWidget* parent)
	:SImageShowWidget(id, "", "", "", parent) {
}

SImageShowWidget::~SImageShowWidget()
{
}

void SImageShowWidget::init()
{
	auto main_layout = new QHBoxLayout;
	main_layout->setContentsMargins(0, 0, 0, 0);

	//左边
	m_wid_labImage = new SRoundedImageWidget(m_id, m_imagePath);

	//标题
	m_lab_imageName = new QLabel(m_imageName);
	m_lab_imageName->setText(m_imageName);
	m_lab_imageName->setObjectName("lab_imageName");
	//内容
	m_area_textArea = new QLabel;
	m_area_textArea->setObjectName("lab_imageDesc");
	/*QString style = "<p style=\"text-indent:26px; line-height: 25px; \">" + m_imageDesc + "</p>";
	m_area_textArea->setText(style);*/
	m_area_textArea->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	m_area_textArea->setWordWrap(true);

	QFontMetrics fontWidth(m_area_textArea->font());//得到每个字符的宽度
	QString elideNote = fontWidth.elidedText(m_area_textArea->text(), Qt::ElideRight, m_area_textArea->width() * 2);
	m_area_textArea->setText(elideNote);//显示省略好的字符串

	auto right = new QVBoxLayout;
	right->setSpacing(0);
	right->addWidget(m_lab_imageName,1);
	right->addWidget(m_area_textArea,4);

	auto left = new QVBoxLayout;
	left->addWidget(m_wid_labImage);

	main_layout->addLayout(left,1);
	main_layout->addLayout(right,2);
	main_layout->setSpacing(0);

	this->setLayout(main_layout);
}

void SImageShowWidget::loadImage(const QString& path, const QString& name, const QString& desc)
{
	m_imagePath = path;
	m_imageName = name;
	m_imageDesc = desc;
	m_wid_labImage->setImagePath(m_imagePath);
	m_lab_imageName->setText(m_imageName);
	m_area_textArea->setText(m_imageDesc);
}

void SImageShowWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(236, 235, 236));
	painter.drawRoundedRect(0, 0, this->width(), this->height(), 30, 30);
	painter.end();
}