#ifndef S_RESULT_CODE_H
#define S_RESULT_CODE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QString>

/*
自定义的错误码，用于返回给客户端，格式为json
*/
class SResultCode {
private:
	struct Code {
		int code;
		const char* codestr;
		const char* message;
		Code(int c, const char* cs, const char* m) : code(c), codestr(cs), message(m) {}
		QByteArray toJson()const {
			QJsonObject json;
			json.insert("code", code);
			json.insert("codestr", codestr);
			json.insert("message", message);
			return QJsonDocument(json).toJson(QJsonDocument::Compact);
		}
		operator QJsonValue()const {
			return code;
		}
	};
public:
#define CODE(name, code, message) inline static Code name = Code(code,#name,message)
	//成功操作[0-999]
	CODE(Success, 0, "成功");
	CODE(SuccessButNoData, 1, "成功，但没有数据");
	CODE(ImageLiked, 2, "成功，点赞成功");
	CODE(ImageUnliked, 3, "成功，取消点赞");
	CODE(ImageStared, 4, "成功，收藏成功");
	CODE(ImageUnStared, 5, "成功，取消收藏");
	CODE(ImageCommented, 6, "成功，评论成功");
	//参数错误[1000-1999]
	CODE(ParamJsonInvalid, 1000, "请求Json参数无效");
	CODE(ParamInvalid, 1001, "参数非法");
	CODE(ParamMissing, 1002, "参数缺失");
	//服务器错误[2000-2999]
	CODE(ServerSqlQueryError, 2000, "sql 执行失败");
	CODE(ServerFileError, 2001, "文件操作失败");
	//用户错误[3000-3999]
	CODE(UserDisabled, 3000, "账户已经被禁用");
	CODE(UserLoginError, 3001, "账号或密码错误");
	CODE(UserAlreadyExists, 3002, "用户已存在");
	CODE(UserNotFound, 3003, "用户不存在");
	CODE(UserUnauthenticated, 3004, "用户未认证,请先登录");
	CODE(UserAuthFormatError, 3005, "用户认证格式错误，仅支持bearer");
	CODE(UserAuthFailed, 3007, "用户认证失败");
	CODE(UserAuthForbidden, 3008, "用户无权限访问");
	CODE(UserAuthInvalid, 3009, "用户认证无效");
	CODE(UserAuthExpired, 3006, "用户认证已过期");
	CODE(UserDeleted, 3010, "用户已被删除");
	CODE(ImageLikedError, 3011, "点赞失败");
	CODE(ImageStaredError, 3012, "收藏失败");
	CODE(ImageCommentError, 3013, "评论失败");
	CODE(ImageNotFound, 3014, "图片不存在");
	CODE(ImageAlreadyDeleted, 3015, "图片已被删除");
#undef CODE

	friend class SResult;
};

class SResult {
public:
	static QByteArray success() {
		return SResultCode::Success.toJson();
	}
	static QByteArray success(const SResultCode::Code& code) {
		return code.toJson();
	}
	static QByteArray success(const QJsonObject& data) {
		QJsonObject obj;
		obj.insert("code", SResultCode::Success.code);
		obj.insert("message", SResultCode::Success.message);
		obj.insert("data", data);
		return QJsonDocument(obj).toJson(QJsonDocument::Compact);
	}

	static QByteArray error(const SResultCode::Code& code) {
		return code.toJson();
	}
	static QByteArray error(const SResultCode::Code& code, const QString& message) {
		QJsonObject obj;
		obj.insert("code", code.code);
		obj.insert("message", message);
		return QJsonDocument(obj).toJson(QJsonDocument::Compact);
	}
};

#endif // S_RESULT_CODE_H