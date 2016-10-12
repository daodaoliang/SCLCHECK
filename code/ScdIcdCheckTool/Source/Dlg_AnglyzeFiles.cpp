// Dlg_AnglyzeFiles.cpp : 实现文件
//

#include "stdafx.h"
#include "Dlg_AnglyzeFiles.h"


// CDlg_AnglyzeFiles 对话框

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


// CDlg_AnglyzeFiles 消息处理程序

BOOL CDlg_AnglyzeFiles::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_ctlBusy.SetNumSteps(30);
	m_ctlBusy.SetSpeed(20);
	m_ctlBusy.SetRange(-100, 100);

	m_ctlBusy.SetMode(BPC_MODE_BUSY);
	m_ctlBusy.Start();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


HBRUSH CDlg_AnglyzeFiles::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)GetStockObject(NULL_BRUSH);

	return hbr;
}
