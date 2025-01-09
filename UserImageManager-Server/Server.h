#ifndef SERVER_H
#define SERVER_H

#include <QHttpServer>

class Server {
public:
	Server();
	
private:
	QHttpServer m_server;

	void route_userInfo();
	void route_userImage();
};

#endif //SERVER_H