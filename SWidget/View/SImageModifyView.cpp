#include "SImageModifyView.h"
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
#include <QScrollArea>

SImageModifyView::SImageModifyView(QWidget* parent)
	: QWidget(parent)
{
	init();

	this->setStyleSheet("  QScrollArea{border:none; background-color: transparent;}\
							QLineEdit{border: 1px solid #DEDEDE; border-radius: 5px; padding: 5px; background-color: transparent;  font-size: 15px; color: #333333;} \
							QLineEdit:hover{ background-color: #E2E2F5;} 		\
                           QComboBox{border: 2px solid #D4DBC7; border-radius: 5px; padding: 5px; background-color: transparent;  font-size: 15px; color: #333333;}  \
							QComboBox:hover{ background-color: #E2E2F5;} 	\
                            QTextEdit{border: 1px solid #DEDEDE; border-radius: 5px; padding: 5px; background-color: transparent; font-size: 15px; color: #333333;} \
		QLabel{font-size: 15px; font-weight: bold; color: #333333; font-family: \"Microsoft YaHei\";}\
		QWidget#ownerWidget{border: 1px solid #DEDEDE;} \
		QWidget#directModifyWidget{border: 1px solid #C8BFE7; background-color:transparent; border-radius: 15px; padding: 30px; margin-top: 10px;}\
		QWidget#indirectModifyWidget{border: 2px solid red; background-color:transparent; border-radius: 15px; padding: 30px; margin-top: 10px;}\
		QPushButton#okBtn{background-color: #ECE6F5; color: black; border-radius: 5px; border: 1px solid black; padding: 5px; font-size: 20px; font-weight: bold;}\
		QPushButton#okBtn:hover{background-color: #F594C5;}");
}

SImageModifyView::~SImageModifyView()
{
}

void SImageModifyView::init()
{
	auto mainLayout = new QVBoxLayout;

	auto scrollArea = new QScrollArea;
	scrollArea->setWidgetResizable(true);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


	auto main_widget = new QWidget;
	m_mainLayout = new QVBoxLayout;


	//------------------第一层-------------------
	QHBoxLayout* topLayout = new QHBoxLayout;

	auto backBtn = new QPushButton;
	backBtn->setIcon(QIcon(":/ResourceClient/back.png"));
	backBtn->setFixedSize(30, 30);
	auto okbtn = new QPushButton("确定修改图片");
	okbtn->setObjectName("okBtn");
	connect(okbtn, &QPushButton::clicked, this, [=]() {
		updateImageInfo();
		});
	connect(backBtn, &QPushButton::clicked, this, [=]() {
		this->close();
		});

	topLayout->addWidget(backBtn);
	topLayout->addStretch();
	topLayout->addWidget(okbtn);
	topLayout->addStretch();
	//-------------------------------------------

	//--------------第二层 图片显示区域----------
	QHBoxLayout* middleLayout = new QHBoxLayout;
	m_previewImage = new QLabel;
	m_previewImage->setFixedSize(this->width() -50, this->height() - 50);
	m_previewImage->setText("图片显示");
	m_previewImage->setAlignment(Qt::AlignCenter);
	m_previewImage->setStyleSheet("background-color: #DBDBDB; border-radius: 15px; border: 1px solid #DEDEDE;");
	m_previewImage->setScaledContents(true);
	middleLayout->addWidget(m_previewImage);
	//-------------------------------------------

	//------------- 图片可以被直接修改的部分--------------------
	QWidget* directModifyWidget = new QWidget;
	directModifyWidget->setObjectName("directModifyWidget");
	auto directModifyLayout = new QVBoxLayout;

	auto labLayout = new QHBoxLayout;
	auto lab1 = new QLabel("可以直接修改：");
	lab1->setStyleSheet("font-size: 25px; font-weight: bold; color: #333333; font-family: \"Microsoft YaHei\";");
	labLayout->addWidget(lab1);
	labLayout->addStretch();

	auto formLayout_name = new QHBoxLayout;
	auto nameLab = new QLabel("图片名称：");
	m_nameEdit = new QLineEdit;
	m_nameEdit->setPlaceholderText("dog");
	formLayout_name->addWidget(nameLab);
	formLayout_name->addWidget(m_nameEdit);

	QHBoxLayout* formLayout_00 = new QHBoxLayout;
	auto typeLab = new QLabel("图片类型：");
	m_typeEdit = new QLineEdit;
	m_typeEdit->setPlaceholderText("动漫/游戏/生活/其他");
	formLayout_00->addWidget(typeLab);
	formLayout_00->addWidget(m_typeEdit);

	QHBoxLayout* formLayout_30 = new QHBoxLayout;
	auto shareLab = new QLabel("图片共享方式：");
	m_shareCombo = new QComboBox;
	m_shareCombo->addItem("私有");
	m_shareCombo->addItem("公开");
	m_shareCombo->addItem("授权");
	m_shareCombo->setCurrentIndex(1);
	formLayout_30->addWidget(shareLab);
	formLayout_30->addWidget(m_shareCombo);
	formLayout_30->addStretch();

	QHBoxLayout* formLayout_31 = new QHBoxLayout;
	auto downloadLab = new QLabel("图片下载方式：");
	m_downloadCombo = new QComboBox;
	m_downloadCombo->addItem("禁止下载");
	m_downloadCombo->addItem("允许下载");
	m_downloadCombo->addItem("授权下载");
	m_downloadCombo->setCurrentIndex(1);
	formLayout_31->addWidget(downloadLab);
	formLayout_31->addWidget(m_downloadCombo);
	formLayout_31->addStretch();
	
	QHBoxLayout* descLayout = new QHBoxLayout;
	auto descLab = new QLabel("图片描述");
	m_imageDescEdit = new QTextEdit;
	m_imageDescEdit->setPlaceholderText("主人很懒，还没有描述这张图片~");
	descLayout->addWidget(descLab);
	descLayout->addWidget(m_imageDescEdit);

	directModifyLayout->addSpacing(5);
	directModifyLayout->addLayout(labLayout);
	directModifyLayout->addSpacing(20);
	directModifyLayout->addLayout(formLayout_name);
	directModifyLayout->addSpacing(20);
	directModifyLayout->addLayout(formLayout_00);
	directModifyLayout->addSpacing(20);
	directModifyLayout->addLayout(formLayout_30);
	directModifyLayout->addSpacing(20);
	directModifyLayout->addLayout(formLayout_31);
	directModifyLayout->addSpacing(20);
	directModifyLayout->addLayout(descLayout);
	directModifyLayout->addSpacing(5);
	directModifyWidget->setLayout(directModifyLayout);
	//--------------------------------------------


	//---------------需要点击图片进入加工后才可以修改-----------------
	auto indirectModifyWidget = new QWidget;
	indirectModifyWidget->setObjectName("indirectModifyWidget");
	auto indirectModifyLayout = new QVBoxLayout;

	auto lab2Layout = new QHBoxLayout;
	auto lab2 = new QLabel("加工图片时修改：");
	lab2->setStyleSheet("font-size: 25px; font-weight: bold; color: #333333; font-family: \"Microsoft YaHei\";");
	lab2Layout->addWidget(lab2);
	lab2Layout->addStretch();

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
	m_qualityEdit->setEnabled(false);
	formLayout_20->addWidget(qualityLab);
	formLayout_20->addWidget(m_qualityEdit);

	indirectModifyLayout->addSpacing(5);
	indirectModifyLayout->addLayout(lab2Layout, 1);
	indirectModifyLayout->addSpacing(20);
	indirectModifyLayout->addLayout(formLayout_01,1);
	indirectModifyLayout->addSpacing(20);
	indirectModifyLayout->addLayout(formLayout_10, 1);
	indirectModifyLayout->addSpacing(20);
	indirectModifyLayout->addLayout(formLayout_11, 1);
	indirectModifyLayout->addSpacing(20);
	indirectModifyLayout->addLayout(formLayout_20, 1);
	indirectModifyLayout->addSpacing(5);
	indirectModifyWidget->setLayout(indirectModifyLayout);
	//----------------------------------------------

	QHBoxLayout* formLayout_21 = new QHBoxLayout;
	auto uploadTimeLab = new QLabel("图片上传时间：");
	m_uploadTimeEdit = new QLineEdit;
	formLayout_21->addWidget(uploadTimeLab);
	formLayout_21->addWidget(m_uploadTimeEdit);

	m_mainLayout->addLayout(topLayout, 1);
	m_mainLayout->addLayout(middleLayout, 4);
	m_mainLayout->addSpacing(50);
	m_mainLayout->addWidget(directModifyWidget, 2);
	m_mainLayout->addSpacing(50);
	m_mainLayout->addWidget(indirectModifyWidget, 2);

	main_widget->setLayout(m_mainLayout);
	scrollArea->setWidget(main_widget);

	mainLayout->addWidget(scrollArea);
	this->setLayout(mainLayout);
	
}

