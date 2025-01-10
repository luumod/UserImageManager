#ifndef PERSONALSPACE_H
#define PERSONALSPACE_H
#include <QWidget>
#include <QStandardItemModel>

class QStackedWidget;
class SImageShowWidget;
class QGridLayout;
class QVBoxLayout;
class PersonalSpace : public QWidget
{
	Q_OBJECT
public:
	PersonalSpace(QWidget* parent = nullptr);
	~PersonalSpace();
	void init();
	void onSearch();
	void loadImage(const QJsonArray& imagesArray);

public slots:
	void onChangePrePage();
	void onChangeNextPage();
signals:
	void parseFinised();
private:
	void initParase(const QJsonObject& obj);
	void parseJson();
private:
	QStandardItemModel* m_model{};

	QList<SImageShowWidget*> m_images;

	QVBoxLayout* main_layout{};
	QGridLayout* images_layout{};

	struct ImageInfo {
		int id;
		int ower_id;
		QString path;
		QString name;
		QString desc;
	};

	int m_firstImageIndex = 0;
	int m_lastImageIndex = 5;
	int m_currentPage = 1;
	QMap<int, ImageInfo> m_imagesInfoMap;


};

#endif // PERSONALSPACE_H