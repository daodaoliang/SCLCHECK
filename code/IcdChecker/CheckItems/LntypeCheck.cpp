#include "LntypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>


using namespace std;
using namespace com_ximpleware;

#define KEY "5BA640"

CLntypeCheck::CLntypeCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLNodeTypeCacheOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetLNodeTypeCacheOfICD");
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDOTypeCacheOfICD");
    }
}

/*

��׼LNodeType���
1������׼LNodeType�ı�׼DO˳���Ƿ���ȷ
2������׼LNodeType�ı�׼DO�����������Ƿ���ȷ
3����׼LNodeType�Ƿ������չ��DO����

*/
void CLntypeCheck::CheckDO(LNodeTypeCache& inst, LNodeTypeCache& temp)
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DOTypeMap& mapDOType = pCacheMgr->GetDOTypeCacheOfICD();

    DOMap& instDOMap = inst.mapDO;
    DOMap& tempDOMap = temp.mapDO;

    Order& tempDoOrder = temp.doOrder;
    Order& instDoOrder = inst.doOrder;

    DOMap::iterator itTempDo = tempDOMap.begin();
    DOMap::iterator itInstDo = instDOMap.begin();

    //ʵ��DO������ģ��DO�е�λ��
    Order::iterator itOrderPre = tempDoOrder.begin();
    Order::iterator itOrderCur = tempDoOrder.begin();
    
    // ��ʾ DO�Ƿ�����
    bool bOrdered = true;

    for (unsigned int iDo = 0; iDo < instDoOrder.size(); iDo++)
    {
        wstring sInstDoName = instDoOrder[iDo];

        DOCache& instDO = instDOMap[sInstDoName];
        DOCache& tempDO = tempDOMap[sInstDoName];

        // �õ���DO�ڱ�׼ģ���е�λ��
        itOrderCur = find(tempDoOrder.begin(), tempDoOrder.end(), sInstDoName);

        if (itOrderCur == tempDoOrder.end()) 
        {
            // 3�� ��չDO
            Report(
                pChecker->CalcRowNumberFromOffset(inst.offset),
                pChecker->GetRowContentByOffset(inst.offset),
                "LNodeType:%s ������չ��DO:%s",
                WtoA(inst.id).c_str(),
                WtoA(instDO.name).c_str()
                );

            continue;
        }

        if (bOrdered && itOrderCur < itOrderPre)
        {
            // 1��DO˳����ģ���в�һ��
            bOrdered = false;

            Report(
                pChecker->CalcRowNumberFromOffset(instDO.offset),
                pChecker->GetRowContentByOffset(instDO.offset),
                "%s",
                "DO��չʾ˳��Ӧ��396��׼�е�˳��һ��"
                );
        }
        itOrderPre = itOrderCur;

        wstring sCDC = mapDOType[instDO.type].cdc;

        if (sCDC != tempDO.type)
        {
            // 2�� DO ���Ͳ�һ��
            Report(
                pChecker->CalcRowNumberFromOffset(instDO.offset),
                pChecker->GetRowContentByOffset(instDO.offset),
                "%s",
                "DO �������׼ģ�岻һ��"
                );
        }
    }
}

bool CLntypeCheck::CheckInternal()
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
    // �õ� ��׼��LNodeType �� �����ICD�ļ��е�LNodeType
    LNodeTypeMap& instLNodeType = pCacheMgr->GetLNodeTypeCacheOfICD();
    LNodeTypeMap& tempLNodeType = pCacheMgr->GetLNodeTypeCacheOfStandard();

    LNodeTypeMap::iterator itInst = instLNodeType.begin();
    LNodeTypeMap::iterator itTemp;

    while (itInst != instLNodeType.end())
    {
        wstring sId = itInst->second.lnClass;

        itTemp = tempLNodeType.find(sId);
        // ���� �Ǳ�׼LNodeType
        if (itTemp == tempLNodeType.end()) 
        {
            itInst++;
            continue;
        }

        // ��� DO ˳�������
        CheckDO(itInst->second, itTemp->second);

        itInst++;
    }

    return true;
}