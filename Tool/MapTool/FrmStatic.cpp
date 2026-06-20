// ChildFrm.cpp : CFrameBase 클래스의 구현
//
#include "stdafx.h"
#include "FrmStatic.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CFrameBase

IMPLEMENT_DYNCREATE(CFrameStatic, CFrameBase)

BEGIN_MESSAGE_MAP(CFrameStatic, CFrameBase)
END_MESSAGE_MAP()


// CFrameBase 생성/소멸
CFrameStatic::CFrameStatic()
{
}

CFrameStatic::~CFrameStatic()
{
}



BOOL CFrameStatic::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~ WS_CAPTION;	//표제[caption]가 붙은 윈도우를 만듬(타이틀바)
	return CFrameBase::PreCreateWindow(cs);
}


