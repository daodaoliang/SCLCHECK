#include "IpValidCheck.h"
#include "../ScdChecker.h"
#include "../ConfigMgr.h"
#include "../CacheMgr.h"
#include "../OutputMgr.h"
#include "../../ScdCheckCommon/charconv.h"
#include "../../ScdCheckCommon/VtdTools.h"
#include <assert.h>

using namespace std;
using namespace com_ximpleware;

#define KEY "7053C8"

CIpValidCheck::CIpValidCheck()
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
    }
}

bool CIpValidCheck::CheckInternal()
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

int CIpValidCheck::StrToIntDef(const char *str, int default_value)
{
    int result = 0;
    int index = 0;

    for(; index < strlen(str); index += 1)
    {
        if(str[index] >= '0' && str[index] <= '9')
        {
            result *= 10;
            result += (str[index] - '0');
        }
        else
        {
            return default_value;
        }
    }

    if(index == 0)
    {
        return default_value;
    }

    return result;
}

bool CIpValidCheck::is_ipaddr(const char* ip)
{
    int temp;
    int dot_count = 0;
    char szip[20];
    const char *pip = szip;

    if(strlen(ip) > 16)
    {
        return false;
    }

    memset(szip, 0, sizeof(szip));
    strcpy(szip, ip);

    for(int index = 0; index < sizeof(szip); index += 1)
    {
        if(szip[index] == '.')
        {
            szip[index] = '\0';
            dot_count += 1;
        }
    }

    if(dot_count != 3)
    {
        return false;
    }

    //�жϵ�һ������
    temp = StrToIntDef(pip, -1);

    if(temp < 0 || temp > 255)
    {
        return false;
    }

    pip += (strlen(pip) + 1);

    //�жϵڶ�������
    temp = StrToIntDef(pip, -1);

    if(temp < 0 || temp > 255)
    {
        return false;
    }

    pip += (strlen(pip) + 1);

    //�жϵ���������
    temp = StrToIntDef(pip, -1);

    if(temp < 0 || temp > 255)
    {
        return false;
    }

    pip += (strlen(pip) + 1);

    //�жϵ��ĸ�����
    temp = StrToIntDef(pip, -1);

    if(temp < 0 || temp > 255)
    {
        return false;
    }

    pip += (strlen(pip) + 1);

    //�жϽ�������
    if(pip[0] != '\0')
    {
        return false;
    }

    return true;
}
