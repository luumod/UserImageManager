﻿#ifndef SUSERCOMMENT_H
#define SUSERCOMMENT_H

#include <QWidget>

class QLabel;
class SUserComment : public QWidget
{
	Q_OBJECT
public:
	SUserComment(int comment_id,const QString& userName, const QString& avatarPath, const QString& comment_content,const QString& commentTime,bool isTop = false, QWidget *parent = nullptr);
	~SUserComment();

	void init();
	inline int getCommentId() const { return m_comment_id; }
	inline void setCommentTop(bool top) { m_isTop = top; }
	inline bool isTopComment() const { return m_isTop; }
signals:
	void deleteComment(int delete_comment_id);
	void topComment(int top_comment_id);
private:
	int m_comment_id{}; //记录此评论的id
	bool m_isTop = false; //记录此评论是否置顶

	QString m_avatarPath{};
	QString m_userName{};
	QString m_comment{};
	QString m_commentTime{};
};

#endif // SUSERCOMMENT_H