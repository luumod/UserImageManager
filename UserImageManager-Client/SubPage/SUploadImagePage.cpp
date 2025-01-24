#include "SUploadImagePage.h"
#include "SRoundedImageWidget.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "SBigIconButton.h"
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
#include <QComboBox>
#include <QHBoxLayout>

SUploadImagePage::SUploadImagePage(QWidget *parent) 
	: QWidget(parent)
{
	this->setContentsMargins(0, 0, 0, 0);
	init();

	this->setStyleSheet(" QLineEdit{border: 1px solid #DEDEDE; border-radius: 5px; padding: 5px; background-color: transparent;  font-size: 15px; color: #333333;} \
                           QComboBox{border: 2px solid #D4DBC7; border-radius: 5px; padding: 5px; background-color: transparent;  font-size: 15px; color: #333333;}  \
                            QTextEdit{border: 1px solid #DEDEDE; border-radius: 5px; padding: 5px; background-color: transparent; font-size: 15px; color: #333333;} \
		QLabel{font-size: 15px; font-weight: bold; color: #333333; font-family: \"Microsoft YaHei\";} ");
}

SUploadImagePage::~SUploadImagePage()
{
}

void SUploadImagePage::init()
{	
	m_mainLayout = new QVBoxLayout;
	

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
	auto uploadBtn = new SBigIconButton(":/ResourceClient/create.png", "上传图片", 10);
	m_previewImage = new QLabel;
	uploadBtn->setFixedSize(300, 300);
	m_previewImage->setFixedSize(300, 300);
	m_previewImage->setText("图片预览");
	m_previewImage->setAlignment(Qt::AlignCenter);
	m_previewImage->setStyleSheet("background-color: #DBDBDB; border-radius: 15px; border: 1px solid #DEDEDE;");
	m_previewImage->setScaledContents(true);
	middleLayout->addWidget(uploadBtn);
	middleLayout->addWidget(m_previewImage);
	//-------------------------------------------

    //------------- 中下层：图片基本信息--------------------
	auto infoleftLayout = new QGridLayout;

	QHBoxLayout* formLayout_00 = new QHBoxLayout;
	auto typeLab = new QLabel("图片类型：");
	m_typeEdit = new QLineEdit;
	m_typeEdit->setPlaceholderText("动漫/游戏/生活/其他");
	formLayout_00->addWidget(typeLab);
	formLayout_00->addWidget(m_typeEdit);

	QHBoxLayout* formLayout_01 = new QHBoxLayout;
	auto formatLab = new QLabel("图片格式：");
	m_formatEdit = new QLineEdit;
	m_formatEdit->setEnabled(false);
	formLayout_01->addWidget(formatLab);
	formLayout_01->addWidget(m_formatEdit);

	QHBoxLayout* formLayout_10 = new QHBoxLayout;
	auto sizeLab = new QLabel("图片大小：");
	m_sizeEdit = new QLineEdit;
	m_sizeEdit->setEnabled(false);
	formLayout_10->addWidget(sizeLab);
	formLayout_10->addWidget(m_sizeEdit);

	QHBoxLayout* formLayout_11 = new QHBoxLayout;
	auto resolutionLab = new QLabel("图片分辨率：");
	m_resolutionEdit = new QLineEdit;
	m_resolutionEdit->setEnabled(false);
	formLayout_11->addWidget(resolutionLab);
	formLayout_11->addWidget(m_resolutionEdit);

	QHBoxLayout* formLayout_20 = new QHBoxLayout;
	auto qualityLab = new QLabel("图片质量：");
	m_qualityEdit = new QLineEdit;
	m_qualityEdit->setText("高清");
	m_qualityEdit->setEnabled(false);
	formLayout_20->addWidget(qualityLab);
	formLayout_20->addWidget(m_qualityEdit);

	QHBoxLayout* formLayout_21 = new QHBoxLayout;
	auto uploadTimeLab = new QLabel("图片上传时间：");
	m_uploadTimeEdit = new QLineEdit;
	m_uploadTimeEdit->setEnabled(false);
	formLayout_21->addWidget(uploadTimeLab);
	formLayout_21->addWidget(m_uploadTimeEdit);

	QHBoxLayout* formLayout_30 = new QHBoxLayout;
	auto shareLab = new QLabel("图片共享方式：");
	m_shareCombo = new QComboBox;
	m_shareCombo->addItem("私有");
	m_shareCombo->addItem("公开");
	m_shareCombo->addItem("授权");
	m_shareCombo->setCurrentIndex(1);
	formLayout_30->addWidget(shareLab);
	formLayout_30->addWidget(m_shareCombo);

	QHBoxLayout* formLayout_31 = new QHBoxLayout;
	auto downloadLab = new QLabel("图片下载方式：");
	m_downloadCombo = new QComboBox;
	m_downloadCombo->addItem("禁止");
	m_downloadCombo->addItem("允许");
	m_downloadCombo->addItem("授权");
	m_downloadCombo->setCurrentIndex(1);
	formLayout_31->addWidget(downloadLab);
	formLayout_31->addWidget(m_downloadCombo);

	infoleftLayout->addLayout(formLayout_00, 0, 0);
	infoleftLayout->addLayout(formLayout_01, 0, 1);
	infoleftLayout->addLayout(formLayout_10, 1, 0);
	infoleftLayout->addLayout(formLayout_11, 1, 1);
	infoleftLayout->addLayout(formLayout_20, 2, 0);
	infoleftLayout->addLayout(formLayout_21, 2, 1);
	infoleftLayout->addLayout(formLayout_30, 3, 0);
	infoleftLayout->addLayout(formLayout_31, 3, 1);

	QWidget* infoWidget = new QWidget;
	infoWidget->setLayout(infoleftLayout);


	QWidget* ownerWidget = new QWidget;
	QVBoxLayout* vLayout_owner = new QVBoxLayout;
	auto ownerLabAvatar = new QLabel;
	ownerLabAvatar->setAlignment(Qt::AlignCenter);
	ownerLabAvatar->setPixmap(QPixmap(":/ResourceClient/default_avatar.png"));
	ownerLabAvatar->setFixedSize(200,200);
	ownerLabAvatar->setScaledContents(true);
	auto ownerName = new QLabel;
	ownerName->setAlignment(Qt::AlignCenter);
	ownerName->setText("图片所有者：" + sApp->userData("user/user_name").toString());
	vLayout_owner->addWidget(ownerLabAvatar);
	vLayout_owner->addWidget(ownerName);
	ownerWidget->setLayout(vLayout_owner);

	auto infoLayout = new QHBoxLayout;
	infoLayout->addWidget(infoWidget,4);
	infoLayout->addWidget(ownerWidget,1);

	//---------------------------------------

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

	m_mainLayout->addLayout(topLayout,1);
	m_mainLayout->addLayout(middleLayout,4);
	m_mainLayout->addLayout(infoLayout,4);
	m_mainLayout->addLayout(descLayout,1);
	m_mainLayout->addLayout(bottomLayout,1);

	setLayout(m_mainLayout);

	connect(uploadBtn, &SBigIconButton::clicked, this, [=]() {
		QString filepath = uploadImage(); //返回图片绝对路径
		if (!filepath.isEmpty()) {
			update(filepath);
		}
		else {
			QMessageBox::warning(this, "失败", "请重新选择图片");
		}
		});
	connect(okbtn, &QPushButton::clicked, this, [=]() {
			postImage();
		});
}

