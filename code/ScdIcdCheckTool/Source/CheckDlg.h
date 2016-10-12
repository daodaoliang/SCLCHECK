#pragma once

#include "..\Shared\Runner.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "..\Shared\CheckComboBox.h"
#include <set>
#include <map>
using namespace std;

class CCheckConfigDlg;

class CCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckDlg)
protected:
    map<wstring,wstring> mapMsgE2C; // 英文标示——> 中文
    // 消息类型缓存
    map<wstring, vector<int>> mapMsg;
    map<wstring, wstring> mapCheckItem;
    map<CString, CString> mapCfgFileList; // 自定义配置文件列表，Key为文件名，value为文件路径
    void GetPrivateCfgFileList();
    CString m_NewCfgName;

    vector<int> m_vecFilter;

    CCriticalSection m_csCheck;
    CCriticalSection m_csExportExcel;
private:
    BOOL m_bScd; // true scd false icd
    BOOL m_bChecking;

    void UserInput(CString sCfgFile, CString sIcdTypeFile);

    class _Runner : public RunnerImpl
    {
    private:
        CCheckDlg& m_outer;
    public:
        _Runner(wstring& sCmd,CCheckDlg& outer) : RunnerImpl(sCmd.c_str()),m_outer(outer)
        {
            m_outer.LockResults();
            m_outer.m_CheckResults.clear();
            m_outer.UnLockResults();
        }
        virtual bool ProcessData(const std::string& sData);
        virtual bool ProcessOver();

    }* m_pInner;

public:
	CCheckDlg(BOOL bScd, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckDlg();

    HANDLE m_hExportExcelThread;
    DWORD  m_iExportExcelThreadID;

    CString m_sExcelPath;

    CheckResults m_CheckResults;
    void LockResults()
    {
        m_csCheck.Lock();
    }
    void UnLockResults()
    {
        m_csCheck.Unlock();
    }
    void Display();

    void LockExportExcel()
    {
        m_csExportExcel.Lock();
    }
    void UnLockExportExcel()
    {
        m_csExportExcel.Unlock();
    }

    CCheckComboBox m_cbxCfgList;
    CCheckComboBox m_cbxLevel;
    CCheckComboBox m_cbxCheckItem;
// 对话框数据
    enum { IDD = IDD_DIALOG_SCD_OR_ICD };

    bool m_bVisibleLast;
    CListCtrl m_objList;
    CCheckConfigDlg *dlg;

    CProgressCtrl m_exportExcelProgress;
    int    m_iExportProgress; // 当前导出的百分数

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonBrowserCfg();
    afx_msg void OnBnClickedButtonBrowserScd();
    afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg void OnBnClickedButtonCheck();
    afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedCheckShowlast();

    afx_msg void OnCheckMsgLevel();
    afx_msg void OnCheckItem();
    afx_msg void OnClose();
    afx_msg LRESULT OnExcelEnd(WPARAM,LPARAM);
    afx_msg LRESULT OnProcessEnd(WPARAM,LPARAM);

    void OnFilterMsg(void);
    afx_msg void OnBnClickedButtonExportExcel();
    afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnButtonConfig();
    afx_msg void OnCheckCfgFileLevel();
    afx_msg void OnButtonCancelExcel();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
