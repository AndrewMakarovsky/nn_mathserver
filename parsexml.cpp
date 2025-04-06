#include "parsexml.h"

namespace parsexml
{
	ParseXML::ParseXML(OpenTagFunction otf, CloseTagFunction ctf, TextTagFunction ttf, void* d)
	{
		openTag = otf;
		closeTag = ctf;
		textTag = ttf;
		rootTagExist = false;

		data = d;
	}

	ParseXML::~ParseXML()
	{
		//TagName.~vector<string>();
	}

	bool ParseXML::parse(const string& xml, string& err)
	{
		size_t k;

		rootTagExist = false;

		try
		{
			for (size_t i = 0; i < xml.size(); i++)
			{
				if (xml[i] == ' ')
				{
					continue;
				}

				if (xml[i] == '<')
				{
					if (xml[i + 1] == '?')
					{
						k = headerXMLtag(xml, i);
					}
					else
					{
						if (xml[i + 1] == '/')
						{
							k = closeXMLtag(xml, i);
						}
						else
						{
							k = openXMLtag(xml, i);
						}
					}

					i = k;
				}
				else
				{
					i = textXMLtag(xml, i);
				}
			}

			if (getTagNameSize() != 0)
			{
				string path = getCurrentPath();
				TagName.clear();
				rootTagExist = false;

				throw xml_struct_is_wrong(path);
			}
		}
		catch (exception& e)
		{
			err.clear();
			err.append(string(e.what()));

			return false;
		}

		return true;
	}

	string ParseXML::getTagAttr(string str, vector<AttrVal>& attr)
	{
		size_t n = str.size();
		if (n == 0)
			return string("");
		string s, ret;
		string dl = " ";
		s = packstr(str);
		vector<string> vs = split(s, dl);
		if (vs.size() == 0)
		{
			ret = s;

			return ret;
		}
		else
		{
			ret = vs[0];
			for (size_t i = 1; i < vs.size(); i++)
			{
				AttrVal av;
				dl = string("=");
				vector<string> vs2 = split(vs[i], dl);
				if (vs2.size() == 2)
				{
					av.name = vs2[0];
					erasequotes(vs2[1], av.val);
				}

				attr.push_back(av);
			}
		}

		return ret;
	}

	size_t ParseXML::headerXMLtag(const string& xml, size_t b)
	{
		for (size_t i = b; i < xml.size(); i++)
		{
			if (xml[i] == '>')
			{
				return i;
			}
		}

		return xml.size();
	}

#define PARSEXML_TMP_STRING_LENGTH 256

	size_t ParseXML::closeXMLtag(const string& xml, size_t b)
	{
		char* s = new char[PARSEXML_TMP_STRING_LENGTH];

		for (size_t i = b; i < xml.size(); i++)
		{
			if (xml[i] == '>')
			{
				size_t q = i - b - 2;
				for (size_t j = 0; j < q; j++)
				{
					s[j] = xml[b + j + 2];
				}

				s[i - b - 2] = 0;

				if (getTagNameSize() == 0)
				{
					rootTagExist = false;
					delete[] s;
					string path = getCurrentPath();
					TagName.clear();

					throw xml_close_tag_wrong(path);
				}

				string str(s);

				if (getTagName() != str)
				{
					rootTagExist = false;
					delete[] s;
					string path = getCurrentPath();
					TagName.clear();

					throw xml_close_tag_wrong(path);
				}

				closeTag(this, str);

				TagName.pop_back();

				delete[] s;

				return i;
			}
		}

		delete[] s;

		return xml.size();
	}

