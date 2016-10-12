#include "CheckItem.h"
#include <stdarg.h>
#include "OutputMgr.h"
#include "../ScdCheckCommon/Common.h"
#include "../ScdCheckCommon/charconv.h"

using namespace std;

CCheckItemBase::CCheckItemBase()
{
}

CCheckItemBase::~CCheckItemBase(void)
{
}

bool CCheckItemBase::IsEnabled() const
{
    return GetItemConfig().bEnabled;
}

ErrorType CCheckItemBase::GetErrorType() const
{
    return GetItemConfig().etValue;
}

std::string CCheckItemBase::GetErrorTypeString(ErrorType etValue) const
{
    switch (etValue)
    {
    case ET_WARNING:
        return "W";

    case ET_ERROR:
        return "E";

    case ET_TIP:
        return "R";

    default:
        return "N/A";
    }
}

const CheckItemConfig &CCheckItemBase::GetItemConfig() const
{
    return m_config;
}

bool CCheckItemBase::Check()
{
    if (IsEnabled())
    {
        COutputMgr::GetInstance()->Error(
            "%s ¼ì²âÏî%s£¬%s¿ªÊ¼¼ì²â\r\n",
            GetNowTimeString().c_str(),
            GetItemConfig().strCheckItemKey.c_str(),
            GetItemConfig().strCheckItemName.c_str());

        bool bResult = CheckInternal();

        COutputMgr::GetInstance()->Error(
            "%s ¼ì²âÏî%s£¬%s¼ì²âÍê±Ï(%s)\r\n",
            GetNowTimeString().c_str(),
            GetItemConfig().strCheckItemKey.c_str(),
            GetItemConfig().strCheckItemName.c_str(),
            CommonType2String(bResult).c_str());

        return bResult;
    }
    else
    {
        COutputMgr::GetInstance()->Error(
            "%s ¼ì²âÏî%s£¬%sÎ´ÆôÓÃ£¬ÂÔ¹ý\r\n",
            GetNowTimeString().c_str(),
            GetItemConfig().strCheckItemKey.c_str(),
            GetItemConfig().strCheckItemName.c_str());

        return true;
    }
}

bool CCheckItemBase::Report(int nRow, const std::string &strXmlText, const char *fmt, ...) const
{
    if (!IsEnabled())
    {
        return false;
    }

    char szText[10 * 1024] = "";
    va_list val;

    va_start(val, fmt);
    _vsnprintf(szText, sizeof(szText), fmt, val);
    va_end(val);

    const CheckItemConfig &cfg = GetItemConfig();

    return 0 < COutputMgr::GetInstance()->Output(
        "<CheckInfo"
        " checkItemKey=\"%s\""
        " checkItemName=\"%s\""
        " position=\"%d\""
        " desc=\"%s\""
        " standard=\"%s\""
        " flag=\"%s\""
        " checkClass=\"%s\""
        " xml=\"%s\""
        "/>\r\n",
        cfg.strCheckItemKey.c_str(),
        cfg.strCheckItemName.c_str(),
        nRow,
        XmlEscape(szText).c_str(),
        cfg.strCheckItemRef.c_str(),
        GetErrorTypeString(GetErrorType()).c_str(),
        cfg.strCheckClassDesc.c_str(),
        UtoA(XmlEscape(strXmlText)).c_str()
        );
}

bool CCheckItemBase::Report(std::string itemPath, ErrorType etValue, int nRow, const std::string &strXmlText, const char *fmt, ...) const
{
    if (!IsEnabled())
    {
        return false;
    }

    char szText[10 * 1024] = "";
    va_list val;

    va_start(val, fmt);
    _vsnprintf(szText, sizeof(szText), fmt, val);
    va_end(val);

    const CheckItemConfig &cfg = GetItemConfig();

    return 0 < COutputMgr::GetInstance()->Output(
        "<CheckInfo"
        " checkItemKey=\"%s\""
        " checkItemPath=\"%s\""
        " checkItemName=\"%s\""
        " position=\"%d\""
        " desc=\"%s\""
        " standard=\"%s\""
        " flag=\"%s\""
        " checkClass=\"%s\""
        " xml=\"%s\""
        "/>\r\n",
        cfg.strCheckItemKey.c_str(),
        itemPath.c_str(),
        cfg.strCheckItemName.c_str(),
        nRow,
        XmlEscape(szText).c_str(),
        cfg.strCheckItemRef.c_str(),
        GetErrorTypeString(etValue).c_str(),
        cfg.strCheckClassDesc.c_str(),
        UtoA(XmlEscape(strXmlText)).c_str()
        );
}

bool CCheckItemBase::Report(ErrorType etValue, int nRow, const std::string &strXmlText, const char *fmt, ...) const
{
    if (!IsEnabled())
    {
        return false;
    }

    char szText[10 * 1024] = "";
    va_list val;

    va_start(val, fmt);
    _vsnprintf(szText, sizeof(szText), fmt, val);
    va_end(val);

    const CheckItemConfig &cfg = GetItemConfig();

    return 0 < COutputMgr::GetInstance()->Output(
        "<CheckInfo"
        " checkItemKey=\"%s\""
        " checkItemName=\"%s\""
        " position=\"%d\""
        " desc=\"%s\""
        " standard=\"%s\""
        " flag=\"%s\""
        " checkClass=\"%s\""
        " xml=\"%s\""
        "/>\r\n",
        cfg.strCheckItemKey.c_str(),
        cfg.strCheckItemName.c_str(),
        nRow,
        XmlEscape(szText).c_str(),
        cfg.strCheckItemRef.c_str(),
        GetErrorTypeString(etValue).c_str(),
        cfg.strCheckClassDesc.c_str(),
        UtoA(XmlEscape(strXmlText)).c_str()
        );
}


void CCheckItemBase::SetConfig(const CheckItemConfig &config)
{
    m_config = config;
}
