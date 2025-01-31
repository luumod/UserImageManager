#include "SUserMessageWidget.h"
#include <QBoxLayout>
#include <QLabel>

SUserMessageWidget::SUserMessageWidget(int image_id, QString user_name, QString message, QString dateTime, QWidget* parent)
	: QWidget(parent)
	, m_image_id(image_id)
	, m_user_name(user_name)
	, m_message(message)
	, m_dateTime(dateTime)
{
	init();

	this->setFixedHeight(100);

	this->setStyleSheet("QLabel#user_name_label{font-size:14px;font-weight:bold;}\
		QLabel#message_label{font-size:12px;font-weight:200;}\
		QLabel#time_label{font-size:16px;font-weight:300;color:gray;}");
}

SUserMessageWidget::SUserMessageWidget(QWidget* parent)
	: SUserMessageWidget(-1, "", "", "", parent)
{
}

SUserMessageWidget::~SUserMessageWidget()
{
}

void SUserMessageWidget::init()
{
	auto main_layout = new QHBoxLayout;

	//------------头像-----------
	auto image_layout = new QVBoxLayout;
	auto image_label = new QLabel;
	image_label->setFixedSize(40, 40);
	image_label->setPixmap(QPixmap(":/ResourceClient/dog.png").scaled(image_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	image_label->setMask(QRegion(image_label->rect(), QRegion::Ellipse));

	image_layout->addWidget(image_label);
	image_layout->addStretch();
	//---------------------------
	
	
	//------------消息内容----------
	auto msg_layout = new QVBoxLayout;
	auto user_name_label = new QLabel(m_user_name);
	user_name_label->setObjectName("user_name_label");
	user_name_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	auto message_label = new QLabel(m_message);
	message_label->setObjectName("message_label");
	message_label->setAlignment(Qt::AlignLeft);

	msg_layout->addWidget(user_name_label);
	msg_layout->addWidget(message_label);
	msg_layout->addStretch();
	//----------
	//----------------------------

	//------------时间--------------
	auto time_layout = new QVBoxLayout;
	auto time_label = new QLabel(m_dateTime);
	time_label->setObjectName("time_label");
	time_label->setAlignment(Qt::AlignCenter);

	time_layout->addWidget(time_label);	
	time_layout->addStretch();
	//------------------------------


	main_layout->addLayout(image_layout);
	main_layout->addLayout(msg_layout);
	main_layout->addStretch();
	main_layout->addLayout(time_layout);

	this->setLayout(main_layout);
}
