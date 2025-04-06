#include "socketxml.h"

namespace socketxml
{
    size_t myXML::getQnt()
    {
        return qnt;
    }

    bool myXML::addQnt(size_t a)
    {
        if (qnt + a > sz)
            return false;

        qnt += a;

        return true;
    }

    bool myXML::subQnt(size_t a)
    {
        if (qnt - a < 0)
            return false;

        qnt -= a;

        return true;
    }

    const void* myXML::getData()
    {
        return const_cast<const void*>(data);
    }

    bool myXML::addData(void* d, int n)
    {
        if (qnt + n > sz)
            throw myxml_data_range();

        char* s = (char*)data;
        char* s2 = (char*)d;

        s += qnt;

        for (int i = 0; i < n; i++)
        {
            s[i] = s2[i];
        }

        qnt += n;

        return true;
    }

    myXML::myXML(void* d, size_t size)
    {
        sz = size;
        qnt = 0;
        data = d;
    }

    bool myXML::getArrayFlag()
    {
        size_t n = arrayFlag.size();
        if (n == 0)
            return false;

        return true;
    }

    void myXML::pushArrayFlag()
    {
        arrayFlag.push_back(0);
    }

    void myXML::pushvInt(int a)
    {
        vInt.push_back(a);
    }

    void myXML::pushvDouble(double a)
    {
        vDouble.push_back(a);
    }

    bool myXML::popArrayFlag()
    {
        size_t n = arrayFlag.size();
        if (n == 0)
            return false;
        else
            arrayFlag.pop_back();

        return true;
    }

    bool myXML::popvInt(int& ret)
    {
        size_t n = vInt.size();
        if (n == 0)
        {
            return false;
        }

        ret = vInt[n - 1];
        vInt.pop_back();

        return true;
    }

    bool myXML::popvDouble(double& ret)
    {
        size_t n = vDouble.size();
        if (n == 0)
        {
            return false;
        }

        ret = vDouble[n - 1];
        vDouble.pop_back();

        return true;
    }

    size_t myXML::getvIntSize()
    {
        return vInt.size();
    }

    size_t myXML::getvDoubleSize()
    {
        return vDouble.size();
    }

    void otf(ParseXML* pxml, const string& name, const vector<AttrVal>&)
    {
        myXML* myxml = (myXML*)pxml->getData();

        if (name == string("ArrayInt"))
        {
            myxml->pushArrayFlag();

            return;
        }

        if (name == string("ArrayDouble"))
        {
            myxml->pushArrayFlag();

            return;
        }
    }

    void ctf(ParseXML* pxml, const string& name)
    {
        myXML* myxml = (myXML*)pxml->getData();

        if (name == string("ArrayInt"))
        {
            if (!myxml->popArrayFlag())
                throw stack_pop_empty();

            int qnt = (int)myxml->getvIntSize();
            int* a = new int[qnt];
            for (int i = qnt - 1; i >= 0; i--)
            {
                myxml->popvInt(a[i]);
            }
            //myxml->addData(&qnt, sizeof(qnt));
            myxml->addData(&a, sizeof(a));

            return;
        }

        if (name == string("ArrayDouble"))
        {
            if (!myxml->popArrayFlag())
                throw stack_pop_empty();

            int qnt = (int)myxml->getvDoubleSize();
            double* a = new double[qnt];
            for (int i = qnt - 1; i >= 0; i--)
            {
                myxml->popvDouble(a[i]);
            }
            //myxml->addData(&qnt, sizeof(qnt));
            myxml->addData(&a, sizeof(a));

            return;
        }
    }

    void ttf(ParseXML* pxml, const string& name)
    {
        myXML* myxml = (myXML*)pxml->getData();

        if ((pxml->getTagName() == string("int")) || (pxml->getTagName() == string("fnum")))
        {
            int x = stoi(name);

            //sscanf(name.c_str(), "%d", &x);
            if (myxml->getArrayFlag())
            {
                myxml->pushvInt(x);
            }
            else
            {
                myxml->addData(&x, sizeof(x));
            }

            return;
        }
        if (pxml->getTagName() == string("double"))
        {
            double x = stod(name);

            //sscanf(name.c_str(), "%lf", &x);
            if (myxml->getArrayFlag())
            {
                myxml->pushvDouble(x);
            }
            else
            {
                myxml->addData(&x, sizeof(x));
            }

            return;
        }
        if (pxml->getTagName() == string("long"))
        {
            long long x = stoll(name);

            myxml->addData(&x, sizeof(x));

            return;
        }
        if (pxml->getTagName() == string("string"))
        {
            char* s = new char[name.size() + 1];
            for (int i = 0; i < name.size(); i++)
            {
                s[i] = name[i];
            }
            s[name.size()] = 0;

            myxml->addData(&s, sizeof(&s));

            return;
        }
    }

