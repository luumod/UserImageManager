#include "SSqlConnectionPool.h"
	
SSqlConnectionPool::SSqlConnectionPool()
	:m_databaseType("QMYSQL")
	,m_maxConnections(10)
	,m_port(3306)
	,m_waitInterval(100)
	,m_waitTime(1000)
{
}
SSqlConnectionPool* SSqlConnectionPool::getInstance()
{
	static SSqlConnectionPool* instance = nullptr;
	if (instance == nullptr)
	{
		instance = new SSqlConnectionPool();
	}
	return instance;
}
SSqlConnectionPool::~SSqlConnectionPool()
{
	QMutexLocker<QMutex> locker(&m_mutex);

	//移除所有连接
	for (const auto& con_name : m_useConnections) {
		QSqlDatabase::removeDatabase(con_name);
	}
	for (const auto& con_name : m_useConnections) {
		QSqlDatabase::removeDatabase(con_name);
	}
}

QSqlDatabase SSqlConnectionPool::openConnection()
{
	QMutexLocker<QMutex> locker(&m_mutex);

	//当前总连接数
	int count=m_useConnections.count() + m_unusedConnections.count();
	//当前连接数已经到达最大连接数
	//并且没有到达最大等待时间
	//并且没有空闲连接
	//则等待其他的连接关闭
	for (size_t i = 0; i < m_waitTime && m_unusedConnections.isEmpty() && count == m_maxConnections; i += m_waitInterval) {
		//等待其他的连接关闭
		m_waitCondition.wait(&m_mutex, m_waitInterval);
		//重新计算总连接数
		count = m_useConnections.count() + m_unusedConnections.count();
	}

	QString con_name;
	//判断是否需要创建新的连接

	//如果有空闲连接，则使用空闲连接
	if (m_unusedConnections.count() > 0) {
		con_name = m_unusedConnections.dequeue();
	}
	//如果没有空闲连接，并且当前总连接数小于最大连接数，则创建新的连接
	else if (count < m_maxConnections) {
		con_name = QString("connection-%1").arg(count + 1);
	}
	else {
		qWarning() << "No available connection";
		return QSqlDatabase();
	}

	auto db = createConnection(con_name);
	if (db.isOpen()) {
		m_useConnections.enqueue(con_name);
	}

	return db;
}

void SSqlConnectionPool::closeConnection(QSqlDatabase db)
{
	QMutexLocker<QMutex> locker(&m_mutex);

	auto con_name = db.connectionName();
	if (m_useConnections.contains(con_name)) {
		m_useConnections.removeOne(con_name);
		m_unusedConnections.enqueue(con_name);
		//唤醒一个正在等待打开线程
		m_waitCondition.wakeOne();
	}

	/*qDebug() << "total connections:" << m_useConnections.count() + m_unusedConnections.count();
	qDebug()<< "use connections:"<<m_useConnections.count();
	qDebug() << "unused connections:" << m_unusedConnections.count();*/
}

QSqlDatabase SSqlConnectionPool::createConnection(const QString& con_name)
{
	if (QSqlDatabase::contains(con_name)) {
		qWarning() << "Connection already exists: " << con_name;
		return QSqlDatabase::database(con_name);
	}

	QSqlDatabase db = QSqlDatabase::addDatabase(m_databaseType, con_name);
	db.setHostName(m_hostName);
	db.setPort(m_port);
	db.setDatabaseName(m_databaseName);
	db.setUserName(m_userName);
	db.setPassword(m_password);

	if (!db.open()) {
		qWarning() << "Failed to open database: " << db.lastError().text();
		return QSqlDatabase();
	}

	return db;
}

void SSqlConnectionPool::releaseConnection(QSqlDatabase db)
{
	//关闭数据库连接
	auto con_name = db.connectionName();
	QSqlDatabase::removeDatabase(con_name);
}

SSqlConnectionWrap::SSqlConnectionWrap()
{
}

SSqlConnectionWrap::~SSqlConnectionWrap()
{
	auto pool = SSqlConnectionPool::getInstance();
	pool->closeConnection(m_db); //关闭数据库连接
}

QSqlDatabase SSqlConnectionWrap::openConnection()
{
	m_db = SSqlConnectionPool::getInstance()->openConnection(); //打开数据库连接
	return m_db;
}
