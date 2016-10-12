#pragma once
#include "afxwin.h"
#include "..\resource.h"
#include "KCBusyProgressCtrl.h"

// CDlg_AnglyzeFiles 对话框

class CDlg_AnglyzeFiles : public CDialog
{
	DECLARE_DYNAMIC(CDlg_AnglyzeFiles)

public:
	CDlg_AnglyzeFiles(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlg_AnglyzeFiles();

// 对话框数据
	enum { IDD = IDD_DLG_ANALYZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CKCBusyProgressCtrl m_ctlBusy;
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
