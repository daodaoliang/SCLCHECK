/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 2C98FA
 * Name: RedLntypeCheck
 * Desc: ����Ƿ������չ��LNodeType����չ��LNodeTypeӦ���������淶�����쳧��ǰ׺_װ���ͺ�_�汾��_LN����(_������׺)������չLNodeType�������ռ�Ҫ����
 * Ref: Q/GDW 1396-2012 ��7.1.7��
 */

#ifndef _RedLntypeCheck_H
#define _RedLntypeCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

#include "..\..\ScdCheckCommon\ValidateCache.h"

class CRedLntypeCheck : public CSingleInstance<CRedLntypeCheck>, CNoCopy, public CCheckItemBase
{
public:
    CRedLntypeCheck();

public:
    virtual bool CheckInternal();

private:
    void CheckExtendLNodeType(LNodeTypeCache& instLNodeType);
};

#endif /* _RedLntypeCheck_H */
