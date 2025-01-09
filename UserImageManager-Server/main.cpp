#include <QCoreApplication>
#include "Server.h"
#include "SSqlConnectionPool.h"

int main(int argc, char* argv[]) {
	QCoreApplication app(argc, argv);

	SSqlConnectionPool::getInstance()->setHostName("localhost");
	SSqlConnectionPool::getInstance()->setPort(3306);
	SSqlConnectionPool::getInstance()->setDatabaseName("user_images_manager_system");
	SSqlConnectionPool::getInstance()->setUserName("root"); 
	SSqlConnectionPool::getInstance()->setPassword("123456");

	Server server;

	return app.exec();
}