#include "SConfigFile.h"

SConfigFile::SConfigFile(const QString& name, QSettings::Format format)
	:m_settings(name,format)
{

}

void SConfigFile::setValue(QAnyStringView key, const QVariant& value)
{
	m_settings.setValue(key, value);
}

QVariant SConfigFile::value(QAnyStringView key) const
{
	return m_settings.value(key, QVariant());
}

QVariant SConfigFile::value(QAnyStringView key, const QVariant& def_value) const
{
#ifdef _DEBUG
	auto v = m_settings.value(key);
	if (!v.isValid()) {
		v = def_value;
		qWarning() << key.toString() << "is not found in config file, use default value";
	}
	return v; 
#endif
	return m_settings.value(key, def_value);
}

void SConfigFile::remove(QAnyStringView key)
{
	m_settings.remove(key);
}