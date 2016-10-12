/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: E81083
 * Name: SSDVoltageLevelCheck_UnNULL
 * Desc: VoltageLevelΪ��/�ǿ�ʱ��Voltageֵ�Ƿ�Ϊ�շǿ�
 * Ref: �����ܱ��վSSD��ģ�����淶��6.3.d)
 */

#ifndef _SSDVoltageLevelCheck_UnNULL_H
#define _SSDVoltageLevelCheck_UnNULL_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDVoltageLevelCheck_UnNULL : public CSingleInstance<CSSDVoltageLevelCheck_UnNULL>, CNoCopy, public CCheckItemBase
{
public:
    CSSDVoltageLevelCheck_UnNULL();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDVoltageLevelCheck_UnNULL_H */
