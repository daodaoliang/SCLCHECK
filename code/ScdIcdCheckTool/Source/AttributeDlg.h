#pragma once

#include "..\Shared\Runner.h"
// CAttributeDlg 对话框

class CAttributeDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttributeDlg)

public:
	CAttributeDlg(CheckInfo& checkInfo, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAttributeDlg();

    CheckInfo& checkInfo;
// 对话框数据
	enum { IDD = IDD_DIALOG_ATTRIBUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
