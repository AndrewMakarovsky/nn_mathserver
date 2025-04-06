#pragma once

#ifndef SOCKETXML_H
#define SOCKETXML_H

#include "parsexml.h"
#include "nnobject.h"
#include <typeinfo>

using namespace parsexml;

namespace socketxml
{
    class array_char_range : public std::exception
    {
        std::string info;
    public:
        array_char_range(std::string e = "array_char_range") : info(e) {}
        const char* what() const noexcept {
            return info.c_str();
        }
    };

    class myxml_data_range : public std::exception
    {
        std::string info;
    public:
        myxml_data_range(std::string e = "myxml_data_range") : info(e) {}
        const char* what() const noexcept {
            return info.c_str();
        }
    };

    class stack_pop_empty : public std::exception
    {
        std::string info;
    public:
        stack_pop_empty(std::string e = "stack_pop_empty") : info(e) {}
        const char* what() const noexcept {
            return info.c_str();
        }
    };

    class CreateXML
    {
    public:
        CreateXML();
        ~CreateXML();

        void addInt(int x);
        void addDouble(double x);
        void addLong(long x);
        void addArrayInt(int* x, int n);
        void addArrayDouble(double* x, int n);
        void addString(char* s);
        string getXML();

    private:
        void addXMLSection(string tag, string val);
        void addXMLTag(string tag);
        void closeXMLTag(string tag);

        string xml;
    };

    class myXML
    {
    public:
        myXML(void* d, size_t size);
        ~myXML() {};

        size_t getQnt();
        bool addQnt(size_t a);
        bool subQnt(size_t a);
        const void* getData();
        bool addData(void* d, int n);
        bool getArrayFlag();
        size_t getvIntSize();
        size_t getvDoubleSize();
        void pushArrayFlag();
        void pushvInt(int a);
        void pushvDouble(double a);
        bool popArrayFlag();
        bool popvInt(int& ret);
        bool popvDouble(double& ret);

    private:
        vector<int> arrayFlag;
        vector<int> vInt;
        vector<double> vDouble;

        size_t sz;
        size_t qnt;
        void* data;
    };

    void otf(ParseXML* pxml, const string& name, const vector<AttrVal>& attr);
    void ctf(ParseXML* pxml, const string& name);
    void ttf(ParseXML* pxml, const string& name);

    void ini_otf(ParseXML* pxml, const string& name, const vector<AttrVal>& attr);
    void ini_ctf(ParseXML* pxml, const string& name);
    void ini_ttf(ParseXML* pxml, const string& name);

    void addstr(char* s1, const char* s2, int max);
    size_t strlength(const char* s);
    int strcompare(const char* s1, const char* s2, size_t max);
    void copystr(char* s1, const char* s2, int max);

    inline void addXMLSection(char* x, int xml_sz, char* tag, char* val)
    {
        addstr(x, "<", xml_sz);
        addstr(x, tag, xml_sz);
        addstr(x, ">", xml_sz);
        addstr(x, val, xml_sz);
        addstr(x, "</", xml_sz);
        addstr(x, tag, xml_sz);
        addstr(x, ">", xml_sz);
    }
/*
    inline void getXML(char*, int)
    {
    }

    template<typename T, typename... Args>
    inline void getXML(char* xml, int xml_sz, T t, Args... a)
    {
        static char s[64];
        //static char s2[8];
        char tname[64];

        tname[0] = 0;
        copystr(tname, typeid(T).name(), 64);

        for (int i = 0; i < 64; i++)
        {
            s[i] = 0;
        }

        if (strcompare(tname, "int", 3) == 0)
        {
            if ((strlength(tname) > 3) && strcompare(tname, "int *", 5) == 0)
            {
                int k = *((int*)(long long)t);

                addstr(xml, "<", xml_sz);
                addstr(xml, "ArrayInt", xml_sz);
                addstr(xml, ">", xml_sz);
                //sprintf(s, "%d", k); 
                copystr(s, to_string(k).c_str(), 64);

                addXMLSection(xml, xml_sz, (char*)"size", s);

                for (int i = 1; i <= k; i++)
                {
                    int k2 = *((int*)(long long)(t + i));
                    //sprintf(s, "%d", k2);
                    copystr(s, to_string(k2).c_str(), 64);
                    //sprintf(s2, "I%d", i - 1);
                    addXMLSection(xml, xml_sz, (char*)"int", s);
                }
                addstr(xml, "</", xml_sz);
                addstr(xml, "ArrayInt", xml_sz);
                addstr(xml, ">", xml_sz);
            }
            else
            {
                int k4 = *(int*)((long long)(&t));
                copystr(s, to_string(k4).c_str(), 64);
                //sprintf_s(s, "%d", t);
                addXMLSection(xml, xml_sz, (char*)"int", s);
            }
        }
        if (strcompare(tname, "long", 4) == 0 || strcompare(tname, "__int64", 6) == 0)
        {
            long long k3 = *(long long*)((long long)(&t));
            copystr(s, to_string(k3).c_str(), 64);
            //sprintf_s(s, "%d", t);
            addXMLSection(xml, xml_sz, (char*)"long", s);
        }
        if (strcompare(tname, "double", 6) == 0)
        {
            if ((strlength(tname) > 6) && strcompare(tname, "double *", 5) == 0)
            {
                int k = (int)(*((double*)(long long)t) + 0.1);

                addstr(xml, "<", xml_sz);
                addstr(xml, "ArrayDouble", xml_sz);
                addstr(xml, ">", xml_sz);
                //sprintf(s, "%d", k);
                copystr(s, to_string(k).c_str(), 64);

                addXMLSection(xml, xml_sz, (char*)"size", s);

                for (int i = 1; i <= k; i++)
                {
                    double k2 = *((double*)(long long)(t + i));
                    //sprintf(s, "%.4lf", k2);
                    copystr(s, to_string(k2).c_str(), 64);
                    //sprintf(s2, "D%d", i - 1);
                    addXMLSection(xml, xml_sz, (char*)"double", s);
                }
                addstr(xml, "</", xml_sz);
                addstr(xml, "ArrayDouble", xml_sz);
                addstr(xml, ">", xml_sz);
            }
            else
            {
                double k3 = *(double*)((long long)(&t));
                copystr(s, to_string(k3).c_str(), 64);
                //sprintf(s, "%.4lf", t);
                addXMLSection(xml, xml_sz, (char*)"double", s);
            }
        }
        if ((strcompare(tname, "char const *", 12) == 0) || (strcompare(tname, "char *", 6) == 0))
        {
            char* k4 = *(char**)((long long)(&t));
            copystr(s, k4, 64);
            //sprintf(s, "%s", t);
            addXMLSection(xml, xml_sz, (char*)"string", s);
        }

        getXML(xml, xml_sz, a...);
    }

    template<typename... Args>
    inline bool getXML(int xml_sz, char* xml, Args... a)
    {
        try
        {
            for (int i = 0; i < xml_sz; i++)
            {
                xml[i] = 0;
            }

            copystr(xml, "<?xml version = \"1.0\"?>", xml_sz);
            addstr(xml, "<root  height =\"20\" width= \"50\">", xml_sz);
            getXML(xml, xml_sz, a...);
            addstr(xml, "</root>", xml_sz);
        }
        catch (exception& e)
        {
            copystr(xml, e.what(), xml_sz);

            return false;
        }

        return true;
    }*/
}

#endif
