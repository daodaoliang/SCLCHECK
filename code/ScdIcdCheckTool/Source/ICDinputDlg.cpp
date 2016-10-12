// ICDinputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\ScdIcdCheckTool.h"
#include "ICDinputDlg.h"
#include "afxdialogex.h"


// CICDinputDlg 对话框

IMPLEMENT_DYNAMIC(CICDinputDlg, CDialog)

CICDinputDlg::CICDinputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CICDinputDlg::IDD, pParent)
{

}

CICDinputDlg::~CICDinputDlg()
{
}

void CICDinputDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_ICD_TYPE, m_cbxIcdType);
    DDX_Control(pDX, IDC_COMBO_ICD_VOLTAGE, m_cbxIcdVoltage);
}


BEGIN_MESSAGE_MAP(CICDinputDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CICDinputDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CICDinputDlg 消息处理程序


void CICDinputDlg::OnBnClickedOk()
{
    int iType = m_cbxIcdType.GetCurSel();
    int iVoltage = m_cbxIcdVoltage.GetCurSel();
    if(iType < 0 || iVoltage < 0)
    {
        AfxMessageBox(_T("请选择保护类型和电压等级"));
        return;
    }
    m_cbxIcdType.GetLBText(iType, sIcdType);
    m_cbxIcdVoltage.GetLBText(iVoltage, sIcdVoltage);

    CDialog::OnOK();
}
