/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: D0620B
 * Name: SCDSMVCheck
 * Desc: SMVԪ�ؼ��
 * Ref: GDW1396
 */

#ifndef _SCDSMVCheck_H
#define _SCDSMVCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDSMVCheck : public CSingleInstance<CSCDSMVCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDSMVCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDSMVCheck_H */
