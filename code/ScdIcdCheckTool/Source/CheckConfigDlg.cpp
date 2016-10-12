// CheckConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\ScdIcdCheckTool.h"
#include "CheckConfigDlg.h"
#include "afxdialogex.h"
#include "..\..\ScdCheckCommon\Common.h"
#include "CheckConfigSaveAsDlg.h"

#include "../TreeListCtrl/supergridctrl.h"
// CCheckConfigDlg 对话框

IMPLEMENT_DYNAMIC(CCheckConfigDlg, CDialog)

CCheckConfigDlg::CCheckConfigDlg(wstring sXmlFile, CString caption,  CWnd* pParent /*=NULL*/)
	: CDialog(CCheckConfigDlg::IDD, pParent), m_sXmlFile(sXmlFile), m_strCaption(caption) 
{

}

CCheckConfigDlg::~CCheckConfigDlg()
{
}

void CCheckConfigDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    // DDX_Control(pDX, IDC_LIST_CONFIG, m_objList);
}


BEGIN_MESSAGE_MAP(CCheckConfigDlg, CDialog)
    //ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CCheckConfigDlg::OnLvnItemchangedList)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CCheckConfigDlg::OnBnClickedButtonCancel)
    ON_BN_CLICKED(IDC_BUTTON_SAVEAS, &CCheckConfigDlg::OnBnClickedButtonSaveAs)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CCheckConfigDlg::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_DEL, &CCheckConfigDlg::OnBnClickedButtonDel)

    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// CCheckConfigDlg 消息处理程序

#include "..\Shared\Markup.h"

BOOL CCheckConfigDlg::LoadConfigXml(wstring sXml, vector<CheckItem>& items)
{
    // 加载 xml文件
    CMarkup xml;
    xml.Load(sXml);
    vector<CheckItem>& vecCheckItem = items;

    map<wstring, wstring> mapMsgE2C;
    mapMsgE2C[L"E"] = L"错误";
    mapMsgE2C[L"R"] = L"提示";
    mapMsgE2C[L"W"] = L"警告";

    while (xml.FindChildElem(L"CheckClass"))
    {
        xml.IntoElem();
        CheckItem ci;
        ci.skey = xml.GetAttrib(L"key");
        ci.sdesc = xml.GetAttrib(L"desc");

        while (xml.FindChildElem(L"CheckItem"))
        {
            xml.IntoElem();
            CheckItem subCi;
            subCi.skey = xml.GetAttrib(L"key");
            subCi.sname = xml.GetAttrib(L"name");
            subCi.sdesc = xml.GetAttrib(L"desc");
            subCi.sflag = mapMsgE2C[xml.GetAttrib(L"flag")];
            subCi.isCheck = (xml.GetAttrib(L"isCheck") == L"true")?L"启用":L"禁用";
            subCi.sStandard = xml.GetAttrib(L"standard");

            while(xml.FindChildElem(L"SubCheckItem"))
            {
                xml.IntoElem();
                CheckItem lastItem;
                lastItem.skey = xml.GetAttrib(L"key");
                lastItem.sname = xml.GetAttrib(L"name");
                lastItem.sdesc = xml.GetAttrib(L"desc");
                lastItem.sflag = mapMsgE2C[xml.GetAttrib(L"flag")];
                lastItem.isCheck = (xml.GetAttrib(L"isCheck") == L"true")?L"启用":L"禁用";
                lastItem.sStandard = xml.GetAttrib(L"standard");
                subCi.vecCheckItem.push_back(lastItem);
                xml.OutOfElem();
            }
            ci.vecCheckItem.push_back(subCi);
            xml.OutOfElem();
        }
        vecCheckItem.push_back(ci);
        xml.OutOfElem();
    }
    return TRUE;
}

int CCheckConfigDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;
    // TODO:  在此添加您专用的创建代码
    m_objList.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0,0,0,0), this, 0x100);
    m_objList.SetExtendedStyle(LVS_EX_GRIDLINES);
    m_objList.ModifyStyleEx(0,WS_EX_CLIENTEDGE);

    LV_COLUMN   lvColumn;
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;

