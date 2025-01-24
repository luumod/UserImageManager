#ifndef  USEEDITDLG_H_
#define  USEEDITDLG_H_

#include "SUserIOBaseView.h"
#include <QJsonObject>

class QPushButton;
class QLabel;
class QRadioButton;
class QLineEdit;

class SUserEditView : public SUserIOBaseView
{
	Q_OBJECT
public:
	SUserEditView(QWidget* parent = nullptr);
	void init();

	void setUser(const QJsonObject& user);
signals:
	void userChanged(const QJsonObject& user);
private:
	QJsonObject m_juser;

	QLineEdit* m_user_account_edit{};
	QLineEdit* m_username_edit{};
	QLineEdit* m_mobile_edit{};
	QLineEdit* m_email_edit{};
};

#endif //! USERDETAILSDLG_H_