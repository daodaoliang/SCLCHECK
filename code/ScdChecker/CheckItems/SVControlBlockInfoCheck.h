/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: D66C1B
 * Name: SVControlBlockInfoCheck
 * Desc: SV���ƿ��������smvID��APPID��MAC��ַ��VLAN-ID����ȱʧ��
 * Ref: GDW1396
 */

#ifndef _SVControlBlockInfoCheck_H
#define _SVControlBlockInfoCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSVControlBlockInfoCheck : public CSingleInstance<CSVControlBlockInfoCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSVControlBlockInfoCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SVControlBlockInfoCheck_H */
