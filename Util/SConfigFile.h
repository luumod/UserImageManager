#ifndef sconfigfile_h
#define sconfigfile_h

#include <QSettings>
#include <QAnyStringView>

/*
都说配置文件是程序的灵魂，那么配置文件的实现就显得尤为重要。
SConfigFile类是配置文件的实现类，它使用QSettings类来实现配置文件的读写。
SConfigFile类提供了以下接口：
- 构造函数：传入配置文件名和格式，构造SConfigFile对象。
- isOpen()：判断配置文件是否打开成功。
- setValue(QAnyStringView key, const QVariant& value)：设置键值对。
- value(QAnyStringView key)const：获取键值对的值。
- value(QAnyStringView key,const QVariant& def_value)const：获取键值对的值，如果键值对不存在，则返回默认值。
- remove(QAnyStringView key)：删除键值对。
*/
class SConfigFile
{
public:
	SConfigFile(const QString& name = "config.ini", QSettings::Format format = QSettings::IniFormat);

	bool isOpen()const { return m_settings.status() == QSettings::NoError; }
	//保存数据到配置文件中：config.ini
	void setValue(QAnyStringView key, const QVariant& value);
	QVariant value(QAnyStringView key)const;
	QVariant value(QAnyStringView key,const QVariant& def_value)const;
	void remove(QAnyStringView key);
private:
	QSettings m_settings;
};

#endif