void SUploadImagePage::update(const QString& filepath)
{
	//固定的由文件得到的信息
	m_fileInfo.setFile(filepath);

	m_titleAbsPath->setText(m_fileInfo.absoluteFilePath());
	m_nameEdit->setText(m_fileInfo.fileName());
	m_sizeEdit->setText(QString::number(QFileInfo(filepath).size() / 1024 ) + "KB");
	m_formatEdit->setText(m_fileInfo.suffix());
	m_resolutionEdit->setText(QString::number(m_previewImage->pixmap().width()) + " x " + QString::number(m_previewImage->pixmap().height()));
	m_uploadTimeEdit->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日 HH:mm:ss"));
	//其余信息由用户填写
}

void SUploadImagePage::passNecessaryInfo()
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

QString SUploadImagePage::uploadImage() {
	auto path = sApp->globalConfig()->value("user/upload_image_path", //默认选择上传文件的路径
		QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
	auto filename = QFileDialog::getOpenFileName(this, "选择上传图片", path, "images (*.jpg;*.jpeg;*.png)");
	if (filename.isEmpty())
		return QString();
	sApp->globalConfig()->setValue("user/upload_image_path", QFileInfo(filename).canonicalPath());
	if (m_file.isOpen()) {
		m_file.close();
	}
	m_file.setFileName(filename);
	if (!m_file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, "失败", "图片打开失败");
		return QString();
	}
	if (m_file.size() > 3 * 1024 * 1034) {
		QMessageBox::warning(this, "失败", "请选择3M以内的图片上传");
		return QString();
	}

	m_previewImage->setPixmap(filename); 
	return filename; //成功加载图片
}

void SUploadImagePage::postImage() {
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
			else{
				qDebug()<< "上传成功";
				QMessageBox::information(this, "成功", "上传成功");
			}
		});
}