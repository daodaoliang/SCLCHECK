/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: B3824A
 * Name: ConfFcdaCheck
 * Desc: ���ݼ����õ�FCDAʵ�ʸ����Ƿ�С�ڻ����ConfDataset��maxAttributes����ֵ���������ݼ��а�����FCDA������ӦΪ0Ҳ��Ӧ����256�������źŹ����޷����ͣ�
 * Ref: GDW1396
 */

#ifndef _ConfFcdaCheck_H
#define _ConfFcdaCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CConfFcdaCheck : public CSingleInstance<CConfFcdaCheck>, CNoCopy, public CCheckItemBase
{
public:
    CConfFcdaCheck();

public:
    virtual bool CheckInternal();

    static void CheckFcdaCount_InCacheMgr(DatasetCache& dataset, int maxFcda);
};

#endif /* _ConfFcdaCheck_H */
