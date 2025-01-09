#ifndef USERMANAGERPAGE_H_
#define  USERMANAGERPAGE_H_

#include <QWidget>
#include <QStandardItemModel>
#include <QModelIndex>

namespace Ui { class User_ManagerPage; }

class UserDetailsDlg;
class UserAddDlg;
class UserEditDlg;
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
	QStringList m_fieldName = { "","user_id","user_name","gender","mobile","email","isEnable","operation" };
	Ui::User_ManagerPage* ui{};
	QStandardItemModel* m_model{};
	QModelIndex m_currentIndex;

	UserDetailsDlg* m_detailsDlg{};
	UserEditDlg* m_userEditDlg{};
	UserAddDlg* m_userAddDlg{};

};

#endif //!  USERMANAGERPAGE_H_