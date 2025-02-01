#include "Server.h"
#include "SResultCode.h"
#include "SSqlConnectionPool.h"
#include "SHttpPartParse.h"
#include "SJwt.h"
#include "QsLog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <optional>
#include <QRandomGenerator>

#define CheckJsonParse(request)\
	QJsonParseError error;\
	auto jdom = QJsonDocument::fromJson(request.body(), &error);\
	if (error.error != QJsonParseError::NoError) {\
		return SResult::error(SResultCode::ParamJsonInvalid);\
	}\

#define CheckSqlQuery(query)\
	if (query.lastError().type() != QSqlError::NoError) {\
		return SResult::error(SResultCode::ServerSqlQueryError);\
	}\

//密钥
const char* SCRETKEY = "Yuleo666";

QJsonObject recordToJson(const QSqlRecord & record) {
	QJsonObject juser;
	for (size_t i = 0; i < record.count(); i++) {
		const auto& v = record.value(i);
		switch (v.typeId())
		{
		case QMetaType::Bool:
			juser.insert(record.fieldName(i), record.value(i).toBool());
			break;
		case QMetaType::Int:
		case QMetaType::Short:
		case QMetaType::UShort:
		case QMetaType::LongLong:
		case QMetaType::ULongLong:
			juser.insert(record.fieldName(i), record.value(i).toInt());
			break;
		case QMetaType::Float:
		case QMetaType::Double:
			juser.insert(record.fieldName(i), record.value(i).toDouble());
			break;
		default:
			juser.insert(record.fieldName(i), record.value(i).toString());
			break;
		}
	}
	return juser;
}

