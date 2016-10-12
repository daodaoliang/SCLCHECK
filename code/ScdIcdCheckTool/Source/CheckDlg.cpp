// ScdCheckDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <afx.h>
#include "..\ScdIcdCheckTool.h"
#include "CheckDlg.h"
#include "afxdialogex.h"
#include "..\Shared\Markup.h"
#include "..\Shared\charconv.h"
#include <algorithm>
#include <iterator>
#include "AttributeDlg.h"
#include "CheckConfigDlg.h"
#include "ICDinputDlg.h"
#include "..\..\ScdCheckCommon\Common.h"
#include "../Shared/libxl/libxl.h"
#pragma comment(lib,"libxl.lib")

const int COL_CHECK_CLASS = 1;
const int COL_CHECK_ROW = 2;
const int COL_CHECK_DESC = 3;
const int COL_CHECK_STANDARD = 4;
const int COL_CHECK_LEVEL = 5;

#define WM_PROCESS_END  WM_USER+100
#define WM_EXCEL_END WM_USER+101

#define ID_TIMER_CHECK 1
#define ID_TIMER_EXCEL 2

// CCheckDlg 对话框

IMPLEMENT_DYNAMIC(CCheckDlg, CDialog)

CCheckDlg::CCheckDlg(BOOL bScd, CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent),m_bScd(bScd)
{
    m_bChecking = FALSE;
    m_bVisibleLast = true;
    mapMsgE2C[L"E"] = L"错误";
    mapMsgE2C[L"R"] = L"提示";
    mapMsgE2C[L"W"] = L"警告";


}

CCheckDlg::~CCheckDlg()
{
}

void CCheckDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_objList);
    DDX_Control(pDX, IDC_COMBO_CFG_FILE, m_cbxCfgList);    
    DDX_Control(pDX, IDC_COMBO_MSG, m_cbxLevel);
    DDX_Control(pDX, IDC_COMBO_CHECKITEM, m_cbxCheckItem);
    DDX_Control(pDX, IDC_PROGRESS_EXPORT_EXCEL, m_exportExcelProgress);
}

BEGIN_MESSAGE_MAP(CCheckDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_BROWSER_CFG, &CCheckDlg::OnBnClickedButtonBrowserCfg)
    ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CCheckDlg::OnBnClickedButtonBrowserScd)
    ON_WM_TIMER()
    ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, &CCheckDlg::OnLvnGetdispinfoList)
    ON_BN_CLICKED(IDC_BUTTON_CHECK, &CCheckDlg::OnBnClickedButtonCheck)
    ON_NOTIFY(NM_CLICK, IDC_LIST, &CCheckDlg::OnNMClickList)
    ON_BN_CLICKED(IDC_CHECK_SHOWLAST, &CCheckDlg::OnBnClickedCheckShowlast)

    // ON_BN_CLICKED(IDC_COMBO_CFG_FILE,&CCheckDlg::OnCheckCfgFileLevel)
    ON_BN_CLICKED(IDC_COMBO_MSG,&CCheckDlg::OnCheckMsgLevel)
    ON_BN_CLICKED(IDC_COMBO_CHECKITEM,&CCheckDlg::OnCheckItem)
    ON_BN_CLICKED(IDC_BUTTON_EXPORT_EXCEL, &CCheckDlg::OnBnClickedButtonExportExcel)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CCheckDlg::OnNMDblclkList)
    ON_COMMAND(IDC_BUTTON_CONFIG, &CCheckDlg::OnButtonConfig)
    ON_COMMAND(IDC_BUTTON_CANCEL_EXCEL, &CCheckDlg::OnButtonCancelExcel)

    ON_MESSAGE(WM_PROCESS_END, &CCheckDlg::OnProcessEnd)
    ON_MESSAGE(WM_EXCEL_END, &CCheckDlg::OnExcelEnd)

    ON_WM_CLOSE()

END_MESSAGE_MAP()

