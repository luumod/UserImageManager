#include "SUserDetailView.h"
#include "SApp.h"
#include "SHttpClient.h"
#include "STextInputEdit.h"
#include "textButton.h"
#include "SSwitchButton.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QHttpMultiPart>

SUserDetailView::SUserDetailView(QWidget* parent)
	:QWidget(parent)
{
	init();
}

void SUserDetailView::init()
{
	setStyleSheet("background-color:white");
	setAttribute(Qt::WA_StyledBackground);

	QFont nameFont = QFont("Corbel", 12);

	auto mlayout = new QVBoxLayout(this);

	//返回按钮
	m_backBtn = new QPushButton("返回");
	m_backBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); //返回按钮固定

	mlayout->addWidget(m_backBtn);

	//（标题）状态
	auto hlayout = new QHBoxLayout;
	m_isEnable_btn = new SSwitchButton;
	m_isEnable_btn->setFixedWidth(45);
	auto lab_enable = new QLabel(" 账号状态：");
	lab_enable->setFont(nameFont);
	hlayout->addWidget(lab_enable);
	hlayout->addWidget(m_isEnable_btn);
	hlayout->addStretch();
	mlayout->addLayout(hlayout);

	//头像
	QHBoxLayout* hlayout_0 = new QHBoxLayout;
	m_avatar_lab = new QLabel;
	m_avatar_lab->setPixmap(QPixmap(":/ResourceClient/default_avatar.png"));
	m_avatar_lab->setFixedSize(120, 120);
	m_avatar_lab->setScaledContents(true);
	auto lab_avatar = new QLabel(" 头  像：");
	lab_avatar->setFont(nameFont);
	hlayout_0->addWidget(lab_avatar);
	hlayout_0->addWidget(m_avatar_lab);
	hlayout_0->addStretch(1);
	mlayout->addLayout(hlayout_0);

	//id
	QHBoxLayout* hlayout_1 = new QHBoxLayout;
	m_inputItem_id = new STextInputEdit("用户id:");
	m_inputItem_id->setValue("1173012900");
	m_inputItem_id->setEnabled(false);
	hlayout_1->addWidget(m_inputItem_id, 2);
	hlayout_1->addStretch(3);
	mlayout->addLayout(hlayout_1);

	//昵称
	QHBoxLayout* hlayout_2 = new QHBoxLayout;
	m_inputItem_name = new STextInputEdit("昵  称:");
	m_inputItem_name->setValue("于良浩");
	m_inputItem_name->setEnabled(false);
	hlayout_2->addWidget(m_inputItem_name, 2);
	hlayout_2->addStretch(3);
	mlayout->addLayout(hlayout_2);

	//邮箱
	QHBoxLayout* hlayout_3 = new QHBoxLayout;
	m_inputItem_email = new STextInputEdit("邮  箱:");
	m_inputItem_email->setValue("1173012900@qq.com");
	m_inputItem_email->setEnabled(false);
	hlayout_3->addWidget(m_inputItem_email, 2);
	hlayout_3->addStretch(3);
	mlayout->addLayout(hlayout_3);

	//电话
	QHBoxLayout* hlayout_4 = new QHBoxLayout;
	m_inputItem_mobile = new STextInputEdit("电  话:");
	m_inputItem_mobile->setValue("13953435852");
	m_inputItem_mobile->setEnabled(false);
	hlayout_4->addWidget(m_inputItem_mobile, 2);
	hlayout_4->addStretch(3);
	mlayout->addLayout(hlayout_4);

	//最下面的空白
	mlayout->addStretch();

	connect(m_backBtn, &QPushButton::clicked, this, &SUserDetailView::close);

	connect(m_isEnable_btn, &SSwitchButton::stateChanged, [=](bool state)
		{
			SHttpClient(URL("/api/user/alter?user_account=" + m_juser.value("user_account").toString())).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json({ {"isEnable",state} })
				.fail([=](const QString& msg, int code) {
					})
				.success([=](const QByteArray& data)
					{
						QJsonParseError error;
						auto jdom = QJsonDocument::fromJson(data, &error);
						if (error.error != QJsonParseError::NoError) {
#if _DEBUG
							qWarning() << "json parse error:" << error.errorString();
#endif
						}
						else
						{
							if (jdom["code"].toInt() < 1000)
							{
								m_juser.insert("isEnable", state);
								emit userChanged(m_juser);
							}
						}
					})
				.post();

		});
}

