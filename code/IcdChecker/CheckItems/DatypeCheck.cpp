#include "DatypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "DE1D56"

CDatypeCheck::CDatypeCheck()
{
    CheckItemConfig cfg;

    // ȡ����Ŀ��������Ϣ�����õ�������
    if (CConfigMgr::GetInstance()->GetItemConfig(KEY, cfg))
    {
        SetConfig(cfg);
    }

    // ��������˱���Ŀ����������ע��
    if (IsEnabled())
    {
        CScdChecker::GetInstance()->RegisterCheckItem(this);

        // �ڴ˴������Ҫĳĳ���͵Ļ�����Ϣ�������Լ��������Ŀ
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDATypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDATypeCacheOfStandard");
    }
}
/*
    ����������Q/GDW-1396��׼�ж���������������ͣ�DAType����
    Ӧ���׼�ж��������һ��(���DAType��BDAԪ��˳���Ƿ�һ�£�
    ���DAType�����������Ƿ���ȷ)
    1��BDAԪ��˳���븽����һ�£�
    2��DAType��ÿ��BDA��name��bType��type��ȫһ�£�
    3��DAType�µ�BDA�븽������ȣ��޶����BDA,����ж����BDA����Ҫ��ʾ����չ��BDA;
*/
void CDatypeCheck::CheckDAType(DATypeCache& instDA, DATypeCache& tempDA)
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    BDAMap& instBDA = instDA.mapBDA;
    BDAMap& tempBDA = tempDA.mapBDA;

    Order& orderBDAInst = instDA.bdaOrder;
    Order& orderBDATemp = tempDA.bdaOrder;

    Order::iterator itOrderBDAInst = orderBDAInst.begin();
    // ʵ��BDA�ڱ�׼BDA�г��ֵ�λ��
    Order::iterator itOrderBDATemp = orderBDATemp.begin();

    bool bOrdered = true;
    while (itOrderBDAInst != orderBDAInst.end())
    {
        // ˳��У��
        Order::iterator itOrder = find(orderBDATemp.begin(), orderBDATemp.end(), *itOrderBDAInst);
        if (itOrder == orderBDATemp.end())
        {
            // ��չBDA
            Report(
                pChecker->CalcRowNumberFromOffset(instDA.offset),
                pChecker->GetRowContentByOffset(instDA.offset),
                "��DA:%s���д�����չ��BDA:%s��",
                WtoA(instDA.id).c_str(),
                WtoA(*itOrderBDAInst).c_str());

            itOrderBDAInst++;
            continue;
        }
        if (bOrdered && (itOrder < itOrderBDATemp))
        {
            // ˳��ߵ�
            bOrdered = false;

            Report(
                pChecker->CalcRowNumberFromOffset(instDA.offset),
                pChecker->GetRowContentByOffset(instDA.offset),
                "��DA:%s����BDA��˳�����׼��һ��",
                WtoA(instDA.id).c_str());
        }
        itOrderBDATemp = itOrder;

        // ���ԱȽ�
        BDACache& inst = instBDA[*itOrderBDAInst];
        BDACache& temp = tempBDA[inst.name];

        if((inst.type != temp.type 
            || inst.bType != temp.bType))
        {
            // ���Բ�һ��
            Report(
                pChecker->CalcRowNumberFromOffset(inst.offset),
                pChecker->GetRowContentByOffset(inst.offset),
                "��BDA:%s������ֵ���׼��һ��",
                WtoA(inst.name).c_str());
        }
        itOrderBDAInst++;
    }
}

bool CDatypeCheck::CheckInternal()
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    // ���������
    COutputMgr *pOutputMgr = COutputMgr::GetInstance();
    // �����ļ�������
    CConfigMgr *pConfigMgr = CConfigMgr::GetInstance();

    assert(pChecker != NULL);
    assert(pCacheMgr != NULL);
    assert(pOutputMgr != NULL);
    assert(pConfigMgr != NULL);

    // custom code goes here

    // �õ� ��׼�ļ� �� ICD����ļ��е�DAType
    DATypeMap& instDAMap = pCacheMgr->GetDATypeCacheOfICD();
    DATypeMap& tempDAMap = pCacheMgr->GetDATypeCacheOfStandard();
    
    DATypeMap::iterator itDA = instDAMap.begin();

    while (itDA != instDAMap.end())
    {        
        CheckDAType(itDA->second,tempDAMap[itDA->first]);

        itDA++;
    }
    return true;
}
