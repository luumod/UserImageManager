#include "SImageDetailDlg.h"
#include "SApp.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QPushButton>
#include <QFile>
#include <QDir>
#include <QPainter>
#include <QGraphicsDropShadowEffect>


SImageDetailDlg::SImageDetailDlg(QWidget* parent)
	:QWidget(parent)
{
	this->setFixedSize(1340, 700);
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

void SImageDetailDlg::init()
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
	m_imageLabel->setScaledContents(true);

	auto loveLayout = new QHBoxLayout;
	auto likeBtn = new QPushButton("👍赞(33)");
	auto downloadBtn = new QPushButton("📄下载(19)");
	auto starBtn = new QPushButton("🌟收藏(32)");
	connect(likeBtn, &QPushButton::clicked, [=]() {
		qDebug() << "likeBtn clicked"; });
	connect(downloadBtn, &QPushButton::clicked, [=]() {
		qDebug() << "downloadBtn clicked"; });
	connect(starBtn, &QPushButton::clicked, [=]() {
		qDebug() << "starBtn clicked"; });
	loveLayout->addWidget(likeBtn);
	loveLayout->addWidget(downloadBtn);
	loveLayout->addWidget(starBtn);

	auto loveWidget = new QWidget;
	loveWidget->setFixedWidth(m_imageLabel->width());
	loveWidget->setLayout(loveLayout);

	//likeBtn->setStyleSheet("font-size: 18px; color: black; font: bold; margin-top: 5px; border: 1px dashed black;");
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

void SImageDetailDlg::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(236, 235, 236));
	painter.drawRoundedRect(0, 0, this->width(), this->height(), 20, 20);
	painter.end();
}

void SImageDetailDlg::setData(ImageInfo info)
{
	m_imageInfo = info;

	updateUi();
}

void SImageDetailDlg::updateUi()
{
	//获取图片
	auto path = QDir::currentPath() + "/" + m_imageInfo.m_path;
	QFile imageFile(path);
	if (!imageFile.open(QIODevice::ReadOnly)) {
		qWarning() << "Failed to open image file:" << imageFile.errorString();
		return;
	}

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
	m_imageLabel->setPixmap(QPixmap::fromImage(QImage(imageFile.fileName())));
}


TopWidget::TopWidget(QWidget* parent)
{
	init();
	this->setFixedSize(1300, 70);
	this->setStyleSheet("background-color: rgb(220,221,224); border-top-left-radius: 70px; border-top-right-radius: 70px; ");
}

void TopWidget::init()
{
	auto mainLayout = new QHBoxLayout;

	m_pathLabel = new QLabel("个人仓库 / 粉色小屋.png:");
	m_pathLabel->setStyleSheet("font-size: 18px; color: black; font-family: 微软雅黑; margin-left: 30px; margin-top: 10px; ");
	m_loveBtn = new QPushButton("⭐收藏");
	m_loveBtn->setStyleSheet("background-color: #FFC300; border-radius: 10px; font-size: 16px; color: white; margin-right: 30px; margin-top: 10px; ");

	mainLayout->addWidget(m_pathLabel);
	mainLayout->addStretch();
	mainLayout->addWidget(m_loveBtn);

	this->setLayout(mainLayout);


}
