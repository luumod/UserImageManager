#ifndef SIMAGEDETAILDLG_H_
#define  SIMAGEDETAILDLG_H_

#include <QWidget>
#include <QJsonObject>
#include "SImageInfo.h"

class QLabel;
class QPushButton;
class QLineEdit;
class QTextEdit;
class SImageDetailDlg: public QWidget
{
	Q_OBJECT
public:
	SImageDetailDlg(QWidget* parent = nullptr);
	void init();

	void setData(ImageInfo info);
	void updateUi();
private:
	ImageInfo m_imageInfo;

	QLabel* m_imageLabel{};
	QPushButton* m_backBtn{};

	QLineEdit* m_imageName{};
	QLineEdit* m_imageSize{};
	QLineEdit* m_imageType{};
	QLineEdit* m_imageOwer{};
	QTextEdit* m_imageDesc{};
};

#endif //! SIMAGEDETAILDLG_H_