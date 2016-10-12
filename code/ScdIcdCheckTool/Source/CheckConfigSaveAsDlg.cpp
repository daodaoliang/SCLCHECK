// CheckConfigSaveAsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "..\ScdIcdCheckTool.h"
#include "CheckConfigSaveAsDlg.h"
#include "afxdialogex.h"


// CCheckConfigSaveAsDlg �Ի���

IMPLEMENT_DYNAMIC(CCheckConfigSaveAsDlg, CDialogEx)

CCheckConfigSaveAsDlg::CCheckConfigSaveAsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCheckConfigSaveAsDlg::IDD, pParent)
{

}

CCheckConfigSaveAsDlg::~CCheckConfigSaveAsDlg()
{
}

void CCheckConfigSaveAsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCheckConfigSaveAsDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CCheckConfigSaveAsDlg::OnBnClickedButtonOK)
    // ON_BN_CLICKED(IDCANCEL, &CCheckConfigSaveAsDlg::OnBnClickedButtonSaveAs)
END_MESSAGE_MAP()

void CCheckConfigSaveAsDlg::OnBnClickedButtonOK()
{
    CString newName;
    GetDlgItemText(IDC_EDIT_SAVEAS_NAME, newName);
    if (newName.IsEmpty())
    {
        AfxMessageBox(L"�������Ʋ���Ϊ�գ����������룡");
        return;
    }
    else 
    {
        map<CString, CString>::iterator it = mapCfgFileList.find(newName);
        if (it != mapCfgFileList.end())
        {
            UINT iRes = AfxMessageBox(_T("�����Ѵ���,�Ƿ񱣴�˴��޸�?"),MB_YESNO);
            if (iRes == IDYES)
            {
                m_NewName = newName;
                CDialog::OnOK();
            }
            else
            {
                return;
            }
        }
        else
        {
            m_NewName = newName;
        }
    }
    CDialog::OnOK();
}


// CCheckConfigSaveAsDlg ��Ϣ�������
