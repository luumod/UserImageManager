#include "SUserMessageWidget.h"
#include <QBoxLayout>
#include <QLabel>

SUserMessageWidget::SUserMessageWidget(QWidget* parent)
	: SUserMessageWidget("0", "用户活动", "暂无活动！", "2021-01-01 12:00:00", parent)
{
}

SUserMessageWidget::SUserMessageWidget(const QString& avator_path, const QString& user_name, const QString& message, const QString& dateTime, QWidget* parent)
	: QWidget(parent)
	, m_avatar_path(avator_path)
	, m_user_name(user_name)
	, m_message(message)
	, m_dateTime(dateTime)
{
	init();

	this->setFixedHeight(100);
	this->setStyleSheet("QLabel#user_name_label{font-size:14px;font-weight:bold;}\
		QLabel#message_label{font-size:13px;font-weight: 300;}\
		QLabel#time_label{font-size:16px;font-weight:300;color:gray;}");
}

SUserMessageWidget::~SUserMessageWidget()
{
}

void SUserMessageWidget::init()
{
	auto main_layout = new QHBoxLayout;

	//------------头像-----------
	auto image_layout = new QVBoxLayout;
	m_image_label = new QLabel;
	m_image_label->setFixedSize(40, 40);
	if (m_avatar_path == "" || m_avatar_path == "0" || m_avatar_path.isEmpty()) {
		m_avatar_path = ":/ResourceClient/dog.png";
	}
	m_image_label->setPixmap(QPixmap(m_avatar_path).scaled(m_image_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	m_image_label->setMask(QRegion(m_image_label->rect(), QRegion::Ellipse));

	image_layout->addWidget(m_image_label);
	image_layout->addStretch();
	//---------------------------
	
	
	//------------消息内容----------
	auto msg_layout = new QVBoxLayout;
	m_user_name_label = new QLabel(m_user_name);
	m_user_name_label->setObjectName("user_name_label");
	m_user_name_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	m_message_label = new QLabel(m_message);
	m_message_label->setObjectName("message_label");
	m_message_label->setAlignment(Qt::AlignLeft);

	msg_layout->addWidget(m_user_name_label);
	msg_layout->addWidget(m_message_label);
	msg_layout->addStretch();
	//----------
	//----------------------------

	//------------时间--------------
	auto time_layout = new QVBoxLayout;
	m_date_time_label = new QLabel(m_dateTime);
	m_date_time_label->setObjectName("time_label");
	m_date_time_label->setAlignment(Qt::AlignCenter);

	time_layout->addWidget(m_date_time_label);
	time_layout->addStretch();
	//------------------------------


	main_layout->addLayout(image_layout);
	main_layout->addLayout(msg_layout);
	main_layout->addStretch();
	main_layout->addLayout(time_layout);

	this->setLayout(main_layout);
}

void SUserMessageWidget::updateUi()
{
	if (m_avatar_path.isEmpty()) {
		m_image_label->clear();
	}
	else {
		m_image_label->setPixmap(QPixmap(m_avatar_path).scaled(m_image_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}

	if (m_user_name.isEmpty()) {
		m_user_name_label->clear();
	}
	else {
		m_user_name_label->setText(m_user_name);
	}

	if (m_message.isEmpty()) {
		m_message_label->clear();
	}
	else {
		m_message_label->setText(m_message);
	}

	if (m_dateTime.isEmpty()) {
		m_date_time_label->clear();
	}
	else {
		m_date_time_label->setText(m_dateTime);
	}
}

void SUserMessageWidget::setData(const QString& avatar_path, const QString& user_name, const QString& message, const QString& dateTime)
{
	m_avatar_path = avatar_path;
	m_user_name = user_name;
	m_message = message;
	m_dateTime = dateTime;

	if (m_avatar_path == "" || m_avatar_path == "0" || m_avatar_path.isEmpty()) {
		m_avatar_path = ":/ResourceClient/dog.png";
	}
	
	updateUi();
}

void SUserMessageWidget::clearData()
{
	m_avatar_path.clear();
	m_user_name.clear();
	m_message.clear();
	m_dateTime.clear();

	updateUi();
}
