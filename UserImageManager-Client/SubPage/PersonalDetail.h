#ifndef PERSONALDETAIL_H_
#define  PERSONALDETAIL_H_

#include <QWidget>
#include <QJsonObject>

class QPushButton;
class QLabel;
class UserEditDlg;
class STextInputEdit;
class textButton;
class SSwitchButton;
class PersonalDetail: public QWidget
{
	Q_OBJECT
public:
	PersonalDetail(QWidget* parent = nullptr);
	void init();

	void setUser(const QJsonObject& user);
protected: 
	void onLoadPersonalInfo();
	void onAvatarDownload();
	void onAvatarUpload();
private:

	textButton* m_changeAvatarBtn{};

	QLabel* m_avatar_lab{};
	STextInputEdit* m_inputItem_id{};
	STextInputEdit* m_inputItem_name{};
	STextInputEdit* m_inputItem_email{};
	STextInputEdit* m_inputItem_mobile{};

	SSwitchButton* m_isEnable_btn{};
	void updateUser();
};

#endif //! PERSONALDETAIL_H_