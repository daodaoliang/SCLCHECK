/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: DF89PO
 * Name: CrcBlankValidCheck
 * Desc: SCD文件中存在IED过程层虚端子CRC32校验码和全站过程层虚端子CRC32校验码是否正确
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
