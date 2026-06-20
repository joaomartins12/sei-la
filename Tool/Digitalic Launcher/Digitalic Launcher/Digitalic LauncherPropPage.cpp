// Digitalic LauncherPropPage.cpp : CDigitalicLauncherPropPage 속성 페이지 클래스의 구현입니다.

#include "stdafx.h"
#include "Digitalic Launcher.h"
#include "Digitalic LauncherPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CDigitalicLauncherPropPage, COlePropertyPage)



// 메시지 맵입니다.

BEGIN_MESSAGE_MAP(CDigitalicLauncherPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// 클래스 팩터리와 GUID를 초기화합니다.

IMPLEMENT_OLECREATE_EX(CDigitalicLauncherPropPage, "DIGITALICLAUNC.DigitalicLauncPropPage.1",
	0x3f408140, 0x44fa, 0x4846, 0xb8, 0x2b, 0x19, 0x2c, 0xc4, 0x8, 0xb4, 0x7b)



// CDigitalicLauncherPropPage::CDigitalicLauncherPropPageFactory::UpdateRegistry -
// CDigitalicLauncherPropPage에서 시스템 레지스트리 항목을 추가하거나 제거합니다.

BOOL CDigitalicLauncherPropPage::CDigitalicLauncherPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_DIGITALICLAUNCHER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CDigitalicLauncherPropPage::CDigitalicLauncherPropPage - 생성자입니다.

CDigitalicLauncherPropPage::CDigitalicLauncherPropPage() :
	COlePropertyPage(IDD, IDS_DIGITALICLAUNCHER_PPG_CAPTION)
{
}



// CDigitalicLauncherPropPage::DoDataExchange - 페이지와 속성 사이에서 데이터를 이동시킵니다.

void CDigitalicLauncherPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
}



// CDigitalicLauncherPropPage 메시지 처리기입니다.