void SUserDetailView::setUser(const QJsonObject& user)
{
	m_juser = user;
	updateUi();
	onAvatarDownload();
}

void SUserDetailView::onAvatarDownload()
{
	SHttpClient(URL("/api/user/avatar")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.param("user_account", m_juser.value("user_account").toString()) //query参数
		.fail([=](const QString& msg, int code) {
			//默认头像
			m_avatar_lab->setPixmap(QPixmap(":/ResourceClient/default_avatar.png"));
			m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Ellipse));
			})
		.success([=](const QByteArray& data)
			{
				//若成功获取头像，则直接显示图片，否则显示json的错误信息，即若返回的是json数据，则说明无头像数据
				if (data.startsWith('{'))
				{
					QJsonParseError error;
					auto jdom = QJsonDocument::fromJson(data, &error);

					m_avatar_lab->setPixmap(QPixmap(":/ResourceClient/default_avatar.png"));
					m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Ellipse));
				}
				else
				{
					auto img = QImage::fromData(data); //从服务器获取图片
					m_avatar_lab->setPixmap(QPixmap::fromImage(img));
					m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Ellipse));
				}
			})
		.get();
}

void SUserDetailView::onAvatarUpload()
{
	auto path =  sApp->globalConfig()->value("other/select_avatar_path",
		QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
	auto filename = QFileDialog::getOpenFileName(this, "选择头像", path, "images (*.jpg;*.jpeg;*.png)");
	if (filename.isEmpty())
		return;
	sApp->globalConfig()->setValue("other/select_avatar_path",QFileInfo(filename).canonicalPath());

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, "失败", "头像打开失败");
		return;
	}

	//上传的头像不能超过3M
	if (file.size() > 3 * 1024 * 1034) {
		QMessageBox::warning(this, "失败", "请选择3M以内的图片");
		return;
	}

	auto url = URL("/api/user/avatar?user_account=" + m_juser.value("user_account").toString());

	QNetworkAccessManager* mangaer = new QNetworkAccessManager(this);
	QNetworkRequest request(url);
	request.setRawHeader("Authorization", "Bearer" + sApp->userData("user/token").toString().toUtf8());
	request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-www-form-urlencoded");

	QHttpMultiPart* mpart = new QHttpMultiPart(this);
	QHttpPart part;
	part.setBody(file.readAll());
	part.setHeader(QNetworkRequest::ContentTypeHeader, QString("image/%1").arg(QFileInfo(filename).suffix()));
	part.setHeader(QNetworkRequest::ContentDispositionHeader, QString(R"(attachment;name="file";filename="%1")")
		.arg(QFileInfo(filename).fileName()));
	mpart->append(part);

	auto reply = mangaer->post(request, mpart);
	connect(reply, &QNetworkReply::finished, [=]
		{
			if (reply->error() != QNetworkReply::NoError)
			{
				qDebug() << "replay error" << reply->errorString();
				m_avatar_lab->setPixmap(QPixmap(":/ResourceClient/default_avatar.png"));
				m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Ellipse));
			}
			else
			{
				m_avatar_lab->setPixmap(QPixmap(filename));
				m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Ellipse));
			}
		});
}

void SUserDetailView::updateUi()
{
	m_inputItem_id->setValue(m_juser.value("user_account").toString());
	m_inputItem_name->setValue(m_juser.value("user_name").toString());
	m_isEnable_btn->setToggle(m_juser.value("isEnable").toBool());
	m_inputItem_email->setValue(m_juser.value("email").toString());
	m_inputItem_mobile->setValue(m_juser.value("mobile").toString());
}
