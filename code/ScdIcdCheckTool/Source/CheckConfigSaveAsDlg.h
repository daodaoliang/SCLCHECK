#pragma once


// CCheckConfigSaveAsDlg �Ի���

class CCheckConfigSaveAsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCheckConfigSaveAsDlg)

public:
	CCheckConfigSaveAsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCheckConfigSaveAsDlg();

    CString m_NewName;
    map<CString, CString> mapCfgFileList; // �Զ��������ļ��б�KeyΪ�ļ�����valueΪ�ļ�·��

// �Ի�������
	enum { IDD = IDD_DIALOG_SAVEAS_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButtonOK();
};
