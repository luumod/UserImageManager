#ifndef LOGINREGISTERDLG_H
#define LOGINREGISTERDLG_H

#include <QDialog>
#include <QStackedWidget> // 多页面切换
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QNetWorkAccessManager> 

class LoginRegister :public QDialog {
public:
	LoginRegister(QWidget* parent = 0);
	~LoginRegister();
private:
	void initUi();
	QWidget* createLoginWidget();
	QWidget* createRegisterWidget();
	void setTip(const QString& text,int time = 0);
	void saveInfo();
	void readInfo();
public slots:
	void onLogin();
	void onRegister();
protected:
	void paintEvent(QPaintEvent* paint)override;
	void closeEvent(QCloseEvent* close)override;
private:
	QRect m_rightRect;
	QRect m_leftRect;
	QRect m_middleRect;

	QWidget* m_loginWidget;
	QWidget* m_registerWidget;
	QStackedWidget* m_stkWidget;
	QLabel* m_tips;
	QTimer* m_timer;

	bool m_isLogining = false;

	//登录
	QLineEdit* m_account;
	QLineEdit* m_password;
	QCheckBox* m_rememberPwd;
	QCheckBox* m_autoLogin;

	//注册
	QLineEdit* m_accountR;
	QLineEdit* m_nameR;
	QLineEdit* m_passwordR;
	QLineEdit* m_rePasswordR;
	QRadioButton* m_sexMaleR;
	QRadioButton* m_sexFemaleR;

	QNetworkAccessManager* m_nam;
};

#endif // LOGINREGISTERDLG_H
