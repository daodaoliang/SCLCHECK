// AttributeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\ScdIcdCheckTool.h"
#include "AttributeDlg.h"
#include "afxdialogex.h"


// CAttributeDlg 对话框

IMPLEMENT_DYNAMIC(CAttributeDlg, CDialog)

CAttributeDlg::CAttributeDlg(CheckInfo& checkInfo, CWnd* pParent /*=NULL*/)
	: CDialog(CAttributeDlg::IDD, pParent),checkInfo(checkInfo)
{

}

CAttributeDlg::~CAttributeDlg()
{
}

void CAttributeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAttributeDlg, CDialog)
END_MESSAGE_MAP()


// CAttributeDlg 消息处理程序


BOOL CAttributeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    // TODO:  在此添加额外的初始化

    SetDlgItemText(IDC_EDIT_MSG, checkInfo.sFlag.c_str());
    SetDlgItemText(IDC_EDIT_POSITION, checkInfo.position.c_str());
    SetDlgItemText(IDC_EDIT_CLASS, checkInfo.sClass.c_str());
    SetDlgItemText(IDC_EDIT_XML, checkInfo.sXml.c_str());
    SetDlgItemText(IDC_EDIT_DESC, checkInfo.sDesc.c_str());
    SetDlgItemText(IDC_EDIT_REFERENCE, checkInfo.sStandard.c_str());

    return TRUE;
}