	size_t ParseXML::openXMLtag(const string& xml, size_t b)
	{
		char* s = new char[PARSEXML_TMP_STRING_LENGTH];

		for (size_t i = b; i < xml.size(); i++)
		{
			if (xml[i] == '>')
			{
				size_t q = i - b - 1;
				for (size_t j = 0; j < q; j++)
				{
					s[j] = xml[b + j + 1];
				}

				s[i - b - 1] = 0;
				vector<AttrVal> v;

				string str(s);

				string tname = getTagAttr(str, v);

				if (getTagNameSize() == 0)
				{
					if (rootTagExist)
					{
						rootTagExist = false;

						delete[] s;

						throw root_tag_not_exist();
					}
					else
					{
						rootTagExist = true;
					}
				}

				TagName.push_back(tname);

				openTag(this, tname, v);

				delete[] s;

				return i;
			}
		}

		delete[] s;

		return xml.size();
	}

	size_t ParseXML::textXMLtag(const string& xml, size_t b)
	{
		char* s = new char[PARSEXML_TMP_STRING_LENGTH];

		for (size_t i = b; i < xml.size(); i++)
		{
			if (xml[i] == '<')
			{
				size_t q = i - b;
				for (size_t j = 0; j < q; j++)
				{
					s[j] = xml[b + j];
				}

				s[i - b] = 0;

				std::string str(s);
				textTag(this, str);

				delete[] s;

				return i - 1;
			}
		}

		delete[] s;

		return xml.size();
	}

	string ParseXML::getTagName()
	{
		size_t n = TagName.size();
		if (n == 0)
		{
			string s("");

			return s;
		}


		return TagName[n - 1];
	}

	size_t ParseXML::getTagNameSize()
	{
		return TagName.size();
	}

	void ParseXML::pushTagName(string str)
	{
		TagName.push_back(str);
	}

	bool ParseXML::popTagName(string& ret)
	{
		size_t n = TagName.size();
		if (n == 0)
		{
			return false;
		}

		ret = TagName[n - 1];
		TagName.pop_back();

		return true;
	}

	void* ParseXML::getData()
	{
		return data;
	}

	string ParseXML::getCurrentPath()
	{
		string ret("");
		size_t qnt = getTagNameSize();

		for(size_t i = 0; i < qnt; i++)
		{
			ret.append(TagName[i]);
			ret.append("/");
		}
		
		return ret;
	}

	vector<string> split(const string& s, const string& delimiter)
	{
		vector<string> ret;
		size_t pos;
		string token;
		size_t i = 0;

		while (i < s.size())
		{
			pos = s.find(delimiter, i);
			if (pos == string::npos)
			{
				token = s.substr(i, pos - i);
				ret.push_back(token);

				break;
			}
			else
			{
				token = s.substr(i, pos - i);
				ret.push_back(token);
				i = pos + 1;
			}
		}

		return ret;
	}

	//Больше одного пробела подряд заменяет на 1 пробел и убирает пробелы до и после знака равенства
	string packstr(string& s)
	{
		size_t max = s.size();
		char* r = new char[max + 1];
		char* r2 = new char[max + 1];
		bool space_flag = false;
		size_t k = 0;
		char c = 0;

		//Убрать больше 1 пробела подряд
		for (size_t i = 0; i < max; i++)
		{
			if (space_flag && s[i] == ' ')
				continue;

			if (s[i] == ' ')
			{
				space_flag = true;
				r[k++] = s[i];
			}
			else
			{
				space_flag = false;
				r[k++] = s[i];
			}
		}
		r[k] = 0;
		k = 0;

		//Убрать пробелы до и после знаков равенства
		for (size_t i = 0; i < max; i++)
		{
			if (r[i] == ' ')
			{
				if (i < max - 1)
				{
					if (r[i + 1] == '=')
						continue;
				}
				if (c == '=')
					continue;

				r2[k++] = c = r[i];
			}
			else
			{
				r2[k++] = c = r[i];
			}
		}
		r2[k] = 0;

		string ret(r2);
		delete[] r2;
		delete[] r;

		return ret;
	}

	bool erasequotes(string& s, string& r)
	{
		size_t n = s.size();

		if (n < 2)
			return false;

		if (s[0] == '\"' && s[n - 1] == '\"')
		{
			r = s.substr(1, n - 2);

			return true;
		}

		return false;
	}
}