std::optional<QByteArray> CheckToken(const QHttpServerRequest& request) {
	//校验token
	auto auth = request.value("Authorization"); //从headers字段中获取
	//无token
	if (auth.isEmpty()) {
		return SResult::error(SResultCode::UserUnauthenticated);
	}
	//token不符合规范
	if (!auth.startsWith("Bearer")) {
		return SResult::error(SResultCode::UserAuthFormatError);
	}
	//取得token
	auto token = auth.mid(strlen("Bearer")); //去掉前缀 获取从位置6-end的token
	auto jwtObject = SJwt::SJwtObject::decode(token, SJwt::SAlgorithm::HS256, SCRETKEY);
	switch (jwtObject.status())
	{
	case SJwt::SJwtObject::Status::Invalid:
		return SResult::error(SResultCode::UserAuthInvalid);
	case SJwt::SJwtObject::Status::Expired:
		return SResult::error(SResultCode::UserAuthExpired);
	default:
		return {};
	}
}
Server::Server()
{
	//开启服务器
	m_server.listen(QHostAddress::Any, 8888);
	

	////插入测试数据
	/*{
		qDebug() << "test data started";
		SSqlConnectionWrap wrap;
		auto db = wrap.openConnection();
		db.transaction();
		QSqlQuery query(db);
		query.prepare("INSERT INTO user_info(user_account,password,user_name,gender,mobile,email) VALUES(?,?,?,?,?,?)");
		for (size_t i = 0; i < 10000; i++)
		{
			query.bindValue(0, QString::number(99999+i));
			query.bindValue(1, QString::number(QRandomGenerator::global()->bounded(100000, 999999)));
			query.bindValue(2, QString("user_%1").arg(i));
			query.bindValue(3, i % 3);
			query.bindValue(4, QString::number(QRandomGenerator::global()->bounded(10000000000, 19999999999)));
			query.bindValue(5, QString("123456%1@qq.com").arg(i));
			if (!query.exec()) {
				qDebug() << "insert error:" << query.lastError().text();
			}
		}
		db.commit();
		qDebug() << "test data finished";
	}*/


	//添加路径丢失处理
	m_server.setMissingHandler([](const QHttpServerRequest& request,
		QHttpServerResponder&& responder) {
			QJsonObject json;
			json.insert("code", "3000");
			json.insert("message", "不支持此操作");
			responder.write(QJsonDocument(json));
		});

	//添加路由
	m_server.route("/api/version", [](const QHttpServerRequest& request) {
		QJsonObject json;
		json.insert("version", "1.0.0");
		json.insert("name", "UserImageManager-Server");
		json.insert("datetime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
		json.insert("copyright", "Copyright © 2025 Yuleo");
		return QJsonDocument(json).toJson(QJsonDocument::Compact);
		});

	route_userInfo();
	route_userImage();
}

void Server::route_userInfo()
{
	QsLogging::Logger& logger = QsLogging::Logger::instance();
	logger.setLoggingLevel(QsLogging::DebugLevel);

	QDir dir;
	dir.mkpath("logs");
	auto fileDest = QsLogging::DestinationFactory::MakeFileDestination("logs/log.txt", QsLogging::EnableLogRotation, QsLogging::MaxSizeBytes(1024 * 1024), QsLogging::MaxOldLogCount(5));
	//auto consoleDest = QsLogging::DestinationFactory::MakeDebugOutputDestination();
	auto funDest = QsLogging::DestinationFactory::MakeFunctorDestination([](const QString& message, QsLogging::Level level) {
		qDebug() << message;
		});


	logger.addDestination(fileDest);
	logger.addDestination(funDest);

	//用户登录
	m_server.route("/api/login", [](const QHttpServerRequest& request) {
		CheckJsonParse(request);

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT * FROM user_info WHERE user_account='%1' AND password='%2' LIMIT 1")
		.arg(jdom["user_account"].toString())
		.arg(jdom["password"].toString()));
		query.exec();
#if _DEBUG
		qDebug() << "用户登录" << query.lastQuery();
#endif
		CheckSqlQuery(query);

		//是否查询到数据
		if (!query.next()) {
			return SResult::error(SResultCode::UserLoginError);
		}

		//检查用户是否禁用
		if (!query.value("isEnable").toBool()) {
			return SResult::success(SResultCode::UserDisabled);
		}

		//检查用户是否被删除
		if (query.value("isDeleted").toBool()) {
			return SResult::success(SResultCode::UserDeleted);
		}

		//生成token jwt
		QJsonObject payLoad = {
			{"user_account", jdom["user_account"]},
			{"iat",QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate)},
			{"exp",QDateTime::currentDateTime().addDays(7).toString(Qt::DateFormat::ISODate)}
		};
		SJwt::SJwtObject jwtObject(SJwt::SAlgorithm::HS256, payLoad, SCRETKEY);


		QJsonObject jLoginUser;
		jLoginUser.insert("id", query.value("id").toString());
		jLoginUser.insert("user_account", query.value("user_account").toString());
		jLoginUser.insert("user_name", query.value("user_name").toString());
		jLoginUser.insert("password", query.value("password").toString());
		jLoginUser.insert("gender", query.value("gender").toInt());
		jLoginUser.insert("mobile", query.value("mobile").toString());
		jLoginUser.insert("email", query.value("email").toString());
		jLoginUser.insert("avatar_path", query.value("avatar_path").toString());
		jLoginUser.insert("isEnable", true);
		jLoginUser.insert("isDeleted", false);
		jLoginUser.insert("has_image_count", query.value("has_image_count").toInt());
		jLoginUser.insert("token", QString(jwtObject.jwt()));

		//{ {"token",QString(jwtObject.jwt())} }
		return SResult::success(jLoginUser); //回复用户登录时生成的token（QJsonObject形式）
		});

	//用户注册
	m_server.route("/api/register", [](const QHttpServerRequest& request) {
		CheckJsonParse(request);

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare("INSERT IGNORE INTO user_info (user_account, password, user_name, gender) VALUES (?,?,?,?)");
		query.bindValue(0, jdom["user_account"].toString());
		query.bindValue(1, jdom["password"].toString());
		query.bindValue(2, jdom["user_name"].toString());
		query.bindValue(3, jdom["gender"].toInt());
		query.exec();
#if _DEBUG
		qDebug() << "用户注册";
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		//检查是否插入成功
		if (query.numRowsAffected() == 0) {
			return SResult::error(SResultCode::UserAlreadyExists);
		}
		//注册成功


		return SResult::success();
		});

	//用户查询(模糊查找)
	m_server.route("/api/user/list", [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		auto isEnable = uquery.queryItemValue("isEnable");
		auto page = uquery.queryItemValue("page").toInt();
		auto pageSize = uquery.queryItemValue("pageSize").toInt();
		auto user_account = uquery.queryItemValue("user_account");
		auto user_name = uquery.queryItemValue("user_name");
		auto mobile = uquery.queryItemValue("mobile");
		auto email = uquery.queryItemValue("email");
		auto gender = uquery.queryItemValue("gender");

		QString filter = "WHERE isDeleted=false ";
		//模糊搜索
		if (!user_account.isEmpty()) {
			filter += QString(" AND user_account LIKE '%%1%'").arg(user_account);
		}
		if (!user_name.isEmpty()) {
			filter += QString(" AND user_name LIKE '%%1%'").arg(user_name);
		}
		if (!mobile.isEmpty()) {
			filter += QString(" AND mobile LIKE '%%1%'").arg(mobile);
		}
		if (!email.isEmpty()) {
			filter += QString(" AND email LIKE '%%1%'").arg(email);
		}
		if (!gender.isEmpty()) {
			filter += QString(" AND gender=%1").arg(gender == "男" ? "1" : gender == "女" ? "2" : "0");
		}

		if (!isEnable.isEmpty()) {
			filter += QString(" and isEnable=%1 ").arg((isEnable == "true" ? 1 : 0));
		}
		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		//查询总记录条数
		query.exec(QString("SELECT COUNT(*) total FROM user_info %1 ").arg(filter));
#if _DEBUG
		qDebug() << "---------------查询总记录数";
		qDebug() << query.lastQuery();
#endif
		query.next();
		int total_records = query.record().value("total").toInt();

		if (pageSize == 0) {
			pageSize = 20;//默认每页20条数据
		}
		//计算总页数
		//56 / 20 = 2; 56 % 20 > 0; => 2 + 1 = 3 总共三页
		auto last_page = total_records / pageSize + (total_records % pageSize ? 1 : 0);
		if (page > last_page || page < 1) {
			page = 1;  // 超出范围，返回第一页
		}

		QString sql = "SELECT id,user_account,user_name,gender,mobile,email,isEnable FROM user_info ";
		sql += filter;
		sql += QString(" LIMIT %1,%2;").arg((page - 1) * pageSize).arg(pageSize);
		query.exec(sql);
#if _DEBUG
		qDebug() << "查询用户信息";
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		QJsonObject jobj;
		if (query.size() > 0) {
			QJsonArray array;
			//将查询结果转换为json数组
			while (query.next()) {
				array.append(recordToJson(query.record()));
			}
			jobj.insert("list", array);
			jobj.insert("cur_page", page);
			jobj.insert("page_size", pageSize);
			jobj.insert("last_page", last_page);
			jobj.insert("total_records", total_records);
		}
		return SResult::success(jobj);
		});

	//用户查询(精确查询:user_account)
	m_server.route("/api/user/queryUser", [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		auto user_account = uquery.queryItemValue("user_account");
		auto isEnable = uquery.queryItemValue("isEnable");
		QString filter = "WHERE isDeleted=false ";
		//通过user_account的查询
		if (!user_account.isEmpty()) {
			filter += QString(" and user_account='%1' ").arg(user_account);
		}
		if (!isEnable.isEmpty()) {
			filter += QString(" and isEnable=%1 ").arg((isEnable == "true" ? 1 : 0));
		}
		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		//查询总记录条数
		query.exec(QString("SELECT * FROM user_info %1 ").arg(filter));
		CheckSqlQuery(query);
#if _DEBUG
		qDebug() << "精确查找:" << user_account;
		qDebug() << query.lastQuery();
#endif
		/*query.next();
		auto id = query.record().value("id").toInt();
		auto user_account = query.record().value("user_account").toInt();;
		auto user_name = query.record().value("user_name").toString();
		auto gender = query.record().value("gender").toInt();
		auto mobile = query.record().value("mobile").toString();
		auto email = query.record().value("email").toString();*/

		QJsonObject jobj;
		if (query.size() > 0) {
			QJsonArray array;
			//将查询结果转换为json数组
			while (query.next()) {
				array.append(recordToJson(query.record()));
			}
			jobj.insert("list", array);
		}
		return SResult::success(jobj);
		});

	//用户创建
	m_server.route("/api/user/create", [](const QHttpServerRequest& request) {
		CheckJsonParse(request);

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		//检查参数完整性
		auto rObj = jdom.object();
		if (!rObj.contains("user_account") ||
			!rObj.contains("user_name")) {
			return SResult::error(SResultCode::ParamMissing);
		}
		auto password = rObj.value("password").toString();
		if (password.isEmpty()) {
			password = "123456"; //默认密码
		}

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		QString sql = QString("INSERT IGNORE INTO user_info (user_account,password,user_name,gender,mobile,email) VALUES('%1','%2','%3',%4,'%5','%6')")
			.arg(jdom["user_account"].toString())
			.arg(password)
			.arg(jdom["user_name"].toString())
			.arg(jdom["gender"].toInt())
			.arg(jdom["mobile"].toString())
			.arg(jdom["email"].toString());
		query.exec(sql);
#if _DEBUG
		qDebug() << "用户创建";
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		//检查是否插入成功
		if (query.numRowsAffected() == 0) {
			return SResult::error(SResultCode::UserAlreadyExists);
		}

		return SResult::success();
		});

	//用户删除
	m_server.route("/api/user/delete", [](const QHttpServerRequest& request) {
		CheckJsonParse(request);

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto jarray = jdom["lists"].toArray();
		if (jarray.isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare("UPDATE user_info SET isDeleted=true WHERE user_account=?");
		for (int i = 0; i < jarray.size(); i++) {
			query.bindValue(0, jarray[i].toString());
			query.exec();
#if _DEBUG
			qDebug() << "用户删除";
			qDebug() << query.lastQuery();
#endif
			CheckSqlQuery(query);
		}

		//检查是否更新成功
		if (query.numRowsAffected() == 0) {
			return SResult::error(SResultCode::UserNotFound);
		}

		return SResult::success();
		});

	//用户修改
	m_server.route("/api/user/alter", [](const QHttpServerRequest& request) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		auto user_account = uquery.queryItemValue("user_account");
		if (user_account.isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		CheckJsonParse(request);
		auto rObj = jdom.object();
		QString sql = "UPDATE user_info SET ";
		if (rObj.contains("user_name")) {
			sql += QString("user_name='%1',").arg(rObj["user_name"].toString());
		}
		if (rObj.contains("password")) {
			sql += QString("password='%1',").arg(rObj["password"].toString());
		}
		if (rObj.contains("gender")) {
			sql += QString("gender=%1,").arg(rObj["gender"].toInt());
		}
		if (rObj.contains("mobile")) {
			sql += QString("mobile='%1',").arg(rObj["mobile"].toString());
		}
		if (rObj.contains("email")) {
			sql += QString("email='%1',").arg(rObj["email"].toString());
		}
		if (rObj.contains("isEnable")) {
			sql += QString("isEnable=%1,").arg(rObj["isEnable"].toBool());
		}
		
		sql.chop(1); //去掉最后一个逗号

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString(sql + " WHERE user_account='%1'").arg(user_account));
		query.exec();
#if _DEBUG
		qDebug() << "用户修改";
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		//检查是否更新成功
		if (query.numRowsAffected() == 0) {
			return SResult::success(SResultCode::SuccessButNoData);
		}

		return SResult::success();
		});

	//用户批量修改
	m_server.route("/api/user/batch_alter", [](const QHttpServerRequest& request) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		CheckJsonParse(request);


		auto jArray = jdom["lists"].toArray();
		if (jArray.isEmpty()) {
			return SResult::success(SResultCode::SuccessButNoData);
		}
		QString resultSql;
		for (int i = 0; i < jArray.size(); i++) {
			auto jObj = jArray.at(i).toObject();
			auto filterObj = jObj.value("filter").toObject();
			auto updateObj = jObj.value("update").toObject();
			QString filter;
			for (auto it = filterObj.constBegin(); it != filterObj.constEnd(); ++it) {
				//如果键等于id
				if (it.key() == "id") {
					filter += QString("id='%1' and ").arg(it.value().toString());
				}
				else if (it.key() == "user_account") {
					filter += QString("user_account='%1' and ").arg(it.value().toString());
				}
				else if (it.key() == "isEnable") {
					filter += QString("isEnable=%1 and ").arg(it.value().toInt());
				}
				else if (it.key() == "isDeleted") {
					filter += QString("isDeleted=%1 and ").arg(it.value().toInt());
				}
			}	
			filter.chop(4); //去掉最后一个and
			QString sql = "UPDATE user_info SET ";
			for (auto it = updateObj.constBegin(); it != updateObj.constEnd(); ++it){
				if (it.key() == "user_name") {
					sql += QString("user_name='%1',").arg(it.value().toString());
				}
				else if (it.key() == "password") {
					sql += QString("password='%1',").arg(it.value().toString());
				}
				else if (it.key() == "gender") {
					sql += QString("gender=%1,").arg(it.value().toInt());
				}
				else if (it.key() == "mobile") {
					sql += QString("mobile='%1',").arg(it.value().toString());
				}
				else if (it.key() == "email") {
					sql += QString("email='%1',").arg(it.value().toString());
				}
				else if (it.key() == "isEnable") {
					sql += QString("isEnable=%1,").arg(it.value().toBool());
				}
			}
			sql.chop(1); //去掉最后一个逗号
			sql = QString(sql + " WHERE %1").arg(filter);
			sql += ";";
			resultSql.append(sql);
		}
#if _DEBUG
		QLOG_DEBUG() << resultSql;
#endif

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.exec(resultSql);
#if _DEBUG
		qDebug() << "用户修改";
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		//检查是否更新成功
		if (query.numRowsAffected() == 0) {
			return SResult::success(SResultCode::SuccessButNoData);
		}

		return SResult::success();
		});

	//用户头像上传: POST
	m_server.route("/api/user/avatar", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_account").isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}
		auto user_account = uquery.queryItemValue("user_account");

		auto data = request.body();
		if (data.isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}
		auto parse = SHttpPartParse(data);
		if (!parse.parse()) {
			return SResult::error(SResultCode::ParamInvalid);
		}
		auto path = "../images/avatar/";
		QDir dir;
		if (!dir.exists(path)) {
			dir.mkpath(path);
		}
		QFile file(QString(path + user_account + "." + QFileInfo(parse.filename()).suffix()));
		if (!file.open(QIODevice::WriteOnly)) {
			return SResult::error(SResultCode::ServerFileError);
		}
		file.write(parse.data());
		//把路径写入数据库
		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("UPDATE user_info SET avatar_path='%1' WHERE user_account='%2'").arg(file.fileName()).arg(user_account));
		query.exec();
