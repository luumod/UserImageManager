#ifndef SHOMEPAGE_H
#define SHOMEPAGE_H

#include <QWidget>

class SDisplayImagesScrollAreaWidget;
class SImagesJointWidget;
class SRescentMovementWidget;
class SImageDetailView;
class SImageViewer;
class SHomePage : public QWidget
{
public:   
	SHomePage(QWidget* parent = nullptr);
	~SHomePage();

	void init();

private:
	SDisplayImagesScrollAreaWidget* m_displayWidget{};
	SImagesJointWidget* m_jointWidget{};
	SRescentMovementWidget* m_recentMovementWidget{};

	SImageViewer* m_imageViewer{};
};


#endif // SHOMEPAGE_H