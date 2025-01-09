#ifndef SHTTPPARTPARSE_H
#define SHTTPPARTPARSE_H

#include <QByteArray>
#include <QMap>

/*
解析HTTP请求中的数据块，包括文件名、数据、头信息等。
*/

class SHttpPartParse {
public:
	SHttpPartParse(const QByteArray& body);
	bool parse();
	const QByteArray& name() const { return m_name; }
	const QByteArray& filename() const { return m_filename; }
	const QByteArray& data() const { return m_data; }
private:
	QByteArray m_body;
	QByteArray m_data;
	QMap<QByteArray, QByteArray> m_headers;
	QByteArray m_name;
	QByteArray m_filename;
};

#endif // SHTTPPARTPARSE_H
