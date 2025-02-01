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
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QDateTimeEdit>

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
	m_imageDescEdit->setPlaceholderText("");
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
		"默认",
		"请输入该图片所属的类型，以便更好的分类图片，如：动漫/游戏/生活/其他，不填则默认", true));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_formatEdit, "图片格式",
		"png/jpg/gif/...",
		"无法输入，加载图片后，系统会自动识别图片格式，如需修改请上传后前往图片加工", false));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_sizeEdit, "图片大小",
		"100KB/200KB/...",
		"无法输入，加载图片后，系统会自动识别图片大小，如需修改请上传后前往图片加工", false));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_resolutionEdit, "图片分辨率",
		"1920x1080 / 2560x1440 /...",
		"无法输入，加载图片后，系统会自动识别图片分辨率，如需修改请上传后前往图片加工", false));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_uploadTimeEdit, "图片上传时间",
		"2021-08-01 12:00:00",
		"默认为当前时间", false));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_shareCombo, "图片共享方式：", 
		{ "私有","公开","授权" },
		"请选择图片共享方式，规定其他用户是否可以查看此图片"));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_downloadCombo, "图片下载方式：",
		{ "私有","公开","授权" },
		"请选择图片下载方式，规定其他用户是否可以下载此图片"));
	contentLayout->addSpacing(50);
	contentLayout->addLayout(createItem(m_imageDescEdit, "图片描述信息：",
		"主人很懒，还没有描述这张图片~",
		"请输入图片的描述信息，让您或其他用户更容易理解图片内容"));
	contentLayout->addLayout(bottomLayout);

	contentWidget->setLayout(contentLayout);
	scrollArea->setWidget(contentWidget);

	main_layout->addWidget(scrollArea);

	this->setLayout(main_layout);

	main_layout->update();
	layout()->activate();

	connect(uploadBtn, &SUploadorDragImageWidget::openFileDialog, this, [=]() {
			QString filepath = uploadImage(); 
			try {
				if (!filepath.isEmpty()) {
					update(filepath);
				}
				else {
					QMessageBox::warning(this, "失败", "请重新选择图片");
				}
			}
			catch (const std::exception& e) {
				QMessageBox::critical(this, "错误", QString("发生错误: %1").arg(e.what()));
			}
			catch (...) {
				QMessageBox::critical(this, "错误", "发生未知错误");
			}
		});
	connect(uploadBtn, &SUploadorDragImageWidget::dragImageFile, this, [=](const QString& filepath) {
		if (!filepath.isEmpty()) {
			if (initFile(filepath)) {
				update(filepath);
				SImage::loadAndCropImage(filepath, m_previewImage);
			}
		}
		});
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

	//其余信息由用户填写或自动生成
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

