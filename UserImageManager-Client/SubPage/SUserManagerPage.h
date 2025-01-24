#ifndef USERMANAGERPAGE_H_
#define  USERMANAGERPAGE_H_

#include <QWidget>
#include <QStandardItemModel>
#include <QModelIndex>

class SUserDetailView;
class SUserAddView;
class SUserEditView;
class QTableView;
class QLineEdit;
class QPushButton;
class QComboBox;
class SUserManagerPage : public QWidget
{
	Q_OBJECT
public:
	SUserManagerPage(QWidget* parent = nullptr);
	~SUserManagerPage();
	void init();
	void showPage();

	void setBatchEnabeld(bool enable);
	void showUserDetails(const QModelIndex& index);
	void showUserEditDlg(const QModelIndex& index);
protected:
	void resizeEvent(QResizeEvent* event)override;
signals:
	void pageChanged(int pageId);
	void totalRecordsCountChanged(int totalCounts);
	void totalPagesCountChanged(int totalPages);
public slots:
	void onSearch();
	void onBatchEnable();
	void onBatchDisable();
	void onBatchDelete();
private:
	void parseJson(const QJsonObject& obj);
	void checkPageValidity();
private:

	int column(const QString& field);
	QList<QStandardItem*> createItems(const QJsonObject& obj);
	QStringList m_fieldName = { "","id","user_account","user_name","gender","mobile","email","isEnable","operation" };
	QStandardItemModel* m_model{};
	QModelIndex m_currentIndex;

	SUserDetailView* m_detailsDlg{};
	SUserEditView* m_userEditDlg{};
	SUserAddView* m_userAddDlg{};

	//布局
	QLineEdit* m_searchEdit_userid{};
	QLineEdit* m_searchEdit_username{};
	QLineEdit* m_searchEdit_mobile{};
	QLineEdit* m_searchEdit_email{};
	QComboBox* m_searchCombo_gender{};
	QPushButton* m_searchBtn{};

	QPushButton* m_userAddBtn{};
	QPushButton* m_batchEnableBtn{};
	QPushButton* m_batchDisableBtn{};
	QPushButton* m_batchDeleteBtn{};

	QTableView* m_tableView{};

	//分页
	int m_currentPage = 1;
	int m_pageSize = 20;
	int m_lastPage{};
	int m_totalCount{};
};

#endif //!  USERMANAGERPAGE_H_