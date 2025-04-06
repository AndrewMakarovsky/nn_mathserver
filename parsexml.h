#pragma once

#ifndef PARSEXML_H
#define PARSEXML_H

#include <string>
#include <vector>

namespace parsexml
{
	using namespace std;

	class ParseXML;

	//������ ��� ��������� ������
	class root_tag_not_exist : public exception
	{
		string info;
	public:
		root_tag_not_exist(string e = "root_tag_not_exist") : info(e) {}
		const char* what() const noexcept {
			return info.c_str();
		}
	};

	class xml_struct_is_wrong : public exception
	{
		string info;
	public:
		xml_struct_is_wrong(string path, string e = "xml_struct_is_wrong")
		{
			info.append("Current path: ");
			info.append(path);
			info.append(", ");
			info.append(e);
		}

		const char* what() const noexcept {
			return info.c_str();
		}
	};

	class xml_close_tag_wrong : public exception
	{
		string info;
	public:
		xml_close_tag_wrong(string path, string e = "xml_close_tag_wrong")
		{
			info.append("Current path: ");
			info.append(path);
			info.append(", ");
			info.append(e);
		}

		const char* what() const noexcept {
			return info.c_str();
		}
	};

	//������� ����: ��� �������� � ��� ��������
	struct AttrVal
	{
		string name;
		string val;
	};

	//���� �������, ������� ��� ������� xml-����� ����� ���������� ��� �������� � �������� ����, � ����� ��� ������, �������
	//��������� ����� ���� � �� ����������� ��������� ����� (���� ��� ����)
	typedef void (*OpenTagFunction)(ParseXML* pxml, const string& name, const vector<AttrVal>& attr);
	typedef void (*CloseTagFunction)(ParseXML* pxml, const string& name);
	typedef void (*TextTagFunction)(ParseXML* pxml, const string& s);

	//����� ��� ������� xml-������
	class ParseXML
	{
	public:
		//������������ ���������� ��������� �� ���������������� ������� � ���������������� ���������, ������� ������ �������
		//������ ���������� �� xml-�����
		ParseXML(OpenTagFunction otf, CloseTagFunction ctf, TextTagFunction ttf, void* d);
		~ParseXML();

		//������ xml-������
		bool parse(const string& xml, string& err); 
		void* getData();
		//��������� ����� �������� ��������� ����
		string getTagName();
		//��������� ������ ����� �������� �����
		size_t getTagNameSize();
		//������ � ���� ������ ��������� ����
		void pushTagName(string str);
		//���������� � �������� �� ����� ���������� ��������� ����, ���� ���� ������, ���������� false, ����� true
		bool popTagName(string& ret);

		//��������� �� ���������������� �������, ������� ������ ������� ������ ���������� �� xml-�����
		OpenTagFunction openTag;
		CloseTagFunction closeTag;
		TextTagFunction textTag;

	private:
		//�������, ������� ��� ������� xml - ����� ����� ���������� ��� �������� ��������� xml-�����, �������� � �������� ����,
		//� ����� ��� ������, ������� ��������� ����� ���� � �� ����������� ��������� ����� (���� ��� ����)
		size_t headerXMLtag(const string& xml, size_t b);
		size_t closeXMLtag(const string& xml, size_t b);
		size_t openXMLtag(const string& xml, size_t b);
		size_t textXMLtag(const string& xml, size_t b);
		//���������� ��� ������� �������� ���� ��� ��� ��������, ���������� ��� ���� � ������ ��������� ����, ���� ��� ����
		string getTagAttr(string str, vector<AttrVal>& attr);
		//������� path (��� �������� ���� �� ������ ������ ������ ��������� ������� xml-����� � ���� ������, ���� ���������� ����
		//�� ����� � ������� ����� - '/')
		string getCurrentPath();

		//��������������� ���������� ��� �������� ������� ��������� ����
		bool rootTagExist;
		//��� �������� ���� �� ������ ������ ������ ��������� ������� xml-����� (������� path)
		vector<string> TagName;
		//��������� �� ���������������� ���������, ������������ ��� ���������� ������ �� xml-�����
		void* data;
	};

	//��������� ������ s �� �����, ��������� ���� �� ���� � ������� ������ delimiter
	vector<string> split(const string& s, const string& delimiter);
	//������ ������ ������� ������ �������� �� 1 ������ � ������� ������� �� � ����� ����� ���������
	string packstr(string& s);
	//���� ������ ������� �� ������ ��� �� 2-� �������� � ���������� � ������������� ���������, ������� �� � ���������� true,
	//����� false
	bool erasequotes(string& s, string& r);
}

#endif