#if _DEBUG
		qDebug() << "用户头像上传";
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		//检查是否更新成功
		/*if (query.numRowsAffected() == 0) {
			return SResult::success(SResultCode::SuccessButNoData);
		}*/

		return SResult::success();
		});

	//用户头像获取: GET
	m_server.route("/api/user/avatar", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_account").isEmpty()) {
			responder.write(SResultCode::ParamMissing.toJson(), "application/json");
			return;
		}
		auto user_account = uquery.queryItemValue("user_account");

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT avatar_path FROM user_info WHERE user_account='%1' AND isDeleted=false").arg(user_account));
#if _DEBUG
		qDebug() << "用户头像获取";
		qDebug() << query.lastQuery();
#endif
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}


		if (!query.next()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}

		auto path = query.value("avatar_path").toString();
		if (path.isEmpty()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}
		QFile file(path); 
		if (!file.open(QIODevice::ReadOnly)) {
			responder.write(SResult::error(SResultCode::ServerFileError, "头像未找到"), "application/json");
			return;
		}
		responder.writeStatusLine();
		responder.writeHeader("Content-Type", QString("image/%1").arg(QFileInfo(file.fileName()).suffix()).toUtf8());
		responder.writeHeader("Content-Length", QByteArray::number(file.size()));
		responder.writeHeader("Content-Disposition", "attachment; filename=" + file.fileName().toUtf8());
		responder.writeBody(file.readAll());
		});
}

void Server::route_userImage()
{
	//用户图片上传: POST
	m_server.route("/api/user/upload_image", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		//根据user_account查找user表中的id
		auto user_account = uquery.queryItemValue("user_account"); //eg. user_account = 1173012900
		if (user_account.isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT id,isEnable,isDeleted FROM user_info WHERE user_account='%1'").arg(user_account));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
#if _DEBUG
		qDebug() << "查询上传此图片的用户的id";
		qDebug() << query.lastQuery();
#endif
		if (!query.next()) {
			return SResult::error(SResultCode::UserNotFound);
		}
		if (query.value("isDeleted").toBool()) {
			return SResult::error(SResultCode::UserDeleted);
		}
		if (!query.value("isEnable").toBool()) {
			return SResult::error(SResultCode::UserDisabled);
		}

		auto owner_id = query.value("id").toInt(); //eg. owner_id <==> user: id 
		auto description = uquery.queryItemValue("description");
		if (description.isEmpty()) {
			description = "";
		}
		auto image_type = uquery.queryItemValue("image_type");
		if (image_type.isEmpty()) {
			image_type = "默认";
		}
		auto image_share = uquery.queryItemValue("image_share").toInt();
		auto image_download = uquery.queryItemValue("image_download").toInt();
		auto image_ResolutionRatio = uquery.queryItemValue("image_ResolutionRatio");
		if (image_ResolutionRatio.isEmpty()) {
			image_ResolutionRatio = "未知";
		}
		auto image_quality = uquery.queryItemValue("image_quality");
		if (image_quality.isEmpty()) {
			image_quality = "未知";
		}
		auto data = request.body();
		if (data.isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}
		auto parse = SHttpPartParse(data);
		if (!parse.parse()) {
			return SResult::error(SResultCode::ParamInvalid);
		}
		auto path = QString("../images/upload/%1/").arg(owner_id);
		QDir dir;
		if (!dir.exists(path)) {
			dir.mkpath(path);
		}
		//把路径写入数据库
		query.prepare(QString("INSERT IGNORE INTO user_image(owner_id,image_name,image_type,upload_time,description,image_format,image_share,image_download,image_ResolutionRatio,image_quality) \
			VALUES (%1,'%2','%3','%4','%5','%6',%7,%8,'%9','%10')")
			.arg(owner_id)
			.arg(QFileInfo(parse.filename()).baseName())
			.arg(image_type)
			.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
			.arg(description)
			.arg(QFileInfo(parse.filename()).suffix())
			.arg(image_share)
			.arg(image_download)
			.arg(image_ResolutionRatio)
			.arg(image_quality)
		);
		query.exec();
#if _DEBUG
		qDebug() << "用户图片上传";
		qDebug()<<query.lastQuery();
#endif
		CheckSqlQuery(query);

		//检查是否插入成功
		if (query.numRowsAffected() == 0) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}

		auto image_id = query.lastInsertId().toInt();
		//图片路径格式：images/upload/owner_id/image_id.后缀名
		QFile file(QString(path + QString::number(image_id) + "." + QFileInfo(parse.filename()).suffix()));
		if (!file.open(QIODevice::WriteOnly)) {
			return SResult::error(SResultCode::ServerFileError);
		}
		file.write(parse.data());  //保存到文件

		//image_path   image_size
		query.prepare(QString("UPDATE user_image SET image_path='%1',image_size=%2 WHERE image_id=%3")
			.arg(path + QString::number(image_id) + "." + QFileInfo(parse.filename()).suffix())
			.arg(file.size())
			.arg(image_id)
		);
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}

		return SResult::success();
		});

	//用户图片修改: POST
	m_server.route("/api/user/modify_image", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request) {
		CheckJsonParse(request);

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		//既然能修改图片，那么图片一定是存在的
		auto image_id = jdom["image_id"].toInt();
		auto image_name = jdom["image_name"].toString();
		auto image_type = jdom["image_type"].toString();
		auto image_format = jdom["image_format"].toString();
		auto image_size = jdom["image_size"].toInt();
		auto image_ResolutionRatio = jdom["image_ResolutionRatio"].toString();
		auto image_quality = jdom["image_quality"].toString();
		auto image_share = jdom["image_share"].toInt();
		auto image_download = jdom["image_download"].toInt();
		auto description = jdom["description"].toString();
		auto isDeleted = jdom["isDeleted"].toInt();

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		//auto data = request.body();
		//if (data.isEmpty()) {
		//	return SResult::error(SResultCode::ParamMissing);
		//}
		//auto parse = SHttpPartParse(data);
		//if (!parse.parse()) {
		//	return SResult::error(SResultCode::ParamInvalid);
		//}
		//auto path = QString("../images/upload/%1/").arg(owner_id);
		//QDir dir;
		//if (!dir.exists(path)) {
		//	dir.mkpath(path);
		//}
		////图片路径格式：images/upload/owner_id/user_account_时间戳_图片原名称.后缀名
		//QFile file(QString(path + user_account
		//	+ "_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss")
		//	+ "_" + QFileInfo(parse.filename()).fileName()));
		//if (!file.open(QIODevice::WriteOnly)) {
		//	return SResult::error(SResultCode::ServerFileError);
		//}
		//file.write(parse.data());
		//把路径写入数据库
		query.prepare(QString("UPDATE user_image SET image_name='%1',image_type='%2',image_format='%3',image_size=%4,image_ResolutionRatio='%5',upload_time='%6',description='%7',image_share=%8,image_download=%9,image_quality='%10' WHERE image_id=%11")
			.arg(image_name)
			.arg(image_type)
			.arg(image_format)
			.arg(image_size)
			.arg(image_ResolutionRatio)
			.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
			.arg(description)
			.arg(image_share)
			.arg(image_download)
			.arg(image_quality)
			.arg(image_id)
		);
		query.exec();
#if _DEBUG
		qDebug() << "用户图片信息更新";
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		//检查是否插入成功
		if (query.numRowsAffected() == 0) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}

		return SResult::success();
		});

	//用户图片获取：获取某个用户上传的全部图片 GET
	m_server.route("/api/user/get_image", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_account").isEmpty()) {
			responder.write(SResultCode::ParamMissing.toJson(), "application/json");
			return;
		}
		auto user_account = uquery.queryItemValue("user_account").toInt();
		auto page = uquery.queryItemValue("page").toInt();
		auto pageSize = uquery.queryItemValue("pageSize").toInt();

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT id FROM user_info WHERE user_account=%1 AND isDeleted=false AND isEnable=true").arg(user_account));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << "用户图片获取";
		qDebug() << query.lastQuery();
