#ifndef SUSERCOMMENT_H
#define SUSERCOMMENT_H

#include <QWidget>

class QLabel;
class SUserComment : public QWidget
{
public:
	SUserComment(const QString& avatarPath, const QString& userName, const QString& comment,const QString& commentTime, QWidget *parent = nullptr);
	~SUserComment();

	void init();
private:
	QString m_avatarPath{};
	QString m_userName{};
	QString m_comment{};
	QString m_commentTime{};
};

#endif // SUSERCOMMENT_H