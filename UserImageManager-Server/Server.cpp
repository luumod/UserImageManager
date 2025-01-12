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
		query.prepare("INSERT INTO user_info(user_id,password,user_name,gender,mobile,email) VALUES(?,?,?,?,?,?)");
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
		query.prepare(QString("SELECT * FROM user_info WHERE user_id='%1' AND password='%2' LIMIT 1")
		.arg(jdom["user_id"].toString())
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
			{"user_id", jdom["user_id"]},
			{"iat",QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate)},
			{"exp",QDateTime::currentDateTime().addDays(7).toString(Qt::DateFormat::ISODate)}
		};
		SJwt::SJwtObject jwtObject(SJwt::SAlgorithm::HS256, payLoad, SCRETKEY);


		QJsonObject jLoginUser;
		jLoginUser.insert("user_id", query.value("user_id").toString());
		jLoginUser.insert("user_name", query.value("user_name").toString());
		jLoginUser.insert("password", query.value("password").toString());
		jLoginUser.insert("gender", query.value("gender").toInt());
		jLoginUser.insert("mobile", query.value("mobile").toString());
		jLoginUser.insert("email", query.value("email").toString());
		jLoginUser.insert("avatar_path", query.value("avatar_path").toString());
		jLoginUser.insert("isEnable", true);
		jLoginUser.insert("isDeleted", false);
		jLoginUser.insert("token", QString(jwtObject.jwt()));

		//{ {"token",QString(jwtObject.jwt())} }
		return SResult::success(jLoginUser); //回复用户登录时生成的token（QJsonObject形式）
		});

	//用户注册
	m_server.route("/api/register", [](const QHttpServerRequest& request) {
		CheckJsonParse(request);

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare("INSERT IGNORE INTO user_info (user_id, password, user_name, gender) VALUES (?,?,?,?)");
		query.bindValue(0, jdom["user_id"].toString());
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
		auto user_id = uquery.queryItemValue("user_id");
		auto user_name = uquery.queryItemValue("user_name");
		auto mobile = uquery.queryItemValue("mobile");
		auto email = uquery.queryItemValue("email");
		auto gender = uquery.queryItemValue("gender");

		QString filter = "WHERE isDeleted=false ";
		//模糊搜索
		if (!user_id.isEmpty()) {
			filter += QString(" AND user_id LIKE '%%1%'").arg(user_id);
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

		QString sql = "SELECT id,user_id,user_name,gender,mobile,email,isEnable FROM user_info ";
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

	//用户查询(精确查询:user_id)
	m_server.route("/api/user/queryUser", [](const QHttpServerRequest& request) {
		//校验参数
		std::optional<QByteArray> token = CheckToken(request);
		if (token.has_value()) { //token校验失败
			return token.value();
		}

		auto uquery = request.query();
		auto user_id = uquery.queryItemValue("user_id");
		auto isEnable = uquery.queryItemValue("isEnable");
		QString filter = "WHERE isDeleted=false ";
		//通过user_id的查询
		if (!user_id.isEmpty()) {
			filter += QString(" and user_id='%1' ").arg(user_id);
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
		qDebug() << "精确查找:" << user_id;
		qDebug() << query.lastQuery();
#endif
		/*query.next();
		auto id = query.record().value("id").toInt();
		auto user_id = query.record().value("user_id").toInt();;
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
		if (!rObj.contains("user_id") ||
			!rObj.contains("user_name")) {
			return SResult::error(SResultCode::ParamMissing);
		}
		auto password = rObj.value("password").toString();
		if (password.isEmpty()) {
			password = "123456"; //默认密码
		}

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		QString sql = QString("INSERT IGNORE INTO user_info (user_id,password,user_name,gender,mobile,email) VALUES('%1','%2','%3',%4,'%5','%6')")
			.arg(jdom["user_id"].toString())
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
		query.prepare("UPDATE user_info SET isDeleted=true WHERE user_id=?");
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
		auto user_id = uquery.queryItemValue("user_id");
		if (user_id.isEmpty()) {
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
		query.prepare(QString(sql + " WHERE user_id='%1'").arg(user_id));
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
				else if (it.key() == "user_id") {
					filter += QString("user_id='%1' and ").arg(it.value().toString());
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
		if (uquery.queryItemValue("user_id").isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}
		auto user_id = uquery.queryItemValue("user_id");

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
		QFile file(QString(path + user_id + "." + QFileInfo(parse.filename()).suffix()));
		if (!file.open(QIODevice::WriteOnly)) {
			return SResult::error(SResultCode::ServerFileError);
		}
		file.write(parse.data());
		//把路径写入数据库
		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("UPDATE user_info SET avatar_path='%1' WHERE user_id='%2'").arg(file.fileName()).arg(user_id));
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
		if (uquery.queryItemValue("user_id").isEmpty()) {
			responder.write(SResultCode::ParamMissing.toJson(), "application/json");
			return;
		}
		auto user_id = uquery.queryItemValue("user_id");

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT avatar_path FROM user_info WHERE user_id='%1' AND isDeleted=false").arg(user_id));
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
		//根据user_id查找user表中的id
		auto user_id = uquery.queryItemValue("user_id"); //eg. user_id = 1173012900
		if (user_id.isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT id,isEnable,isDeleted FROM user_info WHERE user_id='%1'").arg(user_id));
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

		auto ower_user_id = query.value("id").toInt(); //eg. ower_user_id <==> user: id 
		auto description = uquery.queryItemValue("description");
		if (description.isEmpty()) {
			description = "";
		}
		auto data = request.body();
		if (data.isEmpty()) {
			return SResult::error(SResultCode::ParamMissing);
		}
		auto parse = SHttpPartParse(data);
		if (!parse.parse()) {
			return SResult::error(SResultCode::ParamInvalid);
		}
		auto path = QString("../images/upload/%1/").arg(ower_user_id);
		QDir dir;
		if (!dir.exists(path)) {
			dir.mkpath(path);
		}
		//图片路径格式：images/upload/ower_user_id/user_id_时间戳_图片原名称.后缀名
		QFile file(QString(path + user_id 
			+ "_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") 
			+ "_" + QFileInfo(parse.filename()).fileName()));
		if (!file.open(QIODevice::WriteOnly)) {
			return SResult::error(SResultCode::ServerFileError);
		}
		file.write(parse.data());
		//把路径写入数据库
		query.prepare(QString("INSERT IGNORE INTO user_image (ower_user_id, image_path,image_name, image_size,image_type,upload_time,description) VALUES (%1,'%2','%3',%4,'%5','%6','%7')")
			.arg(ower_user_id).arg(file.fileName()).arg(QFileInfo(parse.filename()).fileName())
			.arg(file.size()).arg(QFileInfo(parse.filename()).suffix())
			.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")).arg(description));
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

		return SResult::success();
		});

	//用户图片获取: GET
	m_server.route("/api/user/get_image", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& responder) {

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
		auto user_id = uquery.queryItemValue("user_id").toInt();

		SSqlConnectionWrap wrap;
		QSqlQuery query(wrap.openConnection());
		query.prepare(QString("SELECT id FROM user_info WHERE user_id=%1 AND isDeleted=false AND isEnable=true").arg(user_id));
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}
#if _DEBUG	
		qDebug() << "获取用户id";
		qDebug() << query.lastQuery();
#endif
		query.next();
		if (query.value("id").isNull()) {
			responder.write(SResultCode::UserNotFound.toJson(), "application/json");
			return;	
		}
		auto ower_user_id = query.value("id").toInt(); //获取用户id
		query.prepare(QString("SELECT image_id,image_path,ower_user_id,image_name,image_size,image_type,upload_time,description FROM user_image WHERE ower_user_id=%1")
			.arg(ower_user_id));
#if _DEBUG	
		qDebug() << "用户图片获取";
		qDebug() << query.lastQuery();
#endif
		if (!query.exec()) {
			responder.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
			return;
		}

		QJsonArray jarray;
		while (query.next()) {
			QJsonObject jobj;
			jobj.insert("image_id", query.value("image_id").toInt());
			jobj.insert("ower_user_id", query.value("ower_user_id").toInt());
			jobj.insert("image_path", query.value("image_path").toString());
			jobj.insert("image_name", query.value("image_name").toString());
			jobj.insert("image_size", query.value("image_size").toInt());
			jobj.insert("image_type", query.value("image_type").toString());
			jobj.insert("upload_time", query.value("upload_time").toString());
			jobj.insert("description", query.value("description").toString());
			jarray.append(jobj);
		}

		if (jarray.isEmpty()) {
			responder.write(SResultCode::SuccessButNoData.toJson(), "application/json");
			return;
		}

		QJsonObject jobj;
		jobj.insert("images", jarray);
		responder.write(SResult::success(jobj), "application/json");
		});
}

