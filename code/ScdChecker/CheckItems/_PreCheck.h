#ifndef __PreCheck_H
#define __PreCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../../ScdCheckCommon/ValidateCache.h"
#include "../ScdChecker.h"

class CPreCheck
{
public:
    CPreCheck();

public:
    bool CheckCommon();

    bool ControlBlockCheck();
    bool ConnectedAPValidCheck();
    bool InputsCheck();
    bool DoiInstanceCheck();
    bool FcdaValidCheck();

    bool DaTempValidCheck();
    bool DataTempRedunCheck();

private:
    bool ControlBlockLdCheck(LDCache *p_LD, ControlBlockInfoCache *p_CBInfo);
    bool CommunicationValidCheck(LDCache *p_LD, ControlBlockInfoCache *p_CBInfo, wstring type);
    bool InputsDescValidCheck(LDCache *p_LD, InputsCache *p_In);
    // bool DoiDescBlackCheck(LDCache *p_LD, LogicalNodesInstanceCache *p_LNInstance);
    bool DoiDescBlackCheck(LDCache *p_LD, LogicalNodesInstanceCache *p_LNInstance, FCDACache *p_FCDA);
    // bool DaTempValidCheck(LDCache *p_LD, LogicalNodesInstanceCache *p_LNInstance);

    bool InputsDescValid1Check(wstring &inSigDesc, wstring &outSigDesc, LDCache *p_LD, InputsCache *p_In);
    bool InputsDescValid2Check(wstring &inSigDesc, wstring &outSigDesc, LDCache *p_LD, InputsCache *p_In);

    bool InputsFormatCheck(LDCache &ldCache, InputsCache *p_In);
    bool InputsValidCheck(LDCache *p_LD, InputsCache *p_In);
    bool InputsDtValidCheck(LDCache *p_LD, InputsCache *p_In);

    bool CheckLNodeType(wstring IEDName, LogicalNodesInstanceCache *pLI, wstring &type, bool &bToDo);
    bool CheckLNodeType(wstring IEDName, wstring LNType, wstring DOI, wstring DAI, wstring &type, bool &bToDo);
    bool CheckDOType(wstring &doType, wstring &key, wstring &type, bool &bToDo);
    bool CheckDAType(wstring &daType, wstring &key, wstring &type, bool &bToDo);

private:
    SCDCache *p_SCDCache;
    CScdChecker *pChecker;
    wstring m_LogicalNodeInstance;
};

#endif /* __PreCheck_H */
