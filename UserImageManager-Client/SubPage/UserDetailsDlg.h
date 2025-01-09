#ifndef USERDETAILSDLG_H_
#define  USERDETAILSDLG_H_

#include <QWidget>
#include <QJsonObject>
#include "SSwitchButton.h"

class QPushButton;
class QLabel;
class UserEditDlg;
class textInputItem;
class textButton;
class UserDetailsDlg : public QWidget
{
	Q_OBJECT
public:
	UserDetailsDlg(QWidget* parent = nullptr);
	void init();

	void setUser(const QJsonObject& user);
signals:
	void userChanged(const QJsonObject& user);
	void userDeleted(const QJsonObject& user);
protected: 
	void onAvatarDownload();
	void onAvatarUpload();
private:
	QJsonObject m_juser;	//保存所有经过修改只收的用户数据(变化的)
	QJsonObject m_oldJuser;	//保存设置进来的原始的用户数据(不变的)

	
	QPushButton* m_backBtn{};
	textButton* m_changeAvatarBtn{};

	QLabel* m_avatar_lab{};
	textInputItem* m_inputItem_id{};
	textInputItem* m_inputItem_name{};
	textInputItem* m_inputItem_email{};
	textInputItem* m_inputItem_mobile{};

	SSwitchButton* m_isEnable_btn{};
	void updateUi();
};

#endif //! USERDETAILSDLG_H_