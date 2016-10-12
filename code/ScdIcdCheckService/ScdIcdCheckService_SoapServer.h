#ifndef _SCDICDCHECKSERVICE_SOAPSERVER_H
#define _SCDICDCHECKSERVICE_SOAPSERVER_H

#include <Windows.h>
#include <memory>

namespace Internal
{
    class ScdIcdCheckService_SoapServer_Impl;
}

class ScdIcdCheckService_SoapServer
{
public:
    ScdIcdCheckService_SoapServer(USHORT usPort, DWORD dwThreadCount);
    ~ScdIcdCheckService_SoapServer();

private:
    std::auto_ptr<Internal::ScdIcdCheckService_SoapServer_Impl> m_impl;
};

#endif /* _SCDICDCHECKSERVICE_SOAPSERVER_H */