#endif
		query.next();
		if (query.value("id").isNull()) {
			responder.write(SResultCode::UserNotFound.toJson(), "application/json");
			return;	
		}
		auto id = query.value("id").toInt(); //获取图片所有者的id

		//确定筛选条件
		QString filter = "";
		auto image_name = uquery.queryItemValue("image_name");
		auto image_type = uquery.queryItemValue("image_type");
		auto image_format = uquery.queryItemValue("image_format");
		auto image_share = uquery.queryItemValue("image_share");
		auto image_download = uquery.queryItemValue("image_download");
		//模糊搜索
		if (!image_name.isEmpty()) {
			filter += QString(" AND image_name LIKE '%%1%'").arg(image_name);
		}
		if (!image_type.isEmpty()) {
			filter += QString(" AND image_type LIKE '%%1%'").arg(image_type);
		}
		if (!image_format.isEmpty()) {
			filter += QString(" AND image_format LIKE '%%1%'").arg(image_format);
		}
		if (!image_share.isEmpty()) {
			filter += QString(" AND image_share=%1").arg(image_share.toInt());
		}
		if (!image_download.isEmpty()) {
			filter += QString(" AND image_download=%1").arg(image_download.toInt());
		}


		if (pageSize == 0) {
			pageSize = 6;
		}
		//确保筛选后能得到正确的页数信息
		query.prepare(QString("SELECT COUNT(*) as total FROM user_image where isDeleted=0 AND owner_id=%1 %2" ).arg(id).arg(filter));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << query.lastQuery();
#endif

		query.next();
		auto total_records = query.value("total").toInt(); //图片总数
		auto last_page = total_records / pageSize + (total_records % pageSize ? 1 : 0);
		if (page < 1 || page > last_page) {
			page = 1;
		}

		//查询图片
		QString sql = QString("SELECT * FROM user_image WHERE isDeleted=0 AND owner_id=%1 %2").arg(id).arg(filter);
		sql += QString(" LIMIT %1,%2").arg((page - 1) * pageSize).arg(pageSize);
		query.prepare(sql);
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << query.lastQuery();
#endif

		QJsonArray jarray;
		SSqlConnectionWrap wrap2;
		QSqlQuery query_everyImage_statistics(wrap2.openConnection());
		int i = 0;
		while (query.next()) {
			auto image_id = query.value("image_id").toInt(); //获取每张图片id
			query_everyImage_statistics.prepare(QString("SELECT like_count,star_count,download_count,comment_count FROM image_statistics WHERE image_id=%1").arg(image_id)); //统计每一张图片的统计数据数
			//统计图片点赞数（还可以获取所有点赞的用户，以后可以增加一个开通vip可查看的功能）
			if (!query_everyImage_statistics.exec()) {
				responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
#if _DEBUG	
			qDebug() << query_everyImage_statistics.lastQuery();
#endif
			query_everyImage_statistics.next();
	
			QJsonObject jobj;
			jobj.insert("image_id", image_id);
			jobj.insert("owner_id", query.value("owner_id").toInt());
			jobj.insert("image_path", query.value("image_path").toString());
			jobj.insert("image_name", query.value("image_name").toString());
			jobj.insert("image_size", query.value("image_size").toInt());
			jobj.insert("image_format", query.value("image_format").toString());
			jobj.insert("image_share", query.value("image_share").toInt());
			jobj.insert("image_type", query.value("image_type").toString());
			jobj.insert("image_download", query.value("image_download").toInt());
			jobj.insert("image_ResolutionRatio", query.value("image_ResolutionRatio").toString());
			jobj.insert("image_quality", query.value("image_quality").toString());
			jobj.insert("upload_time", query.value("upload_time").toString());
			jobj.insert("description", query.value("description").toString());
			jobj.insert("like_count", query_everyImage_statistics.value("like_count").toInt());
			jobj.insert("star_count", query_everyImage_statistics.value("star_count").toInt());
			jobj.insert("download_count", query_everyImage_statistics.value("download_count").toInt());
			jobj.insert("comment_count", query_everyImage_statistics.value("comment_count").toInt());
			jarray.append(jobj);
		}

		if (jarray.isEmpty()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}

		QJsonObject jobj;
		jobj.insert("images", jarray);
		jobj.insert("cur_page", page);
		jobj.insert("page_size", pageSize);
		jobj.insert("last_page", last_page);
		jobj.insert("total_records", total_records);
		responder.write(SResult::success(jobj), "application/json");
		return;
		});

	//获取某个用户最热门的n张图片 GET
	m_server.route("/api/user/get_hot_image", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {
			
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_id").isEmpty()) {
			responder.write(SResultCode::ParamMissing.toJson(), "application/json");
			return;
		}
		
		auto get_size = uquery.queryItemValue("get_size").toInt();
		if (uquery.queryItemValue("get_size").isEmpty()) {
			get_size = 6;
		}
		auto user_id = uquery.queryItemValue("user_id").toInt();

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT image_id, (like_count * 1 + star_count * 3 + download_count * 5 + comment_count * 4) AS heat FROM image_statistics WHERE user_id=%1 AND isDeleted=0 ORDER BY heat DESC LIMIT %2")
			.arg(user_id).arg(get_size));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << "获取最热的" << get_size << "张图片";
		qDebug() << query.lastQuery();
