#include "FcdaValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "A63491"

CFcdaValidCheck::CFcdaValidCheck()
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
        CCacheMgr::GetInstance()->AddNeededCacheMark("FCDA");
        CCacheMgr::GetInstance()->AddNeededCacheMark("DataTypeTemplates");
    }
}

// ���ݼ����õ�FCDA�ǿյ�����ֵ�Ƿ�ָ��ģ�����Ѵ��ڵ����ݶ������������
bool CFcdaValidCheck::CheckInternal()
{
    //// �����
    //CScdChecker *pChecker = CScdChecker::GetInstance();
    //// �����������
    //CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    //// ���������
    //COutputMgr *pOutputMgr = COutputMgr::GetInstance();
    //// �����ļ�������
    //CConfigMgr *pConfigMgr = CConfigMgr::GetInstance();

    //assert(pChecker != NULL);
    //assert(pCacheMgr != NULL);
    //assert(pOutputMgr != NULL);
    //assert(pConfigMgr != NULL);

    //SCDCache *p_SCDCache = &pCacheMgr->m_SCDCache;
    //if (NULL != p_SCDCache)
    //{
    //    for (IEDMap::iterator itIED = p_SCDCache->mapIED.begin(); itIED != p_SCDCache->mapIED.end(); itIED++)
    //    {
    //        IEDCache *p_IED = &itIED->second;
    //        for (LDMap::iterator itLD = p_IED->mapLD.begin(); itLD != p_IED->mapLD.end(); itLD++)
    //        {
    //            LDCache *p_LD = &itLD->second;
    //            for (DataSetMap::iterator itDS = p_LD->mapDataSet.begin(); itDS != p_LD->mapDataSet.end(); itDS++)
    //            {
    //                DataSetCache *p_DS = &itDS->second;
    //                for (FCDAMap::iterator itFCDA = p_DS->mapFCDA.begin(); itFCDA !=  p_DS->mapFCDA.end(); itFCDA++)
    //                {
    //                    FCDACache *p_FCDA = &itFCDA->second;
    //                    LogicalNodesInstancesMap::iterator itInstace = p_LD->mapSignalInstance.find(itFCDA->first);
    //                    if (itInstace == p_LD->mapSignalInstance.end())
    //                    {
    //                        int row = pChecker->CalcRowNumberFromOffset(p_FCDA->offset);
    //                        string xml = pChecker->GetRowContentByOffset(p_FCDA->offset);
    //                        string msg = "���ݼ����õ�FCDA��ָ���ģ���в����ڶ�Ӧ�����ݶ������������: IED name=%s, LDevice inst=%s, DataSet name=%s, FCDA=%s";
    //                        Report(row, xml, msg.c_str(), WtoA(p_IED->name).c_str(), WtoA(p_LD->inst).c_str(), WtoA(p_DS->name).c_str(), WtoA(p_FCDA->key).c_str());
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}

    return true;
}