//     lvColumn.cx = 200;
//     lvColumn.pszText = L"项目编号";
//     m_objList.InsertColumn(0, &lvColumn);
// 
//     lvColumn.cx = 100;
//     lvColumn.pszText = L"项目名称";
//     m_objList.InsertColumn(1, &lvColumn);

    lvColumn.cx = 500;
    lvColumn.pszText = L"检测内容";
    m_objList.InsertColumn(0, &lvColumn);

    lvColumn.cx = 100;
    lvColumn.pszText = L"启  用";
    m_objList.InsertColumn(1, &lvColumn);

    lvColumn.cx = 100;
    lvColumn.pszText = L"检测级别";
    m_objList.InsertColumn(2, &lvColumn);

    lvColumn.cx = 100;
    lvColumn.pszText = L"标准";
    m_objList.InsertColumn(3, &lvColumn);

    vector<CheckItem>& vecCheckItem = m_vecCheckItem;
    LoadConfigXml(m_sXmlFile, vecCheckItem);
    for (int iItem = 0; iItem < vecCheckItem.size(); iItem++)
    {
        CheckItem& ci = vecCheckItem[iItem];
        CItemInfo* pRootInfo = new CItemInfo();
        pRootInfo->SetImage(0);
        pRootInfo->SetItemText(ci.sdesc.c_str());
        pRootInfo->AddSubItemText(_T(""));
        pRootInfo->AddSubItemText(_T(""));
        pRootInfo->AddSubItemText(_T(""));
        pRootInfo->m_lParam = (LPARAM)&ci;

        CTreeItem * pRootItem = m_objList.InsertRootItem(pRootInfo);

        for (int iSubItem=0; iSubItem < ci.vecCheckItem.size(); iSubItem++)
        {
            CheckItem& subCi = ci.vecCheckItem[iSubItem];

            CItemInfo* pChildInfo = new CItemInfo();
            pChildInfo->SetImage(2);
            if (subCi.vecCheckItem.size() > 0)
            {
                pChildInfo->SetImage(0);
            }            
            pChildInfo->SetItemText(subCi.sdesc.c_str());
            pChildInfo->AddSubItemText(subCi.isCheck.c_str());

            pChildInfo->SetControlType(CItemInfo::CONTROLTYPE::combobox, 0);
            CStringList lst;
            lst.AddTail(_T("启用"));
            lst.AddTail(_T("禁用"));
            pChildInfo->SetListData(0, &lst);

            pChildInfo->AddSubItemText(subCi.sflag.c_str());
            pChildInfo->SetControlType(CItemInfo::CONTROLTYPE::combobox, 1);
            lst.RemoveAll();
            lst.AddTail(_T("提示"));
            lst.AddTail(_T("警告"));
            lst.AddTail(_T("错误"));
            pChildInfo->SetListData(1, &lst);
            pChildInfo->AddSubItemText(subCi.sStandard.c_str());
            pChildInfo->m_lParam = (LPARAM)&subCi;
            CTreeItem * pChildItem = m_objList.InsertItem(pRootItem,pChildInfo);

            // 完整性检测 子项
            if (subCi.vecCheckItem.size() > 0)
            {
                for (int ilastItem=0; ilastItem < subCi.vecCheckItem.size(); ilastItem++)
                {
                    CheckItem& lastCi = subCi.vecCheckItem[ilastItem];

                    CItemInfo* pLastChildInfo = new CItemInfo();
                    pLastChildInfo->SetImage(2);
                    pLastChildInfo->SetItemText(lastCi.sdesc.c_str());
                    pLastChildInfo->AddSubItemText(lastCi.isCheck.c_str());

                    pLastChildInfo->SetControlType(CItemInfo::CONTROLTYPE::combobox, 0);
                    CStringList lst;
                    lst.AddTail(_T("启用"));
                    lst.AddTail(_T("禁用"));
                    pLastChildInfo->SetListData(0, &lst);

                    pLastChildInfo->AddSubItemText(lastCi.sflag.c_str());
                    pLastChildInfo->SetControlType(CItemInfo::CONTROLTYPE::combobox, 1);
                    lst.RemoveAll();
                    lst.AddTail(_T("提示"));
                    lst.AddTail(_T("警告"));
                    lst.AddTail(_T("错误"));
                    pLastChildInfo->SetListData(1, &lst);

                    pLastChildInfo->AddSubItemText(lastCi.sStandard.c_str());
                    pLastChildInfo->m_lParam = (LPARAM)&lastCi;
                    m_objList.InsertItem(pChildItem, pLastChildInfo);
                }
                // m_objList.Expand(pChildItem, m_objList.NodeToIndex(pChildItem));
            }
        }
        // m_objList.Expand(pRootItem, m_objList.NodeToIndex(pRootItem));
        int nScroll = 0;
        m_objList.ExpandAll(pRootItem, nScroll);
    }
    return 0;
}

