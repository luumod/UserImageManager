#ifndef SCOMMENTDLG_H_
#define  SCOMMENTDLG_H_

#include <QWidget>
#include <QMap>

class CommentTopWidget;
class SUserComment;
class QVBoxLayout;
class SCommentDlg : public QWidget
{
	Q_OBJECT

public:
	SCommentDlg(QWidget* parent = nullptr);
	~SCommentDlg();

	void init();
	void addUserComment(int comment_id, const QString& userName, const QString& userAvatar, const QString& userComment, const QString& time);
	void clearData(int image_id); //传入新的图片id
public slots:
	void deleteComment(int comment_id);
	void topComment(int comment_id);
private:
	CommentTopWidget* m_topWidget{};
	QMap<int, SUserComment*> m_userCommentMap;

	int m_image_id;

	QVBoxLayout* m_bottomLayout{}; //评论区
};

class CommentTopWidget : public QWidget
{
public:
	CommentTopWidget(QWidget* parent = nullptr);
	void init();
};

#endif //! SCOMMENTDLG_H_