#endif

		SSqlConnectionWrap wrap2,wrap3;
		QSqlQuery query2(wrap2.openConnection());
		QSqlQuery query_everyImage_statistics(wrap3.openConnection());
		QJsonArray jarray;
		while (query.next()) { 
			auto image_id = query.value("image_id").toInt(); //获取每张图片id
			auto heat = query.value("heat").toInt(); //获取热度
			query2.prepare(QString("SELECT * FROM user_image WHERE image_id=%1").arg(image_id));
			if (!query2.exec()) {
				responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
			query_everyImage_statistics.prepare(QString("SELECT like_count,star_count,download_count,comment_count FROM image_statistics WHERE image_id=%1").arg(image_id)); //统计每一张图片的统计数据数
			if (!query_everyImage_statistics.exec()) {
				responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
#if _DEBUG	
			qDebug() << query_everyImage_statistics.lastQuery();
#endif
#if _DEBUG	
			qDebug() << query2.lastQuery();
#endif
			query2.next();
			query_everyImage_statistics.next();
			QJsonObject jobj2;
			jobj2.insert("heat", heat);
			jobj2.insert("image_id", image_id);
			jobj2.insert("owner_id", query2.value("owner_id").toInt());
			jobj2.insert("image_path", query2.value("image_path").toString());
			jobj2.insert("image_name", query2.value("image_name").toString());
			jobj2.insert("image_size", query2.value("image_size").toInt());
			jobj2.insert("image_format", query2.value("image_format").toString());
			jobj2.insert("image_share", query2.value("image_share").toInt());
			jobj2.insert("image_type", query2.value("image_type").toString());
			jobj2.insert("image_download", query2.value("image_download").toInt());
			jobj2.insert("image_ResolutionRatio", query2.value("image_ResolutionRatio").toString());
			jobj2.insert("image_quality", query2.value("image_quality").toString());
			jobj2.insert("upload_time", query2.value("upload_time").toString());
			jobj2.insert("description", query2.value("description").toString());
			jobj2.insert("like_count", query_everyImage_statistics.value("like_count").toInt());
			jobj2.insert("star_count", query_everyImage_statistics.value("star_count").toInt());
			jobj2.insert("download_count", query_everyImage_statistics.value("download_count").toInt());
			jobj2.insert("comment_count", query_everyImage_statistics.value("comment_count").toInt());
			jarray.append(jobj2);
		}
		if (jarray.size() <= 0) {
			responder.write(SResultCode::ImageNotFound.toJson(), "application/json");
			return;
		}

		while (jarray.size() < get_size) {
			//不够，则重复添加已经添加的所有图片
			auto tmpArray = jarray;
			for (auto i = 0; i < tmpArray.size(); i++) {
				jarray.append(tmpArray[i]);
			}
		}

		QJsonObject jobj;
		jobj.insert("images", jarray);
		responder.write(SResult::success(jobj), "application/json");
		return;

		});

	//获取全站用户最热门的n张图片 GET
	m_server.route("/api/user/get_hot_image_all", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		auto uquery = request.query();
		auto get_size = uquery.queryItemValue("get_size").toInt();
		if (uquery.queryItemValue("get_size").isEmpty()) {
			get_size = 7;
		}

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT image_id, (like_count * 1 + star_count * 3 + download_count * 5 + comment_count * 4) AS heat FROM image_statistics WHERE isDeleted=0 ORDER BY heat DESC LIMIT %2")
			.arg(get_size));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << "获取全站最热的" << get_size << "张图片";
		qDebug() << query.lastQuery();
#endif

		SSqlConnectionWrap wrap2, wrap3;
		QSqlQuery query2(wrap2.openConnection());
		QSqlQuery query_everyImage_statistics(wrap3.openConnection());
		QJsonArray jarray;
		while (query.next()) {
			auto image_id = query.value("image_id").toInt(); //获取每张图片id
			auto heat = query.value("heat").toInt(); //获取热度
			query2.prepare(QString("SELECT * FROM user_image WHERE image_id=%1").arg(image_id));
			if (!query2.exec()) {
				responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
			query_everyImage_statistics.prepare(QString("SELECT like_count,star_count,download_count,comment_count FROM image_statistics WHERE image_id=%1").arg(image_id)); //统计每一张图片的统计数据数
			if (!query_everyImage_statistics.exec()) {
				responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
#if _DEBUG	
			qDebug() << query_everyImage_statistics.lastQuery();
#endif
#if _DEBUG	
			qDebug() << query2.lastQuery();
#endif
			query2.next();
			query_everyImage_statistics.next();
			QJsonObject jobj2;
			jobj2.insert("heat", heat);
			jobj2.insert("image_id", image_id);
			jobj2.insert("owner_id", query2.value("owner_id").toInt());
			jobj2.insert("image_path", query2.value("image_path").toString());
			jobj2.insert("image_name", query2.value("image_name").toString());
			jobj2.insert("image_size", query2.value("image_size").toInt());
			jobj2.insert("image_format", query2.value("image_format").toString());
			jobj2.insert("image_share", query2.value("image_share").toInt());
			jobj2.insert("image_type", query2.value("image_type").toString());
			jobj2.insert("image_download", query2.value("image_download").toInt());
			jobj2.insert("image_ResolutionRatio", query2.value("image_ResolutionRatio").toString());
			jobj2.insert("image_quality", query2.value("image_quality").toString());
			jobj2.insert("upload_time", query2.value("upload_time").toString());
			jobj2.insert("description", query2.value("description").toString());
			jobj2.insert("like_count", query_everyImage_statistics.value("like_count").toInt());
			jobj2.insert("star_count", query_everyImage_statistics.value("star_count").toInt());
			jobj2.insert("download_count", query_everyImage_statistics.value("download_count").toInt());
			jobj2.insert("comment_count", query_everyImage_statistics.value("comment_count").toInt());
			jarray.append(jobj2);
		}
		if (jarray.size() <= 0) {
			responder.write(SResultCode::ImageNotFound.toJson(), "application/json");
			return;
		}

		while (jarray.size() < get_size) {
			//不够，则重复添加已经添加的所有图片
			auto tmpArray = jarray;
			for (auto i = 0; i < tmpArray.size(); i++) {
				jarray.append(tmpArray[i]);
			}
		}

		QJsonObject jobj;
		jobj.insert("images", jarray);
		responder.write(SResult::success(jobj), "application/json");
		return;

		});

	//用户图片获取：获取某个用户上传的全部图片(包括已删除图片) GET
	m_server.route("/api/user/get_image_all", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_account").isEmpty()) {
			responder.write(SResultCode::ParamMissing.toJson(), "application/json");
			return;
		}
		auto user_account = uquery.queryItemValue("user_account").toInt();
		auto page = uquery.queryItemValue("page").toInt();
		auto pageSize = uquery.queryItemValue("pageSize").toInt();

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT id FROM user_info WHERE user_account=%1 AND isDeleted=false AND isEnable=true").arg(user_account));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << "用户图片获取";
		qDebug() << query.lastQuery();
