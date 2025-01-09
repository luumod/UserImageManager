#include "PersonalInfo.h"
#include "ui_PersonalInfo.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QDir>
#include <QFileDialog>

PersonalInfo::PersonalInfo(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::PersonalInfo)
	, m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	init();
}

PersonalInfo::~PersonalInfo()
{
	delete ui;
}

void PersonalInfo::init()
{
	ui->tableView->setModel(m_model);

	connect(ui->searchEdit, &QLineEdit::returnPressed, this, &PersonalInfo::onSearch);
	connect(ui->searchEdit, &QLineEdit::textChanged, this, [=](const QString& text) {if (text.isEmpty()) onSearch(); });
	connect(ui->searchBtn, &QPushButton::clicked, this, &PersonalInfo::onSearch);
	connect(ui->uploadBtn, &QPushButton::clicked, this, &PersonalInfo::onUpload);
}

void PersonalInfo::onSearch()
{
	QVariantMap params;
	auto filter = ui->searchEdit->text();
	if (!filter.isEmpty()) {
		params.insert("user_id", filter);
	}
	SHttpClient(URL("/api/user/get_image"))
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.debug(true)
		.params(params)
		.success([=](const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
			qWarning() << "Json解析错误: " << error.errorString();
			return;
		}
		parseJson(jdom.object()); // 解析json数据
			})
		.get(); // 发送get请求
}

void PersonalInfo::onUpload()
{
	//auto file = QFileDialog::getOpenFileName(this, "上传图片", QDir::currentPath(), "图片文件 (*.jpg *.png *.jpeg *.gif)");
	//if (!file.isEmpty()) {
	//	QFile f(file);
	//	if (f.open(QIODevice::ReadOnly)) {
	//		auto data = f.readAll();
	//		SHttpClient(URL("/api/user/upload_image"))
	//			.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
	//			.debug(true)
	//			.files({ {"file", file, data} })
	//			.success([=](const QByteArray& data) {
	//			QJsonParseError error;
	//			auto jdom = QJsonDocument::fromJson(data, &error);
	//			if (error.error != QJsonParseError::NoError) {
	//				qWarning() << "Json解析错误: " << error.errorString();
	//				return;
	//			}
	//			parseJson(jdom.object()); // 解析json数据
	//			})
	//			.post(); // 发送post请求
	//	}
	//}
}

void PersonalInfo::parseJson(const QJsonObject& obj)
{
	//image_path,image_name,image_size,image_type,upload_time,description
	m_model->clear();
	m_model->setHorizontalHeaderLabels({ "路径", "名称", "大小", "类型", "上传时间", "图片描述"});
	auto jArray = obj["data"]["images"].toArray();
	for (int i = 0; i < jArray.size(); i++) {
		QList<QStandardItem*> items;
		auto jUser = jArray[i].toObject();

		items.append(new QStandardItem(jUser["image_path"].toVariant().toString()));
		items.append(new QStandardItem(jUser["image_name"].toVariant().toString()));
		items.append(new QStandardItem(jUser["image_size"].toVariant().toInt()));
		items.append(new QStandardItem(jUser["image_type"].toVariant().toString()));
		items.append(new QStandardItem(jUser["upload_time"].toVariant().toString()));
		items.append(new QStandardItem(jUser["description"].toVariant().toString()));

		qDebug()<<jUser["image_path"].toVariant().toString();
		qDebug()<<jUser["image_name"].toVariant().toString();
		qDebug()<<jUser["image_size"].toVariant().toInt();
		qDebug()<<jUser["image_type"].toVariant().toString();
		qDebug()<<jUser["upload_time"].toVariant().toString();
		qDebug()<<jUser["description"].toVariant().toString();
		m_model->appendRow(items);
	}
	qDebug() << jArray;
}
