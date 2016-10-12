/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: EA5BBA
 * Name: FcdaValidCheck
 * Desc: ���ݼ��а�����FCDA��������Ч�Լ�飨����������ƿ鼰�������ƿ飩
 * Ref: ����ʵ����SCD��������߼���
 */

#ifndef _FcdaValidCheck_H
#define _FcdaValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CFcdaValidCheck : public CSingleInstance<CFcdaValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CFcdaValidCheck();

public:
    virtual bool CheckInternal();

    static bool CheckDaType(int offset, std::vector<wstring>& vecDaName, std::wstring sDaType);
    static bool CheckDoType(int offset, std::vector<wstring>& vecDoName, std::wstring sDoType);
    static bool CheckValid_InCacheMgr(FcdaCache& fcda, LDeviceCache& lDevice);
};

#endif /* _FcdaValidCheck_H */
