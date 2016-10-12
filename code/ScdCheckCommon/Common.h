#ifndef _COMMON_H
#define _COMMON_H

#include <stdexcept>
#include <string>
#include <fstream>
#include <vector>

template <class T>
class CSingleInstance
{
protected:
    CSingleInstance() {}
    virtual ~CSingleInstance() {}

public:
    static bool IsInitialized()
    {
        return m_pInstance != NULL;
    }

    static bool InitializeInstance()
    {
        if (IsInitialized())
        {
            return true;
        }

        try 
        {
            m_pInstance = new T;
        }
        catch (std::bad_alloc &)
        {
            return false;
        }

        return true;
    }

    static T *GetInstance()
    {
        if (!IsInitialized())
        {
            throw std::runtime_error("GetInstance need InitializeInstance first");
        }

        return m_pInstance;
    }

    static bool DeleteInstance()
    {
        if (!IsInitialized())
        {
            throw std::runtime_error("DeleteInstance need InitializeInstance first");
        }

        delete m_pInstance;
        m_pInstance = NULL;

        return true;
    }

private:
    static T *m_pInstance;
};

template <class T>
T* CSingleInstance<T>::m_pInstance = NULL;

class CNoCopy
{
protected:
    CNoCopy() {}
    virtual ~CNoCopy() {}

private:
    CNoCopy(const CNoCopy &);
    CNoCopy &operator=(const CNoCopy &);
};

template<class T> std::string CommonType2String(const T &v)
{
    std::stringstream ss;

    ss<<v;

    return ss.str();
}

template<> inline std::string CommonType2String(const std::string &v)
{
    return v;
}

template<> inline std::string CommonType2String(const bool &v)
{
    return v ? "true" : "false";
}

template<class T> void AssignString(std::basic_string<T> &str, const void *p)
{
    if (p == NULL)
    {
        str.erase(str.begin(), str.end());
    }
    else
    {
        str = (const T *)p;
    }
}

class CLastError
{
public:
    static void Get(std::string &strLastErrorMsg);
    static void Set(const std::string &strLastErrorMsg);
};

std::string XmlEscape(const std::string &strText);
std::string FormatXMLMessage(const char *fmt, ...);
std::string GetFileSizeString(const std::string &strFilePath);
std::string GetFileModifyTimeString(const std::string &strFilePath);
std::string GetNowTimeString();
bool DeleteOrRenameFile(const std::string &strFilePath);
int GetSmallFileSize(const std::string &strFilePath);
void SafeDebugMessageA(const char *pFormat, ...);
bool IsSystem64bit();
std::string MakeResultJson(bool bSuccess, const char *fmt, ...);
// ·Ö¸î×Ö·û´®º¯Êý
std::vector<std::wstring> split_string(const std::wstring& sSource, const std::wstring& sDelim, bool ignore_blank = true);

bool ResolveFilePath(std::wstring sPathIn,std::wstring& sPathOut);

template <class T>
void Replace(std::basic_string<T> &str, const std::basic_string<T> &old_str, const std::basic_string<T> &new_str)
{
    std::basic_string<T>::size_type offset = str.find(old_str);

    while (offset != str.npos)
    {
        str.replace(offset, old_str.length(), new_str);

        offset = str.find(old_str, offset + new_str.length());
    }
}

template <class T>
bool DumpStringToFile(const std::basic_string<T> &strText, const std::basic_string<T> &strFilePath)
{
    try
    {
        basic_ofstream<T> fs;

        fs.open(strFilePath, ios_base::out | ios_base::binary);
        fs << strText;
        fs.close();

        return true;
    }
    catch (std::exception &)
    {
    }

    return false;
}

#endif /* _COMMON_H */