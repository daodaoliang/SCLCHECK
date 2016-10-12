#pragma once


// CCheckConfigSaveAsDlg 对话框

class CCheckConfigSaveAsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCheckConfigSaveAsDlg)

public:
	CCheckConfigSaveAsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckConfigSaveAsDlg();

    CString m_NewName;
    map<CString, CString> mapCfgFileList; // 自定义配置文件列表，Key为文件名，value为文件路径

// 对话框数据
	enum { IDD = IDD_DIALOG_SAVEAS_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButtonOK();
};
