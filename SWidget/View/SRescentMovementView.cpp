#include "SRescentMovementView.h"
#include "SUserMessageWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QFile>
#include <QStylePainter>
#include <QStyleOption>

SRescentMovementView::SRescentMovementView(QWidget* parent)
	: QWidget(parent)
{
	init();
	this->setObjectName("SRescentMovementView");

	QString qss_file = ":/ResourceClient/view.css";
	QFile qss(qss_file);
	if (qss.open(QFile::ReadOnly))
	{
		QString qss_style =qss.readAll();
		this->setStyleSheet(qss_style);
	}
}

SRescentMovementView::~SRescentMovementView()
{
}

void SRescentMovementView::init()
{
	auto main_layout = new QVBoxLayout;
	main_layout->setContentsMargins(0, 0, 0, 0);
	//main_layout->setSpacing(10);


	//---------------标题---------------
	auto title_lab = new QLabel(tr("Recent Movements"));
	title_lab->setObjectName("title_lab");
	title_lab->setFixedHeight(60);
	//---------------------------------

	//---------------内容----------------
	m_contentLayout = new QVBoxLayout;
	m_contentLayout->setSpacing(10);
	//-
	addMessage(1, "User1", "I have a new message", "2021-08-10 10:00:00");
	addMessage(2, "User2", "I have a new message", "2021-08-10 10:00:00");
	addMessage(3, "User3", "I have a new message", "2021-08-10 10:00:00");
	addMessage(4, "User4", "I have a new message", "2021-08-10 10:00:00");
	addMessage(5, "User4", "I have a new message", "2021-08-10 10:00:00");
	addMessage(6, "User4", "I have a new message", "2021-08-10 10:00:00");
	addMessage(7, "User4", "I have a new message", "2021-08-10 10:00:00");



	//-----------------------------------
	// 添加分隔线
	QFrame* line = new QFrame();
	line->setFrameShape(QFrame::HLine); // 水平线
	line->setFrameShadow(QFrame::Sunken); // 沉降效果的线

	main_layout->addWidget(title_lab);
	main_layout->addWidget(line);
	main_layout->addLayout(m_contentLayout);

	this->setLayout(main_layout);
}

void SRescentMovementView::addMessage(int image_id, QString user_name, QString message, QString dateTime)
{
	auto msg_widget = new SUserMessageWidget(image_id, user_name, message, dateTime);
	m_contentLayout->addWidget(msg_widget);
}

void SRescentMovementView::paintEvent(QPaintEvent* event)
{
	QStylePainter painter(this);
	QStyleOption opt;
	opt.initFrom(this);
	opt.rect = rect();
	painter.drawPrimitive(QStyle::PE_Widget, opt);

	QWidget::paintEvent(event);
}