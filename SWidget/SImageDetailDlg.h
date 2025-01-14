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

	void setData(ImageInfo info, int image_index);
	void updateUi();

	inline  int currentImageIndex() const { return m_currentImageIndex; }
	inline void nextImage() { m_currentImageIndex++; }
	inline void prevImage() { m_currentImageIndex--; }
	void onLikeBtnClicked();
	void onStarBtnClicked();
signals:
	void imageLiked(int image_index);
	void imageUnLiked(int image_index);
	void imageStared(int image_index);
	void imageUnStared(int image_index);
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	int m_currentImageIndex{};


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

	QPushButton* m_likeBtn{};
	QPushButton* m_starBtn{};
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