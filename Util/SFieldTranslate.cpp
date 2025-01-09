#include "SFieldTranslate.h"
#include <QDebug>

SFieldTranslate* SFieldTranslate::instance()
{
	static SFieldTranslate sft;
	return &sft;
}

void SFieldTranslate::addTrans(const QString& key, const QString& value)
{
	auto it = m_map.find(key);
	if (it != m_map.end()) {
		qWarning() << "SFieldTranslate::addTrans: key already exists:" << key;
	}
	m_map[key] = value;
}

QString SFieldTranslate::trans(const QString& key)
{
	return trans("", key);
}

QString SFieldTranslate::trans(const QString& group, const QString& key)
{
	QString result;
	if (group.isEmpty()) {
		result = key;
	}
	else {
		result = group + "/" + key;
	}
	auto it = m_map.find(result);
	if (it == m_map.end()) {
		qWarning() << "SFieldTranslate::trans: key not found:" << result;
		return key;
	}
	return m_map.value(result);
}
