#include "SRescentMovementWidget.h"
#include "SUserMessageWidget.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QBoxLayout>
#include <QLabel>
#include <QFile>
#include <QStylePainter>
#include <QStyleOption>
#include <QPushButton>
#include <QTimer>

SRescentMovementWidget::SRescentMovementWidget(QWidget* parent)
	: QWidget(parent)
{
	init();
	this->setObjectName("SRescentMovementWidget");
	this->setFixedWidth(400);

	QString qss_file = ":/ResourceClient/view.css";
	QFile qss(qss_file);
	if (qss.open(QFile::ReadOnly))
	{
		QString qss_style =qss.readAll();
		this->setStyleSheet(qss_style);
	}
}

SRescentMovementWidget::~SRescentMovementWidget()
{
}

void SRescentMovementWidget::initWidgets()
{
	m_messageWidgets.resize(MAX_ACTIVITIES_IN_ONE_PAGE);
	for (int i = 0; i < m_messageWidgets.size(); i++){
		m_messageWidgets[i] = new SUserMessageWidget(); //预加载
		m_contentLayout->addWidget(m_messageWidgets[i]);
	}
}

void SRescentMovementWidget::init()
{
	auto main_layout = new QVBoxLayout;
	main_layout->setContentsMargins(0, 0, 0, 0);
	//main_layout->setSpacing(10);


	//---------------标题---------------
	auto top_layout = new QHBoxLayout;
	top_layout->setContentsMargins(0,0,0,0);
	auto title_lab = new QLabel(tr("Recent Movements"));
	title_lab->setObjectName("title_lab");
	title_lab->setFixedHeight(60);

	auto prev_btn = new QPushButton;
	prev_btn->setIcon(QPixmap(":/ResourceClient/pre.png"));
	prev_btn->setObjectName("prev_btn");
	connect(prev_btn, &QPushButton::clicked, this, [=]() {
		if (m_currentPage - 1 >= 1) {
			m_currentPage--;
			onSearch();
		}
		});

	auto next_btn = new QPushButton;
	next_btn->setIcon(QPixmap(":/ResourceClient/next.png"));
	next_btn->setObjectName("next_btn");
	connect(next_btn, &QPushButton::clicked, this, [=]() {
		if (m_currentPage + 1 <= m_lastPage) {
			m_currentPage++;
			onSearch();
		}
		});


	top_layout->addWidget(title_lab);
	top_layout->addStretch();
	top_layout->addWidget(prev_btn);
	top_layout->addWidget(next_btn);
	//---------------------------------

	//---------------内容----------------
	m_contentLayout = new QVBoxLayout;
	m_contentLayout->setSpacing(10);

	initWidgets();

	onSearch(); // 查询最近的动态：第一页

	//定时更新活动
	QTimer* timer = new QTimer(this);
	timer->setInterval(1000 * 60 * 1); // 1分钟更新一次
	connect(timer, &QTimer::timeout, this, [=]() {
		m_currentPage = 1;

		QString originalTitle = title_lab->text();
		title_lab->setText(tr("Recent Movements(update)"));

		onSearch();

		QTimer::singleShot(500, [=]() { // 假设更新完成后1分钟恢复原来的文本
			title_lab->setText(originalTitle);
			});
		});
	timer->start();


	//-----------------------------------
	// 添加分隔线
	QFrame* line = new QFrame();
	line->setFrameShape(QFrame::HLine); // 水平线
	line->setFrameShadow(QFrame::Sunken); // 沉降效果的线

	main_layout->addLayout(top_layout);
	main_layout->addWidget(line);
	main_layout->addLayout(m_contentLayout);

	this->setLayout(main_layout);

	m_contentLayout->update();
	main_layout->update();
	layout()->activate();
}

void SRescentMovementWidget::setData(int idx, const QString& avator_path, const QString& user_name, const QString& message, const QString& dateTime)
{
	m_messageWidgets[idx]->setData(avator_path, user_name, message, dateTime);
}

void SRescentMovementWidget::clearData()
{
	for (int i = 0; i < m_messageWidgets.size(); i++) {
		m_messageWidgets[i]->clearData();
	}
}

void SRescentMovementWidget::paintEvent(QPaintEvent* event)
{
	QStylePainter painter(this);
	QStyleOption opt;
	opt.initFrom(this);
	opt.rect = rect();
	painter.drawPrimitive(QStyle::PE_Widget, opt);

	QWidget::paintEvent(event);
}

void SRescentMovementWidget::onSearch()
{
	QVariantMap params;
	params.insert("user_id", sApp->userData("user/id").toString());
	params.insert("page", m_currentPage);
	params.insert("pageSize", MAX_ACTIVITIES_IN_ONE_PAGE);
	SHttpClient(URL("/api/user/get_activities")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.params(params)
		.fail([=](const QString& msg, int code) {
#ifdef _DEBUG
		qWarning() << msg << code;
#endif
			})
		.success([=](const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
#ifdef _DEBUG
			qWarning() << "Json解析错误: " << error.errorString();
#endif
			return;
		}
		if (jdom["code"].toInt() < 1000) {
			parseJson(jdom.object()); // 解析json数据
		}
		else {
#ifdef _DEBUG 
			qWarning() << "查询数据失败: " << jdom["message"].toString();
#endif
		}
			})
		.get(); // 发送get请求
}

void SRescentMovementWidget::parseJson(const QJsonObject& obj)
{
	clearData(); // 清空之前的数据

	auto jArray = obj["data"]["images"].toArray();
	m_lastPage = obj["data"]["last_page"].toInt();
	m_totalCount = obj["data"]["total_records"].toInt();

	for (int i = 0; i < jArray.size(); i++) {
		auto jObj = jArray[i].toObject();
		int image_id = jObj["id"].toInt();
		QString avatar_path = jObj["avatar_path"].toString();
		QString user_name = jObj["user_name"].toString();
		QString image_name = jObj["image_name"].toString();
		QString created_time = jObj["created_time"].toString();
		QString activity_type = jObj["activity_type"].toString();

		QString message = "";
		if (activity_type == "comment") {
			message = "<span style='color:blue;'>评论了你的图片<span>: " + image_name;
		}
		else if (activity_type == "like") {
			message = "<span style='color:red;'>喜欢了你的图片<span>: " + image_name;
		}
		else if (activity_type == "star") {
			message = "<span style='color:#E063E0;'>收藏了你的图片<span>: " + image_name;
		}

		if (user_name == sApp->userData("user/user_name").toString()) {
			user_name = "您";
			if (activity_type == "comment") {
				message = "<span style='color:blue;'>评论了自己的图片<span>: " + image_name;
			}
			else if (activity_type == "like") {
				message = "<span style='color:red;'>喜欢了自己的图片<span>: " + image_name;
			}
			else if (activity_type == "star") {
				message = "<span style='color:#E063E0;'>收藏了自己的图片<span>: " + image_name;
			}
		}
		setData(i,avatar_path, user_name, message, created_time);
	}
}