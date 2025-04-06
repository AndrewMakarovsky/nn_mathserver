#pragma once

#ifndef NNOBJECT_H
#define NNOBJECT_H

#include <string>

using namespace std;

class IniFile
{
public:
	int portno;
	string ipaddr;
	string DataDirectory;

	static IniFile* GetIniFile();
	~IniFile();

	bool Load();

private:
	static IniFile* iniFile;

protected:
	IniFile();

	string filename;
};

class NNObject
{
public:
	NNObject();
	NNObject(string s);
	~NNObject();
	string getName() { return name; }
	IniFile* getIniFile() { return iniFile; }

private:
	string name;
	static int count;
	IniFile* iniFile;
};

#endif
