/** 
  Copyright (C), 2011-2016, Roof Tech. Co., Ltd. 
  * File name:      WndPreview.cpp
  * Author:         zhang-xch
  * Version:        V1
  * Date:           2013/11/20
  * Description:   界面的实现
  */ 

#include "stdafx.h"
#include "..\ScdIcdCheckTool.h"
#include "..\ScdIcdCheckTool.h"
#include "WndPreview.h"
#include <ShlObj.h>
#include "DlgStdCRC.h"
#include "../Shared/charconv.h"
#include "..\Shared\SuperDog.h"
#include "CheckDlg.h"
// 切换按钮大小定义
#define BTN_WIDTH  255
#define BTN_HEIGHT 65

// 自建按钮Id定义
#define IDC_CLOSE		4002
#define IDC_BTN_1		4003
#define IDC_BTN_2		4004
#define IDC_BTN_3		4005
#define IDC_BTN_4		4006
#define IDC_MINIMIZE    4007


// 图片移动定时器编号
#define TIMER_MOVE		1

// 标准构造函数
CWndPreview::CWndPreview()
{
	m_iHovering = -1;
	m_iSelected = -1;

	m_iXButtonHovering = -1;

	m_bHovering = FALSE;
	m_bTracking = FALSE;

	m_bAutoMove = FALSE;
	m_bMouseDown = FALSE;
	m_iMouseMoveWidth = 0;

	m_iMoveWidthPerSecond = 0;
}

// 标准析构函数
CWndPreview::~CWndPreview()
{
}

// 窗口消息映射表
BEGIN_MESSAGE_MAP(CWndPreview, CWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDC_CLOSE, &CWndPreview::OnBtnClose)

	ON_COMMAND(IDC_MINIMIZE, &CWndPreview::OnBtnMinimize)
	ON_MESSAGE(WM_MOUSELEAVE, &CWndPreview::OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, &CWndPreview::OnMouseHover)
END_MESSAGE_MAP()

// WM_ERASEBKGND消息处理函数
BOOL CWndPreview::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

