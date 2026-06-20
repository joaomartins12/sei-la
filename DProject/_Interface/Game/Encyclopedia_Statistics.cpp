#include "stdafx.h"
#include "Encyclopedia_Statistics.h"

cEncyclopedia_Statistics::cEncyclopedia_Statistics()
{

}

cEncyclopedia_Statistics::~cEncyclopedia_Statistics()
{

}

bool cEncyclopedia_Statistics::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	return true;
}

void cEncyclopedia_Statistics::Destroy()
{
	cBaseWindow::Delete();	
}

void cEncyclopedia_Statistics::DeleteResource()
{
	DeleteScript();
}

void cEncyclopedia_Statistics::Create( int nValue /*= 0 */ )
{
	cBaseWindow::Init();

	CsPoint ptEncyPos = g_pGameIF->GetEncyclopedia()->GetRootClient();

	int nPosX = ptEncyPos.x + 383;
	int nPosY = ptEncyPos.y + 492 - 166;

#ifdef VERSION_USA
	CsPoint ptSize = CsPoint( 240, 166 );
	int nCenterX = 120;
	int nDeltaX = 25;
#elif VERSION_TH
	CsPoint ptSize = CsPoint( 240, 166 );
	int nCenterX = 120;
	int nDeltaX = 25;
#else
	CsPoint ptSize = CsPoint( 145, 166 );
	int nCenterX = 72;
	int nDeltaX = 8;
#endif

	InitScript( "Encyclopedia\\Encyclopedia_StatBG.tga", CsPoint( nPosX, nPosY ) , ptSize, true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );

	// 텍스트
	cText::sTEXTINFO ti;
	ti.Init();

	// 통계 타이틀
	ti.s_eFontSize = CFont::FS_12;
	ti.s_eTextAlign = DT_CENTER;

	ti.SetText( UISTRING_TEXT( "ENCYCLOPEDIA_DIGIMON_ENCYCLOPEDIA_STATISTICS" ).c_str() );
	AddText( &ti, CsPoint( nCenterX, 12 ) );
	
	ti.s_eFontSize = CFont::FS_10;
	ti.s_eTextAlign = DT_RIGHT;

	// 디지몬(계열체) 보유 수
	std::wstring wsText;
	DmCS::StringFn::Format( wsText, L"%s :", UISTRING_TEXT( "ENCYCLOPEDIA_DIGIMON_OWNED" ).c_str() );
	ti.SetText( wsText.c_str() );
	AddText( &ti, CsPoint( nCenterX + nDeltaX, 40 ) );	

	// 도감 달성률
	DmCS::StringFn::Format( wsText, L"%s :", UISTRING_TEXT( "ENCYCLOPEDIA_ACHIEVEMENT_RATE" ).c_str() );
	ti.SetText( wsText.c_str() );
	AddText( &ti, CsPoint( nCenterX + nDeltaX, 60 ) );

	// 디지몬 평균 레벨
	DmCS::StringFn::Format( wsText, L"%s :", UISTRING_TEXT( "ENCYCLOPEDIA_AVERAGE_LEVEL" ).c_str() );
	ti.SetText( wsText.c_str() );
	AddText( &ti, CsPoint( nCenterX + nDeltaX, 80 ) );

	// 디지몬 레벨 총 합
	DmCS::StringFn::Format( wsText, L"%s :", UISTRING_TEXT( "ENCYCLOPEDIA_TOTAL_LEVEL" ).c_str() );
	ti.SetText( wsText.c_str() );
	AddText( &ti, CsPoint( nCenterX + nDeltaX, 100 ) );

	// 디지몬 평균 크기
	DmCS::StringFn::Format( wsText, L"%s :", UISTRING_TEXT( "ENCYCLOPEDIA_AVERAGE_SCALE" ).c_str() );
	ti.SetText( wsText.c_str() );
	AddText( &ti, CsPoint( nCenterX + nDeltaX, 120 ) );

	// 획득 별 수
	DmCS::StringFn::Format( wsText, L"%s :", UISTRING_TEXT( "ENCYCLOPEDIA_NUMBER_OF_STAR" ).c_str() );
	ti.SetText( wsText.c_str() );
	AddText( &ti, CsPoint( nCenterX + nDeltaX, 140 ) );

	ti.s_eTextAlign = DT_LEFT;

	// 디지몬(계열체) 보유 수
	m_szDigimon_Cnt = AddText( &ti, CsPoint( nCenterX + nDeltaX, 40 ) );

	// 도감 달성률
	m_szComp_Rate = AddText( &ti, CsPoint( nCenterX + nDeltaX, 60 ) );

	// 디지몬 평균 레벨
	m_szAvrage_Lv = AddText( &ti, CsPoint( nCenterX + nDeltaX, 80 ) );
	
	// 디지몬 레벨 총 합
	m_szSum_Lv = AddText( &ti, CsPoint( nCenterX + nDeltaX, 100 ) );
	
	// 디지몬 평균 크기
	m_szAvrage_Scale = AddText( &ti, CsPoint( nCenterX + nDeltaX, 120 ) );

	// 획득 별 수
	m_szStar_Cnt = AddText( &ti, CsPoint( nCenterX + nDeltaX, 140 ) );
	
	_SetText_Stat();
}

