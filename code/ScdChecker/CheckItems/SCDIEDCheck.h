/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 69FBFF
 * Name: SCDIEDCheck
 * Desc: IEDԪ���Ƿ���Ϲ淶
 * Ref: GDW1396
 */

#ifndef _SCDIEDCheck_H
#define _SCDIEDCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIEDCheck : public CSingleInstance<CSCDIEDCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIEDCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIEDCheck_H */
