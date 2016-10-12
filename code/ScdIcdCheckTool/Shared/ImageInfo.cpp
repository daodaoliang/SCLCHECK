/** 
  Copyright (C), 2011-2016, Roof Tech. Co., Ltd. 
  * File name:      ImageInfo.cpp
  * Author:         zhang-xch
  * Version:        V1.0
  * Date:           2013/12/25
  * Description:   
  */ 
#include "stdafx.h"
#include "ImageInfo.h"
#include <comdef.h>
#include <shlwapi.h>
#include <cassert>

std::auto_ptr<CImageInfo> CImageInfo::sm_inst;

CImageInfo::CImageInfo(void)
{
}

CImageInfo::~CImageInfo(void)
{
	// Release在析构中调用，因为GdiplusShutdown的原因己调用的原因会导致内存崩溃
	// Release();
}

CImageInfo * CImageInfo::Instance()
{
	if(sm_inst.get() == 0)
		sm_inst = auto_ptr<CImageInfo>(new CImageInfo);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CMachine);
	return sm_inst.get();
	*/
}

Image* CImageInfo::ImageFromResource(UINT nFile)
{
	Image* pImage = NULL;
	if(m_mpImage.find(nFile) != m_mpImage.end())
	{
		return m_mpImage[nFile];
	}

	pImage = FromResource(nFile);
	if(pImage != NULL)
	{
		m_mpImage[nFile] = pImage;
	}

	return pImage;
}

void CImageInfo::Release()
{
	for(map<UINT, Image*>::iterator ciIter = m_mpImage.begin();
		ciIter != m_mpImage.end(); ciIter++)
	{
		delete ciIter->second;
	}
	m_mpImage.clear();
}

Image* CImageInfo::FromResource( UINT uId )
{
	HMODULE hModule = AfxGetResourceHandle();
	HRSRC hRes = ::FindResource( hModule, MAKEINTRESOURCE(uId), _T("PNG"));
	if( hRes == NULL )
	{
		assert(FALSE);
		return NULL;
	}

	DWORD dwSize = ::SizeofResource( hModule, hRes); 
	if( dwSize == 0 )
	{
		assert(FALSE);
		return NULL;
	}

	HGLOBAL hGlobal = ::LoadResource( hModule, hRes); 
	if( hGlobal == NULL )
	{
		assert(FALSE);
		return NULL;
	}

	LPVOID pBuffer = ::LockResource(hGlobal); 
	if( pBuffer == NULL )
	{
		assert(FALSE);
		::FreeResource(hGlobal); 
		return NULL;
	}


	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if(hMem == NULL)
	{
		assert(FALSE);
		::FreeResource(hGlobal); 
		return NULL;
	}

	void* pData = GlobalLock(hMem);
	memcpy(pData, (void *)hGlobal, dwSize);
	GlobalUnlock(hMem);

	Image *pRtImg = NULL;
	IStream* pStream = NULL;
	if(CreateStreamOnHGlobal(hMem, TRUE, &pStream) == S_OK)
	{
		pRtImg = Image::FromStream(pStream, FALSE);
		pStream->Release();
	}

	::FreeResource(hGlobal); 

	return pRtImg;
}
