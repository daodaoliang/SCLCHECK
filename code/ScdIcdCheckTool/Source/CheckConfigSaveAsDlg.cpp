// CheckConfigSaveAsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\ScdIcdCheckTool.h"
#include "CheckConfigSaveAsDlg.h"
#include "afxdialogex.h"


// CCheckConfigSaveAsDlg 对话框

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
        AfxMessageBox(L"方案名称不能为空，请重新输入！");
        return;
    }
    else 
    {
        map<CString, CString>::iterator it = mapCfgFileList.find(newName);
        if (it != mapCfgFileList.end())
        {
            UINT iRes = AfxMessageBox(_T("方案已存在,是否保存此次修改?"),MB_YESNO);
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


// CCheckConfigSaveAsDlg 消息处理程序
