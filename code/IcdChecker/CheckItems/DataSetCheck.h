/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 83395E
 * Name: DataSetCheck
 * Desc: ���ݼ�������Ӧ����Q/GDW-1396��׼�ж��������ԭ��
 * Ref: Q/GDW 1396-2012 ��8.3.1��
 */

#ifndef _DataSetCheck_H
#define _DataSetCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CDataSetCheck : public CSingleInstance<CDataSetCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDataSetCheck();

public:
    virtual bool CheckInternal();

    static void CheckName_InCacheMgr(DatasetCache& dateset, int offset);
};

#endif /* _DataSetCheck_H */