#endif
		query.next();
		if (query.value("id").isNull()) {
			responder.write(SResultCode::UserNotFound.toJson(), "application/json");
			return;
		}
		auto id = query.value("id").toInt(); //获取图片所有者的id

		//确定筛选条件
		QString filter = "";
		auto image_name = uquery.queryItemValue("image_name");
		auto image_type = uquery.queryItemValue("image_type");
		auto image_format = uquery.queryItemValue("image_format");
		auto image_share = uquery.queryItemValue("image_share");
		auto image_download = uquery.queryItemValue("image_download");
		//模糊搜索
		if (!image_name.isEmpty()) {
			filter += QString(" AND image_name LIKE '%%1%'").arg(image_name);
		}
		if (!image_type.isEmpty()) {
			filter += QString(" AND image_type LIKE '%%1%'").arg(image_type);
		}
		if (!image_format.isEmpty()) {
			filter += QString(" AND image_format LIKE '%%1%'").arg(image_format);
		}
		if (!image_share.isEmpty()) {
			filter += QString(" AND image_share=%1").arg(image_share.toInt());
		}
		if (!image_download.isEmpty()) {
			filter += QString(" AND image_download=%1").arg(image_download.toInt());
		}


		if (pageSize == 0) {
			pageSize = 6;
		}
		//确保筛选后能得到正确的页数信息
		query.prepare(QString("SELECT COUNT(*) as total FROM user_image where owner_id=%1 %2").arg(id).arg(filter));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << query.lastQuery();
#endif

		query.next();
		auto total_records = query.value("total").toInt(); //图片总数
		auto last_page = total_records / pageSize + (total_records % pageSize ? 1 : 0);
		if (page < 1 || page > last_page) {
			page = 1;
		}

		//查询图片
		QString sql = QString("SELECT * FROM user_image WHERE owner_id=%1 %2").arg(id).arg(filter);
		sql += QString(" LIMIT %1,%2").arg((page - 1) * pageSize).arg(pageSize);
		query.prepare(sql);
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << query.lastQuery();
#endif

		QJsonArray jarray;
		SSqlConnectionWrap wrap2;
		QSqlQuery query_everyImage_statistics(wrap2.openConnection());
		int i = 0;
		while (query.next()) {
			auto image_id = query.value("image_id").toInt(); //获取每张图片id
			query_everyImage_statistics.prepare(QString("SELECT like_count,star_count,download_count,comment_count FROM image_statistics WHERE image_id=%1").arg(image_id)); //统计每一张图片的统计数据数
			//统计图片点赞数（还可以获取所有点赞的用户，以后可以增加一个开通vip可查看的功能）
			if (!query_everyImage_statistics.exec()) {
				responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
#if _DEBUG	
			qDebug() << query_everyImage_statistics.lastQuery();
#endif
			query_everyImage_statistics.next();

			QJsonObject jobj;
			jobj.insert("image_id", image_id);
			jobj.insert("isDeleted", query.value("isDeleted").toInt());
			jobj.insert("owner_id", query.value("owner_id").toInt());
			jobj.insert("image_path", query.value("image_path").toString());
			jobj.insert("image_name", query.value("image_name").toString());
			jobj.insert("image_size", query.value("image_size").toInt());
			jobj.insert("image_format", query.value("image_format").toString());
			jobj.insert("image_share", query.value("image_share").toInt());
			jobj.insert("image_type", query.value("image_type").toString());
			jobj.insert("image_download", query.value("image_download").toInt());
			jobj.insert("image_ResolutionRatio", query.value("image_ResolutionRatio").toString());
			jobj.insert("image_quality", query.value("image_quality").toString());
			jobj.insert("upload_time", query.value("upload_time").toString());
			jobj.insert("description", query.value("description").toString());
			jobj.insert("like_count", query_everyImage_statistics.value("like_count").toInt());
			jobj.insert("star_count", query_everyImage_statistics.value("star_count").toInt());
			jobj.insert("download_count", query_everyImage_statistics.value("download_count").toInt());
			jobj.insert("comment_count", query_everyImage_statistics.value("comment_count").toInt());
			jarray.append(jobj);
		}

		if (jarray.isEmpty()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}

		QJsonObject jobj;
		jobj.insert("images", jarray);
		jobj.insert("cur_page", page);
		jobj.insert("page_size", pageSize);
		jobj.insert("last_page", last_page);
		jobj.insert("total_records", total_records);
		responder.write(SResult::success(jobj), "application/json");
		return;
		});

	//获取全部用户的图片总数
	m_server.route("/api/user/get_image_count", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare("SELECT has_image_count FROM user_info WHERE isDeleted=false AND isEnable=true");
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
		int total_image_count = 0; 
		while (query.next()) {
			total_image_count += query.value("has_image_count").toInt();
		}
#if _DEBUG
		qDebug() << "所有用户的图片总数:" << total_image_count;
#endif
		QJsonObject jobj;
		jobj.insert("total_image_count", total_image_count);
		responder.write(SResult::success(jobj), "application/json");
		return;
		});

	//获取某一张图片
	m_server.route("/api/user/get_one_image", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("image_id").isEmpty()) {
			responder.write(SResultCode::ParamMissing.toJson(), "application/json");
		}
		auto image_id = uquery.queryItemValue("image_id").toInt();

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT * FROM user_image WHERE image_id=%1").arg(image_id));
#if _DEBUG
		qDebug() << "获取某一张图片的信息";
		qDebug() << query.lastQuery();
#endif
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
		if (!query.next()) {
			responder.write(SResultCode::ImageAlreadyDeleted.toJson(), "application/json");
			return;
		}
		QJsonArray jarray;
		SSqlConnectionWrap wrap2;
		QSqlQuery query_everyImage_statistics(wrap2.openConnection());
		int i = 0;
		query_everyImage_statistics.prepare(QString("SELECT like_count,star_count,download_count,comment_count FROM image_statistics WHERE image_id=%1").arg(image_id)); //统计每一张图片的统计数据数
		if (!query_everyImage_statistics.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << query_everyImage_statistics.lastQuery();
#endif
		query_everyImage_statistics.next();

		QJsonObject jobj;
		jobj.insert("image_id", image_id);
		jobj.insert("isDeleted", query.value("isDeleted").toInt());
		jobj.insert("owner_id", query.value("owner_id").toInt());
		jobj.insert("image_path", query.value("image_path").toString());
		jobj.insert("image_name", query.value("image_name").toString());
		jobj.insert("image_size", query.value("image_size").toInt());
		jobj.insert("image_format", query.value("image_format").toString());
		jobj.insert("image_share", query.value("image_share").toInt());
		jobj.insert("image_type", query.value("image_type").toString());
		jobj.insert("image_download", query.value("image_download").toInt());
		jobj.insert("image_ResolutionRatio", query.value("image_ResolutionRatio").toString());
		jobj.insert("image_quality", query.value("image_quality").toString());
		jobj.insert("upload_time", query.value("upload_time").toString());
		jobj.insert("description", query.value("description").toString());
		jobj.insert("like_count", query_everyImage_statistics.value("like_count").toInt());
		jobj.insert("star_count", query_everyImage_statistics.value("star_count").toInt());
		jobj.insert("download_count", query_everyImage_statistics.value("download_count").toInt());
		jobj.insert("comment_count", query_everyImage_statistics.value("comment_count").toInt());
		jarray.append(jobj);

		if (jarray.isEmpty()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}

		QJsonObject jImage;
		jImage.insert("images", jarray);
		responder.write(SResult::success(jImage), "application/json");
		return;
		});

	//用户删除或恢复图片（一个或多个）
	m_server.route("/api/user/delete_orRestore_image", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request) {
		CheckJsonParse(request);

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto jarray = jdom["lists"].toArray();
		if (jarray.isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}
		auto isDeleted = jdom["isDeleted"].toBool();

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		for (int i = 0; i < jarray.size(); i++) {
			query.prepare(QString("UPDATE user_image SET isDeleted=%1 WHERE image_id=%2").arg(isDeleted).arg(jarray[i].toInt()));
			query.exec();
#if _DEBUG
			qDebug() << "图片删除";
			qDebug() << query.lastQuery();
#endif
			CheckSqlQuery(query);
		}

		//检查是否更新成功
		if (query.numRowsAffected() == 0) {
			return SResult::success(SResultCode::SuccessButNoData);
		}

		return SResult::success();
		});

	//用户图片下载: 下载某一张图片 GET
	m_server.route("/api/user/download_image", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("image_id").isEmpty()) {
			responder.write(SResultCode::ParamMissing.toJson(), "application/json");
			return;
		}
		auto image_id = uquery.queryItemValue("image_id");

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT image_path FROM user_image WHERE isDeleted=false AND image_id='%1'").arg(image_id));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG
		qDebug() << "用户图片下载";
		qDebug() << query.lastQuery();
