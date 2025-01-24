#include "SUserEditView.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QFormLayout>

SUserEditView::SUserEditView(QWidget* parent)
	:SUserIOBaseView(parent)
{
	init();
}

void SUserEditView::init()
{
	setStyleSheet("background-color:white");
	setAttribute(Qt::WA_StyledBackground);

	m_user_account_edit = new QLineEdit;
	m_username_edit = new QLineEdit;
	m_mobile_edit = new QLineEdit;
	m_email_edit = new QLineEdit;
	
	m_user_account_edit->setEnabled(false);

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

	mlayout->addLayout(flayout);

	auto okBtn = new QPushButton("确定");
	auto cancelBtn = new QPushButton("取消");

	auto bhlayout = new QHBoxLayout;
	bhlayout->addStretch();
	bhlayout->addWidget(okBtn);
	bhlayout->addWidget(cancelBtn);

	mlayout->addStretch(0);
	mlayout->addLayout(bhlayout);

	connect(okBtn, &QPushButton::clicked, [=]()
		{
			m_juser.insert("user_account", m_user_account_edit->text());
			m_juser.insert("user_name",m_username_edit->text());
			m_juser.insert("mobile",m_mobile_edit->text());
			m_juser.insert("email",m_email_edit->text());

			SHttpClient(URL("/api/user/alter?user_account=" + m_juser.value("user_account").toString())).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json(m_juser)
				.success([=](const QByteArray& data)
					{
						QJsonParseError error;
						auto jdom = QJsonDocument::fromJson(data, &error);
						if (error.error != QJsonParseError::NoError) {
							qWarning() << "json parse error:" << error.errorString();
							return;
						}
						if (jdom["code"].toInt() < 1000) {
							emit userChanged(m_juser);
							this->close();
						}
						else {
							qWarning() << "alter user failed:" << jdom["message"].toString();
						}
					})
				.post();

		});
	connect(cancelBtn, &QPushButton::clicked, [=]()
		{
			this->close();
		});
}

void SUserEditView::setUser(const QJsonObject& user)
{
	m_juser = user;
	m_user_account_edit->setText(m_juser.value("user_account").toString());
	m_username_edit->setText(m_juser.value("user_name").toString());
	m_mobile_edit->setText(m_juser.value("mobile").toString());
	m_email_edit->setText(m_juser.value("email").toString());
}
