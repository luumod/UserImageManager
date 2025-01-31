#include "SHomePage.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "SDisplayImagesScrollAreaWidget.h"
#include "SImagesJointWidget.h"
#include "SRescentMovementWidget.h"
#include "SImageViewer.h"
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
	connect(m_displayWidget, &SDisplayImagesScrollAreaWidget::noUploadImages, this, [=]() {
		m_displayWidget->deleteLater();
		m_displayWidget = nullptr;
	});
	connect(m_displayWidget, &SDisplayImagesScrollAreaWidget::openImageDetail, this, [=](const QString& path) {
		m_imageViewer = new SImageViewer(path);
		/*m_imageShowWidget->setData(imageInfo);
		m_imageShowWidget->resize(this->size());*/
		m_imageViewer->setAttribute(Qt::WA_DeleteOnClose);
		m_imageViewer->show();
		});
	//-----------------------------------

	//----------------中部-----------------
	auto middle_layout = new QHBoxLayout;
	middle_layout->setContentsMargins(0, 0, 0, 0);

	m_jointWidget = new SImagesJointWidget;
	SHttpClient(URL("/api/user/get_hot_image_all?get_size=" + QString::number(7))).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.success([=](const QByteArray& data) {
		auto json = QJsonDocument::fromJson(data).object();
		if (json["code"].toInt() < 1000) {
			auto Nhot_images_all = json["data"].toObject()["images"].toArray();
			m_jointWidget->setData(Nhot_images_all);
		}//必成功
			})
		.get();
	connect(m_jointWidget, &SImagesJointWidget::showImageViewer, this, [=](const QString& path) {
		m_imageViewer = new SImageViewer(path);
		m_imageViewer->setAttribute(Qt::WA_DeleteOnClose);
		m_imageViewer->show();
		});
	m_jointWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	m_recentMovementWidget = new SRescentMovementWidget;
	m_recentMovementWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	middle_layout->addWidget(m_jointWidget);
	middle_layout->addSpacing(50);
	middle_layout->addWidget(m_recentMovementWidget);
	//-------------------------------------

	if (m_displayWidget) {
		content_layout->addWidget(m_displayWidget);
	}
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
