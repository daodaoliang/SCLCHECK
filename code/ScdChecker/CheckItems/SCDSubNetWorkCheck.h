/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: EFA2C2
 * Name: SCDSubNetWorkCheck
 * Desc: SubnetWorkԪ�ؼ��
 * Ref: GDW1396
 */

#ifndef _SCDSubNetWorkCheck_H
#define _SCDSubNetWorkCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDSubNetWorkCheck : public CSingleInstance<CSCDSubNetWorkCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDSubNetWorkCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDSubNetWorkCheck_H */
