#include "SUploadMultiImageView.h"
#include "SUploadorDragImageWidget.h"
#include "SBigIconButton.h"
#include "SRoundedImageWidget.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "SImage.h"
#include "STextInputEdit.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QHttpMultiPart>
#include <QLabel>
#include <QFileInfo>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QComboBox>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QDateTimeEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QStackedWidget>

SUploadMultiImageView::SUploadMultiImageView(QWidget* parent)
	:QWidget(parent)
{
	init();
}
SUploadMultiImageView::~SUploadMultiImageView() {

}

void SUploadMultiImageView::init() 
{
	auto main_layout = new QVBoxLayout;
	main_layout->setContentsMargins(0, 0, 0, 0);

	///------------
	auto contentLayout = new QVBoxLayout;
	auto contentWidget = new QWidget;
	contentWidget->setContentsMargins(0,0,0,0);

	auto scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setFrameShape(QFrame::NoFrame);


	//------------------top top---------------------
	QHBoxLayout* topTopLayout = new QHBoxLayout;
	auto backBtn = new QPushButton("返回");
	backBtn->setFixedWidth(100);
	backBtn->setIcon(QIcon(":/ResourceClient/back.png"));
	topTopLayout->addWidget(backBtn,Qt::AlignLeft);
	topTopLayout->addStretch();
	//-------------------------------------------

	//--------------中间 上传图片 + 图片预览 区域----------
	m_fileListWidget = new QListWidget;
	m_fileListWidget->setFixedHeight(150);
	m_fileListWidget->setStyleSheet("QListWidget { border: 1px solid #DEDEDE; border-radius:5px; }");

	auto uploadBtn = new SUploadorDragImageWidget;

	// 修改后的middleLayout结构
	QHBoxLayout* middleLayout = new QHBoxLayout;
	middleLayout->setContentsMargins(0, 0, 0, 0);
	auto leftLayout = new QVBoxLayout;

	leftLayout->addWidget(uploadBtn,6);
	leftLayout->addWidget(m_fileListWidget, 4);


	m_previewImage = new QLabel;
	m_previewImage->setFixedSize(800, 700);
	m_previewImage->setText("图片预览");
	m_previewImage->setAlignment(Qt::AlignCenter);
	m_previewImage->setStyleSheet("background-color: transparent; border-radius: 15px; border: 1px solid #DEDEDE;");

	middleLayout->addLayout(leftLayout);
	middleLayout->addWidget(m_previewImage);




	QHBoxLayout* bottomLayout = new QHBoxLayout;
	auto okbtn = new QPushButton("上传");
	bottomLayout->addStretch();
	bottomLayout->addWidget(okbtn);
	bottomLayout->addStretch();

	contentLayout->addLayout(topTopLayout);
	contentLayout->addLayout(middleLayout);
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_labelEdit, "图片类型", "默认", "请输入该图片所属的类型，以便更好的分类图片，如：动漫/游戏/生活/其他，不填则默认"));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_shareCombo, "图片共享方式：",
		{ "私有","公开","授权" },
		"请选择图片共享方式，规定其他用户是否可以查看此图片"));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_downloadCombo, "图片下载方式：",
		{ "私有","公开","授权" },
		"请选择图片下载方式，规定其他用户是否可以下载此图片"));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(bottomLayout);

	contentWidget->setLayout(contentLayout);
	scrollArea->setWidget(contentWidget);

	main_layout->addWidget(scrollArea);

	this->setLayout(main_layout);

	main_layout->update();
	layout()->activate();

	connect(uploadBtn, &SUploadorDragImageWidget::openFileDialog, this, [=]() {
		if (uploadImages()) {
			qDebug() << "uploadImages success";
		}
		});
	connect(m_fileListWidget, &QListWidget::currentRowChanged, this, [=](int currentRow) {
		if (currentRow >= 0 && currentRow < m_uploadFiles.size()) {
			QString filename = m_uploadFiles[currentRow];
			SImage::loadAndCropImage(filename, m_previewImage);
		}
		});
	connect(uploadBtn, &SUploadorDragImageWidget::dragImageFile, this, [=](const QString& filepath) {
		
		});
	connect(okbtn, &QPushButton::clicked, this, [=]() {
		postImages();
		});
	connect(backBtn, &QPushButton::clicked, this, [=]() {
		this->close();
		});
}

QHBoxLayout* SUploadMultiImageView::createItem(QLineEdit*& lineEdit, const QString& label_name, const QString& edit_placholder, const QString& tip_text)
{
	auto image_type_layout = new QHBoxLayout;

	auto typeLayout = new QVBoxLayout;

	auto labLayout = new QHBoxLayout;
	auto typeLab = new QLabel(label_name);
	labLayout->addWidget(typeLab);
	labLayout->addStretch();

	lineEdit = new QLineEdit;
	lineEdit->setPlaceholderText(edit_placholder);
	lineEdit->installEventFilter(this);

	auto tipLayout = new QHBoxLayout;
	auto tipLab = new QLabel(tip_text);
	tipLab->setStyleSheet("color:rgb(89,99,110); font-size:12px;");
	tipLayout->addWidget(tipLab);

	typeLayout->addLayout(labLayout);
	typeLayout->addWidget(lineEdit);
	typeLayout->addLayout(tipLayout);

	image_type_layout->addSpacing(300);
	image_type_layout->addLayout(typeLayout);
	image_type_layout->addSpacing(300);

	image_type_layout->update();
	return image_type_layout;
}

