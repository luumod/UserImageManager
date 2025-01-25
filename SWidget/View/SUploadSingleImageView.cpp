#include "SUploadSingleImageView.h"
#include "SUploadSingleImageView.h"
#include "SUploadSingleImageView.h"
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
#include <QHBoxLayout>

SUploadSingleImageView::SUploadSingleImageView(QWidget* parent)
	:QWidget(parent)
{
	init();

	this->setStyleSheet(" QLineEdit{border: 1px solid #DEDEDE; border-radius: 5px; padding: 5px; background-color: transparent;  font-size: 15px; color: #333333;} \
                           QComboBox{border: 2px solid #D4DBC7; border-radius: 5px; padding: 5px; background-color: transparent;  font-size: 15px; color: #333333;}  \
                            QTextEdit{border: 1px solid #DEDEDE; border-radius: 5px; padding: 5px; background-color: transparent; font-size: 15px; color: #333333;} \
		QLabel{font-size: 15px; font-weight: bold; color: #333333; font-family: \"Microsoft YaHei\";} ");
}
SUploadSingleImageView::~SUploadSingleImageView() {

}

void SUploadSingleImageView::init() 
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

	//------------------top-------------------
	QHBoxLayout* topLayout = new QHBoxLayout;

	auto formLayout_path = new QHBoxLayout;
	auto titleLab = new QLabel("图片路径：");
	m_titleAbsPath = new QLineEdit;
	m_titleAbsPath->setPlaceholderText("完整路径");
	m_titleAbsPath->setEnabled(false);
	formLayout_path->addWidget(titleLab);
	formLayout_path->addWidget(m_titleAbsPath);

	auto formLayout_name = new QHBoxLayout;
	auto nameLab = new QLabel("图片名称：");
	m_nameEdit = new QLineEdit;
	m_nameEdit->setPlaceholderText("dog.png");
	m_nameEdit->setEnabled(false);
	formLayout_name->addWidget(nameLab);
	formLayout_name->addWidget(m_nameEdit);

	topLayout->addLayout(formLayout_path);
	topLayout->addStretch();
	topLayout->addLayout(formLayout_name);
	//-------------------------------------------

	//--------------中间 上传图片 + 图片预览 区域----------
	QHBoxLayout* middleLayout = new QHBoxLayout;
	middleLayout->setContentsMargins(0, 0, 0, 0);

	auto uploadImageLayout = new QVBoxLayout;
	uploadImageLayout->setContentsMargins(0, 0, 0, 0);
	auto uploadBtn = new SUploadorDragImageWidget;
	//uploadBtn->setFixedSize(600, 300);
	uploadImageLayout->addStretch();
	uploadImageLayout->addWidget(uploadBtn);
	uploadImageLayout->addStretch();

	m_previewImage = new QLabel;
	m_previewImage->setFixedSize(800, 700);
	m_previewImage->setText("图片预览");
	m_previewImage->setAlignment(Qt::AlignCenter);
	m_previewImage->setStyleSheet("background-color: transparent; border-radius: 15px; border: 1px solid #DEDEDE;");

	middleLayout->addLayout(uploadImageLayout);
	middleLayout->addWidget(m_previewImage);
	//-------------------------------------------



	//------------- 底部：图片描述 编辑区域----------
	QHBoxLayout* descLayout = new QHBoxLayout;
	auto descLab = new QLabel("图片描述");
	m_imageDescEdit = new QTextEdit;
	m_imageDescEdit->setPlaceholderText("主人很懒，还没有描述这张图片~");
	descLayout->addWidget(descLab);
	descLayout->addWidget(m_imageDescEdit);
	//--------------------------------------------


	QHBoxLayout* bottomLayout = new QHBoxLayout;
	auto okbtn = new QPushButton("上传");
	bottomLayout->addStretch();
	bottomLayout->addWidget(okbtn);
	bottomLayout->addStretch();

	contentLayout->addLayout(topTopLayout);
	contentLayout->addLayout(topLayout);
	contentLayout->addLayout(middleLayout);
	contentLayout->addWidget(drawLine());
	contentLayout->addLayout(createItem(m_typeEdit, "图片类型",
		"动漫/游戏/生活/其他",
		"请选择图片类型，以便更好的分类图片", true));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_formatEdit, "图片格式",
		"png/jpg/gif/...",
		"加载图片后，系统会自动识别图片格式", false));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_sizeEdit, "图片大小",
		"100KB/200KB/...",
		"加载图片后，系统会自动识别图片大小", false));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_resolutionEdit, "图片分辨率",
		"1920x1080 / 2560x1440 /...",
		"加载图片后，系统会自动识别图片分辨率", false));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_qualityEdit, "图片质量",
		"高清/中等/低清",
		"请选择图片质量，以便更好的压缩图片", false));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_uploadTimeEdit, "图片上传时间",
		"2021-08-01 12:00:00",
		"图片上传到服务器的时间", false));
	contentLayout->addLayout(createItem(m_shareCombo, "图片共享方式：", { "私有","公开","授权" }, "请选择图片共享方式，指定其他用户是否可以查看此图片"));
	contentLayout->addLayout(createItem(m_downloadCombo, "图片下载方式：", { "私有","公开","授权" }, "请选择图片下载方式，指定其他用户是否可以下载此图片"));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(descLayout);
	contentLayout->addLayout(bottomLayout);

	contentWidget->setLayout(contentLayout);
	scrollArea->setWidget(contentWidget);

	main_layout->addWidget(scrollArea);

	this->setLayout(main_layout);

	main_layout->update();
	layout()->activate();


	//connect(uploadBtn, &SBigIconButton::clicked, this, [=]() {
	//	QString filepath = uploadImage(); // 返回图片绝对路径
	//	try {
	//		if (!filepath.isEmpty()) {
	//			update(filepath);
	//		}
	//		else {
	//			QMessageBox::warning(this, "失败", "请重新选择图片");
	//		}
	//	}
	//	catch (const std::exception& e) {
	//		QMessageBox::critical(this, "错误", QString("发生错误: %1").arg(e.what()));
	//	}
	//	catch (...) {
	//		QMessageBox::critical(this, "错误", "发生未知错误");
	//	}

	//	});
	connect(okbtn, &QPushButton::clicked, this, [=]() {
		postImage();
		});
	connect(backBtn, &QPushButton::clicked, this, [=]() {
		this->close();
		});
}

