/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 72417E
 * Name: SCDConnectedAPCheck
 * Desc: ConnectedAPԪ�ؼ��
 * Ref: GDW1396
 */

#ifndef _SCDConnectedAPCheck_H
#define _SCDConnectedAPCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CSCDConnectedAPCheck : public CSingleInstance<CSCDConnectedAPCheck>, CNoCopy, public CCheckItemBase
{
public:
    CSCDConnectedAPCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _SCDConnectedAPCheck_H */
