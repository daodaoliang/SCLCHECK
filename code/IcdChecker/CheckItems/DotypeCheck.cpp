#include "DotypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "3E1AE0"

CDotypeCheck::CDotypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
    }
}

/*
����������Q/GDW-1396��׼�ж�������ݶ������ͣ�DOType��Ӧ���׼�ж��������һ�¡�
1�����DOType�ı�׼DAԪ�ء�SDO��Ԫ��˳�������������Ƿ�һ�� ;
2�����DOType�����������Ƿ���ȷ
*/
void CDotypeCheck::CheckDOType(DOTypeCache& instDOType)
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& tempDOTypeMap = pCacheMgr->GetDOTypeCacheOfStandard();

    DOTypeCache& tempDOType = tempDOTypeMap[instDOType.id];
    //���Ե� ��չDO
    if (tempDOType.id != instDOType.id)
    {
        return;
    }

    DAMap& instDaMap = instDOType.mapDaType;
    DAMap& tempDaMap = tempDOType.mapDaType;

    DOMap& instSdoMap = instDOType.mapSDO;
    DOMap& tempSdoMap = tempDOType.mapSDO;

    // �Ƚ�ģ�塢ʵ��DO��da��sdo˳��
    Order& instDaOrder = instDOType.daOrder;
    Order& instSdoOrder = instDOType.sdoOrder;

    Order& tempDaOrder = tempDOType.daOrder;
    Order& tempSdoOrder = tempDOType.sdoOrder;

    Order::iterator itTempDa = tempDaOrder.begin();
    Order::iterator itTempSdo = tempSdoOrder.begin();

    // ���� DA �����˳������������ �Ƿ����׼һ��
    bool bOrdered = true;

    for(unsigned int iDA = 0; iDA < instDaOrder.size(); iDA++)
    {
        wstring sDaName = instDaOrder[iDA];
        // ��ǰDA�ڱ�׼�е�λ��
        Order::iterator itTempDaCur = find(tempDaOrder.begin(), tempDaOrder.end(), sDaName);
        
        if (bOrdered && itTempDaCur < itTempDa)
        {
            // ˳��ƥ��
            Report(
                pChecker->CalcRowNumberFromOffset(instDOType.offset),
                pChecker->GetRowContentByOffset(instDOType.offset),
                "��DO:%s������DA��˳����396��׼�е�˳��һ��",
                WtoA(instDOType.id).c_str()
                );

            bOrdered = false;
        }
        itTempDa = itTempDaCur;

        DACache& instDA = instDaMap[sDaName];
        DACache& tempDA = tempDaMap[sDaName];
        if (!(instDA.name == tempDA.name
            && instDA.bType == tempDA.bType
            && instDA.type == tempDA.type
            && instDA.dchg == tempDA.dchg
            && instDA.qchg == tempDA.qchg))
        {
            //�������Բ���ȫ��һ��
            Report(
                pChecker->CalcRowNumberFromOffset(instDA.offset),
                pChecker->GetRowContentByOffset(instDA.offset),
                "��DO:%s���¡�DA:%s���������������׼����ȫһ��",
                WtoA(instDOType.id).c_str(),
                WtoA(instDA.name).c_str()
                );
        }
    }

    // ���� SDO �����˳������������ �Ƿ����׼һ��
    bOrdered = true;

    for(unsigned int iSdo = 0; iSdo < instSdoOrder.size(); iSdo++)
    {
        wstring sSdoName = instSdoOrder[iSdo];
        // ��ǰsdo�ڱ�׼�е�λ��
        Order::iterator itTempSdoCur = find(tempSdoOrder.begin(), tempSdoOrder.end(), sSdoName);

        if (bOrdered && itTempSdoCur < itTempSdo)
        {
            // ˳��ƥ��
            Report(
                pChecker->CalcRowNumberFromOffset(instDOType.offset),
                pChecker->GetRowContentByOffset(instDOType.offset),
                "��DO:%s������SDO��˳����396��׼�е�˳��һ��",
                WtoA(instDOType.id).c_str()
                );

            bOrdered = false;
        }
        itTempSdo = itTempSdoCur;

        DOCache& instSdo = instSdoMap[sSdoName];
        DOCache& tempSdo = tempSdoMap[sSdoName];
        if (!(instSdo.name == tempSdo.name && instSdo.type == tempSdo.type))
        {
            //�������Բ���ȫ��һ��
            Report(
                pChecker->CalcRowNumberFromOffset(instSdo.offset),
                pChecker->GetRowContentByOffset(instSdo.offset),
                "��DO:%s���¡�SDO:%s���������������׼����ȫһ��",
                WtoA(instDOType.id).c_str(),
                WtoA(instSdo.name).c_str()
                );
        }
    }

    // ������������Ƿ�һ��
    if(instDOType.cdc != tempDOType.cdc)
    {
        Report(
            pChecker->CalcRowNumberFromOffset(instDOType.offset),
            pChecker->GetRowContentByOffset(instDOType.offset),
            "��DO:%s���������������׼��һ��",
            WtoA(instDOType.id).c_str()
            );
    }
}

bool CDotypeCheck::CheckInternal()
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
    // �õ�ICD����ļ��е�DOType
    DOTypeMap& instDOMap = pCacheMgr->GetDOTypeCacheOfICD();
    DOTypeMap::iterator itDO = instDOMap.begin();
    while(itDO != instDOMap.end())
    {
        CheckDOType(itDO->second);

        itDO++;
    }

    return true;
}
