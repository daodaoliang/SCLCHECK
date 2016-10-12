#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "..\TreeListCtrl\SuperGridExt.h"

#include <vector>
using namespace std;

struct CheckItem
{
    wstring skey;
    wstring sname;
    wstring sdesc;
    wstring sflag;
    wstring isCheck;
    wstring sStandard;
    wstring sCategory;
    std::vector<CheckItem> vecCheckItem;
};
// CCheckConfigDlg 对话框

class CCheckConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckConfigDlg)
private:
    wstring m_sXmlFile;
    wstring m_NewFile;
    vector<CheckItem> m_vecCheckItem;

    int SaveConfig();

public:
	CCheckConfigDlg(wstring sXmlFile, CString caption,  CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckConfigDlg();

    static BOOL LoadConfigXml(wstring sXml, vector<CheckItem>& items);
// 对话框数据
	enum { IDD = IDD_DIALOG_CONFIG_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CSuperGridExt m_objList;
    afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
    CComboBox m_cbxCategory;
    CComboBox m_cbxIsCheck;
    CComboBox m_cbxLevel;

    CString m_NewName;
    CString m_strCaption;
    map<CString, CString> mapCfgFileList; // 自定义配置文件列表，Key为文件名，value为文件路径

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnBnClickedButtonCancel();
    afx_msg void OnBnClickedButtonSaveAs();
    afx_msg void OnBnClickedButtonDel();
    afx_msg void OnBnClickedButtonSave();
};
