#include <QCoreApplication>
#include "Server.h"
#include "SSqlConnectionPool.h"

int main(int argc, char* argv[]) {
	QCoreApplication app(argc, argv);

	SSqlConnectionPool::instance()->setHostName("localhost");
	SSqlConnectionPool::instance()->setPort(3306);
	SSqlConnectionPool::instance()->setDatabaseName("user_images_manager_system");
	SSqlConnectionPool::instance()->setUserName("root"); 
	SSqlConnectionPool::instance()->setPassword("123456");

	Server server;

	return app.exec();
}