void SUploadSingleImageView::update(const QString& filepath)
{
	//固定的由文件得到的信息
	m_fileInfo.setFile(filepath);

	m_titleAbsPath->setText(m_fileInfo.absoluteFilePath());
	m_nameEdit->setText(m_fileInfo.fileName());
	m_sizeEdit->setText(QString::number(m_file.size() / 1024) + "KB");
	m_formatEdit->setText(m_fileInfo.suffix());
	m_resolutionEdit->setText(QString::number(QImage(filepath).width()) + " x " + QString::number(QImage(filepath).height()));
	m_uploadTimeEdit->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日 HH:mm:ss"));
	//其余信息由用户填写
}

void SUploadSingleImageView::passNecessaryInfo()
{
	image.set_image_path(m_fileInfo.absoluteFilePath());
	image.set_image_name(m_fileInfo.fileName());
	image.set_image_size(m_fileInfo.size());//B
	image.set_image_format(m_fileInfo.suffix());
	image.set_image_share(m_shareCombo->currentIndex());
	image.set_image_type(m_typeEdit->text());
	image.set_image_download(m_downloadCombo->currentIndex());
	image.set_image_ResolutionRatio(m_resolutionEdit->text());
	image.set_image_quality(m_qualityEdit->text());
	image.set_image_upload_time(m_uploadTimeEdit->text());
	image.set_image_desc(m_imageDescEdit->toPlainText().isEmpty() ? m_imageDescEdit->placeholderText() : m_imageDescEdit->toPlainText());
}

