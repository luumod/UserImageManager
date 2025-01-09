#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QFile>

class QVBoxLayout;
class RoundedImageWidget;
class QTextEdit;
class HomePage : public QWidget
{
	Q_OBJECT
public:
	HomePage(QWidget* parent = nullptr);
	~HomePage();
	void init();

public slots:
	QString uploadImage();
	void postImage(const QString& filepath, const QString& desc);
private:
	QVBoxLayout* m_mainLayout{};

	RoundedImageWidget* m_previewImage{};
	QTextEdit* m_imageDescEdit{};
};

#endif // HOMEPAGE_H