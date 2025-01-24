#ifndef SAPP_H
#define SAPP_H

#include <QApplication>
#include "SConfigFile.h"

#define sApp static_cast<SApp*>(qApp)
#define URL(route) "http://" + sApp->globalConfig()->value("host/host").toString() + route 
#define ServerPath "F:/code/UserImageManager/out/build/x64-debug/UserImageManager-Server/"
#define TableView_ButtonWidth 45
#define TableView_ButtonHeight 20

/*
全局应用类，用于管理全局配置、用户数据等
*/
class SApp : public QApplication {

	Q_OBJECT

public:
	SApp(int& argc, char** argv);

	/*
	user/user_account
	user/password
	user/remember_pwd
	user/auto_login
	*/
	SConfigFile* globalConfig() const;

	void setUserData(const QString& key, const QVariant& data);
	QVariant userData(const QString& key) const;

	void debugUserData();
	void updateUserData(const QJsonObject& juser);
signals:
	void updateUser();
private:
	SConfigFile* m_globalConfig = nullptr;

	void initGlobalConfig();

	QVariantMap m_userData;
};

#endif // SAPP_H