QHBoxLayout* SUploadSingleImageView::createItem(QLineEdit*& lineEdit, const QString& label_name, const QString& edit_placholder, const QString& tip_text, bool enable)
{
	auto image_type_layout = new QHBoxLayout;

	auto typeLayout = new QVBoxLayout;

	auto labLayout = new QHBoxLayout;
	auto typeLab = new QLabel(label_name);
	labLayout->addWidget(typeLab);
	labLayout->addStretch();

	lineEdit = new QLineEdit;
	lineEdit->setPlaceholderText(edit_placholder);
	lineEdit->setEnabled(enable);

	auto tipLayout = new QHBoxLayout;
	auto tipLab = new QLabel(tip_text);
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

QHBoxLayout* SUploadSingleImageView::createItem(QComboBox*& comboBox, const QString& label_name, const QStringList& items, const QString& tip_text)
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

QWidget* SUploadSingleImageView::drawLine()
{
	// 添加白色分割线
	QFrame* line = new QFrame();
	line->setFixedWidth(this->width() - 500);
	line->setFrameShape(QFrame::HLine);  // 水平线条
	line->setFrameShadow(QFrame::Sunken); // 沉下去的阴影效果
	line->setLineWidth(1); // 线条宽度
	line->setStyleSheet("background-color: white;"); // 设置白色背景

	return line;
}

void SUploadSingleImageView::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
}

QString SUploadSingleImageView::uploadImage() {
	// 获取默认上传文件路径
	auto path = sApp->globalConfig()->value("user/upload_image_path",
		QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();

	// 弹出文件选择对话框
	auto filename = QFileDialog::getOpenFileName(this, "选择上传图片", path, "图片文件 (*.jpg *.jpeg *.png *.webp *.bmp *.gif *.svg)");
	if (filename.isEmpty()) {
		return QString();
	}

	// 检查文件是否可以打开
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, "失败", QString("图片打开失败: %1").arg(file.errorString()));
		return QString();
	}

	// 检查文件大小是否超过限制
	if (file.size() > 10 * 1024 * 1024) {
		QMessageBox::warning(this, "失败", QString("请选择10M以内的图片上传，当前图片大小为: %1MB").arg(file.size() / (1024.0 * 1024.0)));
		file.close();
		return QString();
	}

	// 更新全局配置中的文件路径
	sApp->globalConfig()->setValue("user/upload_image_path", QFileInfo(filename).canonicalPath());

	// 关闭之前打开的文件
	if (m_file.isOpen()) {
		m_file.close();
	}

	// 设置新的文件名并打开文件
	m_file.setFileName(filename);
	if (!m_file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, "失败", QString("图片打开失败: %1").arg(m_file.errorString()));
		return QString();
	}

	//异步加载图片到预览图
	SImage::loadAndCropImage(filename, m_previewImage);

	return filename; // 成功加载图片，返回图片绝对路径
}


void SUploadSingleImageView::postImage() {
	passNecessaryInfo();

	///api/user/upload_image?user_account=1173012900&image_type=测试&description=《湘行散记》以作者回乡途中的所见所闻为线索，生动地描绘了湘西地区的自然风光、风土人情，充满了浓郁的乡土气息和诗意情怀。&image_share=0&image_download=1&image_ResolutionRatio=1920 x 1080&image_quality=高清
	auto url = URL("/api/user/upload_image?user_account=" +
		sApp->globalConfig()->value("user/user_account").toString()
		+ "&image_type="
		+ image.m_type
		+ "&description="
		+ image.m_desc
		+ "&image_share="
		+ QString::number(image.m_share)
		+ "&image_download="
		+ QString::number(image.m_download)
		+ "&image_ResolutionRatio="
		+ image.m_ResolutionRatio
		+ "&image_quality="
		+ image.m_quality);

	QNetworkAccessManager* mangaer = new QNetworkAccessManager(this);
	QNetworkRequest request(url);
	request.setRawHeader("Authorization", "Bearer" + sApp->userData("user/token").toString().toUtf8());
	request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-www-form-urlencoded");

	QHttpMultiPart* mpart = new QHttpMultiPart(this);
	QHttpPart part;
	part.setBody(m_file.readAll());
	m_file.seek(0);//重新设置文件指针到开头
	part.setHeader(QNetworkRequest::ContentTypeHeader,
		QString("image/%1").arg(QFileInfo(image.m_path).suffix()));
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
		QString(R"(attachment;name="file";filename="%1")")
		.arg(QFileInfo(image.m_path).fileName()));
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