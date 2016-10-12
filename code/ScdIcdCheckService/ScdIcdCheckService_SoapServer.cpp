#include <WinSock2.h>
#include "ScdIcdCheckService_SoapServer.h"
#include "../gsoap/soapScdIcdCheckerService.h"
#include <list>
#include <vector>

using namespace std;

#define BACKLOG 88

namespace Internal
{
    class ScdIcdCheckService_SoapServer_Impl
    {
    public:
        ScdIcdCheckService_SoapServer_Impl(USHORT usPort, DWORD dwThreadCount)
            : m_usPort(usPort)
        {
            soap_init2(&m_serverSoap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE);
            m_serverSoap.send_timeout = 60;
            m_serverSoap.recv_timeout = 60;
            m_serverSoap.accept_timeout = 3;
            m_serverSoap.max_keep_alive = 20;
            m_serverSoap.bind_flags = SO_REUSEADDR;

            m_sockBase = m_serverSoap.bind(NULL, m_usPort, BACKLOG);

            if (!soap_valid_socket(m_sockBase))
            {
                throw runtime_error("soap_valid_socket error");
            }

            if (dwThreadCount == 0)
            {
                dwThreadCount = 10;
            }
            else if (dwThreadCount > MAXIMUM_WAIT_OBJECTS - 1)
            {
                // dwThreadCount的最大值要预留一个给m_hListenThread
                dwThreadCount = MAXIMUM_WAIT_OBJECTS - 1;
            }

            InitializeCriticalSection(&m_cs);
            m_hSemaphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
            m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

            for (DWORD dwIndex = 0; dwIndex < dwThreadCount; dwIndex += 1)
            {
                m_vectorSoapThreads.push_back(CreateThread(NULL, 0, SoapProc, this, 0, NULL));
            }

            m_hListenThread = CreateThread(NULL, 0, ListenProc, this, 0, NULL);
        }

        ~ScdIcdCheckService_SoapServer_Impl()
        {
            SetEvent(m_hStopEvent);

            // 将m_hListenThread加入向量，一并等待结束
            m_vectorSoapThreads.push_back(m_hListenThread);
            WaitForMultipleObjects((DWORD)m_vectorSoapThreads.size(), &*m_vectorSoapThreads.begin(), TRUE, 5431);

            vector<HANDLE>::const_iterator it = m_vectorSoapThreads.begin();
            for (; it != m_vectorSoapThreads.end(); ++it)
            {
                HANDLE hThread = *it;
                DWORD dwExitCodeThread = 0;

                GetExitCodeThread(hThread, &dwExitCodeThread);

                if (dwExitCodeThread == STILL_ACTIVE)
                {
                    TerminateThread(hThread, 9);
                }

                CloseHandle(hThread);
            }

            m_serverSoap.destroy();

            CloseHandle(m_hStopEvent);
            CloseHandle(m_hSemaphore);
            DeleteCriticalSection(&m_cs);
        }

    private:
        static DWORD CALLBACK ListenProc(LPVOID lpParam)
        {
            ScdIcdCheckService_SoapServer_Impl *pServer = (ScdIcdCheckService_SoapServer_Impl *)lpParam;

            while (TRUE)
            {
                SOAP_SOCKET processer = pServer->m_serverSoap.accept();

                if (!soap_valid_socket(processer))
                {
                    if (pServer->m_serverSoap.errnum != 0 ||
                        WAIT_OBJECT_0 == WaitForSingleObject(pServer->m_hStopEvent, 0)
                        )
                    {
                        soap_print_fault(&pServer->m_serverSoap, stderr);
                        break;
                    }

                    continue;
                }

                if (WAIT_OBJECT_0 == WaitForSingleObject(pServer->m_hStopEvent, 0))
                {
                    break;
                }

                ScdIcdCheckerService* srv = pServer->m_serverSoap.copy();

                if (srv)
                {
                    EnterCriticalSection(&pServer->m_cs);
                    pServer->m_listTasks.push_back(srv);
                    ReleaseSemaphore(pServer->m_hSemaphore, 1, NULL);
                    LeaveCriticalSection(&pServer->m_cs);
                }
            }
            return 0;
        }

        static DWORD CALLBACK SoapProc(LPVOID lpParam)
        {
            ScdIcdCheckService_SoapServer_Impl *pServer = (ScdIcdCheckService_SoapServer_Impl *)lpParam;
            HANDLE hObjects[] = {pServer->m_hSemaphore, pServer->m_hStopEvent};

            while (TRUE)
            {
                DWORD dwResult = WaitForMultipleObjects(RTL_NUMBER_OF(hObjects), hObjects, FALSE, INFINITE);

                if (dwResult == WAIT_OBJECT_0)
                {
                    ScdIcdCheckerService *srv = NULL;

                    EnterCriticalSection(&pServer->m_cs);
                    srv = *pServer->m_listTasks.begin();
                    pServer->m_listTasks.pop_front();
                    LeaveCriticalSection(&pServer->m_cs);

                    srv->serve();
                    srv->destroy();
                    soap_done(srv);
                    soap_free(srv);
                }
                else
                {
                    break;
                }
            }

            return 0;
        }

    private:
        USHORT                       m_usPort;               // 
        ScdIcdCheckerService         m_serverSoap;           // 
        SOAP_SOCKET                  m_sockBase;             // 
        HANDLE                       m_hStopEvent;           // 
        HANDLE                       m_hSemaphore;           // 
        CRITICAL_SECTION             m_cs;                   // 
        HANDLE                       m_hListenThread;        // 
        vector<HANDLE>               m_vectorSoapThreads;    // 
        list<ScdIcdCheckerService *> m_listTasks;            // 
    };
}

ScdIcdCheckService_SoapServer::ScdIcdCheckService_SoapServer(USHORT usPort, DWORD dwThreadCount)
    : m_impl(new Internal::ScdIcdCheckService_SoapServer_Impl(usPort, dwThreadCount))
{
}

ScdIcdCheckService_SoapServer::~ScdIcdCheckService_SoapServer()
{
}
