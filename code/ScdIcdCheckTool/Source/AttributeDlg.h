#pragma once

#include "..\Shared\Runner.h"
// CAttributeDlg �Ի���

class CAttributeDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttributeDlg)

public:
	CAttributeDlg(CheckInfo& checkInfo, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAttributeDlg();

    CheckInfo& checkInfo;
// �Ի�������
	enum { IDD = IDD_DIALOG_ATTRIBUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