QHBoxLayout* SUploadMultiImageView::createItem(QComboBox*& comboBox, const QString& label_name, const QStringList& items, const QString& tip_text)
{
	auto image_type_layout = new QHBoxLayout;

	auto typeLayout = new QVBoxLayout;

	auto labLayout = new QHBoxLayout;
	auto typeLab = new QLabel(label_name);
	labLayout->addWidget(typeLab);
	labLayout->addStretch();


	comboBox = new QComboBox;
	for (const auto& item : items) {
		comboBox->addItem(item);
	}
	comboBox->setCurrentIndex(1);
	comboBox->installEventFilter(this);

	auto tipLayout = new QHBoxLayout;
	auto tipLab = new QLabel(tip_text);
	tipLayout->addWidget(tipLab);

	typeLayout->addLayout(labLayout);
	typeLayout->addWidget(comboBox);
	typeLayout->addLayout(tipLayout);

	image_type_layout->addSpacing(300);
	image_type_layout->addLayout(typeLayout);
	image_type_layout->addSpacing(300);

	image_type_layout->update();
	return image_type_layout;
}

void SUploadMultiImageView::updateFileList(const QStringList& files)
{
}

bool SUploadMultiImageView::uploadImages() 
{
	// 获取默认上传文件路径
	auto path = sApp->globalConfig()->value("user/upload_image_path",
		QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();

	// 弹出文件选择对话框
	auto filenames = QFileDialog::getOpenFileNames(this, "选择上传图片", path, "图片文件 (*.jpg *.jpeg *.png *.webp *.bmp *.gif *.svg)");
	if (filenames.isEmpty()) {
		return false;
	}

	m_uploadFiles.clear();
	foreach(auto filename, filenames) {
		// 检查文件是否可以打开
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly)) {
			QMessageBox::warning(this, "失败", QString("图片打开失败: %1").arg(file.errorString()));
			continue;
		}

		// 检查文件大小是否超过限制
		if (file.size() > 10 * 1024 * 1024) {
			QMessageBox::warning(this, "失败", QString("请选择10M以内的图片上传，当前图片大小为: %1MB").arg(file.size() / (1024.0 * 1024.0)));
			file.close();
			continue;
		}
		m_uploadFiles.append(filename);
	}
	m_fileListWidget->clear();
	m_fileListWidget->addItems(m_uploadFiles);
	return true; 
}

void SUploadMultiImageView::postImages()
{
	foreach(auto filename, m_uploadFiles) {
		if (m_file.isOpen()) {
			m_file.close();
		}
		m_file.setFileName(filename);
		m_file.open(QIODevice::ReadOnly);

		auto type = m_labelEdit->text();
		auto desc = "主人很懒，还没有描述这张图片~";
		auto url = URL("/api/user/upload_image?user_account=" +
			sApp->globalConfig()->value("user/user_account").toString()
			+ "&image_type="
			+ type
			+ "&description="
			+ desc
			+ "&image_share="
			+ QString::number(m_shareCombo->currentIndex())
			+ "&image_download="
			+ QString::number(m_downloadCombo->currentIndex())
			+ "&image_ResolutionRatio="
			+ QString::number(QImage(filename).size().width()) + "x" + QString::number(QImage(filename).size().height()));

		QNetworkAccessManager* mangaer = new QNetworkAccessManager(this);
		QNetworkRequest request(url);
		request.setRawHeader("Authorization", "Bearer" + sApp->userData("user/token").toString().toUtf8());
		request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-www-form-urlencoded");

		QHttpMultiPart* mpart = new QHttpMultiPart(this);
		QHttpPart part;
		part.setBody(m_file.readAll());
		m_file.seek(0);//重新设置文件指针到开头
		part.setHeader(QNetworkRequest::ContentTypeHeader,
			QString("image/%1").arg(QFileInfo(filename).suffix()));
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
			QString(R"(attachment;name="file";filename="%1")")
			.arg(QFileInfo(filename).fileName()));
		mpart->append(part);

		auto reply = mangaer->post(request, mpart);
		connect(reply, &QNetworkReply::finished, [=]
			{
				if (reply->error() != QNetworkReply::NoError)
				{
					qDebug() << "replay error" << reply->errorString();
					QMessageBox::warning(this, "失败", "上传失败");
				}
				else {
					qDebug() << "上传成功";
					QMessageBox::information(this, "成功", "上传成功");
				}
			});

	}
}
