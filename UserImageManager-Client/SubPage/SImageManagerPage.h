#ifndef SImageManagerPage_H_
#define  SImageManagerPage_H_

#include <QWidget>
#include <QStandardItemModel>
#include <QModelIndex>

class SUserDetailView;
class SUserAddView;
class SUserEditView;
class SImageDetailView;
class SImageModifyView;
class QTableView;
class QLineEdit;
class QPushButton;
class QComboBox;
class SImageManagerPage : public QWidget
{
	Q_OBJECT
public:
	SImageManagerPage(QWidget* parent = nullptr);
	~SImageManagerPage();
	void init();
	void onSearch();
	void showPage();
protected:
	void resizeEvent(QResizeEvent* e) override;
signals:
	void pageChanged(int pageId);
	void totalRecordsCountChanged(int totalCounts);
	void totalPagesCountChanged(int totalPages);
private:
	void parseJson(const QJsonObject& obj);
	void checkPageValidity();
private:
	int column(const QString& field);
	QList<QStandardItem*> createItems(const QJsonObject& obj);
	QStringList m_fieldName = { "","image_id","image_name","image_type","image_path","image_format","image_size","image_ResolutionRatio","image_share","image_download","isDeleted","operation" };
	QStandardItemModel* m_model{};
	QModelIndex m_currentIndex;

	SImageDetailView* m_imageDetailView{};
	SImageModifyView* m_imageModifyView{};

	//布局
	QLineEdit* m_searchEdit_imageName{};
	QLineEdit* m_searchEdit_imageType{};
	QComboBox* m_searchCombo_imageFormat{};
	QComboBox* m_searchCombo_imageShare{};
	QComboBox* m_searchCombo_imageDownload{};

	QTableView* m_tableView{};

	//----界面相关
	int m_currentPage = 1;
	int m_pageSize = 6;
	int m_lastPage{};   //最后一页
	int m_totalCount{}; //图片总数
};

#endif //!  SImageManagerPage_H_