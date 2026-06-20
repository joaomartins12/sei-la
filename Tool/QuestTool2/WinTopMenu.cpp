

// WinTopMenu 메시지 처리기입니다.
// WinTopMenu.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WinTopMenu.h"


WinTopMenu*		g_pTopMenu = NULL;

// WinTopMenu

IMPLEMENT_DYNCREATE(WinTopMenu, CFormView)

WinTopMenu::WinTopMenu()
: CFormView(WinTopMenu::IDD)
{

}

WinTopMenu::~WinTopMenu()
{
}


void WinTopMenu::_Init()
{

}


void WinTopMenu::OnBnClickedButtonSave()
{	
	nsCsFileTable::g_pQuestMng->SaveExcel( g_pTop->_GetExpMul() );
}

void WinTopMenu::OnBnClickedButtonLoadTable()
{
	char cPath[ MAX_PATH ];


#ifdef VERSION_USA	//GSP 버전은 고정으로 영문버전
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::ENGLISH, cPath );
#elif VERSION_TW	// 대만
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::TAIWAN, cPath );
#elif VERSION_HK	// 홍콩
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::HONGKONG, cPath );
#elif VERSION_TH
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::THAILAND, cPath );
#else
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::KOREA_TRANSLATION, cPath );
#endif


	nsCsFileTable::g_pItemMng->Delete();
	nsCsFileTable::g_pItemMng->Init( cPath );

	nsCsFileTable::g_pDigimonMng->Delete();
	nsCsFileTable::g_pDigimonMng->Init( cPath );

	nsCsFileTable::g_pMonsterMng->Delete();
	nsCsFileTable::g_pMonsterMng->Init( cPath );

	nsCsFileTable::g_pNpcMng->Delete();
	nsCsFileTable::g_pNpcMng->Init( cPath );

	nsCsMapTable::g_pMapListMng->Delete();
	nsCsMapTable::g_pMapListMng->Init( cPath );

	nsCsMapTable::g_pMapRegionMng->Delete();
	nsCsMapTable::g_pMapRegionMng->Init( cPath );

	CsMessageBox( MB_OK, L"아이템, 디지몬, 몬스터, 엔피씨, 맵리스트, 맵리젼\n재 로드 완료~!!" );
}





void WinTopMenu::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(WinTopMenu, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &WinTopMenu::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_TABLE, &WinTopMenu::OnBnClickedButtonLoadTable)
END_MESSAGE_MAP()

