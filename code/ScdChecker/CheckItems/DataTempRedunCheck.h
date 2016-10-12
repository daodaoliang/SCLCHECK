/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: C2AFAE
 * Name: DataTempRedunCheck
 * Desc: 存在未被引用的LNType、DOtype、DAtype
 * Ref: GDW1396
 */

#ifndef _DataTempRedunCheck_H
#define _DataTempRedunCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CDataTempRedunCheck : public CSingleInstance<CDataTempRedunCheck>, CNoCopy, public CCheckItemBase
{
public:
    CDataTempRedunCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _DataTempRedunCheck_H */
