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

	// �ͷ����н�����Դ
	void	Release();

	// �ͷŽ�����ԴId���ؽ�����Դ
	Image*	ImageFromResource(UINT uId);

private:
	CImageInfo(void);
	static std::auto_ptr<CImageInfo> sm_inst;

	Image*	FromResource(UINT uId);

	map<UINT, Image*> m_mpImage;
};
