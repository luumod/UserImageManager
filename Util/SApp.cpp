#include "SApp.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QJsonObject>

SApp::SApp(int& argc, char** argv)
	: QApplication(argc, argv)
{
	initGlobalConfig();
}

SConfigFile* SApp::globalConfig() const
{
	if (!m_globalConfig){
		Q_ASSERT_X(m_globalConfig != nullptr, __FUNCTION__, "Global config file not created!");
	}
	return m_globalConfig;
}

void SApp::setUserData(const QString& key, const QVariant& data)
{
	m_userData.insert(key, data);
}

QVariant SApp::userData(const QString& key) const
{
	auto it = m_userData.find(key);
	if (it == m_userData.end()) {
		qWarning() << "User data not found:" << key;
		return QVariant();
	}
	return m_userData.value(key);
}

void SApp::debugUserData()
{
	for (const auto& key : m_userData.keys()) {
		qDebug() << "User data:" << key << m_userData.value(key);
	}
}

void SApp::updateUserData(const QJsonObject& juser)
{
	if (juser.value("user_account").toString() == sApp->userData("user/user_account").toString()) {
		//sApp->setUserData("user/avatar_path", QDir::currentPath() + "/" + juser.value("avatar_path").toString());
		sApp->setUserData("user/user_name", juser.value("user_name").toString());
		sApp->setUserData("user/email", juser.value("email").toString());
		sApp->setUserData("user/mobile", juser.value("mobile").toString());
		sApp->setUserData("user/isEnable", juser.value("isEnable").toBool());
		emit updateUser();
	}
}

void SApp::initGlobalConfig()
{
	//判断配置文件是否存在，不存在则创建
	QString configPath = QDir::homePath() + "/config.ini";
	qDebug()<<"configPath:"<<configPath;
	bool isExists = false;
	if (QFile::exists(configPath)) {
		isExists = true;
	}

	m_globalConfig = new SConfigFile(configPath);
	Q_ASSERT_X(m_globalConfig != nullptr, __FUNCTION__, "Global config file create failed!");
	if (!m_globalConfig->isOpen()) {
		qWarning() << "Global config file open failed!";
		return;
	}

	//原本不存在，添加默认配置
	if (!isExists) {
		globalConfig()->setValue("host/host", "127.0.0.1:8888");
	}

}