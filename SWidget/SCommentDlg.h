#ifndef SCOMMENTDLG_H_
#define  SCOMMENTDLG_H_

#include <QWidget>
#include <QList>

class CommentTopWidget;
class SUserComment;
class SCommentDlg : public QWidget
{
	Q_OBJECT

public:
	SCommentDlg(QWidget* parent = nullptr);
	~SCommentDlg();

	void init();
	void addUserComment(SUserComment* userComment);

private:
	CommentTopWidget* m_topWidget{};
	QList<SUserComment*> m_userComments;
};

class CommentTopWidget : public QWidget
{
public:
	CommentTopWidget(QWidget* parent = nullptr);
	void init();
};

#endif //! SCOMMENTDLG_H_
