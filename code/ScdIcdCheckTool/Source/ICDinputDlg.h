#pragma once
#include "afxwin.h"


// CICDinputDlg 对话框

class CICDinputDlg : public CDialog
{
	DECLARE_DYNAMIC(CICDinputDlg)

public:
	CICDinputDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CICDinputDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ICD_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    CString sIcdType;
    CString sIcdVoltage;

    afx_msg void OnBnClickedOk();
    CComboBox m_cbxIcdType;
    CComboBox m_cbxIcdVoltage;
};
