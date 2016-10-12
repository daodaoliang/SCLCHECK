#pragma once

#include "Dlg_AnglyzeFiles.h"
 
class CIcdParser;

// CDlgStdCRC �Ի���

class CDlgStdCRC : public CDialog
{
	DECLARE_DYNAMIC(CDlgStdCRC)

public:
	CDlgStdCRC(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgStdCRC();

// �Ի�������
	enum { IDD = IDD_DLG_CRC };
    BOOL CDlgStdCRC::OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    CDlg_AnglyzeFiles	m_dlg;
    CString sFilePath;
    CString sDate, sTime;
    CString wchCRC, iIEDSize;

    bool GeneraterCRC();
    afx_msg void OnBnClickedButtonSelect();
    afx_msg LRESULT begainThread(WPARAM wPara,LPARAM lPara);
    void insertSCDCRCList(CString sSCDName, CIcdParser& pScd);
};