void cEncyclopedia_Statistics::_SetText_Stat()
{
	EncyclopediaContents::MAP_CIT it    = GetSystem()->GetEncyMapData().begin();
	EncyclopediaContents::MAP_CIT itEnd = GetSystem()->GetEncyMapData().end();

	int nOpenCnt = 0;			// 계열체 오픈 수
	int nAvrageLv = 0;			// 평균 레벨
	int nSumLv = 0;				// 총합레벨
	int nAvrageScale = 0;		// 평균 크기
	int nStarCnt = 0;			// 획득 별 수
	int nCompRate = 0;			// 달성률

	int nMyDigimonCnt = 0;		//보유하고 있는 디지몬 수
	int nCnt = 0;				//계열체 카운트, 평균 계산용
	for( ; it != itEnd ; it++ )
	{
		if( nsCsFileTable::g_pDigimonMng->_IsExceptionDigimon( it->first ) == false )	// 제외 디지몬인지 확인
		{
			//계열체별로
			if( it->second->s_nLevel != 0 )
			{
				nSumLv += it->second->s_nLevel;				// 레벨 더해주고
				nAvrageScale += it->second->s_nSize;		// 크기 더해줌
				nStarCnt += it->second->s_nEnchant + 1;		//별 획득 수, 인덱스라 0부터 있으니 +1해줌

				nMyDigimonCnt++;							//보유하고 있는 디지몬 수
			}
			//해당 계열체 모두 오픈했는지 확인
			if( it->second->s_bIsAllOpen )
				nOpenCnt++;//모두 오픈 상태면 보유 디지몬 계열체 수 ++

			nCnt++;	//총 디지몬 계열체 수 ++
		}
	}

	//디지몬 계열체 보유 수
	std::wstring wsText;
	DmCS::StringFn::Format( wsText, L"%d %s", nOpenCnt, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );
	m_szDigimon_Cnt->SetText( wsText.c_str() );

	// 달성률
	float fRate = (float)nOpenCnt / (float)nCnt;
	DmCS::StringFn::Format( wsText, L"%d %%", (int)( fRate * 100 ) );
	m_szComp_Rate->SetText( wsText.c_str() );

	// 디지몬 레벨 총 합
	DmCS::StringFn::Format( wsText, L"%d Lv", nSumLv );
	m_szSum_Lv->SetText( wsText.c_str() );

	// 디지몬 평균 레벨
	if( nSumLv <= 0 )
		wsText = L"0 Lv";
	else
		DmCS::StringFn::Format( wsText, L"%d Lv", nSumLv / nMyDigimonCnt );
	m_szAvrage_Lv->SetText( wsText.c_str() );

	// 디지몬 평균 크기
	if( nAvrageScale <= 0 )
		wsText = L"0 %";
	else
		DmCS::StringFn::Format( wsText, L"%d %%", nAvrageScale / nMyDigimonCnt );
	m_szAvrage_Scale->SetText( wsText.c_str() );

	// 획득 별 수
	std::wstring wsCount;
	DmCS::StringFn::Format( wsText, L"%d %s", nStarCnt, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );
	m_szStar_Cnt->SetText( wsText.c_str() );
}

void cEncyclopedia_Statistics::Update(float const& fDeltaTime)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCYCLOPEDIA ) == false )
		Close();	//도감 닫히면 같이 닫아줌

	//윈도우 이동 업데이트
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cEncyclopedia_Statistics::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 145, 166 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cEncyclopedia_Statistics::Render()
{
	RenderScript();
	EndRenderScript();
}

void cEncyclopedia_Statistics::ResetDevice()
{
	ResetDeviceScript();
}

void cEncyclopedia_Statistics::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}