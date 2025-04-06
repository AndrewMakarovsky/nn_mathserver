#include "nnobject.h"
#include "parsexml.h"
#include "socketxml.h"
#include <fstream>

#define MAX_INIFILE_SIZE 65536

IniFile* IniFile::GetIniFile()
{
	if (!iniFile)
	{
		iniFile = new IniFile();
	}

	return iniFile;
}

IniFile* IniFile::iniFile = nullptr;

IniFile::IniFile()
{
	filename = string("mathserver.ini");
	Load();
}

IniFile::~IniFile()
{ 
}

bool IniFile::Load()
{
	char* buf = new char[MAX_INIFILE_SIZE];

	parsexml::ParseXML pf(socketxml::ini_otf, socketxml::ini_ctf, socketxml::ini_ttf, this);

	for (int i = 0; i < MAX_INIFILE_SIZE; i++)
	{
		buf[i] = 0;
	}

	ifstream in(filename, ios_base::binary);
	if (in.is_open())
	{
		in.read(buf, MAX_INIFILE_SIZE);
	}

	in.close();

	string s(buf);
	string err;
	pf.parse(s, err);

	return true;
}

int NNObject::count = 0;

NNObject::NNObject()
{
	name = "";
	iniFile = IniFile::GetIniFile();
	count++;
}

NNObject::NNObject(string s)
{
	name = s;
	iniFile = IniFile::GetIniFile();
	count++;
}

NNObject::~NNObject()
{
	count--;
	if (count == 0)
	{
		delete iniFile;
	}
}
