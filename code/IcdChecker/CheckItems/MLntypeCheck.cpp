#include "MLntypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "20544B"

CMLntypeCheck::CMLntypeCheck()
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

    }
}
// ����׼LNodeType��ǿ�ƣ�������ǿ�ƣ�DO�Ƿ����
void CMLntypeCheck::CheckMLNodeTypeDo(LNodeTypeCache& inst, LNodeTypeCache& temp)
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    DOMap& instDOMap = inst.mapDO;
    DOMap& tempDOMap = temp.mapDO;

    Order& tempDoOrder = temp.doOrder;
    Order& instDoOrder = inst.doOrder;

    DOMap::iterator itTempDo = tempDOMap.begin();
    DOMap::iterator itInstDo = instDOMap.begin();
   
    // ������׼ģ�������е�ǿ��DO,����ʵ���������Ƿ����
    while (itTempDo != tempDOMap.end())
    {
        DOCache& objDo = itTempDo->second;

        if (objDo.eOption != L"M")
        {
            itTempDo++;
            continue;
        }

        if (instDOMap.end() == instDOMap.find(objDo.name))
        {
            // ǿ��DO������
            Report(
                pChecker->CalcRowNumberFromOffset(inst.offset),
                pChecker->GetRowContentByOffset(inst.offset),
                "LNodeType��ǿ��DO:%s ������",
                WtoA(objDo.name).c_str()
                );
        }
        itTempDo++;
    }
}

bool CMLntypeCheck::CheckInternal()
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
    LNodeTypeMap& instLNodeType = pCacheMgr->GetLNodeTypeCacheOfICD();
    LNodeTypeMap& tempLNodeType = pCacheMgr->GetLNodeTypeCacheOfStandard();

    LNodeTypeMap::iterator itInst = instLNodeType.begin();
    LNodeTypeMap::iterator itTemp;

    while (itInst != instLNodeType.end())
    {
        wstring sId = itInst->second.lnClass;

        itTemp = tempLNodeType.find(sId);
        // ���� ��չ LNodeType
        if (itTemp == tempLNodeType.end()) 
        {
            itInst++;
            continue;
        }
        // ����׼LNodeType��ǿ�ƣ�������ǿ�ƣ�DO�Ƿ����
        CheckMLNodeTypeDo(itInst->second, itTemp->second);
        itInst++;
    }

    return true;
}
