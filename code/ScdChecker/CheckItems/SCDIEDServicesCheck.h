/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 30D5AC
 * Name: SCDIEDServicesCheck
 * Desc: ServisesԪ���Ƿ���Ϲ淶
 * Ref: GDW1396
 */

#ifndef _SCDIEDServicesCheck_H
#define _SCDIEDServicesCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIEDServicesCheck : public CSingleInstance<CSCDIEDServicesCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIEDServicesCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIEDServicesCheck_H */
