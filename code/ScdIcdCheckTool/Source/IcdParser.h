#ifndef _ICDPARSER_H
#define _ICDPARSER_H

class CIcdParser
{
public:
    CIcdParser(LPCTSTR lpFilePath);
    virtual ~CIcdParser();

    CString GetCrc() const;

    BOOL Parse();


private:
    CString m_strFilePath;
    CString m_strHeaderVersion;
    CString m_strIedType;
    CString m_strCrc;
    CString m_strErrorMessage;
};

#endif /* _ICDPARSER_H */