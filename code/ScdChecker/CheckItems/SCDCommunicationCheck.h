/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 5A05FF
 * Name: SCDCommunicationCheck
 * Desc: CommunicationԪ�ؼ��
 * Ref: GDW1396
 */

#ifndef _SCDCommunicationCheck_H
#define _SCDCommunicationCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDCommunicationCheck : public CSingleInstance<CSCDCommunicationCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDCommunicationCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDCommunicationCheck_H */
