#include "SHttpPartParse.h"
#include <QDebug>

SHttpPartParse::SHttpPartParse(const QByteArray& body)
	: m_body(body)
{
	parse();
}

bool SHttpPartParse::parse()
{
	//查找空行
	int pos = m_body.indexOf("\r\n\r\n");
	//获取头
	auto header = m_body.mid(0, pos + strlen("\r\n\r\n")).trimmed();
	//拆分头
	auto headerList = header.split('\n');
	//获取边界
	auto boundary = headerList.at(0).trimmed();
	for (int i = 1; i < headerList.size(); i++) {
		auto pair = headerList.at(i).trimmed().split(':');
		m_headers.insert(pair.first().trimmed(), pair.last().trimmed());
	}

	auto it = m_headers.find("Content-Disposition");
	if (it == m_headers.end()) {
		return false;
	}
	//"form-data; name=\"upload_avatar\"; filename=\"6666.png\""
	auto values = it.value().split(';');
	values.removeFirst(); // ["name=\"upload_avatar\"" , "filename=\"6666.png\""]
	for (const auto& instruct : values) {
		auto tmp = instruct.split('=');
		auto key = tmp.first().trimmed();
		auto value = tmp.last().removeIf([](const char& ch) { return ch == '\"'; });
		/*
		"name" "upload_avatar"
		"filename" "6666.png"
		*/
		if (key == "name") {
			m_name = value;
		}
		else if (key == "filename") {
			m_filename = value;
		}	
	}

	auto endPos = m_body.lastIndexOf(boundary);
	m_data = m_body.mid(pos + strlen("\r\n\r\n"), endPos - pos - strlen("\r\n\r\n") - 2);
	return true;
}
