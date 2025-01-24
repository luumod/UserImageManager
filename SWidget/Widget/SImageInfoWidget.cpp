#include "SImageInfoWidget.h"
#include "SApp.h"
#include "SHttpClient.h"
#include "SResultCode.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QPushButton>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QStandardPaths>
#include <QMessageBox>

SImageInfoWidget::SImageInfoWidget(QWidget* parent)
	:QWidget(parent)
{
	//this->setFixedSize(1340, 700);
	init();
	setMouseTracking(true);
	this->setStyleSheet(" QLineEdit{border: 1px solid #DEDEDE; border-radius: 5px; padding: 5px; background-color: transparent; width: 300px; height: 50px; font-size: 15px; color: #333333;} \
                           QComboBox{border: 2px solid #D4DBC7; border-radius: 5px; padding: 5px; background-color: transparent; width: 300px; height: 50px; font-size: 15px; color: #333333;}  \
                            QTextEdit{border: 1px solid #DEDEDE; border-radius: 5px; padding: 5px; background-color: transparent; font-size: 15px; color: #333333;} \
		QLabel{font-size: 15px; font-weight: bold; color: #333333; font-family: \"Microsoft YaHei\";}\
        QPushButton{border:2px solid #D4DBC7; border-radius: 5px; padding: 5px; background-color: transparent; width: 300px; height: 50px; font-size: 15px; color: #333333;}\
		QPushButton:hover{ background-color: #F8E6FF;}");

	//为此窗口的边缘添加阴影
	QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
	shadowEffect->setBlurRadius(15); // 设置模糊半径
	shadowEffect->setColor(QColor(100, 100, 100, 128)); // 设置阴影颜色（这里是半透明的黑色）
	shadowEffect->setOffset(10, 10); // 设置阴影的偏移量
	this->setGraphicsEffect(shadowEffect);
}

