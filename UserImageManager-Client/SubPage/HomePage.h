#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QFile>
#include <QFileInfo>
#include "SImageInfo.h"

class QVBoxLayout;
class RoundedImageWidget;
class QTextEdit;
class QLineEdit;
class QComboBox; 
class QLabel;
class HomePage : public QWidget
{
	Q_OBJECT
public:
	HomePage(QWidget* parent = nullptr);
	~HomePage();
	void init();
	
	void update(const QString& filepath);
	void passNecessaryInfo();
 public slots:
	QString uploadImage();
	void postImage();
private:
	QVBoxLayout* m_mainLayout{};

	QFile m_file;
	QLabel* m_previewImage{};



	//存储信息
	ImageInfo image;
	QFileInfo m_fileInfo;

	//图片信息Ui
	QLineEdit* m_titleAbsPath{};
	QLineEdit* m_nameEdit{};
	QLineEdit* m_typeEdit{};
	QLineEdit* m_formatEdit{};
	QLineEdit* m_sizeEdit{};
	QLineEdit* m_resolutionEdit{};
	QLineEdit* m_qualityEdit{};
	QLineEdit* m_uploadTimeEdit{};
    QComboBox* m_shareCombo{};
	QComboBox* m_downloadCombo{};

	QTextEdit* m_imageDescEdit{};

};

#endif // HOMEPAGE_H