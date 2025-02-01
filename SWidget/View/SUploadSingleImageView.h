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
class QDateTimeEdit;
class SUploadSingleImageView : public QWidget
{
public:
	SUploadSingleImageView(QWidget* parent = nullptr);
	~SUploadSingleImageView();

	void init();
	void update(const QString& filepath);
	void passNecessaryInfo();
	QHBoxLayout* createItem(QLineEdit*& lineEdit, const QString& label_name, const QString& edit_placholder, const QString& tip_text,bool enable);
	QHBoxLayout* createItem(QDateTimeEdit*& dateTimeEdit, const QString& label_name, const QString& edit_placholder, const QString& tip_text, bool enable);
	QHBoxLayout* createItem(QComboBox*& ComboBox, const QString& label_name, const QStringList& items, const QString& tip_text);
	QHBoxLayout* createItem(QTextEdit*& textEdit, const QString& label_name, const QString& desc, const QString& tip_text);
	QWidget* drawLine();
protected:
	void resizeEvent(QResizeEvent* event) override;
	bool eventFilter(QObject* watched, QEvent* event)override;
public slots:
	QString uploadImage();
	void postImage();
private:
	QColor defaultBorderColor = QColor(209, 217, 224);
	QColor focusBorderColor = QColor(9, 105, 218);
	QColor backgroundColor = QColor(246, 248, 250);

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
	QDateTimeEdit* m_uploadTimeEdit{};
	QComboBox* m_shareCombo{};
	QComboBox* m_downloadCombo{};

	QTextEdit* m_imageDescEdit{};
};


#endif // SUPLOADSINGLEIMAGEVIEW_H