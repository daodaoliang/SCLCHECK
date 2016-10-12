// ICDinputDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "..\ScdIcdCheckTool.h"
#include "ICDinputDlg.h"
#include "afxdialogex.h"


// CICDinputDlg �Ի���

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


// CICDinputDlg ��Ϣ�������


void CICDinputDlg::OnBnClickedOk()
{
    int iType = m_cbxIcdType.GetCurSel();
    int iVoltage = m_cbxIcdVoltage.GetCurSel();
    if(iType < 0 || iVoltage < 0)
    {
        AfxMessageBox(_T("��ѡ�񱣻����ͺ͵�ѹ�ȼ�"));
        return;
    }
    m_cbxIcdType.GetLBText(iType, sIcdType);
    m_cbxIcdVoltage.GetLBText(iVoltage, sIcdVoltage);

    CDialog::OnOK();
}
