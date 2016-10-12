// Dlg_AnglyzeFiles.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dlg_AnglyzeFiles.h"


// CDlg_AnglyzeFiles �Ի���

IMPLEMENT_DYNAMIC(CDlg_AnglyzeFiles, CDialog)

CDlg_AnglyzeFiles::CDlg_AnglyzeFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_AnglyzeFiles::IDD, pParent)
{

}

CDlg_AnglyzeFiles::~CDlg_AnglyzeFiles()
{
}

void CDlg_AnglyzeFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_NEWBUSY, m_ctlBusy);
}


BEGIN_MESSAGE_MAP(CDlg_AnglyzeFiles, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlg_AnglyzeFiles ��Ϣ�������

BOOL CDlg_AnglyzeFiles::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_ctlBusy.SetNumSteps(30);
	m_ctlBusy.SetSpeed(20);
	m_ctlBusy.SetRange(-100, 100);

	m_ctlBusy.SetMode(BPC_MODE_BUSY);
	m_ctlBusy.Start();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


HBRUSH CDlg_AnglyzeFiles::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)GetStockObject(NULL_BRUSH);

	return hbr;
}
