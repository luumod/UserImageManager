﻿#ifndef USERDETAILSDLG_H_
#define  USERDETAILSDLG_H_

#include <QWidget>
#include <QJsonObject>

class QPushButton;
class QLabel;
class UserEditDlg;
class STextInputEdit;
class textButton;
class SSwitchButton;
class UserDetailsDlg : public QWidget
{
	Q_OBJECT
public:
	UserDetailsDlg(QWidget* parent = nullptr);
	void init();

	void setUser(const QJsonObject& user);
signals:
	void userChanged(const QJsonObject& user);
protected: 
	void onAvatarDownload();
	void onAvatarUpload();
private:
	QJsonObject m_juser;
	
	QPushButton* m_backBtn{};

	QLabel* m_avatar_lab{};
	STextInputEdit* m_inputItem_id{};
	STextInputEdit* m_inputItem_name{};
	STextInputEdit* m_inputItem_email{};
	STextInputEdit* m_inputItem_mobile{};

	SSwitchButton* m_isEnable_btn{};
	void updateUi();
};

#endif //! USERDETAILSDLG_H_