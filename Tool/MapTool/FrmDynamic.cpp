// ChildFrm.cpp : CFrameBase 클래스의 구현
//
#include "stdafx.h"
#include "FrmDynamic.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CFrameBase

IMPLEMENT_DYNCREATE(CFrameDynamic, CFrameBase)

BEGIN_MESSAGE_MAP(CFrameDynamic, CFrameBase)	
END_MESSAGE_MAP()


// CFrameBase 생성/소멸
CFrameDynamic::CFrameDynamic()
{
}

CFrameDynamic::~CFrameDynamic()
{
	
}

void CFrameDynamic::_BringToTop()
{
	SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE ); //컨트롤 크기 및 위치, 표시 level을 변경할 때 사용하는 함수
				//SWP_NOSIZE = x, y 인수를 무시하고 현재크기 고수  SWP_NOMOVE = x,y 인수 무시하고 현재위치 고수
				//SWP_NOACTIVATE = 윈도우 비활성화
}