// WM_CREATE消息处理函数
int  CWndPreview::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(_T("SSDT"));

	// 主页面前景资源
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDR_FRAME);

	// 主页面背景资源
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDR_BACK_FREE);

	// 调整主窗口大小及位置
	CRect cClientRect;
	GetClientRect(&cClientRect);
	cClientRect.right = pFrameImage->GetWidth();
	cClientRect.bottom = pFrameImage->GetHeight();
	MoveWindow(&cClientRect);
	CenterWindow(GetParent());

	// 将主窗口变为圆角窗口
	CRgn hRgnClient;
	hRgnClient.CreateRoundRectRgn(0,pFrameImage->GetHeight()-pBackFree->GetHeight(),pFrameImage->GetWidth()+1,pFrameImage->GetHeight()+1,7,7); 
	CRgn hRgnButton;
	hRgnButton.CreateRoundRectRgn(19,0,303+1,60+1,3,3); 
	hRgnClient.CombineRgn(&hRgnButton,&hRgnClient,RGN_OR);
	SetWindowRgn(hRgnClient,FALSE); 

	// 添加主页面显示信息
	AddButton(_T("ICD校验"), _T(" ICD File Check"), IDR_ICON_DESKTOP, IDR_BACK_DESKTOP);
	AddButton(_T("SCD校验"), _T("SCD File Check"), IDR_ICON_UCENTER, IDR_BACK_UCENTER);
	AddButton(_T("ICD入网检测"), _T("ICD Network Testing"), IDR_ICON_BAOHU, IDR_BACK_BAOHU);
	AddButton(_T("CRC校验码"), _T("Cyclic Redundancy Check"), IDR_ICON_SCAN, IDR_BACK_SCAN);

	CRect rtButton;
	CXButton dcControl;

	// 创建关闭按钮
	rtButton.top = pFrameImage->GetHeight()-pBackFree->GetHeight();
	rtButton.bottom = rtButton.top + 22;
	rtButton.right = cClientRect.Width()-4;
	rtButton.left = rtButton.right-47;
	dcControl.CreateButton(this, rtButton, IDR_BTN_CLOSE, 4, NULL, 0, IDC_CLOSE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建最小化按钮
	rtButton.top = pFrameImage->GetHeight()-pBackFree->GetHeight();
	rtButton.bottom = rtButton.top + 22;
	rtButton.right = rtButton.right-47;
	rtButton.left = rtButton.right-47;
	dcControl.CreateButton(this, rtButton, IDR_BTN_MINIMIZE, 4, NULL, 0, IDC_MINIMIZE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// 创建子页面1中按钮
	rtButton.left = 600;
	rtButton.top = 330;
	rtButton.bottom = rtButton.top + 60;
	rtButton.right = rtButton.left +240;
	dcControl.CreateButton(this, rtButton, IDR_BTN_GREEN, 3, NULL, 0, IDC_BTN_1);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面2中按钮
	rtButton.left = pFrameImage->GetWidth()*1 + 600;
	rtButton.top = 330;
	rtButton.bottom = rtButton.top + 60;
	rtButton.right = rtButton.left +240;
	dcControl.CreateButton(this, rtButton, IDR_BTN_GREEN, 3, NULL, 0, IDC_BTN_2);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面3中按钮
	rtButton.left = pFrameImage->GetWidth()*2 + 600;
	rtButton.top = 330;
	rtButton.bottom = rtButton.top + 60;
	rtButton.right = rtButton.left +240;
	dcControl.CreateButton(this, rtButton, IDR_BTN_GREEN, 3, NULL, 0, IDC_BTN_3);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 创建子页面4中按钮
	rtButton.left = pFrameImage->GetWidth()*3 + 600;
	rtButton.top = 330;
	rtButton.bottom = rtButton.top + 60;
	rtButton.right = rtButton.left +240;
	dcControl.CreateButton(this, rtButton, IDR_BTN_GREEN, 3, NULL, 0, IDC_BTN_4);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// 设置当前显示第1个子页面
	m_iSelected = 0;
	return 0;
}

// WM_PAINT消息处理函数
void CWndPreview::OnPaint()
{
	// 使用UNICODE字符转换
	USES_CONVERSION;

	CPaintDC dc(this);
	CMemoryDC dcMem(&dc);

	// 获取窗口矩形位置
	CRect rtClient;
	GetClientRect(rtClient);

	// 获取重绘矩形位置
	CRect rcPaint;
	dcMem.GetClipBox(&rcPaint);

	Gdiplus::Graphics graphics(dcMem.m_hDC);

	// 主页面前景资源
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDR_FRAME);

	// 按钮上方箭头资源
	Image *pArrowImage = CImageInfo::Instance()->ImageFromResource(IDR_ARROW);

	// 主页面背景资源
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDR_BACK_FREE);

	// 绘制最上方新版特性按钮的背景
	RectF gdiRect;
	gdiRect.X = 19, gdiRect.Y = 0, gdiRect.Width = 305, gdiRect.Height = 5;
	graphics.DrawImage(pBackFree, gdiRect, 0, 0, 305, 5,UnitPixel);

	// 绘制主框架背景图片
	for(int i = -1; i <= (int)m_ButtonList.size(); i++)
	{
		// 计算当前图片显示位置是否在主界面显示范围
		int iCurrentX = (i*(int)pFrameImage->GetWidth() - (m_iSelected * (int)pFrameImage->GetWidth())+m_iMouseMoveWidth);
		if(iCurrentX > rtClient.Width() ||  iCurrentX + rtClient.Width() < 0)
		{
			continue;
		}

		Image* pBackImage = NULL;
		if(i == -1 || i == (int)m_ButtonList.size())
		{
			pBackImage = pBackFree;
		}
		else
		{
			pBackImage = CImageInfo::Instance()->ImageFromResource(m_ButtonList[i].nBack);
		}
		gdiRect.X = iCurrentX;
		gdiRect.Y=pFrameImage->GetHeight()-pBackImage->GetHeight();
		gdiRect.Height = pBackImage->GetHeight();
		gdiRect.Width = pBackImage->GetWidth();
		graphics.DrawImage(pBackImage, gdiRect, 0, 0, pBackImage->GetWidth(), pBackImage->GetHeight(),UnitPixel);
	}

	// 绘制主框架前景图片
	graphics.DrawImage(pFrameImage, 0, 0, pFrameImage->GetWidth(), pFrameImage->GetHeight());

	// 绘制界面切换按钮上方的箭头
	int iCurrent = ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth)*BTN_WIDTH/(int)pFrameImage->GetWidth();
	if(iCurrent >= 0)
	{
		gdiRect.X=0, gdiRect.Y=465; gdiRect.Width = iCurrent; gdiRect.Height = pArrowImage->GetHeight();
		graphics.DrawImage(pArrowImage, gdiRect, 0, 0, 1, pArrowImage->GetHeight(), UnitPixel);
	}

	gdiRect.X=iCurrent, gdiRect.Y=465; gdiRect.Width = pArrowImage->GetWidth(); gdiRect.Height = pArrowImage->GetHeight();
	graphics.DrawImage(pArrowImage, gdiRect, 0,0, pArrowImage->GetWidth(), pArrowImage->GetHeight(), UnitPixel);

	if( iCurrent + (int)pArrowImage->GetWidth() < (int)pFrameImage->GetWidth())
	{
		gdiRect.X=iCurrent + (int)pArrowImage->GetWidth(), gdiRect.Y=465; gdiRect.Width = pFrameImage->GetWidth() - (iCurrent + pArrowImage->GetWidth()); gdiRect.Height = pArrowImage->GetHeight();
		graphics.DrawImage(pArrowImage, gdiRect, 0,0, 1, pArrowImage->GetHeight(), UnitPixel);
	}

	// 绘制主展示界面图片切换按钮
	StringFormat stringFormat;
	stringFormat.SetFormatFlags( StringFormatFlagsNoWrap);
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentNear);

	// 汉语字体
	LOGFONT lfFont;
	memset(&lfFont, 0, sizeof(lfFont));
	lfFont.lfHeight = -17;
	lfFont.lfWeight |= FW_BOLD;
	lstrcpy(lfFont.lfFaceName, _T("黑体"));
	Font font(dcMem.GetSafeHdc(), &lfFont);
	// 英语字体
	LOGFONT lfEnFont;
	memset(&lfEnFont, 0, sizeof(lfEnFont));
	lfEnFont.lfHeight = -11;
	//lfEnFont.lfWeight |= FW_BOLD;
	lstrcpy(lfEnFont.lfFaceName, _T("Impact"));
	Font Enfont(dcMem.GetSafeHdc(), &lfEnFont);

	Image *pBackHover = CImageInfo::Instance()->ImageFromResource(IDR_HOVER);
	for(int i = 0; i < (int)m_ButtonList.size(); i++)
	{
		CRect rcButton;
		GetButtonRect( i, rcButton );
		Point point(rcButton.left, rcButton.top);
		// 判断按钮是否需要刷新
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcButton, rcPaint))
		{
			continue;
		}

		// 获取按钮状态
		int iOffset = 0;
		if(m_iSelected == i)iOffset = 2;
		else if(m_iHovering == i)iOffset = 1;
		else iOffset = 0;

		// 绘制按钮背景
		RectF gdiRect;
		gdiRect.X=point.X, gdiRect.Y=point.Y; gdiRect.Width = pBackHover->GetWidth()/3; gdiRect.Height = pBackHover->GetHeight();
		graphics.DrawImage(pBackHover, gdiRect, pBackHover->GetWidth()/3*iOffset, 0 , pBackHover->GetWidth()/3, pBackHover->GetHeight(), UnitPixel);

		// 绘制按钮图标
		Image* pIconImage = CImageInfo::Instance()->ImageFromResource(m_ButtonList[i].nIcon);
		gdiRect.X=point.X+10, gdiRect.Y=point.Y+11; gdiRect.Width = pIconImage->GetWidth(); gdiRect.Height = pIconImage->GetHeight();
		graphics.DrawImage(pIconImage, gdiRect);

		// 绘制按钮文字
		CString sName = m_ButtonList[i].sName;
		CStringW wName = T2W(sName.GetBuffer());
		SolidBrush brush((ARGB)Color::White);
		gdiRect.X=point.X+69, gdiRect.Y=point.Y+15; gdiRect.Width = rcButton.Width()-58; gdiRect.Height = rcButton.Height() -20;
		graphics.DrawString(wName, wName.GetLength(), &font, gdiRect,&stringFormat, &brush);

		// 绘制英文
		CString sEnName = m_ButtonList[i].sEnName;
		CStringW wEnName = T2W(sEnName.GetBuffer());
		SolidBrush Enbrush((ARGB)Color::White);
		gdiRect.X=point.X+69, gdiRect.Y=point.Y+35; gdiRect.Width = rcButton.Width()-58; gdiRect.Height = rcButton.Height() -20;
		graphics.DrawString(wEnName, wEnName.GetLength(), &Enfont, gdiRect,&stringFormat, &Enbrush);
	}

	// 绘制界面关闭按钮等其它按钮
	for(size_t i = 0; i< m_XButtonList.size(); i++)
	{
		CXButton& dcControl = m_XButtonList[i];
		CRect rcControl;
		if(i == 0 || i == 1)
		{
			// 当前按钮是窗口关闭按钮
			dcControl.GetRect(&rcControl);
		}
		else
		{
			// 当前按钮是子页面中的功能按钮
			rcControl = m_XButtonRectList[i];
			rcControl.left -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			rcControl.right -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			dcControl.SetRect(&rcControl);			
		}
		// 判断当前按钮是否需要重绘
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		dcControl.DrawButton(graphics);
	}
	
}

