/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: EA80EF
 * Name: DataSetTypeValidCheck
 * Desc: GOOSE���ݼ�Ӧ���õ�DA���������ã�����SV���ݼ�������DO
 * Ref: Q/GDW 1396-2012 ��10.1.1��11.1.1��
 */

#ifndef _DataSetTypeValidCheck_H
#define _DataSetTypeValidCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"
class CDataSetTypeValidCheck : public CSingleInstance<CDataSetTypeValidCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDataSetTypeValidCheck();

public:
    virtual bool CheckInternal();

    void CheckValid_InCacheMgr(std::wstring sCBType, FcdaCache& fcda);
};

#endif /* _DataSetTypeValidCheck_H */