BOOL CCheckConfigDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetIcon(theApp.LoadIcon(IDR_MAINFRAME), FALSE);
    CString caption = CString("检测项方案-") + m_strCaption;
    SetWindowTextW(caption);

    // 若为默认方案 则不显示删除和保存按钮
    if (m_strCaption == L"默认方案")
    {
        m_NewName = "默认方案";

        GetDlgItem(IDC_BUTTON_SAVE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_DEL)->ShowWindow(SW_HIDE);

        wstring filePath;
        ResolveFilePath(L"cfg\\private\\", filePath);
        TCHAR szExe[MAX_PATH] = {0};
        lstrcpyn(szExe, filePath.c_str(), MAX_PATH);
        PathAppendW(szExe, (LPCTSTR)(L"默认方案.xml"));
        if (!PathFileExists(szExe))
        {
            m_NewFile = szExe;
            SaveConfig();
        }
    }

    CRect rc;
    GetClientRect(rc);
    if(m_objList.GetSafeHwnd())
    {
        m_objList.MoveWindow(rc.left, rc.top, rc.Width(), rc.Height() - 35);
        m_objList.Invalidate();
    }
    SetDlgItemText(IDC_EDIT_NEWNAME, m_NewName);
    
    return TRUE;
}

void CCheckConfigDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if (m_objList.GetSafeHwnd())
    {
        CRect rc;
        GetClientRect(&rc);
        m_objList.MoveWindow(rc.left, rc.top, rc.Width(), rc.Height() - 35);
        m_objList.Invalidate();
        //m_objList.MoveWindow(&rc);
        //m_objList.Invalidate();
    }
    // TODO: 在此处添加消息处理程序代码
}

