#ifndef SUPLOADSINGLEIMAGEVIEW_H
#define SUPLOADSINGLEIMAGEVIEW_H

#include <QWidget>
#include "SImageInfo.h"
#include <QFileInfo>

class QHBoxLayout;
class QVBoxLayout;
class SRoundedImageWidget;
class QTextEdit;
class QLineEdit;
class QComboBox;
class QLabel;
class SUploadSingleImageView : public QWidget
{
public:
	SUploadSingleImageView(QWidget* parent = nullptr);
	~SUploadSingleImageView();

	void init();
	void update(const QString& filepath);
	void passNecessaryInfo();
	QHBoxLayout* createItem(QLineEdit*& lineEdit, const QString& label_name, const QString& edit_placholder, const QString& tip_text,bool enable);
	QHBoxLayout* createItem(QComboBox*& lineEdit, const QString& label_name, const QStringList& items, const QString& tip_text);
	QWidget* drawLine();
protected:
	void resizeEvent(QResizeEvent* event) override;
public slots:
	QString uploadImage();
	void postImage();
private:
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


#endif // SUPLOADSINGLEIMAGEVIEW_H