#ifndef SPERSONALCENTERPAGE_H_
#define  SPERSONALCENTERPAGE_H_

#include <QWidget>
#include <QApplication>
#include <QJsonObject>

class QPushButton;
class QLabel;
class SUserEditView;
class STextInputEdit;
class textButton;
class SSwitchButton;
class SPersonalCenterPage: public QWidget
{

	Q_OBJECT

public:
	SPersonalCenterPage(QWidget* parent = nullptr);
	void init();
	void showPage();
public slots:
	void updateUser();
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
};

#endif //! SPERSONALCENTERPAGE_H_