#ifndef SFIELDTRANSLATE_H
#define SFIELDTRANSLATE_H

#include <QString>
#include <QMap>

class SFieldTranslate {
public:
	static SFieldTranslate* instance();
	void addTrans(const QString& key, const QString& value);
	QString trans(const QString& key);
	QString trans(const QString& group, const QString& key);
private:

	QMap<QString, QString> m_map;
};

#endif // SFIELDTRANSLATE_H