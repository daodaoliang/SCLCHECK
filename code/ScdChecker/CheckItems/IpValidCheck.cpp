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

    // 取本条目的配置信息并配置到本对象
    if (CConfigMgr::GetInstance()->GetItemConfig(KEY, cfg))
    {
        SetConfig(cfg);
    }

    // 如果启用了本条目，则向检测器注册
    if (IsEnabled())
    {
        CScdChecker::GetInstance()->RegisterCheckItem(this);

        // 在此处添加需要某某类型的缓存信息，还可以继续添加条目
    }
}

bool CIpValidCheck::CheckInternal()
{
    // 检测器
    CScdChecker *pChecker = CScdChecker::GetInstance();
    // 缓存管理器器
    CCacheMgr *pCacheMgr = CCacheMgr::GetInstance();
    // 输出管理器
    COutputMgr *pOutputMgr = COutputMgr::GetInstance();
    // 配置文件管理器
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

    //判断第一个数字
    temp = StrToIntDef(pip, -1);

    if(temp < 0 || temp > 255)
    {
        return false;
    }

    pip += (strlen(pip) + 1);

    //判断第二个数字
    temp = StrToIntDef(pip, -1);

    if(temp < 0 || temp > 255)
    {
        return false;
    }

    pip += (strlen(pip) + 1);

    //判断第三个数字
    temp = StrToIntDef(pip, -1);

    if(temp < 0 || temp > 255)
    {
        return false;
    }

    pip += (strlen(pip) + 1);

    //判断第四个数字
    temp = StrToIntDef(pip, -1);

    if(temp < 0 || temp > 255)
    {
        return false;
    }

    pip += (strlen(pip) + 1);

    //判断结束符号
    if(pip[0] != '\0')
    {
        return false;
    }

    return true;
}
