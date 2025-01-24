#include "SUserAddView.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QFormLayout>

SUserAddView::SUserAddView(QWidget* parent)
	:SUserIOBaseView(parent)
{
	init();
}

void SUserAddView::init()
{

	setStyleSheet("background-color: white;");
	setAttribute(Qt::WA_StyledBackground);

	m_user_account_edit = new QLineEdit;
	m_username_edit = new QLineEdit;
	m_mobile_edit = new QLineEdit;
	m_email_edit = new QLineEdit;
	m_def_password_rbtn = new QRadioButton("默认密码");
	m_custom_password_rbtn = new QRadioButton("自定义密码");
	m_password_edit = new QLineEdit;

	auto mlayout = new QVBoxLayout(centralWidget());

	auto flayout = new QFormLayout;
	flayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::WrapAllRows);
	//flayout->setContentsMargins(0, 0, 0, 0);
	//flayout->setSpacing(6);

	flayout->addRow("<font color=red>*</font>用户ID", m_user_account_edit);
	flayout->addRow("<font size=2 face='宋体' color='gray'>最大长度64位，允许英文字母、数字</font>", new QLabel);

	flayout->addRow("<font color=red>*</font>用户名", m_username_edit);
	flayout->addRow("<font size=2 face='宋体' color='gray'>最大长度64位，允许中文、英文字母、数字、特殊字符</font>", new QLabel);

	flayout->addRow("<font color=gray>*</font>电话号码", m_mobile_edit);
	flayout->addRow("<font size=2 face='宋体' color='gray'>最大长度11位，允许数字及-符号</font>", new QLabel);

	flayout->addRow("<font color=gray>*</font>电子邮箱", m_email_edit);
	flayout->addRow("<font size=2 face='宋体' color='gray'>最大长度32位，允许英文字母、数字、@</font>", new QLabel);

	auto hlayot = new QHBoxLayout;
	hlayot->addWidget(m_def_password_rbtn);
	hlayot->addWidget(m_custom_password_rbtn);
	hlayot->addStretch();

	flayout->addRow("<font color=red>*</font>初始密码",hlayot);
	flayout->addRow(m_password_edit);

	mlayout->addLayout(flayout);

	{
		auto okBtn = new QPushButton("确定");
		auto cancelBtn = new QPushButton("取消");

		auto bhlayout = new QHBoxLayout;
		bhlayout->addWidget(okBtn);
		bhlayout->addWidget(cancelBtn);

		mlayout->addStretch(0);
		mlayout->addLayout(bhlayout);

		connect(okBtn, &QPushButton::clicked, this, [=]() {
			auto user_account = m_user_account_edit->text();

			m_juser.insert("user_account", user_account);
			m_juser.insert("user_name", m_username_edit->text());
			m_juser.insert("mobile", m_mobile_edit->text());
			m_juser.insert("email", m_email_edit->text());
			m_juser.insert("isEnable", 1);
			m_juser.insert("password", m_custom_password_rbtn->isChecked() ? m_password_edit->text() : "");

			SHttpClient(URL("/api/user/create")).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json(m_juser)
				.success([=](const QByteArray& data) {
				QJsonParseError error;
				auto jdom = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
#ifndef _DEBUG 
					qWarning() << "json数据解析错误" << error.errorString();
#endif
				}
				if (jdom["code"].toInt() < 1000) {
					//为了查询到用户的id
					SHttpClient(URL("/api/user/queryUser")).debug(true)
						.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
						.param("user_account", user_account) //查询user_account的用户信息
						.fail([=](const QString& msg, int code) {
							})
						.success([=](const QByteArray& data)
							{
								QJsonParseError error;
								auto jdom = QJsonDocument::fromJson(data, &error);
								if (jdom["code"].toInt() < 1000) {
									auto id = jdom["data"]["list"].toArray()[0].toObject()["id"].toInt();
									m_juser.insert("id", id);
									emit newUser(m_juser);
								}

							})
						.get();
				}
					})
				.post();

			

			});



		connect(cancelBtn, & QPushButton::clicked, this, [=]() {
			this->close();
			});

	}

	m_def_password_rbtn->setChecked(true);
	m_password_edit->hide();
	connect(m_custom_password_rbtn, &QRadioButton::toggled, m_password_edit, &QLineEdit::setVisible);

	m_user_account_edit->setText(QDateTime::currentDateTime().toString("yyMMddhhmmsszz"));
}

void SUserAddView::setUser(const QJsonObject& user)
{
	m_juser = user;
	//m_backBtn->setText(m_juser.value("user_name").toString());
	//m_user_account_lab->setText(m_juser.value("user_account").toString());
	//m_username_lab->setText(m_juser.value("user_name").toString());
	////m_isEnable_btn->setText(m_juser.value("user_account").toString());
	//m_isEnable_btn->setToggle(m_juser.value("isEnable").toBool());
	//m_mobile_lab->setText(m_juser.value("mobile").toString());
	//m_email_lab->setText(m_juser.value("email").toString());
}