void SImageModifyView::setData(const QJsonObject& jImage)
{
	image = ImageInfo(jImage);

	//加载ui数据
	//m_titleAbsPath->setText(m_fileInfo.absoluteFilePath());
	m_nameEdit->setText(image.m_name);
	m_sizeEdit->setText(QString::number(image.m_size / 1024) + "KB");
	m_formatEdit->setText(image.m_format);
	m_resolutionEdit->setText(image.m_ResolutionRatio);
	m_uploadTimeEdit->setText(image.m_upload_time);
	m_typeEdit->setText(image.m_type);
	m_qualityEdit->setText(image.m_quality);
	m_shareCombo->setCurrentIndex(image.m_share);
	m_downloadCombo->setCurrentIndex(image.m_download);
	m_imageDescEdit->setText(image.m_desc);

	//加载图片预览
	QPixmap pixmap(image.m_path);
	m_previewImage->setPixmap(pixmap);
	m_previewImage->setFixedSize(500, 500);
}

//提交修改的图片信息
void SImageModifyView::updateImageInfo()
{
	QJsonObject jImage;
	jImage["image_id"] = image.m_id;
	jImage["image_name"] = m_nameEdit->text();
	jImage["image_type"] = m_typeEdit->text();
	jImage["image_format"] = m_formatEdit->text();
	jImage["image_size"] = image.m_size;
	jImage["image_ResolutionRatio"] = m_resolutionEdit->text();
	jImage["image_quality"] = m_qualityEdit->text();
	jImage["image_share"] = m_shareCombo->currentIndex();
	jImage["image_download"] = m_downloadCombo->currentIndex();
	jImage["description"] = m_imageDescEdit->toPlainText();
	/*
	可以改：image_name,image_type,image_share,image_download,description
	*/
	SHttpClient(URL("/api/user/modify_image")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json(jImage)
		.fail([=](const QString& msg, int code) {
#ifdef _DEBUG
		qWarning() << msg << code;
#endif
			})
		.success([=](const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
#ifdef _DEBUG
			qWarning() << "Json解析错误: " << error.errorString();
#endif
			return;
		}
		if (jdom["code"].toInt() < 1000) {
			QMessageBox::information(this, "提示", "修改图片信息成功！");
			emit onCloseAndModified();
			this->close();
		}
		else {
#ifdef _DEBUG 
			qWarning() << "查询数据失败: " << jdom["message"].toString();
#endif
		}
			})
		.post(); // 发送post请求
		
}