// WM_LBUTTONDOWN消息处理函数
void CWndPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bMouseDown)
		return;

	// 测试鼠标是否处在切换按钮上
	CRect rtButton;
	int iButton = HitTest(point, rtButton);
	if(iButton != -1)
	{
		if(m_bAutoMove)
		{
			return;
		}

		// 设置主界面图片自动移动，跳转到选定图片
		CRect rtClient;
		GetClientRect(&rtClient);
		m_iMouseMoveWidth = (iButton-m_iSelected)*rtClient.Width();
		m_iMoveWidthPerSecond = (m_iMouseMoveWidth>0?m_iMouseMoveWidth:-m_iMouseMoveWidth)/10;
		if(m_iMoveWidthPerSecond < 20)m_iMoveWidthPerSecond = 20;

		SetTimer(TIMER_MOVE, 30, NULL);
		m_bAutoMove = TRUE;

		m_iSelected = iButton;
		return;
	}

	// 测试鼠标是否处在其它按钮上
	iButton = XButtonHitTest(point, rtButton);
	if(iButton != -1)
	{
		if(m_bAutoMove)
		{
			return;
		}
		m_XButtonList[iButton].LButtonDown();
		if (iButton != 0 && iButton != 1
			&& TRUE == m_XButtonList[iButton].m_bPressed)
		{
			CString sFileType;
			switch(iButton)
			{
			case 2:
				OnBnClickedBtnICDCheck();
				break;
			case 3:
				OnBnClickedBtnSCDCheck();
				break;
			case 4:
				OnBnClickedBtnSCDCmp();
				break;
			case 5:
				OnBnClickedBtnCRCCheck();
				break;
			default:
				break;
			}
		}		
		return;
	}

	// 鼠标处于标题位置
	if(point.y < 53)
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
		return;
	}

	// 鼠标处于界面图片之上
	if(m_bAutoMove)
	{
		// 当前图片正在自动移动
		KillTimer(TIMER_MOVE);
		m_bAutoMove = FALSE;
		m_iMouseDownX = point.x - m_iMouseMoveWidth;
	}
	else
	{
		m_iMouseDownX = point.x;
	}
	m_bMouseDown = TRUE;
	SetCapture();
}

