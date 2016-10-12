/** 
  Copyright (C), 2011-2016, Roof Tech. Co., Ltd. 
  * File name:      WndPreview.cpp
  * Author:         zhang-xch
  * Version:        V1
  * Date:           2013/11/20
  * Description:   �����ʵ��
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
// �л���ť��С����
#define BTN_WIDTH  255
#define BTN_HEIGHT 65

// �Խ���ťId����
#define IDC_CLOSE		4002
#define IDC_BTN_1		4003
#define IDC_BTN_2		4004
#define IDC_BTN_3		4005
#define IDC_BTN_4		4006
#define IDC_MINIMIZE    4007


// ͼƬ�ƶ���ʱ�����
#define TIMER_MOVE		1

// ��׼���캯��
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

// ��׼��������
CWndPreview::~CWndPreview()
{
}

// ������Ϣӳ���
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

// WM_ERASEBKGND��Ϣ������
BOOL CWndPreview::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

// WM_CREATE��Ϣ������
int  CWndPreview::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(_T("SSDT"));

	// ��ҳ��ǰ����Դ
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDR_FRAME);

	// ��ҳ�汳����Դ
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDR_BACK_FREE);

	// ���������ڴ�С��λ��
	CRect cClientRect;
	GetClientRect(&cClientRect);
	cClientRect.right = pFrameImage->GetWidth();
	cClientRect.bottom = pFrameImage->GetHeight();
	MoveWindow(&cClientRect);
	CenterWindow(GetParent());

	// �������ڱ�ΪԲ�Ǵ���
	CRgn hRgnClient;
	hRgnClient.CreateRoundRectRgn(0,pFrameImage->GetHeight()-pBackFree->GetHeight(),pFrameImage->GetWidth()+1,pFrameImage->GetHeight()+1,7,7); 
	CRgn hRgnButton;
	hRgnButton.CreateRoundRectRgn(19,0,303+1,60+1,3,3); 
	hRgnClient.CombineRgn(&hRgnButton,&hRgnClient,RGN_OR);
	SetWindowRgn(hRgnClient,FALSE); 

	// �����ҳ����ʾ��Ϣ
	AddButton(_T("ICDУ��"), _T(" ICD File Check"), IDR_ICON_DESKTOP, IDR_BACK_DESKTOP);
	AddButton(_T("SCDУ��"), _T("SCD File Check"), IDR_ICON_UCENTER, IDR_BACK_UCENTER);
	AddButton(_T("ICD�������"), _T("ICD Network Testing"), IDR_ICON_BAOHU, IDR_BACK_BAOHU);
	AddButton(_T("CRCУ����"), _T("Cyclic Redundancy Check"), IDR_ICON_SCAN, IDR_BACK_SCAN);

	CRect rtButton;
	CXButton dcControl;

	// �����رհ�ť
	rtButton.top = pFrameImage->GetHeight()-pBackFree->GetHeight();
	rtButton.bottom = rtButton.top + 22;
	rtButton.right = cClientRect.Width()-4;
	rtButton.left = rtButton.right-47;
	dcControl.CreateButton(this, rtButton, IDR_BTN_CLOSE, 4, NULL, 0, IDC_CLOSE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������С����ť
	rtButton.top = pFrameImage->GetHeight()-pBackFree->GetHeight();
	rtButton.bottom = rtButton.top + 22;
	rtButton.right = rtButton.right-47;
	rtButton.left = rtButton.right-47;
	dcControl.CreateButton(this, rtButton, IDR_BTN_MINIMIZE, 4, NULL, 0, IDC_MINIMIZE);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);


	// ������ҳ��1�а�ť
	rtButton.left = 600;
	rtButton.top = 330;
	rtButton.bottom = rtButton.top + 60;
	rtButton.right = rtButton.left +240;
	dcControl.CreateButton(this, rtButton, IDR_BTN_GREEN, 3, NULL, 0, IDC_BTN_1);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��2�а�ť
	rtButton.left = pFrameImage->GetWidth()*1 + 600;
	rtButton.top = 330;
	rtButton.bottom = rtButton.top + 60;
	rtButton.right = rtButton.left +240;
	dcControl.CreateButton(this, rtButton, IDR_BTN_GREEN, 3, NULL, 0, IDC_BTN_2);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��3�а�ť
	rtButton.left = pFrameImage->GetWidth()*2 + 600;
	rtButton.top = 330;
	rtButton.bottom = rtButton.top + 60;
	rtButton.right = rtButton.left +240;
	dcControl.CreateButton(this, rtButton, IDR_BTN_GREEN, 3, NULL, 0, IDC_BTN_3);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ������ҳ��4�а�ť
	rtButton.left = pFrameImage->GetWidth()*3 + 600;
	rtButton.top = 330;
	rtButton.bottom = rtButton.top + 60;
	rtButton.right = rtButton.left +240;
	dcControl.CreateButton(this, rtButton, IDR_BTN_GREEN, 3, NULL, 0, IDC_BTN_4);
	m_XButtonList.push_back(dcControl);
	m_XButtonRectList.push_back(rtButton);

	// ���õ�ǰ��ʾ��1����ҳ��
	m_iSelected = 0;
	return 0;
}

// WM_PAINT��Ϣ������
void CWndPreview::OnPaint()
{
	// ʹ��UNICODE�ַ�ת��
	USES_CONVERSION;

	CPaintDC dc(this);
	CMemoryDC dcMem(&dc);

	// ��ȡ���ھ���λ��
	CRect rtClient;
	GetClientRect(rtClient);

	// ��ȡ�ػ����λ��
	CRect rcPaint;
	dcMem.GetClipBox(&rcPaint);

	Gdiplus::Graphics graphics(dcMem.m_hDC);

	// ��ҳ��ǰ����Դ
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDR_FRAME);

	// ��ť�Ϸ���ͷ��Դ
	Image *pArrowImage = CImageInfo::Instance()->ImageFromResource(IDR_ARROW);

	// ��ҳ�汳����Դ
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDR_BACK_FREE);

	// �������Ϸ��°����԰�ť�ı���
	RectF gdiRect;
	gdiRect.X = 19, gdiRect.Y = 0, gdiRect.Width = 305, gdiRect.Height = 5;
	graphics.DrawImage(pBackFree, gdiRect, 0, 0, 305, 5,UnitPixel);

	// ��������ܱ���ͼƬ
	for(int i = -1; i <= (int)m_ButtonList.size(); i++)
	{
		// ���㵱ǰͼƬ��ʾλ���Ƿ�����������ʾ��Χ
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

	// ���������ǰ��ͼƬ
	graphics.DrawImage(pFrameImage, 0, 0, pFrameImage->GetWidth(), pFrameImage->GetHeight());

	// ���ƽ����л���ť�Ϸ��ļ�ͷ
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

	// ������չʾ����ͼƬ�л���ť
	StringFormat stringFormat;
	stringFormat.SetFormatFlags( StringFormatFlagsNoWrap);
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentNear);

	// ��������
	LOGFONT lfFont;
	memset(&lfFont, 0, sizeof(lfFont));
	lfFont.lfHeight = -17;
	lfFont.lfWeight |= FW_BOLD;
	lstrcpy(lfFont.lfFaceName, _T("����"));
	Font font(dcMem.GetSafeHdc(), &lfFont);
	// Ӣ������
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
		// �жϰ�ť�Ƿ���Ҫˢ��
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcButton, rcPaint))
		{
			continue;
		}

		// ��ȡ��ť״̬
		int iOffset = 0;
		if(m_iSelected == i)iOffset = 2;
		else if(m_iHovering == i)iOffset = 1;
		else iOffset = 0;

		// ���ư�ť����
		RectF gdiRect;
		gdiRect.X=point.X, gdiRect.Y=point.Y; gdiRect.Width = pBackHover->GetWidth()/3; gdiRect.Height = pBackHover->GetHeight();
		graphics.DrawImage(pBackHover, gdiRect, pBackHover->GetWidth()/3*iOffset, 0 , pBackHover->GetWidth()/3, pBackHover->GetHeight(), UnitPixel);

		// ���ư�ťͼ��
		Image* pIconImage = CImageInfo::Instance()->ImageFromResource(m_ButtonList[i].nIcon);
		gdiRect.X=point.X+10, gdiRect.Y=point.Y+11; gdiRect.Width = pIconImage->GetWidth(); gdiRect.Height = pIconImage->GetHeight();
		graphics.DrawImage(pIconImage, gdiRect);

		// ���ư�ť����
		CString sName = m_ButtonList[i].sName;
		CStringW wName = T2W(sName.GetBuffer());
		SolidBrush brush((ARGB)Color::White);
		gdiRect.X=point.X+69, gdiRect.Y=point.Y+15; gdiRect.Width = rcButton.Width()-58; gdiRect.Height = rcButton.Height() -20;
		graphics.DrawString(wName, wName.GetLength(), &font, gdiRect,&stringFormat, &brush);

		// ����Ӣ��
		CString sEnName = m_ButtonList[i].sEnName;
		CStringW wEnName = T2W(sEnName.GetBuffer());
		SolidBrush Enbrush((ARGB)Color::White);
		gdiRect.X=point.X+69, gdiRect.Y=point.Y+35; gdiRect.Width = rcButton.Width()-58; gdiRect.Height = rcButton.Height() -20;
		graphics.DrawString(wEnName, wEnName.GetLength(), &Enfont, gdiRect,&stringFormat, &Enbrush);
	}

	// ���ƽ���رհ�ť��������ť
	for(size_t i = 0; i< m_XButtonList.size(); i++)
	{
		CXButton& dcControl = m_XButtonList[i];
		CRect rcControl;
		if(i == 0 || i == 1)
		{
			// ��ǰ��ť�Ǵ��ڹرհ�ť
			dcControl.GetRect(&rcControl);
		}
		else
		{
			// ��ǰ��ť����ҳ���еĹ��ܰ�ť
			rcControl = m_XButtonRectList[i];
			rcControl.left -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			rcControl.right -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			dcControl.SetRect(&rcControl);			
		}
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		dcControl.DrawButton(graphics);
	}
	
}

// WM_LBUTTONDOWN��Ϣ������
void CWndPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bMouseDown)
		return;

	// ��������Ƿ����л���ť��
	CRect rtButton;
	int iButton = HitTest(point, rtButton);
	if(iButton != -1)
	{
		if(m_bAutoMove)
		{
			return;
		}

		// ����������ͼƬ�Զ��ƶ�����ת��ѡ��ͼƬ
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

	// ��������Ƿ���������ť��
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

	// ��괦�ڱ���λ��
	if(point.y < 53)
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
		return;
	}

	// ��괦�ڽ���ͼƬ֮��
	if(m_bAutoMove)
	{
		// ��ǰͼƬ�����Զ��ƶ�
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

// WM_MOUSEMOVE��Ϣ������
void CWndPreview::OnMouseMove(UINT nFlags, CPoint point) 
{
	// ������뿪�¼����и���
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

	// ����Ѿ���������ͼƬ����
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

	// ��������Ƿ����л���ť��
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

	// ��������Ƿ���������ť��
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

// WM_LBUTTONUP��Ϣ������
void CWndPreview::OnLButtonUp(UINT nFlags, CPoint point)
{
	// ����Ѿ���������ͼƬ����
	if(m_bMouseDown)
	{
		m_bMouseDown = FALSE;
		ReleaseCapture();

		CRect rtClient;
		GetClientRect(&rtClient);
		// ��������϶���
		if(m_iMouseMoveWidth > 0)
		{
			if(m_iSelected > 0 && m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth -= rtClient.Width();
				m_iSelected--;
			}
		}
		// ��������϶���
		else
		{
			if(m_iSelected < (int)m_ButtonList.size()-1 && -m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth += rtClient.Width();
				m_iSelected++;
			}
		}
		// ����������ͼƬ�Զ��ƶ����ָ���ǰͼƬ������ת��ѡ��ͼƬ
		m_iMoveWidthPerSecond = (m_iMouseMoveWidth>0?m_iMouseMoveWidth:-m_iMouseMoveWidth)/10;
		if(m_iMoveWidthPerSecond < 20)m_iMoveWidthPerSecond = 20;
		SetTimer(TIMER_MOVE, 30, NULL);
		m_bAutoMove = TRUE;
		return;
	}

	// ��������Ƿ���������ť��
	CRect rtButton;
	int iButton = XButtonHitTest(point, rtButton);
	if(iButton != -1)
	{
		m_XButtonList[iButton].LButtonUp();
	}
}

// WM_LBUTTONDBLCLK��Ϣ������
void CWndPreview::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

// WM_TIMER��Ϣ������
void CWndPreview::OnTimer(UINT_PTR nIDEvent)
{
	if(!m_bAutoMove)
		return;

	// ����Ѿ������϶������Ѿ�ѡ����ͼƬ
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
	// ����Ѿ������϶������Ѿ�ѡ���ҷ�ͼƬ
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

// �ڴ������ٺ����
void CWndPreview::PostNcDestroy()
{
	CWnd::PostNcDestroy();

	// �Զ��ͷŴ�����Դ
	delete this;
}

// ���ڹرհ�ť�¼�
void CWndPreview::OnBtnClose()
{
	SendMessage( WM_SYSCOMMAND,SC_CLOSE);
}

// ������С����ť�¼�
void CWndPreview::OnBtnMinimize()
{
	SendMessage( WM_SYSCOMMAND,SC_MINIMIZE);
}

// ���ͼƬҳ�溯��
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

// ������������л���ťλ��
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

// ��ȡָ���л���ť����λ��
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

// �����������������ťλ��
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

// �����봰����Ϣ֪ͨ
LRESULT CWndPreview::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	m_bHovering = TRUE;

	return 0;
}

// ����뿪������Ϣ֪ͨ
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
		AfxMessageBox(_T("��ȡ��ȫ����֤ʧ�ܣ�"));
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
		AfxMessageBox(_T("��ȡ��ȫ����֤ʧ�ܣ�"));
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
		AfxMessageBox(_T("��ȡ��ȫ����֤ʧ�ܣ�"));
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
		AfxMessageBox(_T("��ȡ��ȫ����֤ʧ�ܣ�"));
		return;
	}
#endif

	CString sFileType = _T("SCD�ļ�(*.scd)|*.scd||");
	CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_FILEMUSTEXIST , sFileType, NULL, 0, false );
	if(dlg.DoModal() == IDOK)
	{
		sFilePath = dlg.GetPathName();

#ifdef _GrandDOG
		if (!CheckSuperDog())
		{
			AfxMessageBox(_T("��ȡ��ȫ����֤ʧ�ܣ�"));
			return;
		} 
#endif
		// CDlgGeneraterCRC dlg;
        CDlgStdCRC dlg;
		dlg.sFilePath = sFilePath;
		dlg.DoModal();	
	}
}
