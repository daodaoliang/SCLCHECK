/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: DB5780
 * Name: SchemaCheck
 * Desc: Schema校验
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