// CCheckDlg 消息处理程序
void CCheckDlg::OnFilterMsg(void)
{
    m_vecFilter.clear();
    // 同一复选框的set取并集
    int nLevelCount =  m_cbxLevel.GetCount();
    for (int iLevel = 0; iLevel < nLevelCount; iLevel++)
    {
        if (!m_cbxLevel.GetCheck(iLevel))
        {
            continue;
        }
        CString sLevel;
        m_cbxLevel.GetLBText(iLevel, sLevel);

        int nCheckCount =  m_cbxCheckItem.GetCount();
        for (int iCheck = 0; iCheck < nCheckCount; iCheck++)
        {
            if (!m_cbxCheckItem.GetCheck(iCheck))
            {
                continue;
            }
            CString sCheck;
            m_cbxCheckItem.GetLBText(iCheck, sCheck);

            LockResults();
            vector<int>& vecTmp = mapMsg[(LPCTSTR)(sLevel+sCheck)];
            m_vecFilter.insert(m_vecFilter.end(),vecTmp.begin(),vecTmp.end()) ;
            UnLockResults();
        }
    }

    m_objList.SetItemCountEx(m_vecFilter.size());
    m_objList.Invalidate();

    CString sSummer;
    sSummer.Format(_T("总记录数:%d    显示记录数:%d"), m_CheckResults.size(), m_vecFilter.size());
    SetDlgItemText(IDC_STATIC_SUMMER, (LPCTSTR)sSummer);
}

void CCheckDlg::OnCheckMsgLevel()
{
    OnFilterMsg();
}
void CCheckDlg::OnCheckItem()
{
    OnFilterMsg();
}

void CCheckDlg::OnBnClickedButtonBrowserCfg()
{
    CString sFileType = _T("配置文件(*.xml)|*.xml||");
    CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_FILEMUSTEXIST , sFileType, NULL, 0, false);
    if(dlg.DoModal() == IDOK)
    {
        SetDlgItemText(IDC_COMBO_CFG_FILE, (LPCTSTR)dlg.GetPathName());
    }
}

void CCheckDlg::OnBnClickedButtonBrowserScd()
{
    if (m_bScd)
    {
        CString sFileType = _T("SCD文件(*.scd)|*.scd||");
        CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_FILEMUSTEXIST , sFileType, NULL, 0, false);
        if(dlg.DoModal() == IDOK)
        {
            SetDlgItemText(IDC_EDIT_FILE, (LPCTSTR)dlg.GetPathName());
        }
    }
    else
    {
        CString sFileType = _T("ICD文件(*.icd)|*.icd||");
        CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_FILEMUSTEXIST , sFileType, NULL, 0, false);
        if(dlg.DoModal() == IDOK)
        {
            SetDlgItemText(IDC_EDIT_FILE, (LPCTSTR)dlg.GetPathName());
        }
    }
}

bool CCheckDlg::_Runner::ProcessData(const std::string& sData)
{
    // 解析数据
    CMarkup xml;
    xml.SetDoc(AtoW(sData) + L">");
    if (!xml.FindElem(L"CheckInfo"))
    {
        return true;
    }

    CheckInfo ci;
    ci.sKey = xml.GetAttrib(L"checkItemKey");
    ci.sName = xml.GetAttrib(L"checkItemName");
    ci.sDesc = xml.GetAttrib(L"desc");
    ci.position = xml.GetAttrib(L"position");
    ci.sStandard = xml.GetAttrib(L"standard");
    ci.sFlag = m_outer.mapMsgE2C[xml.GetAttrib(L"flag")];
    ci.sClass = xml.GetAttrib(L"checkClass");
    ci.sXml = xml.GetAttrib(L"xml");
    // 添加到缓存
    m_outer.LockResults();

    int iNum = m_outer.m_CheckResults.size();
    m_outer.m_CheckResults.push_back(ci);
    //按可能的过滤条件缓存记录序号
    m_outer.mapMsg[ci.sFlag + ci.sClass].push_back(iNum);
    m_outer.UnLockResults();

    m_outer.mapCheckItem[ci.sClass] = ci.sClass;

    if (ci.sName.empty() || ci.sKey.empty() || ci.sClass.empty() || ci.sDesc.empty() || ci.sFlag.empty() || ci.sStandard.empty())
    {
        AfxMessageBox(AtoW(sData).c_str());
    }
    return true;
}

bool CCheckDlg::_Runner::ProcessOver()
{
    m_outer.SendMessage(WM_PROCESS_END);

    return true;
}

