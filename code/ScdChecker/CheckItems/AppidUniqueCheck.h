/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: E51D6C
 * Name: AppidUniqueCheck
 * Desc: Communication��SMV��GSE�е�APPID�Ƿ�ȫ��Ψһ
 * Ref: GDW1396
 */

#ifndef _AppidUniqueCheck_H
#define _AppidUniqueCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CAppidUniqueCheck : public CSingleInstance<CAppidUniqueCheck>, CNoCopy, public CCheckItemBase
{
public:
    CAppidUniqueCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _AppidUniqueCheck_H */
