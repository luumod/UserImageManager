#ifndef SIMAGEMODIFYVIEW_H
#define SIMAGEMODIFYVIEW_H

#include <QWidget>
#include <QFile>
#include <QFileInfo>
#include "SImageInfo.h"

class QVBoxLayout;
class SRoundedImageWidget;
class QTextEdit;
class QLineEdit;
class QComboBox;
class QLabel;
class SImageModifyView : public QWidget
{
	Q_OBJECT
public:
	SImageModifyView(QWidget* parent = nullptr);
	~SImageModifyView();
	void init();

	void setData(const QJsonObject& jImage);
public slots:
	void updateImageInfo();
signals:
	void onCloseAndModified();
private:
	QVBoxLayout* m_mainLayout{};

	QFile m_file;
	QLabel* m_previewImage{};

	//存储信息
	ImageInfo image;

	//图片信息Ui
	QLineEdit* m_titleAbsPath{};
	QLineEdit* m_nameEdit{};
	QLineEdit* m_typeEdit{};
	QLineEdit* m_formatEdit{};
	QLineEdit* m_sizeEdit{};
	QLineEdit* m_resolutionEdit{};
	QLineEdit* m_uploadTimeEdit{};
	QComboBox* m_shareCombo{};
	QComboBox* m_downloadCombo{};

	QTextEdit* m_imageDescEdit{};

};


#endif // !SIMAGEMODIFYVIEW_H
