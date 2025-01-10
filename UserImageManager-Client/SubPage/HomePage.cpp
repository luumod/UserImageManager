#include "HomePage.h"
#include "RoundedImageWidget.h"
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
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFormLayout>

HomePage::HomePage(QWidget *parent) 
	: QWidget(parent)
{
	this->setContentsMargins(0, 0, 0, 0);
	init();

}

HomePage::~HomePage()
{
}

void HomePage::init()
{	
	m_mainLayout = new QVBoxLayout;
	
	QHBoxLayout* topLayout = new QHBoxLayout;

	QFormLayout* formLayout_1 = new QFormLayout;
	auto titleLab = new QLabel("图片路径：");
	auto titleInput = new QLineEdit;
	titleInput->setPlaceholderText("完整路径");
	formLayout_1->addRow(titleLab, titleInput);

	QFormLayout* formLayout_2 = new QFormLayout;
	auto typeLab = new QLabel("图片类型：");
	auto typeCombo = new QComboBox;
	typeCombo->addItem("头像");
	typeCombo->addItem("背景");
	typeCombo->addItem("其他");
	formLayout_2->addRow(typeLab, typeCombo);

	QFormLayout* formLayout_3 = new QFormLayout;
	auto saveLab = new QLabel("图片共享方式：");
	auto saveCombo = new QComboBox;
	saveCombo->addItem("公有");
	saveCombo->addItem("私有");
	saveCombo->addItem("仅好友可见");
	formLayout_3->addRow(saveLab, saveCombo);

	topLayout->addLayout(formLayout_1,4);
	topLayout->addSpacing(20);
	topLayout->addLayout(formLayout_2,2);
	topLayout->addSpacing(20);
	topLayout->addLayout(formLayout_3,2);

	m_mainLayout->addLayout(topLayout);

	QHBoxLayout* middleLayout = new QHBoxLayout;
	auto uploadBtn = new SBigIconButton(":/ResourceClient/create.png", "上传图片", 10);
	m_previewImage = new RoundedImageWidget;
	uploadBtn->setFixedSize(300,300);
	m_previewImage->setFixedSize(300,300);
	m_previewImage->setStyleSheet("border: 1px solid #ccc;");
	middleLayout->addWidget(uploadBtn);
	middleLayout->addWidget(m_previewImage);
	m_mainLayout->addLayout(middleLayout);

	QHBoxLayout* contentLayout = new QHBoxLayout;
	auto descLab = new QLabel("图片描述");
	m_imageDescEdit = new QTextEdit;
	m_imageDescEdit->setPlaceholderText("主人很懒，还没有描述这张图片~");
	contentLayout->addWidget(descLab);
	contentLayout->addWidget(m_imageDescEdit);
	m_mainLayout->addLayout(contentLayout);


	QHBoxLayout* bottomLayout = new QHBoxLayout;
	auto okbtn = new QPushButton("上传");
	bottomLayout->addStretch();
	bottomLayout->addWidget(okbtn);
	bottomLayout->addStretch();
	m_mainLayout->addLayout(bottomLayout);

	this->setLayout(m_mainLayout);

	connect(uploadBtn, &SBigIconButton::clicked, this, [=]() {
		QString filepath = uploadImage(); //返回图片路径
		if (!filepath.isEmpty()) {
			titleInput->setText(filepath);
		}
		else {
			QMessageBox::warning(this, "失败", "请重新选择图片");
		}
		});
	connect(okbtn, &QPushButton::clicked, this, [=]() {
		//检查合法性
		if (m_previewImage->isNull()) {
			QMessageBox::warning(this, "失败", "请先上传图片");
		}
		else {
			//上传图片
			auto desc = m_imageDescEdit->toPlainText().isEmpty() ? m_imageDescEdit->placeholderText() : m_imageDescEdit->toPlainText();
			postImage(titleInput->text(), desc);
		}
		});
}

QString HomePage::uploadImage() {
	auto path = sApp->globalConfig()->value("user/upload_image_path", //默认选择上传文件的路径
		QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
	auto filename = QFileDialog::getOpenFileName(this, "选择上传图片", path, "images (*.jpg;*.jpeg;*.png)");
	if (filename.isEmpty())
		return QString();
	sApp->globalConfig()->setValue("user/upload_image_path", QFileInfo(filename).canonicalPath());

	m_previewImage->setPixmap(filename); 
	return filename;
}

void HomePage::postImage(const QString& filepath, const QString& desc) {
	QFile file = QFile(filepath);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, "失败", "图片打开失败");
		return;
	}

	//上传的头像不能超过3M
	if (file.size() > 3 * 1024 * 1034) {
		QMessageBox::warning(this, "失败", "请选择3M以内的图片上传");
		return;
	}
	auto url = URL("/api/user/upload_image?user_id=" 
		+ sApp->globalConfig()->value("user/user_id").toString() 
		+ "&description=" + desc);

	QNetworkAccessManager* mangaer = new QNetworkAccessManager(this);
	QNetworkRequest request(url);
	request.setRawHeader("Authorization", "Bearer" + sApp->userData("user/token").toString().toUtf8());
	request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-www-form-urlencoded");

	QHttpMultiPart* mpart = new QHttpMultiPart(this);
	QHttpPart part;
	part.setBody(file.readAll());
	part.setHeader(QNetworkRequest::ContentTypeHeader,
		QString("image/%1").arg(QFileInfo(filepath).suffix()));
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
		QString(R"(attachment;name="file";filename="%1")")
		.arg(QFileInfo(filepath).fileName()));
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