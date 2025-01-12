#ifndef PERSONALSPACE_H
#define PERSONALSPACE_H

#include <QWidget>
#include <QStandardItemModel>
#include "SImageInfo.h"

class QStackedWidget;
class SImageShowWidget;
class QGridLayout;
class QVBoxLayout;
class SImageDetailDlg;
class QHBoxLayout;
class PersonalSpace : public QWidget
{
	Q_OBJECT
public:
	PersonalSpace(QWidget* parent = nullptr);
	~PersonalSpace();
	void init();
	void onSearch();
protected:
	void resizeEvent(QResizeEvent* e) override;
public slots:
	void onChangePrePage();
	void onChangeNextPage();
	void onClickedOneImage(int id);
signals:
	void parseFinised();
private:
	void initParase(const QJsonObject& obj);
	void loadImage(const QJsonArray& imagesArray);
	void parseJson();
private:
	QStandardItemModel* m_model{};

	QList<SImageShowWidget*> m_images;

	QVBoxLayout* main_layout{};
	//QGridLayout* images_layout{};
	QHBoxLayout* images_layout_1{};
	QHBoxLayout* images_layout_2{};
	QHBoxLayout* images_layout_3{};

	int m_firstImageIndex = 0;
	int m_lastImageIndex = 5;
	int m_currentPage = 0;  //默认第0页 
	QMap<int, ImageInfo> m_imagesInfoMap;

	SImageDetailDlg* m_imageDetailDlg{};
};

#endif // PERSONALSPACE_H