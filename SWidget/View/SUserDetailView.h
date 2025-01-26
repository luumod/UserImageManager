#ifndef USERDETAILVIEW_H_
#define  USERDETAILVIEW_H_

#include <QWidget>
#include <QJsonObject>

class QPushButton;
class QLabel;
class SUserEditView;
class STextInputEdit;
class textButton;
class SSwitchButton;
class SUserDetailView : public QWidget
{
	Q_OBJECT
public:
	SUserDetailView(QWidget* parent = nullptr);
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

#endif //! USERDETAILVIEW_H_