#include "ScdIcdCheckService_Config.h"
#include <Shlwapi.h>
#include "../ScdCheckCommon/charconv.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace boost::property_tree;

#define DEFAULT_PORT    23389

CScdIcdCheckService_Config::CScdIcdCheckService_Config(void)
    : m_nThreadCount(0)
    , m_nPortID(DEFAULT_PORT)
{
    InitPaths();

    if (!Load())
    {
        Save();
    }
}

CScdIcdCheckService_Config::~CScdIcdCheckService_Config(void)
{
    // do nothing
}

bool CScdIcdCheckService_Config::Load()
{
    for (vector<string>::const_iterator it = m_vectorFilePaths.begin(); it != m_vectorFilePaths.end(); ++it)
    {
        try
        {
            ptree pt;

            read_xml(*it, pt);

            m_nThreadCount = pt.get<int>("cfg.thread_count");
            m_nPortID      = pt.get<int>("cfg.port_id");

            m_strConfigFilePath = *it;

            return true;
        }
        catch (std::exception &)
        {
        }
    }

    return false;
}

bool CScdIcdCheckService_Config::Save() const
{
    if (m_strConfigFilePath.empty())
    {
        if (!m_vectorFilePaths.empty())
        {
            m_strConfigFilePath = *m_vectorFilePaths.begin();
        }
    }

    if (m_strConfigFilePath.empty())
    {
        return false;
    }

    try
    {
        ptree pt;

        pt.put("cfg.port_id", m_nPortID);
        pt.put("cfg.thread_count", m_nThreadCount);

        write_xml(m_strConfigFilePath, pt);

        return true;
    }
    catch (std::exception &)
    {
    }

    return false;
}

int CScdIcdCheckService_Config::GetThreadCount() const
{
    if (m_nThreadCount > 0)
    {
        return m_nThreadCount;
    }
    else
    {
        SYSTEM_INFO si;

        ZeroMemory(&si, sizeof(si));
        GetSystemInfo(&si);

        return si.dwNumberOfProcessors * 2 + 2;
    }
}

int CScdIcdCheckService_Config::GetPortID() const
{
    return m_nPortID;
}

void CScdIcdCheckService_Config::InitPaths()
{
    char szPath[MAX_PATH];
    const char *lpAddtions[] = {
        "\\..\\ScdIcdCheckService.cfg",
        "\\..\\..\\ScdIcdCheckService.cfg",
        "\\..\\..\\Config\\ScdIcdCheckService.cfg",
    };

    vector<string>().swap(m_vectorFilePaths);

    for (int i = 0; i < RTL_NUMBER_OF(lpAddtions); ++i)
    {
        GetModuleFileNameA(GetModuleHandle(NULL), szPath, RTL_NUMBER_OF(szPath));
        PathAppendA(szPath, lpAddtions[i]);

        m_vectorFilePaths.push_back(szPath);
    }
}