int CCheckConfigDlg::SaveConfig()
{
    map<wstring, wstring> mapMsgC2E;
    mapMsgC2E[L"错误"] = L"E";
    mapMsgC2E[L"提示"] = L"R";
    mapMsgC2E[L"警告"] = L"W";

    CMarkup xml;
    xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
    xml.AddElem(_T("SCLConfig"));
    xml.IntoElem();
    POSITION pos = m_objList.GetRootHeadPosition();
    CString sTmp;
    while (pos != NULL)
    {
        CTreeItem* pParent = (CTreeItem*)m_objList.GetNextRoot(pos);
        CItemInfo* pInfo = m_objList.GetData(pParent);
        CheckItem* pCi = (CheckItem*)pInfo->m_lParam;
        xml.AddElem(_T("CheckClass"));
        xml.AddAttrib(_T("key"), pCi->skey.c_str());
        xml.AddAttrib(_T("desc"),(LPCTSTR)pInfo->GetItemText());

        xml.IntoElem();
        POSITION posChild = m_objList.GetHeadPosition(pParent);
        while(posChild != NULL)
        {
            CTreeItem* pChild = (CTreeItem*)m_objList.GetNextChild(pParent, posChild);
            pInfo = m_objList.GetData(pChild);
            CheckItem* pSubCi = (CheckItem*)pInfo->m_lParam;
            xml.AddElem(_T("CheckItem"));
            xml.AddAttrib(_T("key"), pSubCi->skey.c_str());
            xml.AddAttrib(_T("name"), pSubCi->sname.c_str());
            xml.AddAttrib(_T("desc"),(LPCTSTR)pInfo->GetItemText());
            xml.AddAttrib(_T("flag"),mapMsgC2E[(LPCTSTR)pInfo->GetSubItem(1)]);
            xml.AddAttrib(_T("isCheck"),(pInfo->GetSubItem(0)==_T("启用"))?_T("true"):_T("false"));
            xml.AddAttrib(_T("standard"),(LPCTSTR)pInfo->GetSubItem(2));

            if (m_objList.NumChildren(pChild) > 0)
            {
                // 三级条目
                xml.IntoElem();
                POSITION posSubChild = m_objList.GetHeadPosition(pChild);
                while(posSubChild != NULL)
                {
                    CTreeItem* pSubChild = (CTreeItem*)m_objList.GetNextChild(pChild, posSubChild);
                    pInfo = m_objList.GetData(pSubChild);
                    CheckItem* pSubChildCi = (CheckItem*)pInfo->m_lParam;
                    xml.AddElem(_T("SubCheckItem"));
                    xml.AddAttrib(_T("key"), pSubChildCi->skey.c_str());
                    xml.AddAttrib(_T("name"), pSubChildCi->sname.c_str());
                    xml.AddAttrib(_T("desc"),(LPCTSTR)pInfo->GetItemText());
                    xml.AddAttrib(_T("flag"),mapMsgC2E[(LPCTSTR)pInfo->GetSubItem(1)]);
                    xml.AddAttrib(_T("isCheck"),(pInfo->GetSubItem(0)==_T("启用"))?_T("true"):_T("false"));                    
                    xml.AddAttrib(_T("standard"),(LPCTSTR)pInfo->GetSubItem(2));
                }
                xml.OutOfElem();
            }
        }
        xml.OutOfElem();        
    }
    xml.OutOfElem();
    //CMarkup xmlOld;
    //xmlOld.Load(m_sXmlFile);
    xml.Save(m_NewFile);

    /*if(xmlOld.GetDoc() != xml.GetDoc())
    {
    xml.Save(m_NewFile);
    UINT iRes = AfxMessageBox(_T("是否保存此次修改?"),MB_YESNOCANCEL);
    if (iRes == IDYES)
    {
    xml.Save(m_NewFile);
    }
    else
    {
    return iRes;
    }
    }*/
    return 0;
}

void CCheckConfigDlg::OnClose()
{
    //SetFocus();

    //if (SaveConfig() == IDCANCEL)
    //{
    //    return;
    //}
    OnBnClickedButtonSave();

    CDialog::OnClose();
}

void CCheckConfigDlg::OnBnClickedButtonSaveAs()
{
    CCheckConfigSaveAsDlg saveAsDlg;
    saveAsDlg.mapCfgFileList = mapCfgFileList;
    saveAsDlg.DoModal();
    CString sNewName = saveAsDlg.m_NewName;

    m_NewName = sNewName;
    sNewName += ".xml";
    wstring sOut;
    TCHAR szExe[MAX_PATH] = {0};
    ResolveFilePath(L"cfg\\private\\", sOut);
    lstrcpyn(szExe, sOut.c_str(), MAX_PATH);
    PathAppendW(szExe, (LPCTSTR)sNewName);
    m_NewFile = szExe;

    SaveConfig();
    CDialog::OnOK();
}

void CCheckConfigDlg::OnBnClickedButtonCancel()
{
    CDialog::OnCancel();
}

void CCheckConfigDlg::OnBnClickedButtonDel()
{
    UINT iRes = AfxMessageBox(_T("是否删除此方案?"),MB_YESNO);
    if (iRes == IDNO)
    {
        return;   
    }
    else
    {
        CString filePath = mapCfgFileList[m_NewName];
        if (PathFileExists(filePath))
        {
            DeleteFile(filePath);
        }
    }
    m_NewName = "默认方案";
    CDialog::OnCancel();
}

void CCheckConfigDlg::OnBnClickedButtonSave()
{
    
    CString newName = m_NewName;
    newName += ".xml";
    wstring sOut;
    TCHAR szExe[MAX_PATH] = {0};
    ResolveFilePath(L"cfg\\private\\", sOut);
    lstrcpyn(szExe, sOut.c_str(), MAX_PATH);
    PathAppendW(szExe, (LPCTSTR)newName);
    m_NewFile = szExe;

    SaveConfig();
    CDialog::OnOK();
}
