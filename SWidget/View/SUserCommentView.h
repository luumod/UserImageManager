#ifndef SUERCOMMENTVIEW_H_
#define  SUERCOMMENTVIEW_H_

#include <QWidget>
#include <QMap>

class CommentTopWidget;
class SUserCommentWidget;
class QVBoxLayout;
class QTextEdit;
class SUserCommentView : public QWidget
{
	Q_OBJECT

public:
	SUserCommentView(QWidget* parent = nullptr);
	~SUserCommentView();

	void init();
	void addUserComment(int comment_id, const QString& userName, const QString& userAvatar, const QString& userComment, const QString& time, int isTop = false);
	void clearData(int image_id, int comment_count, int image_index); //传入新的图片id与该图片的评论数
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
public slots:
	void deleteComment(int comment_id);
	void topComment(int comment_id);
signals:
	void commentCountChange(int image_id,int comment_count);
private:
	QColor defaultBorderColor = QColor("#888888");
	QColor focusBorderColor = QColor(64, 158, 255);

	CommentTopWidget* m_topWidget{};
	QMap<int, SUserCommentWidget*> m_userCommentMap;

	int m_image_id;
	int m_image_index = 0;
	int m_image_comment_count = 0;

	QTextEdit* m_commentEdit{}; //评论编辑框
	QVBoxLayout* m_bottomLayout{}; //评论区
};

class CommentTopWidget : public QWidget
{
public:
	CommentTopWidget(QWidget* parent = nullptr);
	void init();
};

//class CommentEditWidget :public 

#endif //! SUERCOMMENTVIEW_H_
