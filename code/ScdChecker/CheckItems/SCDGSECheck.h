/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 2391A5
 * Name: SCDGSECheck
 * Desc: GSEԪ�ؼ��
 * Ref: GDW1396
 */

#ifndef _SCDGSECheck_H
#define _SCDGSECheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDGSECheck : public CSingleInstance<CSCDGSECheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDGSECheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDGSECheck_H */
