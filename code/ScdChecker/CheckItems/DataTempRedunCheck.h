/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: C2AFAE
 * Name: DataTempRedunCheck
 * Desc: ����δ�����õ�LNType��DOtype��DAtype
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
