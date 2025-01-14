#ifndef SCOMMENTDLG_H_
#define  SCOMMENTDLG_H_

#include <QWidget>
#include <QList>

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
	void addUserComment(SUserComment* userComment);
	void setData(const QString& userName, const QString& userAvatar, const QString& userComment,const QString& time);
	void clearData();
private:
	CommentTopWidget* m_topWidget{};
	QList<SUserComment*> m_userComments;

	QVBoxLayout* m_bottomLayout{}; //评论区
};

class CommentTopWidget : public QWidget
{
public:
	CommentTopWidget(QWidget* parent = nullptr);
	void init();
};

#endif //! SCOMMENTDLG_H_
