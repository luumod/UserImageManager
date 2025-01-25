#include "SHomePage.h"
#include "SDisplayImagesScrollAreaWidget.h"
#include "SImagesJointWidget.h"
#include "SRescentMovementView.h"
#include <QBoxLayout>
#include <QScrollArea>

SHomePage::SHomePage(QWidget* parent)
	: QWidget(parent)
{
	init();
	this->setMinimumSize(1600, 900);
	/*QPalette palette;
	palette.setColor(QPalette::Window, QColor(246,247,251));
	this->setPalette(palette);
	this->setAutoFillBackground(true);*/
}

SHomePage::~SHomePage()
{
}

void SHomePage::init()
{
	auto main_layout = new QVBoxLayout;
	main_layout->setContentsMargins(0, 0, 0, 0);

	QScrollArea* scroll_area = new QScrollArea;
	scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scroll_area->setWidgetResizable(true);

	auto content_widget = new QWidget;
	auto content_layout = new QVBoxLayout(content_widget);

	//----------------顶部---------------
	m_displayWidget = new SDisplayImagesScrollAreaWidget;
	//-----------------------------------

	//----------------中部-----------------
	auto middle_layout = new QHBoxLayout;
	middle_layout->setContentsMargins(0, 0, 0, 0);

	m_jointWidget = new SImagesJointWidget;
	m_jointWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_recentMovementView = new SRescentMovementView;
	m_recentMovementView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	middle_layout->addWidget(m_jointWidget);
	middle_layout->addSpacing(50);
	middle_layout->addWidget(m_recentMovementView);
	//-------------------------------------

	content_layout->addWidget(m_displayWidget);
	content_layout->addSpacing(50);
	content_layout->addLayout(middle_layout);
	auto w = new QWidget;
	w->setFixedHeight(300);
	content_layout->addWidget(w);

	scroll_area->setWidget(content_widget);
	main_layout->addWidget(scroll_area);

	this->setLayout(main_layout);

	content_layout->update();
	main_layout->update();
	this->update();
	layout()->activate();
}
