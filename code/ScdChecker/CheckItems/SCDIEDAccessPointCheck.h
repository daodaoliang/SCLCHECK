/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 509878
 * Name: SCDIEDAccessPointCheck
 * Desc: AccessPointԪ���Ƿ���Ϲ淶
 * Ref: GDW1396
 */

#ifndef _SCDIEDAccessPointCheck_H
#define _SCDIEDAccessPointCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIEDAccessPointCheck : public CSingleInstance<CSCDIEDAccessPointCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIEDAccessPointCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIEDAccessPointCheck_H */
