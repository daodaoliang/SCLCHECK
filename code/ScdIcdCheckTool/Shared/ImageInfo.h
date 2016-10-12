/** 
  Copyright (C), 2011-2016, Roof Tech. Co., Ltd. 
  * File name:      ImageInfo.h
  * Author:         zhang-xch
  * Version:        V1.0
  * Date:           2013/12/25
  * Description:   
  */ 
#pragma once
#include <map>
#include <memory>
using namespace std;

class CImageInfo
{
public:
	static CImageInfo * Instance();

	~CImageInfo(void);

	// 释放所有界面资源
	void	Release();

	// 释放界面资源Id加载界面资源
	Image*	ImageFromResource(UINT uId);

private:
	CImageInfo(void);
	static std::auto_ptr<CImageInfo> sm_inst;

	Image*	FromResource(UINT uId);

	map<UINT, Image*> m_mpImage;
};
