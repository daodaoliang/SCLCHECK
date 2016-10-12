#include "MDotypeCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "BB4024"

CMDotypeCheck::CMDotypeCheck()
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
// ����׼DOType��ǿ��DAԪ���Ƿ����
void CMDotypeCheck::CheckMDOTypeDA(DOTypeCache& inst, DOTypeCache& temp)
{
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();

    DAMap& instDAMap = inst.mapDaType;
    DAMap& tempDAMap = temp.mapDaType;

    DAMap::iterator itTempDA = tempDAMap.begin();
    DAMap::iterator itInstDA = instDAMap.begin();

    // ������׼ģ�������е�ǿ��DO,����ʵ���������Ƿ����
    while (itTempDA != tempDAMap.end())
    {
        DACache& objDA = itTempDA->second;

        if (objDA.eOption != L"M")
        {
            itTempDA++;
            continue;
        }

        if (instDAMap.end() == instDAMap.find(objDA.name))
        {
            // ǿ��DA������
            Report(
                pChecker->CalcRowNumberFromOffset(inst.offset),
                pChecker->GetRowContentByOffset(inst.offset),
                "��DO:%s����ǿ�ơ�DA:%s��������",
                WtoA(inst.id).c_str(),
                WtoA(objDA.name).c_str()
                );
        }
        itTempDA++;
    }
}

bool CMDotypeCheck::CheckInternal()
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
    DOTypeMap& instDOTypeMap = pCacheMgr->GetDOTypeCacheOfICD();
    DOTypeMap& tempDOTypeMap = pCacheMgr->GetDOTypeCacheOfStandard();

    DOTypeMap::iterator itInst = instDOTypeMap.begin();
    DOTypeMap::iterator itTemp;

    while (itInst != instDOTypeMap.end())
    {
        wstring sId = itInst->second.id;

        itTemp = tempDOTypeMap.find(sId);
        // ���� ��չ DOType
        if (itTemp == tempDOTypeMap.end()) 
        {
            itInst++;
            continue;
        }
        // ����׼DOType��ǿ�ƣ�������ǿ�ƣ�DA�Ƿ����
        CheckMDOTypeDA(itInst->second, itTemp->second);
        itInst++;
    }


    return true;
}
