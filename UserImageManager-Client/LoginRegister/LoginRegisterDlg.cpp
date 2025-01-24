#include "LoginRegisterDlg.h"
#include "EventFilterObject.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QPainter> 
#include <QStaticText>
#include <QPushButton>
#include <QFile>
#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QNetWorkReply>
#include <QTimer>
#include <QMessageBox>

LoginRegister::LoginRegister(QWidget* parent)
	:QDialog(parent)
	, m_nam(new QNetworkAccessManager(this))
{
	//在库中的资源，必须手动初始化
	Q_INIT_RESOURCE(LoginRegister_Res);
	initUi();
}

LoginRegister::~LoginRegister()
{
	Q_CLEANUP_RESOURCE(LoginRegister_Res);
}


void LoginRegister::initUi()
{
	//去除边框
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	//让无边框窗口可以拖动
	installEventFilter(new EventFilterObject(this));
	//窗口背景透明
	setAttribute(Qt::WA_TranslucentBackground);

	resize(740, 470);
	//右边白色矩形
	m_rightRect = QRect(5, 40, width() - 5 * 2, height() - 40 * 2);
	//左边的矩形
	m_leftRect = QRect(5, 5, 185, 440);
	//中间的图形
	m_middleRect = QRect(5, 5, 370, 440);

	m_tips = new QLabel(this);
	m_tips->setGeometry(434, 380, 200, 30);
	m_tips->setAlignment(Qt::AlignCenter);
	m_timer = new QTimer(this);
	m_timer->setInterval(2000);
	m_timer->callOnTimeout([=] { m_tips->clear(); m_timer->stop(); });


	//关闭按钮
	auto closeBtn = new QPushButton(this);
	closeBtn->setGeometry(width() - 45, 45, 30, 30);
	closeBtn->setObjectName("closeBtn");
	connect(closeBtn, &QPushButton::clicked, this, &LoginRegister::close);

	//堆栈窗口
	m_stkWidget = new QStackedWidget(this);
	m_stkWidget->setGeometry(434, 150, 200, 240);
	m_stkWidget->setContentsMargins(0, 0, 0, 0);

	m_loginWidget = createLoginWidget();
	m_registerWidget = createRegisterWidget();
	m_stkWidget->addWidget(m_loginWidget); //默认显示登录页面
	m_stkWidget->addWidget(m_registerWidget);

	//加载样式表
	QFile file(":/LoginRegister_Res/style.css");
	if (file.open(QIODevice::ReadOnly)) {
		setStyleSheet(file.readAll());
	}
}

QWidget* LoginRegister::createLoginWidget()
{
	auto config = sApp->globalConfig();

	QWidget* w = new QWidget;
	m_account = new QLineEdit;
	m_account->setPlaceholderText("账号/手机号/邮箱");
	m_account->setText(config->value("user/user_account").toString());

	m_password = new QLineEdit;
	m_password->setPlaceholderText("密码");
	m_password->setEchoMode(QLineEdit::Password);
	if (config->value("user/remember_pwd").toBool()) {
		m_password->setText(config->value("user/password").toString());
	}

	m_rememberPwd = new QCheckBox("记住密码");
	m_rememberPwd->setChecked(config->value("user/remember_pwd").toBool());

	m_autoLogin = new QCheckBox("自动登录");
	m_autoLogin->setChecked(config->value("user/auto_login").toBool());

	auto registerBtn = new QPushButton("注册账号");
	auto loginBtn = new QPushButton("登录");

	m_rememberPwd->setObjectName("rememberPwd");
	m_autoLogin->setObjectName("autoLogin");
	registerBtn->setObjectName("registerBtn");
	loginBtn->setObjectName("loginBtn");

	connect(m_autoLogin, &QCheckBox::stateChanged, this, [=](int checked) {
		if (checked == Qt::Checked) {
			m_rememberPwd->setChecked(true);
		}
		config->setValue("user/auto_login", checked == Qt::Checked);
		});

	connect(m_rememberPwd, &QCheckBox::stateChanged, this, [=](int checked) {
		if (checked == Qt::Unchecked) {
			m_autoLogin->setChecked(false);
		}
		config->setValue("user/remember_pwd", checked == Qt::Checked);
		});

	connect(registerBtn, &QPushButton::clicked, this, [=]() {
		m_stkWidget->setCurrentWidget(m_registerWidget);
		});

	connect(loginBtn, &QPushButton::clicked, this, &LoginRegister::onLogin);

	//自动登录
	if (config->value("user/auto_login").toBool()) {
		loginBtn->click();
	}

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(m_rememberPwd);
	hlayout->addWidget(m_autoLogin);
	hlayout->addWidget(registerBtn);

	auto vlayout = new QVBoxLayout;
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->addWidget(m_account);
	vlayout->addWidget(m_password);
	vlayout->addLayout(hlayout);
	vlayout->addWidget(loginBtn);

	w->setLayout(vlayout);

	return w;
}