void SImageInfoWidget::init()
{
	auto mainLayout = new QVBoxLayout;

	//-----------顶部-------------
	m_topWidget = new TopWidget;
	//---------------------------

	//---------------中间---------------------
	auto middleLayout = new QHBoxLayout;
	//中间左边
	auto middleLeftLayout = new QVBoxLayout;
	m_imageLabel = new QLabel;
	m_imageLabel->setFixedSize(500, 500);
	m_imageLabel->setAlignment(Qt::AlignCenter);
	//m_imageLabel->setScaledContents(true);

	auto loveLayout = new QHBoxLayout;
	m_likeBtn = new QPushButton("👍赞(33)");
	m_downloadBtn = new QPushButton("📄下载(19)");
	m_starBtn = new QPushButton("🌟收藏(32)");
	connect(m_likeBtn, &QPushButton::clicked, this, &SImageInfoWidget::onLikeBtnClicked);
	connect(m_downloadBtn, &QPushButton::clicked, this, &SImageInfoWidget::onDownloadBtnClicked);
	connect(m_starBtn, &QPushButton::clicked, this, &SImageInfoWidget::onStarBtnClicked);
	loveLayout->addWidget(m_likeBtn);
	loveLayout->addWidget(m_downloadBtn);
	loveLayout->addWidget(m_starBtn);

	auto loveWidget = new QWidget;
	loveWidget->setFixedWidth(m_imageLabel->width());
	loveWidget->setLayout(loveLayout);

	middleLeftLayout->addWidget(m_imageLabel);
	middleLeftLayout->addWidget(loveWidget);

	//中间右边
	auto middleRightLayout = new QVBoxLayout;

	auto titleLayout = new QHBoxLayout;
	m_imageName = new QLabel("粉色小屋");
	m_imageName->setStyleSheet("font-size: 24px; color: black; font-family: 微软雅黑; margin-left: 10px; margin-top: 10px; margin-bottom: 10px; ");
	titleLayout->addWidget(m_imageName);
	titleLayout->addStretch();

	auto typeLayout = new QHBoxLayout;
	auto typeLab = new QLabel("图片类型：");
	m_imageType = new QLineEdit;
	m_imageType->setReadOnly(true);
	m_imageType->setText("风景");
	typeLayout->addWidget(typeLab);
	typeLayout->addWidget(m_imageType);
	typeLayout->addStretch();

	auto formatLayout = new QHBoxLayout;
	auto formatLab = new QLabel("图片格式");
	m_imageFormat = new QLineEdit;
	m_imageFormat->setReadOnly(true);
	m_imageFormat->setText("png");
	formatLayout->addWidget(formatLab);
	formatLayout->addWidget(m_imageFormat);
	formatLayout->addStretch();

	auto ResolutionRatioLayout = new QHBoxLayout;
	auto RRLab = new QLabel("分辨率");
	m_imageRR = new QLineEdit;
	m_imageRR->setReadOnly(true);
	m_imageRR->setText("1920 * 1080");
	ResolutionRatioLayout->addWidget(RRLab);
	ResolutionRatioLayout->addWidget(m_imageRR);
	ResolutionRatioLayout->addStretch();

	auto ownerLayout = new QHBoxLayout;
	auto ownerLab = new QLabel("所有者");
	m_imageOwer = new QLineEdit;
	m_imageOwer->setReadOnly(true);
	m_imageOwer->setText("于良浩");
	ownerLayout->addWidget(ownerLab);
	ownerLayout->addWidget(m_imageOwer);
	ownerLayout->addStretch();

	auto sizeLayout = new QHBoxLayout;
	auto sizeLab = new QLabel("文件大小");
	m_imageSize = new QLineEdit;
	m_imageSize->setReadOnly(true);
	m_imageSize->setText("3MB");
	sizeLayout->addWidget(sizeLab);
	sizeLayout->addWidget(m_imageSize);
	sizeLayout->addStretch();

	auto timeLayout = new QHBoxLayout;
	auto timeLab = new QLabel("上传时间");
	m_imageDate = new QLineEdit;
	m_imageDate->setReadOnly(true);
	m_imageDate->setText("2025-01-12 16:49:11");
	timeLayout->addWidget(timeLab);
	timeLayout->addWidget(m_imageDate);
	timeLayout->addStretch();

	auto descLayout = new QHBoxLayout;
	auto descLab = new QLabel("图片介绍");
	m_imageDesc = new QTextEdit;
	m_imageDesc->setReadOnly(true);
	m_imageDesc->setText("这幅图片展示了一座充满浪漫气息的粉色小屋。小屋坐落在绿意盎然的花园中，周围点缀着各种花朵和树木，营造出一种宁静而又充满活力的氛围。粉色的外墙与周围的自然景色相得益彰，显得既温馨又可爱。屋顶覆盖着青色的瓦片，增添了几分复古的感觉。");
	descLayout->addWidget(descLab);
	descLayout->addWidget(m_imageDesc);

	middleRightLayout->addLayout(titleLayout);
	middleRightLayout->addLayout(typeLayout);
	middleRightLayout->addLayout(formatLayout);
	middleRightLayout->addLayout(ResolutionRatioLayout);
	middleRightLayout->addLayout(ownerLayout);
	middleRightLayout->addLayout(sizeLayout);
	middleRightLayout->addLayout(timeLayout);
	middleRightLayout->addLayout(descLayout);

	middleLayout->addLayout(middleLeftLayout);
	middleLayout->addSpacing(15);
	middleLayout->addLayout(middleRightLayout);

	auto middleWidget = new QWidget;
	middleWidget->setLayout(middleLayout);
	middleWidget->setFixedSize(m_topWidget->width(), 600);
	
	//-----------------------------------------


	//主布局
	mainLayout->addWidget(m_topWidget);
	mainLayout->addWidget(middleWidget);

	this->setLayout(mainLayout);
}

void SImageInfoWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(236, 235, 236));
	painter.drawRoundedRect(0, 0, this->width(), this->height(), 20, 20);
	painter.end();
}

void SImageInfoWidget::setData(ImageInfo info,int image_index)
{
	m_currentImageIndex = image_index;
	m_imageInfo = info;
	updateUi();
}