QHBoxLayout* SUploadSingleImageView::createItem(QDateTimeEdit*& DateTimeEdit, const QString& label_name, const QString& edit_placholder, const QString& tip_text, bool enable)
{
	auto image_type_layout = new QHBoxLayout;

	auto typeLayout = new QVBoxLayout;

	auto labLayout = new QHBoxLayout;
	auto typeLab = new QLabel(label_name);
	labLayout->addWidget(typeLab);
	labLayout->addStretch();

	DateTimeEdit = new QDateTimeEdit;

	// 设置日期时间格式（如：2025-02-01 12:30:00）
	DateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");

	QFont font;
	font.setPointSize(12); 
	font.setFamily("Arial"); 
	DateTimeEdit->setFont(font);

	DateTimeEdit->setStyleSheet("QDateTimeEdit {"
		"border-radius: 5px;" 
		"border: 2px solid gray;"
		"padding: 5px;"
		"background-color: transparent;"
		"}");

	DateTimeEdit->setFixedHeight(40);
	DateTimeEdit->setFixedWidth(400);

	DateTimeEdit->setDateTime(QDateTime::currentDateTime());
	DateTimeEdit->installEventFilter(this);

	auto tipLayout = new QHBoxLayout;
	auto tipLab = new QLabel(tip_text);
	tipLab->setStyleSheet("color:rgb(89,99,110); font-size:12px;");
	tipLayout->addWidget(tipLab);

	typeLayout->addLayout(labLayout);
	typeLayout->addWidget(DateTimeEdit);
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

QHBoxLayout* SUploadSingleImageView::createItem(QTextEdit*& textEdit, const QString& label_name, const QString& desc, const QString& tip_text)
{
	auto image_type_layout = new QHBoxLayout;

	auto typeLayout = new QVBoxLayout;

	auto labLayout = new QHBoxLayout;
	auto typeLab = new QLabel(label_name);
	labLayout->addWidget(typeLab);
	labLayout->addStretch();

	textEdit = new QTextEdit;
	textEdit->setPlaceholderText(desc);
	textEdit->installEventFilter(this);
	textEdit->setMinimumHeight(100);
	textEdit->resize(textEdit->width(), textEdit->minimumHeight());
#define MAX_INPUT_LIMIT 300
	auto charCountLabel = new QLabel("0/" + QString::number(MAX_INPUT_LIMIT) + "字");
	charCountLabel->setStyleSheet("font-size: 14px; font-family: 微软雅黑;");
	connect(textEdit, &QTextEdit::textChanged, [=]() {
		int currentLength = textEdit->toPlainText().length();
		int maxLength = MAX_INPUT_LIMIT;
		if (currentLength > maxLength)
			currentLength = maxLength;
		QString charCount = QString("%1/%2").arg(currentLength).arg(maxLength);
		charCountLabel->setText(charCount);

		if (currentLength >= MAX_INPUT_LIMIT) {
			QString textString = textEdit->toPlainText().left(MAX_INPUT_LIMIT);
			textEdit->blockSignals(true); // Temporarily block signals
			textEdit->setText(textString);
			textEdit->blockSignals(false); // Re-enable signals
			QTextCursor textCursor = textEdit->textCursor();
			textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
			textEdit->setTextCursor(textCursor);
		}

		// 获取文档的尺寸
		QTextDocument* document = textEdit->document();
		QSizeF size = document->size();

		int newHeight = qMin(qMax(document->size().height(), textEdit->minimumHeight() * 1.0), 400.0); // 限制最大高度为400
		textEdit->setFixedHeight(newHeight);
		});

	auto tipLayout = new QHBoxLayout;
	auto tipLab = new QLabel(tip_text);
	tipLab->setStyleSheet("color:rgb(89,99,110); font-size:12px;");
	tipLayout->addWidget(tipLab);
	tipLayout->addStretch();
	tipLayout->addWidget(charCountLabel);

	typeLayout->addLayout(labLayout);
	typeLayout->addWidget(textEdit);
	typeLayout->addLayout(tipLayout);

	image_type_layout->addSpacing(300);
	image_type_layout->addLayout(typeLayout);
	image_type_layout->addSpacing(300);

	image_type_layout->update();
	return image_type_layout;
}

bool SUploadSingleImageView::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == m_typeEdit) {
		if (event->type() == QEvent::FocusIn) {
			m_typeEdit->setStyleSheet(QString("border: 3px solid %1; background-color:%2;").arg(focusBorderColor.name()).arg(backgroundColor.name()));
		}
		else if (event->type() == QEvent::FocusOut) {
			m_typeEdit->setStyleSheet(QString("border: 1px solid %1; background-color:%2;").arg(defaultBorderColor.name()).arg("transparent"));
		}
	}
	else if (watched == m_uploadTimeEdit) {
		if (event->type() == QEvent::FocusIn) {
			m_uploadTimeEdit->setStyleSheet(QString("border: 3px solid #4CAF50; background-color: %1").arg(backgroundColor.name()));
		}
		else if (event->type() == QEvent::FocusOut) {
			m_uploadTimeEdit->setStyleSheet(QString("border: 1px solid %1; background-color:%2;").arg(defaultBorderColor.name()).arg("transparent"));
		}
	}
	else if (watched == m_shareCombo ) {
		
	}
	else if (watched == m_downloadCombo) {
		
	}
	else if (watched == m_imageDescEdit){
		if (event->type() == QEvent::FocusIn) {
			m_imageDescEdit->setStyleSheet(QString("border: 3px solid %1; background-color:%2;").arg(focusBorderColor.name()).arg(backgroundColor.name()));
		}
		else if (event->type() == QEvent::FocusOut) {
			m_imageDescEdit->setStyleSheet(QString("border: 1px solid %1; background-color:%2;").arg(defaultBorderColor.name()).arg("transparent"));
		}
	}
	return QWidget::eventFilter(watched, event);
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

bool SUploadSingleImageView::initFile(const QString& filepath)
{
	// 关闭之前打开的文件
	if (m_file.isOpen()) {
		m_file.close();
	}

	// 设置新的文件名并打开文件
	m_file.setFileName(filepath);
	if (!m_file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, "失败", QString("图片打开失败: %1").arg(m_file.errorString()));
		return false;
	}
	return true;
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

	//异步加载图片到预览图
	if (initFile(filename)) {
		SImage::loadAndCropImage(filename, m_previewImage);
		return filename; // 成功加载图片，返回图片绝对路径
	}
	return QString(); // 失败，返回空字符串
}


void SUploadSingleImageView::postImage() {
	passNecessaryInfo();

	///api/user/upload_image?user_account=1173012900&image_type=测试&description=《湘行散记》以作者回乡途中的所见所闻为线索，生动地描绘了湘西地区的自然风光、风土人情，充满了浓郁的乡土气息和诗意情怀。&image_share=0&image_download=1&image_ResolutionRatio=1920 x 1080&
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
		+ image.m_ResolutionRatio);

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