// WM_MOUSEMOVE消息处理函数
void CWndPreview::OnMouseMove(UINT nFlags, CPoint point) 
{
	// 对鼠标离开事件进行跟踪
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	if(m_bAutoMove)
	{
		return;
	}

	// 鼠标已经在主界面图片按下
	if(m_bMouseDown)
	{
		CRect rtClient;
		GetClientRect(&rtClient);
		m_iMouseMoveWidth = point.x - m_iMouseDownX;
		if(m_iSelected == 0 && m_iMouseMoveWidth > rtClient.Width())
		{
			m_iMouseMoveWidth = rtClient.Width();
		}
		if(m_iSelected == (int)m_ButtonList.size()-1 && -m_iMouseMoveWidth > rtClient.Width())
		{
			m_iMouseMoveWidth = -rtClient.Width();
		}
		Invalidate();
		return;
	}

	// 测试鼠标是否处在切换按钮上
	CRect rtButton;
	int iButton = HitTest(point, rtButton);
	if(iButton != m_iHovering)
	{
		if(m_iHovering != -1)
		{
			CRect oldRect;
			GetButtonRect(m_iHovering, oldRect);
			m_iHovering = -1;
			InvalidateRect(oldRect);
		}
		if(iButton != -1)
		{
			m_iHovering = iButton;
			InvalidateRect(rtButton);
		};
	}

	// 测试鼠标是否处在其它按钮上
	iButton = XButtonHitTest(point, rtButton);
	if(iButton != m_iXButtonHovering)
	{
		if(m_iXButtonHovering != -1)
		{
			m_XButtonList[m_iXButtonHovering].MouseLeave();
			m_iXButtonHovering = -1;
		}
		if(iButton != -1)
		{
			m_iXButtonHovering = iButton;
			m_XButtonList[m_iXButtonHovering].MouseHover();
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

// WM_LBUTTONUP消息处理函数
void CWndPreview::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 鼠标已经在主界面图片按下
	if(m_bMouseDown)
	{
		m_bMouseDown = FALSE;
		ReleaseCapture();

		CRect rtClient;
		GetClientRect(&rtClient);
		// 鼠标向右拖动动
		if(m_iMouseMoveWidth > 0)
		{
			if(m_iSelected > 0 && m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth -= rtClient.Width();
				m_iSelected--;
			}
		}
		// 鼠标向左拖动动
		else
		{
			if(m_iSelected < (int)m_ButtonList.size()-1 && -m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth += rtClient.Width();
				m_iSelected++;
			}
		}
		// 设置主界面图片自动移动，恢复当前图片，或跳转到选定图片
		m_iMoveWidthPerSecond = (m_iMouseMoveWidth>0?m_iMouseMoveWidth:-m_iMouseMoveWidth)/10;
		if(m_iMoveWidthPerSecond < 20)m_iMoveWidthPerSecond = 20;
		SetTimer(TIMER_MOVE, 30, NULL);
		m_bAutoMove = TRUE;
		return;
	}

	// 测试鼠标是否处在其它按钮上
	CRect rtButton;
	int iButton = XButtonHitTest(point, rtButton);
	if(iButton != -1)
	{
		m_XButtonList[iButton].LButtonUp();
	}
}

// WM_LBUTTONDBLCLK消息处理函数
void CWndPreview::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

// WM_TIMER消息处理函数
void CWndPreview::OnTimer(UINT_PTR nIDEvent)
{
	if(!m_bAutoMove)
		return;

	// 鼠标已经向右拖动，或已经选择左方图片
	if(m_iMouseMoveWidth > 0)
	{
		if(m_iMouseMoveWidth - m_iMoveWidthPerSecond <= 0)
		{
			m_iMouseMoveWidth = 0;
			Invalidate();
			KillTimer(TIMER_MOVE);
			m_bAutoMove = FALSE;
		}
		else
		{
			m_iMouseMoveWidth-= m_iMoveWidthPerSecond;
			Invalidate();
		}
	}
	// 鼠标已经向左拖动，或已经选择右方图片
	else
	{
		if(m_iMouseMoveWidth + m_iMoveWidthPerSecond >= 0)
		{
			m_iMouseMoveWidth = 0;
			Invalidate();
			KillTimer(TIMER_MOVE);
			m_bAutoMove = FALSE;
		}
		else
		{
			m_iMouseMoveWidth+= m_iMoveWidthPerSecond;
			Invalidate();
		}
	}

	CWnd::OnTimer(nIDEvent);
}

// 在窗口销毁后调用
void CWndPreview::PostNcDestroy()
{
	CWnd::PostNcDestroy();

	// 自动释放窗口资源
	delete this;
}

// 窗口关闭按钮事件
void CWndPreview::OnBtnClose()
{
	SendMessage( WM_SYSCOMMAND,SC_CLOSE);
}

// 窗口最小化按钮事件
void CWndPreview::OnBtnMinimize()
{
	SendMessage( WM_SYSCOMMAND,SC_MINIMIZE);
}

// 添加图片页面函数
int  CWndPreview::AddButton( LPCTSTR szName, LPCTSTR szEnName, UINT nIcon, UINT nBack )
{
	ButtonInfo buttonInfo;
	buttonInfo.sName = szName;
	buttonInfo.sEnName = szEnName;
	buttonInfo.nIcon = nIcon;
	buttonInfo.nBack = nBack;
	m_ButtonList.push_back(buttonInfo);
	return (int)m_ButtonList.size()-1;
}

// 测试鼠标所在切换按钮位置
int  CWndPreview::HitTest( CPoint point, CRect& rtButton )
{
	if(point.y < 483 || point.y > 555)
	{
		return -1;
	}

	if(point.x%BTN_WIDTH < 6 || point.x%BTN_WIDTH > 249)
	{
		return -1;
	}

	if(!GetButtonRect(point.x/BTN_WIDTH, rtButton))
	{
		return -1;
	}

	return point.x/BTN_WIDTH;
}

// 获取指定切换按钮所在位置
BOOL CWndPreview::GetButtonRect( int iButton, CRect& rtButton )
{
	if(iButton >= (int)m_ButtonList.size())
	{
		return FALSE;
	}

	rtButton.top = 483;
	rtButton.bottom = 555;
	rtButton.left = 1+ iButton * BTN_WIDTH + 8;
	rtButton.right =  1+ iButton * BTN_WIDTH + 260;

	return TRUE;
}

// 测试鼠标所在其它按钮位置
int  CWndPreview::XButtonHitTest( CPoint point, CRect& rtButton )
{
	for(size_t i = 0; i< m_XButtonList.size(); i++)
	{
		CXButton& dcControl = m_XButtonList[i];
		if(dcControl.PtInButton(point))
		{
			m_XButtonList[i].GetRect(&rtButton);
			return (int)i;
		}
	}
	return -1;
}

// 鼠标进入窗口消息通知
LRESULT CWndPreview::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	m_bHovering = TRUE;

	return 0;
}

// 鼠标离开窗口消息通知
LRESULT CWndPreview::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHovering = FALSE;
	if(m_iHovering != -1)
	{
		CRect oldRect;
		GetButtonRect(m_iHovering, oldRect);
		m_iHovering = -1;
		InvalidateRect(oldRect);
	}
	if(m_iXButtonHovering != -1)
	{
		m_XButtonList[m_iXButtonHovering].MouseLeave();
		m_iXButtonHovering = -1;
	}
	return 0;
}