QWidget* LoginRegister::createRegisterWidget()
{
	QWidget* w = new QWidget;

	 m_accountR = new QLineEdit;
	 m_accountR->setPlaceholderText("账号");
	 m_nameR = new QLineEdit;
	 m_nameR->setPlaceholderText("姓名");
	 m_passwordR = new QLineEdit;
	 m_passwordR->setEchoMode(QLineEdit::Password);
	 m_passwordR->setPlaceholderText("输入密码");
	 m_rePasswordR = new QLineEdit;
	 m_rePasswordR->setPlaceholderText("再次输入密码");
	 m_rePasswordR->setEchoMode(QLineEdit::Password);
	 m_sexMaleR = new QRadioButton("男");
	 m_sexFemaleR = new QRadioButton("女");

	auto registerBtn = new QPushButton("注册");
	auto backBtn = new QPushButton("返回登录");

	registerBtn->setObjectName("registerComfirmBtn");
	backBtn->setObjectName("backBtn");

	connect(backBtn, &QPushButton::clicked, this, [=]() {
		m_stkWidget->setCurrentWidget(m_loginWidget);
		});

	connect(registerBtn, &QPushButton::clicked, this,&LoginRegister::onRegister);

	auto hlayout1 = new QHBoxLayout;
	hlayout1->addStretch();
	hlayout1->addWidget(m_sexMaleR);
	hlayout1->addStretch();
	hlayout1->addWidget(m_sexFemaleR);
	hlayout1->addStretch();

	auto hlayout2 = new QHBoxLayout;
	hlayout2->setSpacing(0);
	hlayout2->addWidget(registerBtn);
	hlayout2->addWidget(backBtn);

	auto vlayout = new QVBoxLayout;
	vlayout->addWidget(m_accountR);
	vlayout->addWidget(m_nameR);
	vlayout->addWidget(m_passwordR);
	vlayout->addWidget(m_rePasswordR);
	vlayout->addLayout(hlayout1);
	vlayout->addLayout(hlayout2);
	w->setLayout(vlayout);

	return w;
}

void LoginRegister::setTip(const QString& text,int time)
{
	m_timer->stop();
	m_tips->setText("<font color=red>" + text + "</font>");
	m_timer->start();
}

void LoginRegister::saveInfo()
{
	QSettings settings("user.ini",QSettings::IniFormat);
	settings.setValue("account", m_account->text());
	if (m_rememberPwd->isChecked()) {
		settings.setValue("password", m_password->text());
	}
	else {
		settings.setValue("password", "");
	}
	settings.setValue("rememberPwd", m_rememberPwd->isChecked());

}

void LoginRegister::readInfo()
{
	QSettings settings("user.ini", QSettings::IniFormat);
	m_account->setText(QString(settings.value("account").toString()));
	auto isRem = settings.value("rememberPwd").toBool();
	m_rememberPwd->setChecked(isRem);
	if (isRem) {
		m_password->setText(QString(settings.value("password").toString()));
	}
}

void LoginRegister::onLogin() {
	if (m_isLogining) {
		setTip("正在登录中，不允许重复登录", 2);
		return; 
	}

	auto acc = m_account->text();
	auto pwd = m_password->text();
	auto isRememPwd = m_rememberPwd->isChecked();
	if (acc.isEmpty()) {
		setTip("请输入账号",1);
		return;
	}
	else if (pwd.isEmpty()) {
		setTip("请输入密码",1);
		return;
	}
	setTip("登陆中~");

	SHttpClient(URL("/api/login"))
		.debug(true)
		.json(QJsonObject{ {"user_account",acc},{"password",pwd} }) // 发送json数据
		.fail([=](const QString& msg, int code)
			{
				if (code == QNetworkReply::ConnectionRefusedError) {
					QMessageBox::critical(this, "错误", "服务器连接失败，请检查网络连接");
				}
				setTip(msg,2);
			})
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdom = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
					qWarning() << "json数据解析失败";
					setTip(jdom["message"].toString(), 2); 
					return;
				}
				else {
					if (jdom["code"].toInt() < 1000) { //jdom.object().value("code").tointeger()
						auto token = jdom["data"]["token"].toString();
						sApp->setUserData("user/token", token); 
						sApp->setUserData("user/id", jdom["data"]["id"].toString());
						sApp->setUserData("user/user_account",jdom["data"]["user_account"].toString());
						sApp->setUserData("user/user_name", jdom["data"]["user_name"].toString());
						sApp->setUserData("user/gender", jdom["data"]["gender"].toInt());
						sApp->setUserData("user/password", jdom["data"]["password"].toString());
						sApp->setUserData("user/mobile", jdom["data"]["mobile"].toString());
						sApp->setUserData("user/email", jdom["data"]["email"].toString());
						sApp->setUserData("user/avatar_path", QDir::currentPath() + "/" + jdom["data"]["avatar_path"].toString());
						sApp->setUserData("user/isEnable", jdom["data"]["isEnable"].toBool());
						sApp->setUserData("user/isDeleted", jdom["data"]["isDeleted"].toBool());
						sApp->setUserData("user/has_image_count", jdom["data"]["has_image_count"].toInt());

						sApp->debugUserData();
						
						//登陆成功后，保存信息
						auto config = sApp->globalConfig();
						config->setValue("user/user_account",jdom["data"]["user_account"].toString());
						config->setValue("user/password", jdom["data"]["password"].toString());
						config->setValue("user/remember_pwd", isRememPwd);
						config->setValue("user/auto_login", m_autoLogin->isChecked());

						setTip("登录成功",2);
						accept(); //关闭窗口
					}
					else {
						setTip(jdom["message"].toString(), 2);
					}
				}
			})
		.complete([=](bool hasError) {
			m_isLogining = false;
			})
		.post(); //发送post请求
}

