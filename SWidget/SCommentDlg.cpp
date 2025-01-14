#include "SCommentDlg.h"
#include "SUserComment.h"
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
#include <QScrollArea>
#include <QScrollBar>

SCommentDlg::SCommentDlg(QWidget* parent)
	: QWidget(parent)
{
	this->setFixedWidth(1400);
	init();
}

inline SCommentDlg::~SCommentDlg()
{
}

inline void SCommentDlg::init()
{

	auto main_layout = new QVBoxLayout;
	this->setLayout(main_layout);

	m_topWidget = new CommentTopWidget;

	//顶部
	main_layout->addWidget(m_topWidget);

	//评论输入区
	auto middleLayout = new QHBoxLayout;
	auto textLab = new QLabel("评论");
	textLab->setAlignment(Qt::AlignCenter);
	textLab->setStyleSheet("font-size: 22px; font-family: 微软雅黑; border: 2px solid rgb(232,231,232);");

	auto textLayout = new QVBoxLayout;
	auto textEdit = new QTextEdit;
	textEdit->setFixedHeight(300);
	textEdit->setPlaceholderText("请输入评论...");
	textEdit->setCursorWidth(2);

	QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
	shadowEffect->setBlurRadius(10); // 设置模糊半径
	shadowEffect->setColor(QColor(100, 100, 100, 128)); // 设置阴影颜色（这里是半透明的黑色）
	shadowEffect->setOffset(0, 0); // 设置阴影的偏移量
	textEdit->setGraphicsEffect(shadowEffect);

	auto submitLayout = new QHBoxLayout;
	auto nameEdit = new QLineEdit;
	nameEdit->setPlaceholderText("请输入昵称...");
	nameEdit->setFixedWidth(300);
	nameEdit->setFixedHeight(30);
	nameEdit->setStyleSheet("font-size: 18px; font-family: 微软雅黑; border: 1px solid black; background-color: transparent;");

	auto emailEdit = new QLineEdit;
	emailEdit->setPlaceholderText("请输入邮箱...");
	emailEdit->setFixedWidth(300);
	emailEdit->setFixedHeight(30);
	emailEdit->setStyleSheet("font-size: 18px; font-family: 微软雅黑; border: 1px solid black; background-color: transparent;");

	auto submitBtn = new QPushButton("提交");
	submitBtn->setFixedWidth(100);
	submitBtn->setFixedHeight(30);
	submitBtn->setStyleSheet("font-size: 18px; font-family: 微软雅黑; background-color: rgb(167,184,205); color: white; border: none;");

	submitLayout->addWidget(nameEdit);
	submitLayout->addWidget(emailEdit);
	submitLayout->addStretch();
	submitLayout->addWidget(submitBtn);


	textLayout->addSpacing(30);
	textLayout->addWidget(textEdit);
	textLayout->addLayout(submitLayout);


	middleLayout->addWidget(textLab, 1);
	middleLayout->addLayout(textLayout, 7);

	//底部：评论区
	m_bottomLayout = new QVBoxLayout;
	m_bottomLayout->setSpacing(20);
		


	main_layout->addLayout(middleLayout);
	main_layout->addStretch();
	main_layout->addLayout(m_bottomLayout);

	main_layout->setSpacing(5);
}

void SCommentDlg::addUserComment(SUserComment* userComment)
{
	m_userComments.push_back(userComment);
}

void SCommentDlg::setData(const QString& userName, const QString& userAvatar, const QString& userComment, const QString& time)
{
	addUserComment(new SUserComment(userAvatar,userName , userComment, time));
	for (const auto& userComment : m_userComments) {
		m_bottomLayout->addWidget(userComment);
	}
}

void SCommentDlg::clearData()
{
	for (int i = m_userComments.size() - 1; i >= 0; i--) {
		auto userComment = m_userComments[i];
		if (userComment) {
			m_bottomLayout->removeWidget(userComment);
			delete userComment;
			userComment = nullptr;
		}
	}
	m_userComments.clear();
}


CommentTopWidget::CommentTopWidget(QWidget* parent)
{
	this->setFixedSize(1400, 70);
	init();
	this->setStyleSheet("background-color: rgb(220,221,224); border-top-left-radius: 30px; border-top-right-radius: 30px;");
}

void CommentTopWidget::init()
{
	auto mainLayout = new QHBoxLayout;

	auto titleLab = new QLabel("评论");
	titleLab->setFixedHeight(this->height());
	titleLab->setFixedWidth(200);
	titleLab->setStyleSheet("color: white; font-size: 20px; font-weight: bold; font-family: 微软雅黑; background-color: rgb(167,184,205);");
	titleLab->setAlignment(Qt::AlignCenter);
	titleLab->setText("评论");

	mainLayout->addWidget(titleLab);
	mainLayout->addStretch();

	this->setLayout(mainLayout);


}