    void ini_otf(ParseXML* pxml, const string& name, const vector<AttrVal>&)
    {
    }

    void ini_ctf(ParseXML* pxml, const string& name)
    {
    }

    void ini_ttf(ParseXML* pxml, const string& name)
    {
        IniFile* ini = (IniFile*)pxml->getData();

        if ((pxml->getTagName() == string("ip")))
        {
            ini->ipaddr = string(name);

            return;
        }
        if (pxml->getTagName() == string("DataDirectory"))
        {
            ini->DataDirectory = string(name);

            return;
        }
        if (pxml->getTagName() == string("port"))
        {
            int x = stoi(name);

            ini->portno = x;

            return;
        }
    }

    void copystr(char* s1, const char* s2, int max)
    {
        char* p = s1;
        const char* p2 = s2;
        int i = 0;

        while (*p2 && i < max - 1)
        {
            *p++ = *p2++;
            i++;
        }
        if (i == max)
            throw array_char_range();

        *p = 0;
    }

    void addstr(char* s1, const char* s2, int max)
    {
        char* p = s1;
        const char* p2 = s2;
        size_t n = strlength(p);

        if (n + strlength(s2) >= max - 1)
            throw array_char_range();

        p += n;
        while (*p2)
        {
            *p++ = *p2++;
        }
        *p = 0;
    }

    size_t strlength(const char* s)
    {
        size_t ret = 0;

        while (*s)
        {
            s++;
            ret++;
        }

        return ret;
    }

    int strcompare(const char* s1, const char* s2, size_t max)
    {
        const unsigned char* p1 = (const unsigned char*)s1;
        const unsigned char* p2 = (const unsigned char*)s2;

        for (size_t i = 0; i < max; i++)
        {
            if (*p1 > *p2)
                return 1;
            if (*p1 < *p2)
                return -1;
            if (*p1 == 0 && *p2 != 0)
                return -1;
            if (*p1 != 0 && *p2 == 0)
                return 1;
            if (*p1 == 0 && *p2 == 0)
                return 0;

            p1++;
            p2++;
        }

        return 0;
    }

    CreateXML::CreateXML()
    {
        xml = string("<?xml version = \"1.0\"?><root  height =\"20\" width= \"50\">");
    }

    CreateXML::~CreateXML()
    {
    }

    string CreateXML::getXML()
    {
        return xml + string("</root>");
    }

    void CreateXML::addInt(int x)
    {
        string val = to_string(x);
        addXMLSection(string("int"), val);
    }

    void CreateXML::addDouble(double x)
    {
        string val = to_string(x);
        addXMLSection(string("double"), val);
    }

    void CreateXML::addLong(long x)
    {
        string val = to_string(x);
        addXMLSection(string("long"), val);
    }

    void CreateXML::addArrayInt(int* x, int n)
    {
        addXMLTag(string("ArrayInt"));
        for(int i = 0; i < n; i++)
        {
            addXMLSection(string("int"), to_string(x[i]));
        }
        closeXMLTag(string("ArrayInt"));
    }

    void CreateXML::addArrayDouble(double* x, int n)
    {
        addXMLTag(string("ArrayDouble"));
        for(int i = 0; i < n; i++)
        {
            addXMLSection(string("double"), to_string(x[i]));
        }
        closeXMLTag(string("ArrayDouble"));
    }

    void CreateXML::addString(char* s)
    {
        addXMLSection(string("string"), string(s));
    }

    void CreateXML::addXMLTag(string tag)
    {
        xml += (string("<") + tag + string(">"));

    }

    void CreateXML::closeXMLTag(string tag)
    {
        xml += (string("</") + tag + string(">"));
    }

    void CreateXML::addXMLSection(string tag, string val)
    {
        addXMLTag(tag);
        xml += val;
        closeXMLTag(tag);
    }
}
