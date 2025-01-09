#ifndef SS_SQL_CONNECTION_POOL_H
#define SS_SQL_CONNECTION_POOL_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QMutexLocker>

class SSqlConnectionPool {
public:
	SSqlConnectionPool();
	static SSqlConnectionPool* getInstance();
	~SSqlConnectionPool();


	QSqlDatabase openConnection();
	void closeConnection(QSqlDatabase db);

	void setHostName(const QString& host) { m_hostName = host; }
	void setPort(quint16 port) { m_port = port; }
	void setDatabaseName(const QString& databaseName) { m_databaseName = databaseName; }
	void setUserName(const QString& userName) { m_userName = userName; }
	void setPassword(const QString& password) { m_password = password; }
	void setDatabaseType(const QString& databaseType) { m_databaseType = databaseType; }
	void setMaxConnections(qint32 maxConnections) { m_maxConnections = maxConnections; }
private:
	QSqlDatabase createConnection(const QString& con_name);
	void releaseConnection(QSqlDatabase db);

	QString m_hostName;
	quint16 m_port;
	QString m_databaseName;
	QString m_userName;
	QString m_password;
	QString m_databaseType;

	QMutex m_mutex;						//互斥锁
	QWaitCondition m_waitCondition;		//条件
	qint32 m_maxConnections;			//最大连接数
	qint32 m_waitInterval;				//等待时间间隔(ms)
	qint32 m_waitTime;					//等待时间(ms)
	QQueue<QString> m_useConnections;	//正在使用的连接
	QQueue<QString> m_unusedConnections;//空闲的连接

};

class SSqlConnectionWrap {
public:
	SSqlConnectionWrap();
	~SSqlConnectionWrap();

	QSqlDatabase openConnection();
private:
	QSqlDatabase m_db;
};

#endif // SS_SQL_CONNECTION_POOL_H