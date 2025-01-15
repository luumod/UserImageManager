#include "SCommentDlg.h"
#include "SUserComment.h"
#include "SHttpClient.h"
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
#include <QScrollArea>
#include <QScrollBar>
#include <QMessageBox>

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
	auto m_commentEdit = new QTextEdit;
	m_commentEdit->setFixedHeight(300);
	m_commentEdit->setPlaceholderText("请输入评论...");
	m_commentEdit->setCursorWidth(2);

	QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
	shadowEffect->setBlurRadius(10); // 设置模糊半径
	shadowEffect->setColor(QColor(100, 100, 100, 128)); // 设置阴影颜色（这里是半透明的黑色）
	shadowEffect->setOffset(0, 0); // 设置阴影的偏移量
	m_commentEdit->setGraphicsEffect(shadowEffect);

	auto submitLayout = new QHBoxLayout;
	auto m_nameEdit = new QLineEdit;
	m_nameEdit->setPlaceholderText("请输入昵称（选填）：");
	m_nameEdit->setFixedWidth(300);
	m_nameEdit->setFixedHeight(30);
	m_nameEdit->setStyleSheet("font-size: 18px; font-family: 微软雅黑; border: 1px solid black; background-color: transparent;");

	auto m_emailEdit = new QLineEdit;
	m_emailEdit->setPlaceholderText("请输入邮箱（选填）：");
	m_emailEdit->setFixedWidth(300);
	m_emailEdit->setFixedHeight(30);
	m_emailEdit->setStyleSheet("font-size: 18px; font-family: 微软雅黑; border: 1px solid black; background-color: transparent;");

	auto submitBtn = new QPushButton("提交");
	submitBtn->setFixedWidth(100);
	submitBtn->setFixedHeight(30);
	submitBtn->setStyleSheet("font-size: 18px; font-family: 微软雅黑; background-color: rgb(167,184,205); color: white; border: none;");

	submitLayout->addWidget(m_nameEdit);
	submitLayout->addWidget(m_emailEdit);
	submitLayout->addStretch();
	submitLayout->addWidget(submitBtn);


	textLayout->addSpacing(30);
	textLayout->addWidget(m_commentEdit);
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


	connect(submitBtn, &QPushButton::clicked, this, [=]() {
		//检查输入
		auto input_comment = m_commentEdit->toPlainText();
		if (input_comment.isEmpty()) {
			QMessageBox::warning(this, "提示", "评论不能为空！");
			return;
		}
		//当前评论者的id
		auto user_id = sApp->userData("user/id").toInt();
		SHttpClient(URL("/api/user/comment_image?user_id=" + QString::number(user_id) + "&image_id=" + QString::number(m_image_id) + "&comment_content=" + input_comment)).debug(true)
			.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
			.success([=](const QByteArray& data) {
			QJsonParseError error;
			auto jdom = QJsonDocument::fromJson(data, &error);
			if (error.error != QJsonParseError::NoError) {
				qWarning() << "Json解析错误: " << error.errorString();
				return;
			}
			if (jdom["code"].toInt() < 1000) {
				auto comment_id = jdom["data"]["comment_id"].toInt();//返回新插入的评论的id
				//评论成功，更新布局
				addUserComment(
					comment_id,
					sApp->userData("user/user_name").toString(),
					sApp->userData("user/avatar_path").toString(),
					input_comment,
					QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
			}
				})
			.post(); // 发送post请求
		});
}

void SCommentDlg::deleteComment(int comment_id) {
	//根据comment_id来删除
	SHttpClient(URL("/api/user/delete_comment?owner_id=" + sApp->userData("user/id").toString() + "&comment_id=" + QString::number(comment_id))).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.success([=](const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
			qWarning() << "Json解析错误: " << error.errorString();
			return;
		}
		if (jdom["code"].toInt() < 1000) {
			//删除成功，更新布局
			m_bottomLayout->removeWidget(m_userCommentMap[comment_id]); 
			delete m_userCommentMap[comment_id]; //会自动断开信号与槽
			m_userCommentMap[comment_id] = nullptr;
			m_userCommentMap.remove(comment_id);
		}
			})
		.post(); // 发送post请求
}


void SCommentDlg::topComment(int comment_id)
{
	m_bottomLayout->removeWidget(m_userCommentMap[comment_id]);
	m_bottomLayout->insertWidget(0, m_userCommentMap[comment_id]);
}

void SCommentDlg::addUserComment(int comment_id, const QString& userName, const QString& userAvatar, const QString& comment_conetent, const QString& time)
{
	auto cW = new SUserComment(comment_id, userName, userAvatar, comment_conetent, time); //新的评论
	connect(cW, &SUserComment::deleteComment, this, &SCommentDlg::deleteComment); //有必要一个评论连接一个信号？
	connect(cW, &SUserComment::topComment, this, &SCommentDlg::topComment); //有必要一个评论连接一个信号？
	m_userCommentMap.insert(comment_id, cW);
	//添加到布局中
	m_bottomLayout->addWidget(cW);
}

void SCommentDlg::clearData(int new_image_id)
{
	m_image_id = new_image_id;//更新的图片id
	for (auto& userCommentWidget : m_userCommentMap) {
		if (userCommentWidget) {
			m_bottomLayout->removeWidget(userCommentWidget);
			delete userCommentWidget;
			userCommentWidget = nullptr;
		}
	}
	m_userCommentMap.clear();
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
