/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 81DA52
 * Name: SCDHeaderCheck
 * Desc: HeaderԪ���Ƿ���Ϲ淶
 * Ref: GDW1396
 */

#ifndef _SCDHeaderCheck_H
#define _SCDHeaderCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDHeaderCheck : public CSingleInstance<CSCDHeaderCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDHeaderCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDHeaderCheck_H */