#endif

		if (!query.next()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}

		auto relative_path = query.value("image_path").toString(); //../images/upload/1/1173012900_20250113175619_Spider man.png
		if (relative_path.isEmpty()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}
		QFile file(QDir::currentPath() + "/" + relative_path); //absolute path
		if (!file.open(QIODevice::ReadOnly)) {
			responder.write(SResult::error(SResultCode::ServerFileError, "头像未找到"), "application/json");
			return;
		}

		//更新统计表
		query.prepare(QString("UPDATE image_statistics SET download_count=download_count+1 WHERE image_id=%1").arg(image_id));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}

		responder.writeStatusLine();
		responder.writeHeader("Content-Type", QString("image/%1").arg(QFileInfo(file.fileName()).suffix()).toUtf8());
		responder.writeHeader("Content-Length", QByteArray::number(file.size()));
		responder.writeHeader("Content-Disposition", "attachment; filename=" + file.fileName().toUtf8());
		responder.writeBody(file.readAll());
		});

	//用户图片点赞：POST
	m_server.route("/api/user/like_image", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_id").isEmpty() || uquery.queryItemValue("image_id").isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		auto user_id = uquery.queryItemValue("user_id");
		auto image_id = uquery.queryItemValue("image_id");
		
		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		//首先查询点赞是否已经存在
		query.prepare(QString("SELECT * FROM image_like WHERE user_id=%1 AND image_id=%2").arg(user_id.toInt()).arg(image_id.toInt()));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query);
		if (query.next()) {
			//已经点赞过了，取消点赞
			query.prepare(QString("DELETE FROM image_like WHERE user_id=%1 AND image_id=%2").arg(user_id.toInt()).arg(image_id.toInt()));
			if (!query.exec()) {
				return SResult::error(SResultCode::ServerSqlQueryError);
			}
			CheckSqlQuery(query);
#if _DEBUG
			qDebug() << "用户取消图片点赞";
			qDebug() << query.lastQuery();
#endif
			return SResult::success(SResultCode::ImageUnliked);
		}

		SSqlConnectionWrap wrap2;
		QSqlQuery query2(wrap2.openConnection());
		//查询归属者
		query2.prepare(QString("SELECT owner_id FROM user_image WHERE image_id=%1").arg(image_id.toInt()));
		if (!query2.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query2);
		query2.next();
		auto owner_id = query2.value("owner_id").toInt();
		query.prepare(QString("INSERT IGNORE INTO image_like(user_id, image_id, owner_user_id, like_time) VALUES (%1,%2,%3,'%4')")
			.arg(user_id.toInt())
			.arg(image_id.toInt())
			.arg(owner_id)
			.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));

		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}

		CheckSqlQuery(query);

#if _DEBUG
		qDebug() << "用户图片点赞";
		qDebug() << query.lastQuery();
#endif

		//检查是否插入成功
		if (query.numRowsAffected() == 0) {
			return SResult::error(SResultCode::ImageLikedError);
		}

		return SResult::success(SResultCode::ImageLiked);
		});

	//检查用户是否点赞过图片：GET
	m_server.route("/api/user/like_image", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_id").isEmpty() || uquery.queryItemValue("image_id").isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		auto user_id = uquery.queryItemValue("user_id");
		auto image_id = uquery.queryItemValue("image_id");

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		//首先查询点赞是否已经存在
		query.prepare(QString("SELECT * FROM image_like WHERE user_id=%1 AND image_id=%2").arg(user_id.toInt()).arg(image_id.toInt()));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query);
		if (query.next()) {
			return SResult::success(SResultCode::ImageLiked);
		}

		return SResult::success(SResultCode::ImageUnliked);
		});

	//用户图片收藏：POST
	m_server.route("/api/user/star_image", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_id").isEmpty() || uquery.queryItemValue("image_id").isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		auto user_id = uquery.queryItemValue("user_id");
		auto image_id = uquery.queryItemValue("image_id");

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		//首先查询收藏是否已经存在
		query.prepare(QString("SELECT * FROM image_star WHERE user_id=%1 AND image_id=%2").arg(user_id.toInt()).arg(image_id.toInt()));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query);
		if (query.next()) {
			//已经收藏过了，取消收藏
			query.prepare(QString("DELETE FROM image_star WHERE user_id=%1 AND image_id=%2").arg(user_id.toInt()).arg(image_id.toInt()));
			if (!query.exec()) {
				return SResult::error(SResultCode::ServerSqlQueryError);
			}
			CheckSqlQuery(query);
#if _DEBUG
			qDebug() << "用户取消图片收藏";
			qDebug() << query.lastQuery();
#endif
			return SResult::success(SResultCode::ImageUnStared);
		}

		SSqlConnectionWrap wrap2;
		QSqlQuery query2(wrap2.openConnection());
		//查询归属者
		query2.prepare(QString("SELECT owner_id FROM user_image WHERE image_id=%1").arg(image_id.toInt()));
		if (!query2.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query2);
		query2.next();
		auto owner_id = query2.value("owner_id").toInt();
		query.prepare(QString("INSERT IGNORE INTO image_star(user_id, image_id,owner_user_id, star_time) VALUES (%1,%2,%3,'%4')")
			.arg(user_id.toInt())
			.arg(image_id.toInt())
			.arg(owner_id)
			.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));

		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}

		CheckSqlQuery(query);

#if _DEBUG
		qDebug() << "用户图片收藏";
		qDebug() << query.lastQuery();
