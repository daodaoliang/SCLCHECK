/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: CE18E6
 * Name: XmlCheck
 * Desc: XML��ʽ�淶
 * Ref: GDW1396
 */

#ifndef _XmlCheck_H
#define _XmlCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CXmlCheck : public CSingleInstance<CXmlCheck>, CNoCopy, public CCheckItemBase
{
public:
    CXmlCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _XmlCheck_H */
