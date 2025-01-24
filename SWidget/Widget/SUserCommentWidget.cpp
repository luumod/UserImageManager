#include "SUserCommentWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QPixmap>

SUserCommentWidget::SUserCommentWidget(int comment_id, const QString& userName, const QString& avatarPath, const QString& comment_content, const QString& commentTime, bool isTop, QWidget* parent)
	: QWidget(parent)
	, m_comment_id(comment_id)
	, m_userName(userName)
	, m_avatarPath(avatarPath)
	, m_comment(comment_content)
	, m_commentTime(commentTime)
	, m_isTop(isTop)
{
	//this->setFixedSize(1400, 140);
	init();
	this->setObjectName("this");

	/*QPalette palette;
	palette.setColor(QPalette::Window, QColor("#DAE6D4"));
	this->setPalette(palette);
	this->setAutoFillBackground(true);*/

	if (m_isTop) { //置顶评论
		this->setStyleSheet("\
         QTextEdit{font-size: 22px; font-weight: bold; font-family: 微软雅黑; border: 3px solid red; border-radius: 15px; background-color: transparent;} \
		");
	}
	else {
		this->setStyleSheet("\
         QTextEdit{font-size: 18px; font-family: 微软雅黑; border: 3px solid #CAE5C6; border-radius: 15px; background-color: transparent;} \
		");
	}
}

SUserCommentWidget::~SUserCommentWidget(){
}

void SUserCommentWidget::init()
{
	auto mainLayout = new QHBoxLayout;

	//左侧：头像 + 昵称
	auto leftLayout = new QVBoxLayout;


	if (m_avatarPath.isEmpty()) {
		m_avatarPath = ":/ResourceClient/default_avatar.png";
	}
	QPixmap headPixmap(m_avatarPath);
	auto avatarLabel = new QLabel;
	avatarLabel->setFixedSize(70, 70);
	avatarLabel->setPixmap(headPixmap);
	avatarLabel->setMask(QRegion(avatarLabel->rect(), QRegion::RegionType::Ellipse));
	avatarLabel->setScaledContents(true);
	auto nickNameLabel = new QLabel(m_userName);
	nickNameLabel->setAlignment(Qt::AlignCenter);

	leftLayout->addWidget(avatarLabel);
	leftLayout->addWidget(nickNameLabel);

	//中侧：评论内容
	auto contentLayout = new QVBoxLayout;
	auto contentEdit = new QTextEdit;
	contentEdit->setText(m_comment);
	contentEdit->setMinimumHeight(120);
	contentEdit->setReadOnly(true);

	auto timeLabel = new QLabel;
	timeLabel->setText(m_commentTime);
	timeLabel->setStyleSheet("font-size: 14px; color: rgb(150,150,150); font-family: 微软雅黑; ");
	timeLabel->setAlignment(Qt::AlignLeft);

	contentLayout->setSpacing(2);
	contentLayout->addWidget(contentEdit);
	contentLayout->addWidget(timeLabel);

	//右侧：删除 + 回复
	auto rightLayout = new QVBoxLayout;
	auto deleteButton = new QPushButton("删除");
	auto setTopButton = new QPushButton("置顶");
	connect(deleteButton, &QPushButton::clicked, this, [=]() {emit deleteComment(m_comment_id); });
	connect(setTopButton, &QPushButton::clicked, this, [=]() {emit topComment(m_comment_id); });

	rightLayout->addWidget(deleteButton);
	rightLayout->addWidget(setTopButton);

	//主布局
	mainLayout->addLayout(leftLayout);
	mainLayout->addLayout(contentLayout);
	mainLayout->addLayout(rightLayout);

	this->setLayout(mainLayout);
}
