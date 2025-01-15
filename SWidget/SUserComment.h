#ifndef SUSERCOMMENT_H
#define SUSERCOMMENT_H

#include <QWidget>

class QLabel;
class SUserComment : public QWidget
{
	Q_OBJECT
public:
	SUserComment(int comment_id,const QString& userName, const QString& avatarPath, const QString& comment_content,const QString& commentTime, QWidget *parent = nullptr);
	~SUserComment();

	void init();
	inline int getCommentId() const { return m_comment_id; }
signals:
	void deleteComment(int delete_comment_id);
	void topComment(int top_comment_id);
private:
	int m_comment_id{}; //记录此评论的id

	QString m_avatarPath{};
	QString m_userName{};
	QString m_comment{};
	QString m_commentTime{};
};

#endif // SUSERCOMMENT_H