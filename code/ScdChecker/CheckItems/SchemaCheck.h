/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: DB5780
 * Name: SchemaCheck
 * Desc: SchemaУ��
 * Ref: GDW1396
 */

#ifndef _SchemaCheck_H
#define _SchemaCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include <vector>
#include <iostream>
using namespace std;

class CSchemaCheck : public CSingleInstance<CSchemaCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSchemaCheck();

public:
    virtual bool CheckInternal();

private:
    void InitPaths();

private:
    std::vector<std::string> m_vectorFilePaths;
    wstring xsdPath;
};

#endif /* _SchemaCheck_H */
