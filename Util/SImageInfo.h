#ifndef SIMAGEINFO_H_
#define SIMAGEINFO_H_

#include <QJsonObject>

class ImageInfo {
public:
	int m_id{};
	int m_owner_id{}; //user_account

	QString m_path{}; //absolute path
	QString m_name{};
	int m_size{};
	QString m_format{};
	int m_share{};
	QString m_type{};
	int m_download{};
	QString m_ResolutionRatio{};
	QString m_upload_time{};
	QString m_desc{};

	int m_likeCount{};//点赞数
	int m_starCount{};//收藏数
	int m_downloadCount{};//下载数
	int m_commentCount{};//评论数

	bool isEmpty = true;

	ImageInfo() {}
	ImageInfo(const QJsonObject& jImage) {
		m_id = jImage.value("image_id").toInt();
		m_owner_id = jImage.value("owner_id").toInt();   // id
		m_path = jImage.value("image_path").toString();
		m_name = jImage.value("image_name").toString();
		m_size = jImage.value("image_size").toInt();
		m_format = jImage.value("image_format").toString();
		m_share = jImage.value("image_share").toInt();
		m_type = jImage.value("image_type").toString();
		m_download = jImage.value("image_download").toInt();
		m_ResolutionRatio = jImage.value("image_ResolutionRatio").toString();
		m_upload_time = jImage.value("upload_time").toString();
		m_desc = jImage.value("description").toString();
		m_likeCount = jImage.value("like_count").toInt();
		m_starCount = jImage.value("star_count").toInt();
		m_downloadCount = jImage.value("download_count").toInt();
		m_commentCount = jImage.value("comment_count").toInt();

		isEmpty = false;
	}

	ImageInfo(int id, int owner_id, const QString& path, const QString& name, int size, const QString& format, int share, const QString& type, int download, const QString& resolutionRatio, const QString& upload_time, const QString& desc, int likeCount, int starCount, int downloadCount, int commentCount)
		: m_id(id), m_owner_id(owner_id), m_path(path), m_name(name), m_size(size), m_format(format), m_share(share), m_type(type), m_download(download), m_ResolutionRatio(resolutionRatio), m_upload_time(upload_time), m_desc(desc), m_likeCount(likeCount), m_starCount(starCount), m_downloadCount(downloadCount), m_commentCount(commentCount) {
		isEmpty = false;
	}
	inline void set_image_id(int id) { m_id = id; }
	inline void set_image_owner_id(int owner_id) { m_owner_id = owner_id; }
	inline void set_image_path(const QString& path) { m_path = path; }
	inline void set_image_name(const QString& name) { m_name = name; }
	inline void set_image_size(int size) { m_size = size; }
	inline void set_image_format(const QString& format) { m_format = format; }
	inline void set_image_share(int share) { m_share = share; }
	inline void set_image_type(const QString& type) { m_type = type; }
	inline void set_image_download(int download) { m_download = download; }
	inline void set_image_ResolutionRatio(const QString& resolutionRatio) { m_ResolutionRatio = resolutionRatio; }
	inline void set_image_upload_time(const QString& upload_time) { m_upload_time = upload_time; }
	inline void set_image_desc(const QString& desc) { m_desc = desc; }
	inline void set_image_likeCount(int likeCount) { m_likeCount = likeCount; }
	inline void set_image_starCount(int starCount) { m_starCount = starCount; }
};

#endif