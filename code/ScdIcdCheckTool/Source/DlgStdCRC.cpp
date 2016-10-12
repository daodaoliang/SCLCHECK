// DlgStdCRC.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "..\ScdIcdCheckTool.h"
#include "DlgStdCRC.h"
#include "afxdialogex.h"

#include "IcdParser.h"


// CDlgStdCRC �Ի���

IMPLEMENT_DYNAMIC(CDlgStdCRC, CDialog)

CDlgStdCRC::CDlgStdCRC(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStdCRC::IDD, pParent)
{

}

CDlgStdCRC::~CDlgStdCRC()
{
}

void CDlgStdCRC::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

#define POSTMESSAGE_CRC (WM_USER+102)

BEGIN_MESSAGE_MAP(CDlgStdCRC, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_SELECT, &CDlgStdCRC::OnBnClickedButtonSelect)
    ON_MESSAGE(POSTMESSAGE_CRC, &CDlgStdCRC::begainThread)
END_MESSAGE_MAP()


BOOL CDlgStdCRC::OnInitDialog()
{
    CDialog::OnInitDialog();

    PostMessage(POSTMESSAGE_CRC);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

CWinThread *g_pCRCThread = NULL;
bool	g_bCRCCompated = false;

//  ��ӽ������߳�
UINT		threadCRCLoad(LPVOID data)
{
    //EnterCriticalSection(&g_cs);

    CDlgStdCRC* pMain = (CDlgStdCRC*)data;

    if (!pMain->GeneraterCRC())
    {
        AfxMessageBox(L"�ļ�����ʧ�ܣ���ȷ���ļ����Ϲ淶��");
    }
    pMain->m_dlg.m_ctlBusy.End();
    pMain->m_dlg.SendMessage(WM_CLOSE);
    g_bCRCCompated = true;
    //LeaveCriticalSection(&g_cs);
    return 0;
}



LRESULT CDlgStdCRC::begainThread(WPARAM wPara,LPARAM lPara)
{
    g_bCRCCompated = false;
    g_pCRCThread = AfxBeginThread(threadCRCLoad, this);
    if (!g_bCRCCompated)

        m_dlg.DoModal();
    return 0;
}

// CDlgStdCRC ��Ϣ�������


void CDlgStdCRC::OnBnClickedButtonSelect()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CString sFileType = _T("SCD�ļ�(*.scd)|*.scd||");
    CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_FILEMUSTEXIST , sFileType, NULL, 0, false);
    if(dlg.DoModal() == IDOK)
    {
        sFilePath = dlg.GetPathName();

        g_bCRCCompated = false;
        g_pCRCThread = AfxBeginThread(threadCRCLoad, this);
        if (!g_bCRCCompated)
            m_dlg.DoModal();
    }
}

bool CDlgStdCRC::GeneraterCRC()
{
    sDate = L"";
    sTime = L"";
    wchCRC = L"";
    iIEDSize = L"";

    if (sFilePath.IsEmpty())
    {
        AfxMessageBox(_T("��ѡ����Ҫ�����ļ���"));
        return false;
    }
    CString sSCDName = sFilePath.Mid(sFilePath.ReverseFind(_T('\\')) + 1);

    CIcdParser parser(sFilePath);
    if (!parser.Parse())
    {
        AfxMessageBox(_T("����CRCʧ�ܣ�"));
        return  false;
    }

    //PSCDCRCSTRUW pSCDCRC;
    //GetSCDCRC(sFilePath, &pSCDCRC);
    insertSCDCRCList(sSCDName, parser);
    //SCDFreeW(pSCDCRC);
    return true;
}

void CDlgStdCRC::insertSCDCRCList(CString sSCDName, CIcdParser &pScd)
{
    CString name = _T("��           ��:  ") + sSCDName;
    GetDlgItem(IDC_STATIC_FILENAME)->SetWindowTextW(name);
    CString path = _T("�ļ�·��:  ");
    GetDlgItem(IDC_STATIC_FILEPATH)->SetWindowTextW(path);
    GetDlgItem(IDC_EDIT2)->SetWindowTextW(sFilePath);

    CTime tm;
    tm = CTime::GetCurrentTime();
    sDate = tm.Format(_T("%Y.%m.%d"));
    sTime = tm.Format(_T("%H:%M:%S"));

    CString date = _T("��           ��:  ") + sDate;
    GetDlgItem(IDC_STATIC_DATE)->SetWindowTextW(date);
    CString time = _T("ʱ        ��:  ") + sTime;
    GetDlgItem(IDC_STATIC_TIME)->SetWindowTextW(time);


    wchCRC.Format(_T("%s"), pScd.GetCrc());
    //iIEDSize.Format(_T("%d"),pScd->iIEDSize);
    // CString SCDCRC = _T("У    ��   ��:  ") + wchCRC;
    CString SCDCRC = _T("У    ��   ��:  ") ;
    GetDlgItem(IDC_STATIC_SCDCRC)->SetWindowTextW(SCDCRC);
    GetDlgItem(IDC_EDIT)->SetWindowTextW(wchCRC.MakeUpper());
    //CString iedCount = _T("У�������:  ") + iIEDSize;
    //GetDlgItem(IDC_STATIC_CRCCOUNT)->SetWindowTextW(iedCount);

}