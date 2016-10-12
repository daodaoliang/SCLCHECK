/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 1F61CB
 * Name: CdcCheck
 * Desc: ���������಻�����������䣨����Ƿ������չ��CDC���ͣ�STGΪ�°�396�������䣩
 * Ref: Q/GDW 1396-2012 ��7.1.5��
 */

#ifndef _CdcCheck_H
#define _CdcCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CCdcCheck : public CSingleInstance<CCdcCheck>, CNoCopy, public CCheckItemBase
{
public:
    CCdcCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _CdcCheck_H */
