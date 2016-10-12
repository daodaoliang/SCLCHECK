/** 
  Copyright (C), 2011-2016, Roof Tech. Co., Ltd. 
  * File name:      SingleInst.h
  * Author:         zhang-xch
  * Version:        V1.0
  * Date:           2013/12/25
  * Description:   
  */ 
#pragma once

class CSingleInst
{
public:
	CSingleInst(LPCTSTR lpszGuid);
	~CSingleInst(void);

	BOOL	AppStart();	//	return IsFirstInstance
	void	AppEnd();

	void	InitCompleted(HWND hMainWnd);
	BOOL	WaitForInitCompleted(HWND &hMainWnd, DWORD dwMillisecond = INFINITE);

	void	InitCompleted(LPVOID pData = NULL, UINT uSize = 0);
	BOOL	WaitForInitCompleted(LPVOID pData = NULL, UINT uSize = 0, DWORD dwMillisecond = INFINITE);

protected:
	CString	m_strGuid;
	CString	m_strGuidMapFile;
	
	HANDLE	m_hInstMutex;
	HANDLE	m_hMapFile;
};

//CSingleInst singleInst("Test1");
//
//BOOL CMainClientProcess::InitInstance()
//{
//	if(!CProcessBase::InitInstance())
//	{
//		return FALSE;
//	}
//
//	BOOL bRunning = !singleInst.AppStart();
//	if(bRunning)
//	{
//		HWND hWnd;
//		singleInst.WaitForInitCompleted(&hWnd, sizeof(HWND));
//
//		ShowWindow(hWnd, SW_SHOW);
//		SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
//		return FALSE;
//	}
//	else
//	{
//		this->m_pMainWnd = CWndInterface::CreateMainFrame();
//		this->m_pMainWnd->ShowWindow(SW_SHOW);
//		this->m_pMainWnd->UpdateWindow();
//
//		theMainWnd = this->m_pMainWnd->GetSafeHwnd();
//
//		singleInst.InitCompleted(&theMainWnd, sizeof(theMainWnd));
//	}
//
//	return TRUE;
//}
//
//int CMainClientProcess::ExitInstance()
//{
//	singleInst.AppEnd();
//	return CProcessBase::ExitInstance();
//}

// AppStart,InitComplete, WaitForInitCompleted必须在同一线程中使用