LRESULT CCheckDlg::OnProcessEnd(WPARAM,LPARAM)
{
    KillTimer(1);
    Display();
    if (m_pInner)
    {
        delete m_pInner;
        m_pInner = NULL;
    }

    m_cbxLevel.ResetContent();

    m_cbxLevel.SetCheck(m_cbxLevel.AddString(L"错误"));
    m_cbxLevel.SetCheck(m_cbxLevel.AddString(L"警告"));
    m_cbxLevel.SetCheck(m_cbxLevel.AddString(L"提示"));

    m_cbxCheckItem.ResetContent();
    map<wstring,wstring>::iterator it = mapCheckItem.begin();
    while (it != mapCheckItem.end())
    {
        if (!it->first.empty())
        {
            int iIndex = m_cbxCheckItem.AddString(it->first.c_str());
            m_cbxCheckItem.SetCheck(iIndex);
        }
        it++;
    }

    m_cbxCheckItem.Invalidate();
    m_cbxLevel.Invalidate();

    CString sSummer;
    sSummer.Format(_T("总记录数:%d    显示记录数:%d"), m_CheckResults.size(), m_CheckResults.size());
    SetDlgItemText(IDC_STATIC_SUMMER, (LPCTSTR)sSummer);

    GetDlgItem(IDC_COMBO_MSG)->EnableWindow(TRUE);
    GetDlgItem(IDC_COMBO_CHECKITEM)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_EXPORT_EXCEL)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_CHECK)->EnableWindow(TRUE);

    GetDlgItem(IDC_BUTTON_CHECK)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_CONFIG)->EnableWindow(TRUE);

    GetDlgItem(IDC_BUTTON_BROWSER_CFG)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_BROWSER)->EnableWindow(TRUE);

    m_bChecking = FALSE;

    return TRUE;
}

void CCheckDlg::Display()
{
    LockResults();
    int nCount = m_CheckResults.size();
    UnLockResults();

    m_objList.SetItemCountEx(nCount, LVSICF_NOSCROLL|LVSICF_NOINVALIDATEALL);
    if (m_bVisibleLast)
    {
        m_objList.EnsureVisible(nCount-1,FALSE);
    }
}

void CCheckDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    if (pNMItemActivate->iItem < 0)
    {
        return;
    }
    m_bVisibleLast = false;
    m_objList.EnsureVisible(pNMItemActivate->iItem, FALSE);
    ((CButton*)GetDlgItem(IDC_CHECK_SHOWLAST))->SetCheck(m_bVisibleLast);
}

void CCheckDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == ID_TIMER_CHECK)
    {
        Display();
    }
    else
    {
        LockExportExcel();
        m_exportExcelProgress.SetPos(m_iExportProgress);
        UnLockExportExcel();
    }
    CDialog::OnTimer(nIDEvent);
}

void CCheckDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    *pResult = 0;

    LVITEM* pItem = &pDispInfo->item;
    if (pItem == NULL || !(pItem->mask & LVIF_TEXT))
    {
        return;
    }

    int iItem = pItem->iItem;
    CString sNum;
    sNum.Format(L"%d",iItem+1);

    if(m_vecFilter.size() > 0)
    {
        iItem = m_vecFilter[iItem];
    }

    LockResults();
    CheckInfo ci = m_CheckResults[iItem];
    UnLockResults();

    switch(pItem->iSubItem)
    {
    case 0:
        {
            lstrcpyn(pItem->pszText, (LPCTSTR)sNum, pItem->cchTextMax);
        }
        break;
    case COL_CHECK_CLASS:
        lstrcpyn(pItem->pszText, ci.sClass.c_str(), pItem->cchTextMax);
        break;
    case COL_CHECK_ROW:
        lstrcpyn(pItem->pszText, ci.position.c_str(), pItem->cchTextMax);
        break;
    case COL_CHECK_DESC:
        lstrcpyn(pItem->pszText, ci.sDesc.c_str(), pItem->cchTextMax);
        break;
    case COL_CHECK_STANDARD:
        lstrcpyn(pItem->pszText, ci.sStandard.c_str(), pItem->cchTextMax);
        break;
    case COL_CHECK_LEVEL:
        lstrcpyn(pItem->pszText, ci.sFlag.c_str(), pItem->cchTextMax);
        break;
    }
}

