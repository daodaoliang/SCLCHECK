/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: E9DFBC
 * Name: LDdoiCheck
 * Desc: LN0/LN��DOI��SDI��DAI����������ģ�����õ���Ч�ԣ�ʵ��������DOI/SDI/DAI�Ķ��������Ƿ�����������ģ�嶨������ݶ���ṹһ�£�
 * Ref: GDW1396
 */

#ifndef _LDdoiCheck_H
#define _LDdoiCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CLDdoiCheck : public CSingleInstance<CLDdoiCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLDdoiCheck();

public:
    virtual bool CheckInternal();

    static bool CheckLnTypeValid_InCacheMgr(LNCache& ln, DOMap& mapDo);
    static bool CheckDOTypeValid_InCacheMgr(DOICache& doi, DOMap& mapDo, DOTypeCache& doType);
    static void CheckDATypeValid_InCacheMgr(int offset, wstring sDaName, SdiCache& sdi);
    static bool CheckSDITypeValid_InCacheMgr(int offset, wstring sSdiName, SdiCache& sdi);

};

#endif /* _LDdoiCheck_H */
