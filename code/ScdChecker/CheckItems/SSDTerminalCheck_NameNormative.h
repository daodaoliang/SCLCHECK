/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 88181E
 * Name: SSDTerminalCheck_NameNormative
 * Desc: Terminal����name�����Ƿ���Tn��������
 * Ref: �����ܱ��վSSD��ģ�����淶��6.5.6
 */

#ifndef _SSDTerminalCheck_NameNormative_H
#define _SSDTerminalCheck_NameNormative_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSSDTerminalCheck_NameNormative : public CSingleInstance<CSSDTerminalCheck_NameNormative>, CNoCopy, public CCheckItemBase
{
public:
    CSSDTerminalCheck_NameNormative();

public:
    virtual bool CheckInternal();
};

#endif /* _SSDTerminalCheck_NameNormative_H */
