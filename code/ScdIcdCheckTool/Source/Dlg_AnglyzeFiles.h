#pragma once
#include "afxwin.h"
#include "..\resource.h"
#include "KCBusyProgressCtrl.h"

// CDlg_AnglyzeFiles �Ի���

class CDlg_AnglyzeFiles : public CDialog
{
	DECLARE_DYNAMIC(CDlg_AnglyzeFiles)

public:
	CDlg_AnglyzeFiles(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlg_AnglyzeFiles();

// �Ի�������
	enum { IDD = IDD_DLG_ANALYZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CKCBusyProgressCtrl m_ctlBusy;
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
