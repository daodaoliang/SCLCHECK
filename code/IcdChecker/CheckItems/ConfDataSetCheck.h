/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: AF455E
 * Name: ConfDataSetCheck
 * Desc: ���ݼ�DataSet>ʵ�ʸ����Ƿ�С�ڻ����ConfDataset��max����ֵ
 * Ref: GDW1396
 */

#ifndef _ConfDataSetCheck_H
#define _ConfDataSetCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CConfDataSetCheck : public CSingleInstance<CConfDataSetCheck>, CNoCopy, public CCheckItemBase
{
public:
    CConfDataSetCheck();

public:
    virtual bool CheckInternal();

    static void CheckDataset_InCacheMgr(ICDCache& icd);
};

#endif /* _ConfDataSetCheck_H */
