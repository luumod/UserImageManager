#ifndef USERMANAGERPAGE_H_
#define  USERMANAGERPAGE_H_

#include <QWidget>
#include <QStandardItemModel>
#include <QModelIndex>

class UserDetailsDlg;
class UserAddDlg;
class UserEditDlg;
class QTableView;
class QLineEdit;
class QPushButton;
class QComboBox;
class UserManagerPage : public QWidget
{
	Q_OBJECT
public:
	UserManagerPage(QWidget* parent = nullptr);
	~UserManagerPage();
	void init();

	void setBatchEnabeld(bool enable);

	void showUserDetails(const QModelIndex& index);
	void showUserEditDlg(const QModelIndex& index);
protected:
	void resizeEvent(QResizeEvent* event)override;
public slots:
	void onSearch();
	void onBatchEnable();
	void onBatchDisable();
	void onBatchDelete();
private:
	void parseJson(const QJsonObject& obj);
private:

	int column(const QString& field);
	QList<QStandardItem*> createItems(const QJsonObject& obj);
	QStringList m_fieldName = { "","id","user_id","user_name","gender","mobile","email","isEnable","operation" };
	QStandardItemModel* m_model{};
	QModelIndex m_currentIndex;

	UserDetailsDlg* m_detailsDlg{};
	UserEditDlg* m_userEditDlg{};
	UserAddDlg* m_userAddDlg{};

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
};

#endif //!  USERMANAGERPAGE_H_