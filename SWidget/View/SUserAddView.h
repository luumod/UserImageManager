#ifndef  USERADDDLG_H_
#define  USERADDDLG_H_

#include "SUserIOBaseView.h"
#include <QJsonObject>

class QPushButton;
class QLabel;
class QRadioButton;
class QLineEdit;

class SUserAddView:public SUserIOBaseView
{
	Q_OBJECT
public:
	SUserAddView(QWidget* parent = nullptr);
	void init();

	void setUser(const QJsonObject& user);
signals:
	void newUser(const QJsonObject& user);
private:
	QJsonObject m_juser;

	QLineEdit* m_user_account_edit{};
	QLineEdit* m_username_edit{};
	QLineEdit* m_mobile_edit{};
	QLineEdit* m_email_edit{};
	QRadioButton* m_def_password_rbtn{};
	QRadioButton* m_custom_password_rbtn{};
	QLineEdit* m_password_edit{};
};

#endif //! USERADDDLG_H_