BOOL CCheckDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetIcon(theApp.LoadIcon(IDR_MAINFRAME), FALSE);
    // TODO:  在此添加额外的初始化
    m_objList.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT );

    m_objList.InsertColumn(0, L"序号", LVCFMT_LEFT, 67);
    m_objList.InsertColumn(COL_CHECK_CLASS, L"校验项目", LVCFMT_LEFT, 181);
    m_objList.InsertColumn(COL_CHECK_ROW, L"所在行", LVCFMT_LEFT, 65);
    m_objList.InsertColumn(COL_CHECK_DESC, L"检测结果", LVCFMT_LEFT, 506);
    m_objList.InsertColumn(COL_CHECK_STANDARD, L"参考规范", LVCFMT_LEFT, 92);
    m_objList.InsertColumn(COL_CHECK_LEVEL, L"校验类型", LVCFMT_LEFT, 59);

    ((CButton*)GetDlgItem(IDC_CHECK_SHOWLAST))->SetCheck(m_bVisibleLast);
    
    GetDlgItem(IDC_COMBO_MSG)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_CHECKITEM)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_EXPORT_EXCEL)->EnableWindow(FALSE);

    TCHAR szExe[MAX_PATH] = {0};
    wstring sOut;
    ResolveFilePath(L"cfg", sOut);
    lstrcpyn(szExe, sOut.c_str(), MAX_PATH);

    if (!m_bScd)
    {
        PathAppend(szExe, L"ICDCheckCfg.xml");
        SetWindowText(L"ICD 检测");
    }
    else
    {
        PathAppend(szExe, L"SCLCheckCfg.xml");
        SetWindowText(L"SCD 检测");
    }
    
    GetPrivateCfgFileList();
    SetDlgItemText(IDC_COMBO_CFG_FILE, szExe);

    return TRUE;
}
// ICD检测配置文件中LnValidCheck是否启用
void CCheckDlg::UserInput(CString sCfgFile, CString sIcdTypeFile)
{
    // LnValidCheck
    vector<CheckItem> vecCheckItem;
    CCheckConfigDlg::LoadConfigXml((LPCTSTR)sCfgFile, vecCheckItem);
    for (int iItem=0; iItem < vecCheckItem.size(); iItem++)
    {
        for (int iSubItem = 0; iSubItem < vecCheckItem[iItem].vecCheckItem.size(); iSubItem++)
        {
            CheckItem& ci = vecCheckItem[iItem].vecCheckItem[iSubItem];
            if(ci.sname == _T("LnValidCheck") && ci.isCheck == _T("启用"))
            {
                // 保护类型 和 电压等级等参数 输入
                CICDinputDlg dlg;
                if (dlg.DoModal() == IDOK)
                {
                    TCHAR szIcdTypeFile[MAX_PATH] = {0};
                    lstrcpyn(szIcdTypeFile, (LPCTSTR)sIcdTypeFile, MAX_PATH);
                    PathRemoveFileSpec(szIcdTypeFile);
                    PathAppend(szIcdTypeFile,_T("icd_type.xml"));
                    CMarkup xml;
                    xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
                    xml.AddElem(_T("Config"));
                    xml.IntoElem();
                    xml.AddElem(_T("ICD"));
                    xml.AddAttrib(_T("name"),(LPCTSTR)dlg.sIcdType);
                    xml.AddAttrib(_T("voltage"),(LPCTSTR)dlg.sIcdVoltage);
                    xml.OutOfElem();
                    xml.Save(szIcdTypeFile);
                }
                break;
            }
        }
    }
}

