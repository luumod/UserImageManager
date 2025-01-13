#ifndef SIMAGEDETAILDLG_H_
#define  SIMAGEDETAILDLG_H_

#include <QWidget>
#include <QJsonObject>
#include "SImageInfo.h"

class QLabel;
class QPushButton;
class QLineEdit;
class QTextEdit;
class TopWidget;
class SImageDetailDlg: public QWidget
{
	Q_OBJECT
public:
	SImageDetailDlg(QWidget* parent = nullptr);
	void init();

	void setData(ImageInfo info);
	void updateUi();
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	TopWidget* m_topWidget{};

	ImageInfo m_imageInfo;

	QLabel* m_imageLabel{};
	QPushButton* m_backBtn{};

	QLabel* m_imageName{};
	QLineEdit* m_imageType{};
	QLineEdit* m_imageFormat{};
	QLineEdit* m_imageRR{};
	QLineEdit* m_imageOwer{};
	QLineEdit* m_imageSize{};
	QLineEdit* m_imageDate{};
	QTextEdit* m_imageDesc{};
};

class TopWidget : public QWidget
{
public:
	TopWidget(QWidget* parent = nullptr);
	void init();
private:
	QLabel* m_pathLabel{};
	QPushButton* m_loveBtn{};
};

#endif //! SIMAGEDETAILDLG_H_