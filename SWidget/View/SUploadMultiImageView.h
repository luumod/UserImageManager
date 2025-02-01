#ifndef SUPLOADMULTIIMAGEVIEW_H
#define SUPLOADMULTIIMAGEVIEW_H

#include <QWidget>
#include <QMap>
#include "SImageInfo.h"
#include <QFileInfo>

class QProgressBar;
class QListWidget;
class QLabel;
class QHBoxLayout;
class QLineEdit;
class QComboBox;
class QTextEdit;
class SUploadMultiImageView : public QWidget
{
public:
	SUploadMultiImageView(QWidget* parent = nullptr);
	~SUploadMultiImageView();

	void init();
	void updateFileList(const QStringList& files);
	QHBoxLayout* createItem(QLineEdit*& lineEdit, const QString& label_name, const QString& edit_placholder, const QString& tip_text);
	QHBoxLayout* createItem(QComboBox*& comboBox, const QString& label_name, const QStringList& items, const QString& tip_text);
public slots:
	bool uploadImages();
	void postImages();
private:
	QListWidget* m_fileListWidget{};
	QStringList m_uploadFiles;
	QLabel* m_previewImage{};

	QFile m_file;

	QLineEdit* m_labelEdit{};
	QComboBox* m_shareCombo{}; 
	QComboBox* m_downloadCombo{};
	QTextEdit* m_imageDescEdit{};
};


#endif // !SUPLOADMULTIIMAGEVIEW_H