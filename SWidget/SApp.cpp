﻿#include "SApp.h"
#include <QFile>
#include <QDir>

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
	auto it = m_userData.find(key);
	if (it != m_userData.end()) {
		qWarning() << "User data already exists:" << key;
	}
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