void CWndPreview::OnBnClickedBtnICDCheck()
{
#ifdef _GrandDOG
	if (!CheckSuperDog())
	{
		AfxMessageBox(_T("读取安全狗认证失败！"));
		return;
	}
#endif

    CCheckDlg dlg(FALSE);
    dlg.DoModal();
    return;
}

void CWndPreview::OnBnClickedBtnSCDCheck()
{
#ifdef _GrandDOG
	if (!CheckSuperDog())
	{
		AfxMessageBox(_T("读取安全狗认证失败！"));
		return;
	}
#endif

    CCheckDlg dlg(TRUE);
    dlg.DoModal();
    return;
}

BOOL ForceRemoveDirectory(LPCTSTR lpDirPath)
{
    SHFILEOPSTRUCT fo = {NULL};
    TCHAR szDirPath[MAX_PATH];

    wnsprintf(szDirPath, RTL_NUMBER_OF(szDirPath), TEXT("%s%c%c"), lpDirPath, TEXT('\0'), TEXT('\0'));

    fo.wFunc = FO_DELETE;
    fo.pFrom = szDirPath;
    fo.pTo = NULL;
    fo.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_NO_UI | FOF_SILENT;

    return 0 == SHFileOperation(&fo);
}

BOOL ExtractResoureFiles(LPCTSTR lpPath, LPCTSTR lpType, int nResId)
{
    BOOL bRet = FALSE;
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HRSRC hRsrc = FindResource(hInstance, MAKEINTRESOURCE(nResId), lpType);

    if(hRsrc != NULL)
    {
        DWORD dwLength = SizeofResource(hInstance, hRsrc);
        HGLOBAL hGlobal = LoadResource(hInstance, hRsrc);

        if(hGlobal != NULL)
        {
            LPVOID lpBuffer = LockResource(hGlobal);

            if(lpBuffer != NULL)
            {
                HANDLE hFile = CreateFile(lpPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

                if(hFile != INVALID_HANDLE_VALUE)
                {
                    DWORD dwBytesWritten = 0;

                    bRet = WriteFile(hFile, lpBuffer, dwLength, &dwBytesWritten, NULL);
                    CloseHandle(hFile);
                }

                UnlockResource(hGlobal);
            }
        }
    }

    return bRet;
}

DWORD CALLBACK ExtractAndRunIcdCrcCalcToolProc(LPVOID lpParam)
{
    TCHAR szWorkPath[MAX_PATH];
    TCHAR szFilePath[MAX_PATH];

    ExpandEnvironmentStrings(TEXT("%tmp%\\_IcdCrcCalcTool"), szWorkPath, RTL_NUMBER_OF(szWorkPath));
    SHCreateDirectory(NULL, TtoW(szWorkPath).c_str());

    wnsprintf(szFilePath, RTL_NUMBER_OF(szFilePath), TEXT("%s\\IcdCrcCalcTool_%lu.exe"), szWorkPath, GetCurrentThreadId());

    if (!ExtractResoureFiles(szFilePath, TEXT("tool"), IDR_ICDCRCCALCTOOL))
    {
        return 0;
    }

    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;

    if (CreateProcess(NULL, szFilePath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hThread);
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
    }

    DeleteFile(szFilePath);
    ForceRemoveDirectory(szWorkPath);

    return 0;
}

void CWndPreview::OnBnClickedBtnSCDCmp()
{
#ifdef _GrandDOG
	if (!CheckSuperDog())
	{
		AfxMessageBox(_T("读取安全狗认证失败！"));
		return;
	}
#endif

    CloseHandle(CreateThread(NULL, 0, ExtractAndRunIcdCrcCalcToolProc, NULL, 0, NULL));
}

void CWndPreview::OnBnClickedBtnCRCCheck()
{
#ifdef _GrandDOG
	if (!CheckSuperDog())
	{
		AfxMessageBox(_T("读取安全狗认证失败！"));
		return;
	}
#endif

	CString sFileType = _T("SCD文件(*.scd)|*.scd||");
	CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_FILEMUSTEXIST , sFileType, NULL, 0, false );
	if(dlg.DoModal() == IDOK)
	{
		sFilePath = dlg.GetPathName();

#ifdef _GrandDOG
		if (!CheckSuperDog())
		{
			AfxMessageBox(_T("读取安全狗认证失败！"));
			return;
		} 
#endif
		// CDlgGeneraterCRC dlg;
        CDlgStdCRC dlg;
		dlg.sFilePath = sFilePath;
		dlg.DoModal();	
	}
}
