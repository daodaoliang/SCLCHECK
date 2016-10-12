/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: DF89PO
 * Name: CrcBlankValidCheck
 * Desc: SCD�ļ��д���IED���̲������CRC32У�����ȫվ���̲������CRC32У�����Ƿ���ȷ
 * Ref: GDW1396
 */

#ifndef _CrcBlankValidCheck_H
#define _CrcBlankValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"
#include <Windows.h>

class CCrcBlankValidCheck : public CSingleInstance<CCrcBlankValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CCrcBlankValidCheck();
    ~CCrcBlankValidCheck();
public:
    virtual bool CheckInternal();
    static DWORD CALLBACK ScdCRCWorkProc(LPVOID lpParam);
private:

    HANDLE hWorkThread;
    LPCSTR lzText;

    bool CreatMap();
    std::map<wstring, wstring> m_CrcMap;
};

#endif /* _CrcBlankValidCheck_H */