void CCheckDlg::OnBnClickedButtonCheck()
{
    CString sCfg,sFile;
    GetDlgItemText(IDC_EDIT_FILE, sFile);
    GetDlgItemText(IDC_COMBO_CFG_FILE, sCfg);

    if (!PathFileExists((LPCTSTR)sFile))
    {
        ::MessageBox(GetSafeHwnd(), _T("检测文件不存在,请重新选择!"), _T("警告"), MB_OK);
        return;
    }
    if (!PathFileExists((LPCTSTR)sCfg))
    {
        if (!PathFileExists((LPCTSTR)mapCfgFileList[sCfg]))
        {
            ::MessageBox(GetSafeHwnd(), _T("检测项配置文件不存在,请重新选择!"), _T("警告"), MB_OK);
            return;
        }
        else
        {
            sCfg = mapCfgFileList[sCfg];
        }
    }

    SetDlgItemText(IDC_STATIC_SUMMER, _T(""));
    GetDlgItem(IDC_COMBO_MSG)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_CHECKITEM)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_CHECK)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_CONFIG)->EnableWindow(FALSE);

    GetDlgItem(IDC_BUTTON_BROWSER_CFG)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_BROWSER)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_EXPORT_EXCEL)->EnableWindow(FALSE);

    m_cbxLevel.ResetContent();
    m_cbxCheckItem.ResetContent();
    m_objList.SetItemCountEx(0);

    wstring sExe;

    if (!m_bScd)
    {
        if (IsSystem64bit())
        {
            sExe += _T("icdchecker64.exe");
        }
        else
        {
            sExe += _T("icdchecker.exe");
        }
        // 特殊检测项的参数输入
        UserInput(sCfg, sFile);
    }
    else
    {
        if (IsSystem64bit())
        {
            sExe += _T("scdchecker64.exe");
        }
        else
        {
            sExe += _T("scdchecker.exe");
        }
    }
    ResolveFilePath(sExe,sExe);

    wstringstream ss;
    ss << sExe.c_str() << L" \""<< (LPCTSTR)sCfg << L"\" \"" << (LPCTSTR)sFile << L"\"";

    mapMsg.clear();
    m_vecFilter.clear();

    m_bChecking = TRUE;
    m_pInner = new _Runner(ss.str(),*this);
    SetTimer(1,100,NULL);
}

void CCheckDlg::OnBnClickedCheckShowlast()
{
    m_bVisibleLast = ((CButton*)GetDlgItem(IDC_CHECK_SHOWLAST))->GetCheck();
}

