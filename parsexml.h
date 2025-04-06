#pragma once

#ifndef PARSEXML_H
#define PARSEXML_H

#include <string>
#include <vector>

namespace parsexml
{
	using namespace std;

	class ParseXML;

	//Классы для обработки ошибок
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

	//Атрибут тэга: имя атрибута и его значение
	struct AttrVal
	{
		string name;
		string val;
	};

	//Типы функций, которые при разборе xml-файла будут вызываться при открытии и закрытии тэга, а также для текста, который
	//находится между ними и не принадлежит вложенным тэгам (если они есть)
	typedef void (*OpenTagFunction)(ParseXML* pxml, const string& name, const vector<AttrVal>& attr);
	typedef void (*CloseTagFunction)(ParseXML* pxml, const string& name);
	typedef void (*TextTagFunction)(ParseXML* pxml, const string& s);

	//Класс для разбора xml-файлов
	class ParseXML
	{
	public:
		//Конструктору передаются указатели на пользовательские функции и пользовательскую структуру, которые должны извлечь
		//нужную информацию из xml-файла
		ParseXML(OpenTagFunction otf, CloseTagFunction ctf, TextTagFunction ttf, void* d);
		~ParseXML();

		//Разбор xml-файлов
		bool parse(const string& xml, string& err); 
		void* getData();
		//Получение имени текущего открытого тега
		string getTagName();
		//Получение общего числа открытых тегов
		size_t getTagNameSize();
		//Запись в стек нового открытого тега
		void pushTagName(string str);
		//Извлечение и удаление из стека последнего открытого тега, если стек пустой, возвращает false, иначе true
		bool popTagName(string& ret);

		//Указатели на пользовательские функции, которые должны извлечь нужную информацию из xml-файла
		OpenTagFunction openTag;
		CloseTagFunction closeTag;
		TextTagFunction textTag;

	private:
		//Функции, которые при разборе xml - файла будут вызываться при проверке заголовка xml-файла, открытии и закрытии тэга,
		//а также для текста, который находится между ними и не принадлежит вложенным тэгам (если они есть)
		size_t headerXMLtag(const string& xml, size_t b);
		size_t closeXMLtag(const string& xml, size_t b);
		size_t openXMLtag(const string& xml, size_t b);
		size_t textXMLtag(const string& xml, size_t b);
		//Вызывается для разбора описания тега при его открытии, возвращает имя тега и вектор атрибутов тега, если они есть
		string getTagAttr(string str, vector<AttrVal>& attr);
		//Текущий path (все открытые теги на данный момент работы алгоритма разбора xml-файла в виде строки, теги отделяются друг
		//от други с помощью слэша - '/')
		string getCurrentPath();

		//Вспомогательная переменная для проверки наличия корневого тега
		bool rootTagExist;
		//Все открытые теги на данный момент работы алгоритма разбора xml-файла (текущий path)
		vector<string> TagName;
		//Указатель на пользовательскую структуру, используемую для извлечения данных из xml-файла
		void* data;
	};

	//Разбиение строки s на части, отделённые друг от друг с помощью строки delimiter
	vector<string> split(const string& s, const string& delimiter);
	//Больше одного пробела подряд заменяет на 1 пробел и убирает пробелы до и после знака равенства
	string packstr(string& s);
	//Если строка состоит не меньше чем из 2-х символов и начинается и заканчивается кавычками, удаляет их и возвращает true,
	//иначе false
	bool erasequotes(string& s, string& r);
}

#endif
