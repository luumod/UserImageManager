#ifndef PERSONALSPACE_H
#define PERSONALSPACE_H

#include <QWidget>
#include <QStandardItemModel>
#include "SImageInfo.h"

class QStackedWidget;
class SImageShowWidget;
class QGridLayout;
class QVBoxLayout;
class SImageDetailView;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class SPersonalImagePage : public QWidget
{
	Q_OBJECT
public:
	SPersonalImagePage(QWidget* parent = nullptr);
	~SPersonalImagePage();
	void init();
	void onSearch();
	void showPage();
	void updateLayouts();;
protected:
	void resizeEvent(QResizeEvent* e) override;
public slots:
	void onChangePrePage();
	void onChangeNextPage();
	void onClickedOneImage(int id);
	void onChangeNextImage(int next_id);
	void onChangePrevImage(int prev_id);
signals:
	void parseFinised();
private:
	void initImageShowWidgets();
	void initParase(const QJsonObject& obj);
	void loadImage(const QJsonArray& imagesArray);
	void parseJson();
private:
	QStandardItemModel* m_model{};

	QList<SImageShowWidget*> m_images;
	QLabel* m_totalLab{};
	QLineEdit* m_pageEdit{};
	QLabel* m_totalPageLab{};

	QVBoxLayout* main_layout{};
	QHBoxLayout* images_layout_1{};
	QHBoxLayout* images_layout_2{};
	QHBoxLayout* images_layout_3{};

	int m_currentPage = 1;  // 1 --> m_totalPage
	int m_totalPage = 0;    //总页数从1开始
	int m_totalImages = 0; //总图片数量
	QMap<int, ImageInfo> m_imagesInfoMap;

	SImageDetailView* m_imageDetailPage{};
};

#endif // PERSONALSPACE_H