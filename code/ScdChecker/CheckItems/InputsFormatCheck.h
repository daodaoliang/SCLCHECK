/* 此段内容提取自默认配置文件，和实际情况可能有出入
 * 
 * Key: 307BCA
 * Name: InputsFormatCheck
 * Desc: GOOSE 和SV配置的ExtRef内部intAddr的值是否符合规范中的命名格式要求，规范格式应为“LD/LN.DO.DA”(Q/GDW 396-2009)或“n-A:LD/LN.DO.DA”（Q/GDW 396-2012）；
 * Ref: GDW1396
 */

#ifndef _InputsFormatCheck_H
#define _InputsFormatCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CInputsFormatCheck : public CSingleInstance<CInputsFormatCheck>, CNoCopy, public CCheckItemBase
{
public:
    CInputsFormatCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _InputsFormatCheck_H */
