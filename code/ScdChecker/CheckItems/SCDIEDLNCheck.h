/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 40F963
 * Name: SCDIEDLNCheck
 * Desc: LNԪ���Ƿ���Ϲ淶
 * Ref: GDW1396
 */

#ifndef _SCDIEDLNCheck_H
#define _SCDIEDLNCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDIEDLNCheck : public CSingleInstance<CSCDIEDLNCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDIEDLNCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDIEDLNCheck_H */
