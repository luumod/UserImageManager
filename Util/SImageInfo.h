#ifndef SIMAGEINFO_H_
#define SIMAGEINFO_H_

class ImageInfo {
public:
	int image_id{};
	int image_ower_id{};
	QString image_path{};
	QString image_name{};
	QString image_desc{};
	ImageInfo(){}
	ImageInfo(int image_id, int image_ower_id, const QString& image_path, const QString& image_name, const QString& image_desc)
		:image_id(image_id), image_ower_id(image_ower_id), image_path(image_path), image_name(image_name), image_desc(image_desc) {
	}
};

#endif