#endif

		//检查是否插入成功
		if (query.numRowsAffected() == 0) {
			return SResult::error(SResultCode::ImageStaredError);
		}

		return SResult::success(SResultCode::ImageStared);
		});

	//检查用户是否收藏过图片：GET
	m_server.route("/api/user/star_image", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_id").isEmpty() || uquery.queryItemValue("image_id").isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		auto user_id = uquery.queryItemValue("user_id");
		auto image_id = uquery.queryItemValue("image_id");

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		//首先查询点赞是否已经存在
		query.prepare(QString("SELECT * FROM image_star WHERE user_id=%1 AND image_id=%2").arg(user_id.toInt()).arg(image_id.toInt()));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query);
		if (query.next()) {
			return SResult::success(SResultCode::ImageStared);
		}

		return SResult::success(SResultCode::ImageUnStared);
		});

	//用户评论：POST
	m_server.route("/api/user/comment_image", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_id").isEmpty() || uquery.queryItemValue("image_id").isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		auto user_id = uquery.queryItemValue("user_id");
		auto image_id = uquery.queryItemValue("image_id");
		auto comment_content = uquery.queryItemValue("comment_content");
		if (comment_content.isEmpty()) {
			comment_content = "这张图片真不错！";//默认评论
		}

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		SSqlConnectionWrap wrap2;
		QSqlQuery query2(wrap2.openConnection());
		//查询归属者
		query2.prepare(QString("SELECT owner_id FROM user_image WHERE image_id=%1").arg(image_id.toInt()));
		if (!query2.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query2);
		query2.next();
		auto owner_id = query2.value("owner_id").toInt();
		query.prepare(QString("INSERT IGNORE INTO image_comment(image_id, user_id,owner_user_id,comment_content, comment_time) VALUES (%1,%2,%3,'%4','%5')")
			.arg(image_id.toInt())
			.arg(user_id.toInt())
			.arg(owner_id)
			.arg(comment_content)
			.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query);

#if _DEBUG
		qDebug() << "用户图片评论";
		qDebug() << query.lastQuery();
#endif

		//检查是否插入成功
		if (query.numRowsAffected() == 0) {
			return SResult::error(SResultCode::ImageCommentError);
		}

		auto comment_id = query.lastInsertId().toInt(); //获取刚插入的评论id

		return SResult::success({ {"comment_id",comment_id} });
		});
	
	//获取某个图片的全部评论，以及对应的所有评论者信息：GET
	m_server.route("/api/user/get_comment", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("image_id").isEmpty()) {
			responder.write(SResultCode::ParamMissing.toJson(), "application/json");
			return;
		}
		auto image_id = uquery.queryItemValue("image_id").toInt();

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		//按照优先级排序（评论置顶功能）
		query.prepare(QString("SELECT * FROM image_comment WHERE image_id=%1 AND isDeleted=false ORDER BY priority DESC;")
			.arg(image_id));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
		QJsonArray jarray;
		SSqlConnectionWrap wrap2;
		QSqlQuery query_user_info(wrap2.openConnection());
		int i = 0;
		while (query.next()) {
			auto comment_id = query.value("comment_id").toInt(); //获取每个评论的id
			auto user_id = query.value("user_id").toInt(); //获取每个评论对应的用户id
			auto comment_content = query.value("comment_content").toString(); //获取每个评论对应的内容
			auto priority = query.value("priority").toInt(); //获取每个评论的优先级
			auto comment_time = query.value("comment_time").toDateTime().toString("yyyy-MM-dd HH:mm:ss"); //获取每个评论的时间
			query_user_info.prepare(QString("SELECT user_name,avatar_path FROM user_info WHERE id=%1").arg(user_id)); //获取评论者的重要信息
			if (!query_user_info.exec()) {
				responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
#if _DEBUG	
			qDebug() << query_user_info.lastQuery();
#endif
			query_user_info.next();

			QJsonObject jobj;
			jobj.insert("comment_id", comment_id);
			jobj.insert("priority", priority);
			jobj.insert("comment_content", comment_content);
			jobj.insert("comment_time", comment_time);
			jobj.insert("user_name", query_user_info.value("user_name").toString());
			jobj.insert("avatar_path", query_user_info.value("avatar_path").toString());
			jarray.append(jobj);
		}

		if (jarray.isEmpty()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}

		QJsonObject jobj;
		jobj.insert("users", jarray);
		responder.write(SResult::success(jobj), "application/json");
		});

	//图片所有者可以删除某一条评论：POST
	m_server.route("/api/user/delete_comment", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("owner_id").isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		auto delete_comment_user_id = uquery.queryItemValue("owner_id").toInt();//执行删除操作的用户id
		auto comment_id = uquery.queryItemValue("comment_id").toInt();
	
		//根据评论id来找到对应的图片id
		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT image_id,owner_user_id FROM image_comment WHERE comment_id=%1").arg(comment_id));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query);
#if _DEBUG
		qDebug() << query.lastQuery();
#endif
		query.next();
		auto image_id = query.value("image_id").toInt(); 
		auto owner_id = query.value("owner_user_id").toInt();

		if (delete_comment_user_id != owner_id) {
			//不是图片的所有者，不能删除评论
			return SResult::error(SResultCode::UserAuthForbidden);
		}

		//删除评论
		query.prepare(QString("UPDATE image_comment SET isDeleted=1 WHERE comment_id=%1").arg(comment_id));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
#if _DEBUG
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		return SResult::success();
		});

	//图片所有者可以置顶某一条评论：POST
	m_server.route("/api/user/top_comment", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("owner_id").isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		auto top_comment_user_id = uquery.queryItemValue("owner_id").toInt();//执行操作的用户id
		auto comment_id = uquery.queryItemValue("comment_id").toInt();

		//根据评论id来找到对应的图片id
		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT image_id,owner_user_id FROM image_comment WHERE comment_id=%1").arg(comment_id));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
		CheckSqlQuery(query);
#if _DEBUG
		qDebug() << query.lastQuery();
#endif
		query.next();
		auto image_id = query.value("image_id").toInt();
		auto owner_id = query.value("owner_user_id").toInt();
		if (top_comment_user_id != owner_id) {
			//不是图片的所有者，不能置顶评论
			return SResult::error(SResultCode::UserAuthForbidden);
		}

		//取消原来的置顶评论
		query.prepare(QString("UPDATE image_comment SET priority = 0 WHERE image_id = %1 AND priority = 999;").arg(image_id));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
#if _DEBUG
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		//置顶新的评论
		query.prepare(QString("UPDATE image_comment SET priority = 999 WHERE comment_id = %1;").arg(comment_id));
		if (!query.exec()) {
			return SResult::error(SResultCode::ServerSqlQueryError);
		}
#if _DEBUG
		qDebug() << query.lastQuery();
#endif
		CheckSqlQuery(query);

		return SResult::success();
		});

	//获取某个用户主的全部活动 GET
	m_server.route("/api/user/get_activities", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {

		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			responder.write(token.value(), "application/json");
			return;
		}

		auto uquery = request.query();
		if (uquery.queryItemValue("user_id").isEmpty()) {
			responder.write(SResultCode::ParamMissing.toJson(), "application/json");
			return;
		}
		auto owner_user_id = uquery.queryItemValue("user_id").toInt();
		auto page = uquery.queryItemValue("page").toInt();
		auto pageSize = uquery.queryItemValue("pageSize").toInt();

		if (pageSize == 0) {
			pageSize = 7;
		}

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		//确保筛选后能得到正确的页数信息
		query.prepare(QString("SELECT COUNT(*) as total FROM user_activities where owner_user_id=%1").arg(owner_user_id));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << query.lastQuery();
#endif

		query.next();
		auto total_records = query.value("total").toInt(); //活动总数
		auto last_page = total_records / pageSize + (total_records % pageSize ? 1 : 0);
		if (page < 1 || page > last_page) {
			page = 1;
		}

		//查询活动
		QString sql = QString("SELECT * FROM user_activities WHERE owner_user_id=%1 ORDER BY created_time desc ").arg(owner_user_id);
		sql += QString(" LIMIT %1,%2").arg((page - 1) * pageSize).arg(pageSize);
		query.prepare(sql);
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << "全部活动: ";
		qDebug() << query.lastQuery();
#endif

		QJsonArray jarray;
		SSqlConnectionWrap wrap2;
		QSqlQuery query_image_info(wrap2.openConnection());
		SSqlConnectionWrap wrap3;
		QSqlQuery query_user_info(wrap3.openConnection());
		while (query.next()) {
			auto actor_user_id = query.value("actor_user_id").toInt(); //每个活动的发送者id
			auto image_id = query.value("image_id").toInt(); //每个活动的目标图片id

			//获取图片名字
			query_image_info.prepare(QString("SELECT image_name FROM user_image WHERE image_id=%1").arg(image_id)); 
			if (!query_image_info.exec()) {
				responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
#if _DEBUG	
			qDebug() << query_image_info.lastQuery();
#endif
			query_image_info.next();

			//获取活动发送者的名字和头像
			query_user_info.prepare(QString("SELECT user_name,avatar_path FROM user_info WHERE id=%1").arg(actor_user_id));
			if (!query_user_info.exec()) {
				responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
#if _DEBUG	
			qDebug() << query_user_info.lastQuery();
#endif
			query_user_info.next();


			QJsonObject jobj;
			jobj.insert("user_name", query_user_info.value("user_name").toString());
			jobj.insert("avatar_path", query_user_info.value("avatar_path").toString());
			jobj.insert("image_name", query_image_info.value("image_name").toString());
			jobj.insert("activity_type", query.value("activity_type").toString());
			jobj.insert("created_time", query.value("created_time").toDateTime().toString("yyyy-MM-dd HH:mm:ss"));
			jarray.append(jobj);
		}

		if (jarray.isEmpty()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}

		QJsonObject jobj;
		jobj.insert("images", jarray);
		jobj.insert("cur_page", page);
		jobj.insert("page_size", pageSize);
		jobj.insert("last_page", last_page);
		jobj.insert("total_records", total_records);
		responder.write(SResult::success(jobj), "application/json");
		return;
		});
}