void SImageInfoWidget::updateUi()
{
	//获取图片
	auto path = QDir::currentPath() + "/" + m_imageInfo.m_path;
	QFile imageFile(path);
	if (!imageFile.open(QIODevice::ReadOnly)) {
		qWarning() << "Failed to open image file:" << imageFile.errorString();
		return;
	}

	m_topWidget->updateUi(m_imageInfo.m_name);

	//图片名称
	m_imageName->setText(m_imageInfo.m_name);
	//图片类型
	m_imageType->setText(m_imageInfo.m_type);
	//图片格式
	m_imageFormat->setText(m_imageInfo.m_format);
	//图片分辨率
	m_imageRR->setText(m_imageInfo.m_ResolutionRatio);
	//图片归属
	m_imageOwer->setText(sApp->userData("user/user_name").toString());
	//图片尺寸
	m_imageSize->setText(QString::number(m_imageInfo.m_size / 1024.0 ) + "KB");
	//图片上传时间
	m_imageDate->setText(m_imageInfo.m_upload_time);
	//图片介绍
	m_imageDesc->setText(m_imageInfo.m_desc);
	//加载图片
	m_imageLabel->setPixmap(QPixmap::fromImage(QImage(imageFile.fileName())).scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

	//获取点赞数，同时判断当前用户是否点赞
	SHttpClient(URL("/api/user/like_image?image_id=" + QString::number(m_imageInfo.m_id) + "&user_id=" + sApp->userData("user/id").toString())).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.success([=](const QByteArray& data) {
		auto json = QJsonDocument::fromJson(data).object();
		if (json["code"].toInt() < 1000) {
			if (json["code"].toInt() == SResultCode::ImageLiked.code) {
				//已经点赞了，显示取消点赞按钮
				m_likeBtn->setText(QString("👍取消赞(%1)").arg(m_imageInfo.m_likeCount));
			}
			else if (json["code"].toInt() == SResultCode::ImageUnliked.code) {
				//已经取消点赞了，显示点赞按钮
				m_likeBtn->setText(QString("👍点赞(%1)").arg(m_imageInfo.m_likeCount));
			}
		}
		else {
			qWarning() << "Failed to like image:" << json["message"].toString();
		}
			})
		.get();

	//获取收藏数，同时判断当前用户是否收藏
	SHttpClient(URL("/api/user/star_image?image_id=" + QString::number(m_imageInfo.m_id) + "&user_id=" + sApp->userData("user/id").toString())).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.success([=](const QByteArray& data) {
		auto json = QJsonDocument::fromJson(data).object();
		if (json["code"].toInt() < 1000) {
			if (json["code"].toInt() == SResultCode::ImageStared.code) {
				//已经收藏了，显示取消收藏按钮
				m_starBtn->setText(QString("🌟取消收藏(%1)").arg(m_imageInfo.m_starCount));
			}
			else if (json["code"].toInt() == SResultCode::ImageUnStared.code) {
				//已经取消收藏了，显示收藏按钮
				m_starBtn->setText(QString("🌟收藏(%1)").arg(m_imageInfo.m_starCount));
			}
		}
		else {
			qWarning() << "Failed to star image:" << json["message"].toString();
		}
			})
		.get();

	//获取下载数（下载无需查询）
	m_downloadBtn->setText(QString("📄下载次数(%1)").arg(m_imageInfo.m_downloadCount));
}

bool SImageInfoWidget::nextImage()
{
	auto total_image_count = sApp->userData("user/totla_image_count_in_one_page").toInt();
	if (m_currentImageIndex < total_image_count - 1) {
		m_currentImageIndex++;
		return true;
	}
	return false;
}

bool SImageInfoWidget::prevImage()
{
	if (m_currentImageIndex > 0) {
		m_currentImageIndex--;
		return true;
	}
	return false;
}

//点赞 - 取消点赞
void SImageInfoWidget::onLikeBtnClicked()
{
	SHttpClient(URL("/api/user/like_image?image_id=" + QString::number(m_imageInfo.m_id) + "&user_id=" + sApp->userData("user/id").toString())).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.success([=](const QByteArray& data) {
			auto json = QJsonDocument::fromJson(data).object();
			if (json["code"].toInt() < 1000) {
				if (json["code"].toInt() == SResultCode::ImageLiked.code) {
					//点赞成功
					m_imageInfo.m_likeCount++;
					m_likeBtn->setText(QString("👍取消赞(%1)").arg(m_imageInfo.m_likeCount));
					emit imageLiked(m_currentImageIndex);
				}
				else if (json["code"].toInt() == SResultCode::ImageUnliked.code) {
					//取消点赞成功
					m_imageInfo.m_likeCount--;
					m_likeBtn->setText(QString("👍点赞(%1)").arg(m_imageInfo.m_likeCount));
					emit imageUnLiked(m_currentImageIndex);
				}
			}
			else {
				qWarning() << "Failed to like image:" << json["message"].toString();
			}
		})
		.post();
}

//下载图片
void SImageInfoWidget::onDownloadBtnClicked()
{
	SHttpClient(URL("/api/user/download_image?image_id=" + QString::number(m_imageInfo.m_id))).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.fail([=](const QString& msg, int code) {
			})
		.success([=](const QByteArray& data){
				if (data.startsWith('{')){
					qDebug() << "下载图片失败";
				}
				else{
					auto img = QImage::fromData(data); //从服务器获取图片
					
					auto path = sApp->globalConfig()->value("user/download_image_path", //默认选择下载图片的存放路径
						QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
					auto filename = QFileDialog::getSaveFileName(this, "下载图片", path, "Images (*.png *.jpg *.jpeg)");
					if (filename.isEmpty()) {
						qDebug() << "取消下载";
						return;
					}
					sApp->globalConfig()->setValue("user/download_image_path", QFileInfo(filename).absoluteDir().absolutePath());
					if (img.save(filename)) { //保存图片到本地
						QMessageBox::information(this, "下载成功", "图片已保存到" + filename);
						m_imageInfo.m_downloadCount++;
						emit imageDownloaded(m_currentImageIndex);
						m_downloadBtn->setText(QString("📄下载次数(%1)").arg(m_imageInfo.m_downloadCount));
					}
					else {
						QMessageBox::warning(this, "下载失败", "图片保存失败");
					}
				}
			})
		.get();
}

//收藏- 取消收藏
void SImageInfoWidget::onStarBtnClicked()
{
	SHttpClient(URL("/api/user/star_image?image_id=" + QString::number(m_imageInfo.m_id) + "&user_id=" + sApp->userData("user/id").toString())).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.success([=](const QByteArray& data) {
		auto json = QJsonDocument::fromJson(data).object();
		if (json["code"].toInt() < 1000) {
			if (json["code"].toInt() == SResultCode::ImageStared.code) {
				//收藏成功
				m_imageInfo.m_starCount++;
				m_starBtn->setText(QString("🌟取消收藏(%1)").arg(m_imageInfo.m_starCount));
				emit imageStared(m_currentImageIndex);
			}
			else if (json["code"].toInt() == SResultCode::ImageUnStared.code) {
				//取消收藏成功
				m_imageInfo.m_starCount--;
				m_starBtn->setText(QString("🌟收藏(%1)").arg(m_imageInfo.m_starCount));
				emit imageUnStared(m_currentImageIndex);
			}
		}
		else {
			qWarning() << "Failed to star image:" << json["message"].toString();
		}
			})
		.post();
}


TopWidget::TopWidget(QWidget* parent)
{
	init();
	this->setMinimumWidth(1500);
	this->setFixedHeight(70);
	this->setStyleSheet("background-color: rgb(220,221,224); border-top-left-radius: 70px; border-top-right-radius: 70px; ");
}

void TopWidget::init()
{
	auto mainLayout = new QHBoxLayout;

	m_pathLabel = new QLabel("个人仓库 / 粉色小屋:");
	m_pathLabel->setStyleSheet("font-size: 18px; color: black; font-family: 微软雅黑; margin-left: 30px; margin-top: 10px; ");
	m_loveBtn = new QPushButton("⭐收藏");
	m_loveBtn->setStyleSheet("background-color: #FFC300; border-radius: 10px; font-size: 16px; color: white; margin-right: 30px; margin-top: 10px; ");

	mainLayout->addWidget(m_pathLabel);
	mainLayout->addStretch();
	mainLayout->addWidget(m_loveBtn);

	this->setLayout(mainLayout);


}

void TopWidget::updateUi(const QString& name)
{
	m_pathLabel->setText(QString("我的图片 / %1:").arg(name));
}
