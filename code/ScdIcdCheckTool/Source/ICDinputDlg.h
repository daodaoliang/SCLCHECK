#pragma once
#include "afxwin.h"


// CICDinputDlg �Ի���

class CICDinputDlg : public CDialog
{
	DECLARE_DYNAMIC(CICDinputDlg)

public:
	CICDinputDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CICDinputDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ICD_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    CString sIcdType;
    CString sIcdVoltage;

    afx_msg void OnBnClickedOk();
    CComboBox m_cbxIcdType;
    CComboBox m_cbxIcdVoltage;
};