void LoginRegister::onRegister(){
	auto acc = m_accountR->text();
	auto uname = m_nameR->text();
	auto pwd = m_passwordR->text();
	auto rPwd = m_rePasswordR->text();
	int gender = m_sexMaleR->isChecked() ? 1 : (m_sexFemaleR->isChecked() ? 2 : 0); //1男 2女 0未知

	if (acc.isEmpty()) {
		setTip("请输入账号",1);
		return;
	}
	else if (uname.isEmpty()) {
		setTip("请输入昵称",1);
		return;
	}
	else if (pwd.isEmpty()) {
		setTip("请输入密码",1);
		return;
	}
	else if (rPwd.isEmpty()) {
		setTip("请再次输入密码",1);
		return;
	}
	else if (pwd != rPwd) {
		setTip("两次输入密码不一致",1);
		return;
	}

	setTip("注册中~");


	SHttpClient(URL("/api/register"))
		.debug(true)
		.json(QJsonObject{ {"user_account",acc},{"password",pwd},{"user_name",uname}, { "gender",gender } }) // json body数据
		.fail([=](const QString& msg, int code) {
#ifdef _DEBUG
			qDebug() << msg << code;
#endif
			setTip(msg);
			})
		.success([=](const QByteArray& data) {
			QJsonParseError error;
			auto jdom = QJsonDocument::fromJson(data, &error);
			if (error.error != QJsonParseError::NoError) {
#ifdef _DEBUG
				qWarning() << "json数据解析失败";
#endif
				setTip(jdom["message"].toString(), 2);
				return;
			}
			if (jdom["code"].toInteger() < 1000) {
				setTip("注册成功",2);
				//两秒后自动返回登录窗口
				auto backBtn = findChild<QPushButton*>("backBtn");
				backBtn->click();
			}
			else {
				setTip(jdom["message"].toString(),2);
			}
			})
		.complete([=](bool hasError) {
			m_isLogining = false;
			})
		.post();
}

void LoginRegister::paintEvent(QPaintEvent* paint)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	//绘制右边的白色矩形
	painter.setPen(Qt::NoPen);
	painter.fillRect(m_rightRect, Qt::white);

	//绘制左边的矩形
	painter.fillRect(m_leftRect, qRgb(27,45,67));

	//绘制中间的弦
	painter.setBrush(QColor(qRgb(27, 45, 67)));
	painter.drawChord(m_middleRect, -90 * 16, 180 * 16);

	//绘制Welcome
	painter.setPen(Qt::white);
	painter.setFont(QFont("微软雅黑", 30));
	painter.drawStaticText(48, 60,QStaticText ("Welcome!"));
	painter.setFont(QFont("微软雅黑", 10,QFont::Bold));
	painter.drawStaticText(48, 120,QStaticText ("登录注册界面--Yuleo"));

	//梦想
	painter.drawPixmap(QRect(40,200,250,250), QPixmap(":/LoginRegister_Res/cai.png"));
	painter.drawPixmap(QRect(40, 200, 250, 250), QPixmap(":/LoginRegister_Res/dream.png"));

	//logo
	painter.drawPixmap(QPoint(423,70), QPixmap(":/LoginRegister_Res/dunkai.png"));

}

void LoginRegister::closeEvent(QCloseEvent* close)
{

}
