#include "DataSetCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "83395E"

CDataSetCheck::CDataSetCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("GetDataSetTypeOfStandard");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_IED");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_Services");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_AccessPoint");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_LN0");
        CCacheMgr::GetInstance()->AddNeededCacheMark("Travell_DataSet");
    }
}

bool CDataSetCheck::CheckInternal()
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

    return true;
}

void CDataSetCheck::CheckName_InCacheMgr(DatasetCache& dateset, int offset)
{
    CDataSetCheck* pDataSetCheck = CDataSetCheck::GetInstance();

    if (!pDataSetCheck->IsEnabled())
    {
        return;
    }
    // �����
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // �����������
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();

    DataSetTypeMap& mapDataset = pCacheMgr->GetDataSetTypeOfStandard();

    DataSetTypeMap::iterator itDataset = mapDataset.begin();
    while (itDataset != mapDataset.end())
    {
        wstring sNameTemp = itDataset->second.sName;
        int iPos = dateset.sName.find(sNameTemp);
        if (iPos == 0)
        {
            return;
        }
        itDataset++;
    }

    pDataSetCheck->Report(
        pChecker->CalcRowNumberFromOffset(offset),
        pChecker->GetRowContentByOffset(offset),
        "���ݼ�������Ӧ������Q/GDW-1396��׼�ж��������ԭ��(ǰ׺+����): LDevice inst=%s  DataSet name=%s ",
        WtoA(dateset.sLDevice).c_str(),
        WtoA(dateset.sName).c_str());

}