static DWORD CALLBACK ExportExcelProc(LPVOID lpParam)
{
    CCheckDlg* pScdCheck = (CCheckDlg*)lpParam;

    using namespace libxl;
    Book* pBook = xlCreateXMLBook();
    if(pBook==NULL)
    {
        pScdCheck->m_hExportExcelThread = NULL;
        pScdCheck->OnButtonCancelExcel();
        return 0;
    }

    pBook->setKey(L"济南容弗科技有限公司", L"windows-26292c0705cde60766bc6667a2hce3l9");
    Sheet* pSheet = pBook->addSheet(_T("检测报告"));
    if(pSheet == NULL)
    {
        pScdCheck->m_hExportExcelThread = NULL;
        pScdCheck->OnButtonCancelExcel();
        ::MessageBox(NULL, _T("创建工作薄失败,请重试!"), _T("友情提示"), MB_OK);
        return 0;
    }

    pBook->setRgbMode();
    //表头格式
    Format* pHeaderFormat = pBook->addFormat();
    pHeaderFormat->setAlignH(ALIGNH_CENTER);
    pHeaderFormat->setBorder(BORDERSTYLE_THIN);
    libxl::Font* pHeaderFont = pBook->addFont();
    pHeaderFont->setBold(true);
    pHeaderFont->setSize(10);
    pHeaderFont->setName(_T("宋体"));
    pHeaderFormat->setFont(pHeaderFont);
    pHeaderFormat->setWrap(true);
    pHeaderFormat->setAlignV(ALIGNV_CENTER);
    /*内容行格式*/
    //内容字体
    libxl::Font* pContentFont = pBook->addFont();
    pContentFont->setBold(false);
    pContentFont->setSize(9);
    pContentFont->setName(_T("宋体"));
    //横向左对齐

    Format* pContentFormatOddLeft = pBook->addFormat();
    pContentFormatOddLeft->setFont(pContentFont);
    pContentFormatOddLeft->setBorder(BORDERSTYLE_THIN);
    pContentFormatOddLeft->setWrap(true);
    pContentFormatOddLeft->setAlignV(ALIGNV_CENTER);
    pContentFormatOddLeft->setAlignH(ALIGNH_LEFT);
    //偶数
    //横向左对齐
    Format* pContentFormatEvenLeft = pBook->addFormat();
    pContentFormatEvenLeft = pBook->addFormat();
    libxl::Color clrEven = pBook->colorPack(240,240,240);
    pContentFormatEvenLeft->setFont(pContentFont);
    pContentFormatEvenLeft->setFillPattern(FILLPATTERN_SOLID);
    pContentFormatEvenLeft->setPatternForegroundColor(clrEven);
    pContentFormatEvenLeft->setBorder(BORDERSTYLE_THIN);
    pContentFormatEvenLeft->setWrap(true);
    pContentFormatEvenLeft->setAlignV(ALIGNV_CENTER);
    pContentFormatEvenLeft->setAlignH(ALIGNH_LEFT);

    //填充sheet内容
    CString sValue;
    const int ColumnWidth[][3] = {{8, 0}, {20, 0}, {10, 1}, {100, 0},
    {10, 0}, {10, 0}, {20, 0}, {200, 0}};
    //设置列宽信息
    for(int iCol = 0; iCol < ARRAYSIZE(ColumnWidth); iCol++)
    {
        pSheet->setCol(iCol, iCol, ColumnWidth[iCol][0]);
    }
    //填写 表头
    {
        pSheet->writeStr(0, 0, _T("序号"), pHeaderFormat);
        pSheet->writeStr(0, 1, _T("校验项目"), pHeaderFormat);
        pSheet->writeStr(0, 2, _T("所在行"), pHeaderFormat);
        pSheet->writeStr(0, 3, _T("检测结果"), pHeaderFormat);
        pSheet->writeStr(0, 4, _T("参考规范"), pHeaderFormat);
        pSheet->writeStr(0, 5, _T("校验类型"), pHeaderFormat);
        pSheet->writeStr(0, 6, _T("原文内容"), pHeaderFormat);
        pSheet->setRow(0, 25);
    }
    //填写 表内容
    pScdCheck->SetTimer(ID_TIMER_EXCEL, 1000, NULL);
    int nRecordCount = pScdCheck->m_CheckResults.size();

    pScdCheck->m_exportExcelProgress.SetRange32(0, 2 * nRecordCount);

    double dRecordCount = nRecordCount;
    for(int iRecord = 1; iRecord <= nRecordCount; iRecord++)
    {
        CheckInfo& ci = pScdCheck->m_CheckResults[iRecord-1];

        CString sNum;
        sNum.Format(_T("%d"), iRecord);

        Format* pFormat = (iRecord % 2)? pContentFormatEvenLeft : pContentFormatOddLeft;

        pSheet->writeStr(iRecord, 0, sNum, pFormat);
        pSheet->writeStr(iRecord, 1, ci.sClass.c_str(), pFormat);
        pSheet->writeStr(iRecord, 2, ci.position.c_str(), pFormat);
        pSheet->writeStr(iRecord, 3, ci.sDesc.c_str(), pFormat);
        pSheet->writeStr(iRecord, 4, ci.sStandard.c_str(), pFormat);
        pSheet->writeStr(iRecord, 5, ci.sFlag.c_str(), pFormat);
        pSheet->writeStr(iRecord, 6, ci.sXml.c_str(), pFormat);
        pSheet->setRow(iRecord, 25);

        pScdCheck->LockExportExcel();
        pScdCheck->m_iExportProgress = iRecord;
        pScdCheck->UnLockExportExcel();
    }

    pBook->save(pScdCheck->m_sExcelPath);
    pBook->release();

    pScdCheck->LockExportExcel();
    pScdCheck->m_iExportProgress = 2 * nRecordCount;
    pScdCheck->UnLockExportExcel();

    pScdCheck->SendMessage(WM_EXCEL_END, NULL, NULL);
    return 0;
}

void CCheckDlg::OnBnClickedButtonExportExcel()
{
    TCHAR szPath[MAX_PATH] = {0};
    lstrcpy(szPath, _T("检测报告.xlsx"));

    CFileDialog fileDlg(false);
    fileDlg.m_ofn.lpstrTitle = _T("文件选择对话框");
    fileDlg.m_ofn.lpstrFilter = _T("Excel Files(*.xlsx)\0\0");
    fileDlg.m_ofn.lpstrFile = szPath;
    int iOk = fileDlg.DoModal();
    if (iOk != IDOK)
    {
        return ;
    }

    PathAddExtension(szPath, _T(".xlsx"));
    m_sExcelPath = szPath;

    m_exportExcelProgress.SetPos(0);
    m_exportExcelProgress.ShowWindow(SW_SHOW);
    GetDlgItem(IDC_BUTTON_CANCEL_EXCEL)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_BUTTON_EXPORT_EXCEL)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_CHECK)->EnableWindow(FALSE);

    m_hExportExcelThread = CreateThread(NULL, 0, ExportExcelProc, this, 0, &m_iExportExcelThreadID);
}

void CCheckDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    int iItem = pNMItemActivate->iItem;
    if (iItem == -1)
    {
        return;
    }
    if (m_vecFilter.size() > 0)
    {
        iItem = m_vecFilter[iItem];
    }
    LockResults();
    CheckInfo ci = m_CheckResults[iItem];
    UnLockResults();
    CAttributeDlg dlg(ci);
    dlg.DoModal();

    *pResult = 0;
}

void CCheckDlg::OnButtonConfig()
{
    CString sConfigXml;
    GetDlgItemText(IDC_COMBO_CFG_FILE, sConfigXml);
    CString shortName = sConfigXml;
    CString strCaption = "默认方案";
    if (!PathFileExists((LPCTSTR)sConfigXml))
    {
        if (!PathFileExists((LPCTSTR)mapCfgFileList[sConfigXml]))
        {
            AfxMessageBox(_T("配置文件不存在,请重新选择!"));
            return;
        }
        else
        {
            strCaption = sConfigXml;
            sConfigXml = mapCfgFileList[sConfigXml];
        }        
    }

    dlg = new CCheckConfigDlg((LPCTSTR)sConfigXml, strCaption);
    dlg->m_NewName = shortName;
    dlg->mapCfgFileList = mapCfgFileList;
    dlg->DoModal();

    // 更新下拉列表
    m_NewCfgName = dlg->m_NewName;
    GetPrivateCfgFileList();
}


LRESULT CCheckDlg::OnExcelEnd(WPARAM,LPARAM)
{
    KillTimer(ID_TIMER_EXCEL);
    GetDlgItem(IDC_BUTTON_CANCEL_EXCEL)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_EXPORT_EXCEL)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_CHECK)->EnableWindow(TRUE);
    m_exportExcelProgress.ShowWindow(SW_HIDE);
    m_exportExcelProgress.SetPos(0);
    m_hExportExcelThread = NULL;
    AfxMessageBox(_T("导出成功"));

    return TRUE;
}

void CCheckDlg::OnButtonCancelExcel()
{
   TerminateThread(m_hExportExcelThread,0);
   DeleteFile(m_sExcelPath);

   KillTimer(ID_TIMER_EXCEL);
   GetDlgItem(IDC_BUTTON_CANCEL_EXCEL)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_BUTTON_EXPORT_EXCEL)->EnableWindow(TRUE);
   GetDlgItem(IDC_BUTTON_CHECK)->EnableWindow(TRUE);
   m_exportExcelProgress.ShowWindow(SW_HIDE);
   m_exportExcelProgress.SetPos(0);
}


BOOL CCheckDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if( pMsg->message == WM_KEYDOWN )	
    {
        if(pMsg->wParam == VK_END)	
        {
            m_bVisibleLast = true;
            ((CButton*)GetDlgItem(IDC_CHECK_SHOWLAST))->SetCheck(m_bVisibleLast);
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CCheckDlg::OnClose()
{
    if (m_bChecking)
    {
        AfxMessageBox(_T("检测过程中,暂时无法关闭该窗口!"));
        return;
    }
    CDialog::OnClose();
}

// 从cfg下读取配置文件
void CCheckDlg::GetPrivateCfgFileList()
{
    m_cbxCfgList.ResetContent();
    mapCfgFileList.clear();

    wstring sOut;
    TCHAR szExe[MAX_PATH] = {0};
    ResolveFilePath(L"cfg\\private\\", sOut);
    lstrcpyn(szExe, sOut.c_str(), MAX_PATH);
    PathAppendW(szExe, L"*.*");

    CFileFind finder;
    BOOL bFind = finder.FindFile(szExe);
    while (bFind)
    {
        bFind = finder.FindNextFile();
        if (finder.IsDots())
        {
            continue;
        }
        CString fileName = finder.GetFileName();
        PathRemoveExtensionW(fileName.GetBuffer());
        CString filePath = finder.GetFilePath();
        mapCfgFileList.insert(make_pair(fileName, filePath));
        
        int index = m_cbxCfgList.AddString((LPCTSTR)fileName);
        if (fileName == m_NewCfgName)
        {
            m_cbxCfgList.SetCurSel(index);
        }
    }
    finder.Close();
}
