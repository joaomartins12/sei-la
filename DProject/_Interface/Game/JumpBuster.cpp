
#include "stdafx.h"
#include "JumpBuster.h"

#define IF_JUMPBUSTER_DELTA_STRING		18

cJumpBuster::cJumpBuster():m_pZoneScrollBar(NULL),m_pMapScrollBar(NULL),m_pCancelButton(NULL)
,m_pAcceptButton(NULL),m_pMouseOnMask(NULL),m_pSelectMask(NULL)
,m_nMouseOnMaskIndex_Zone(0),m_nSelectMaskIndex_Zone(0),m_bRenderMouseOnMask_Zone(false),m_bRenderSelectMask_Zone(false)
,m_nMouseOnMaskIndex_Map(0),m_nSelectMaskIndex_Map(0),m_bRenderMouseOnMask_Map(false),m_bRenderSelectMask_Map(false)
,m_nJumpBusterItemID(0),m_nItemInvenPos(-1),m_bVipMemberShip(false)
{
}

cJumpBuster::~cJumpBuster()
{

}

void cJumpBuster::Destroy()
{
	cBaseWindow::Delete();
	CsC_AvObject* pTamer = g_pCharMng->GetTamerUser();

	if( pTamer != NULL )
	{
		std::map<int, u4>* p_mapSkill		= g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerSkillMap();
		std::map<int, u4>::iterator it		= p_mapSkill->begin();
		std::map<int, u4>::iterator itEnd	= p_mapSkill->end();

		for( ; it != itEnd; ++it )
		{
			if( it->first != cSkill::JUMPBOOSTER )
				continue;

			CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, it->second );
			if( net::game )
				net::game->SendTamerActiveSkillEnd( pTamerSkill->Get_TSkill_Info()->s_nIndex );

			p_mapSkill->erase( cSkill::JUMPBOOSTER );
			break;
		}
	}
}

void cJumpBuster::DeleteResource()
{
	DeleteScript();
	m_ZoneList.Delete();
	m_MapList.Delete();

	SAFE_NIDELETE( m_pMouseOnMask );
	SAFE_NIDELETE( m_pSelectMask );

	if( g_pDataMng )
	{
		if( m_nItemInvenPos < 0 )
			return;
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nItemInvenPos ) );
	}

	cMessageBox::DelMsg( 30055, false );
}

void cJumpBuster::Create( int nValue /*=0*/ )
{
	m_nItemInvenPos = -1;
	m_nJumpBusterItemID = nValue;

	cBaseWindow::Init();

	InitScript( "JumpBuster\\Win.tga", CsPoint( 150, 150 ) , CsPoint( 467, 388 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( UISTRING_TEXT( "JUMPBUSTER_TITLE_MAP_LIST" ).c_str() );

	m_pCancelButton = AddButton( CsPoint( 445, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	m_pAcceptButton = AddButton( CsPoint( 392, 354 ), CsPoint( 61, 30 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_MOVE" ).c_str() );
	m_pAcceptButton->SetEnable( false );

	m_pZoneScrollBar = AddScrollBar(	cScrollBar::TYPE_1, CsPoint( 205, 53 ), CsPoint( 16, 280 ), cScrollBar::GetDefaultBtnSize(),
										CsRect( CsPoint( 5, 46 ), CsSIZE( 220, 295 ) ), 9 );
	m_pMapScrollBar = AddScrollBar(		cScrollBar::TYPE_1, CsPoint( 444, 53 ), CsPoint( 16, 280 ), cScrollBar::GetDefaultBtnSize(),
										CsRect( CsPoint( 235, 46 ), CsSIZE( 220, 295 ) ), 9 );

	m_pMouseOnMask = NiNew cSprite;
	m_pMouseOnMask->Init( NULL, CsPoint::ZERO, CsPoint( 190, 24 ), "Talk\\Common_Seletbar.tga", false );
	m_pMouseOnMask->SetAlpha( 0.5f );
	m_pSelectMask = NiNew cSprite;
	m_pSelectMask->Init( NULL, CsPoint::ZERO, CsPoint( 190, 24 ), "Talk\\Common_Seletbar.tga", false );

	m_ZoneList.SetBuddy( m_pZoneScrollBar );
	m_MapList.SetBuddy( m_pMapScrollBar );

	// 셀렉트 바
	m_bRenderSelectMask_Zone = m_bRenderMouseOnMask_Zone = false;
	m_bRenderSelectMask_Map = m_bRenderMouseOnMask_Map = false;
	m_nSelectMaskIndex_Zone = m_nMouseOnMaskIndex_Zone = -1;
	m_nSelectMaskIndex_Map = m_nMouseOnMaskIndex_Map = -1;


	_MakeZoneList();
}

bool cJumpBuster::_CheckMapList( CsWorldMap* pZone )
{
	// 내부 리스트에 디스플레이할 리스트가 있는지 검색 한다.
	std::list< CsAreaMap* >* pList = pZone->GetAreaList();
	std::list< CsAreaMap* >::iterator it = pList->begin();
	std::list< CsAreaMap* >::iterator itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		if( g_pDataMng->GetMapRegion()->IsOpenedMap( ( *it )->GetInfo()->s_nMapID ) == false )
			continue;

		if( nsCsFileTable::g_pBaseMng->IsUseJumpBuster( m_nJumpBusterItemID, ( *it )->GetInfo()->s_nMapID ) == true )
			return true;
	}
	return false;
}

void cJumpBuster::_MakeZoneList()
{
	assert_cs( m_ZoneList.GetSize() == 0 );
	assert_cs( m_MapList.GetSize() == 0 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem );

	CsWorldMap::MAP* pWorldMap = nsCsFileTable::g_pWorldMapMng->GetWorldList();
	CsWorldMap::MAP_IT itWorld = pWorldMap->begin();
	CsWorldMap::MAP_IT itEndWorld = pWorldMap->end();
	for( ; itWorld != itEndWorld; ++itWorld )
	{
		// 내부 리스트에 디스플레이할 리스트가 있는지 검색 한다.
		if( _CheckMapList( itWorld->second ) == false )
			continue;

		cString* pString = NiNew cString;
		ti.SetText( itWorld->second->GetInfo()->s_szName );
		pString->AddText( &ti );
		pString->SetValue1( 0 );
		pString->SetValue2( m_ZoneList.GetSize() );
		pString->SetValuePointer1( itWorld->second );

		m_ZoneList.AddTail( pString );
	}

	m_pZoneScrollBar->SetRange( CsPoint( 0, m_ZoneList.GetSize() ) );
}

void cJumpBuster::_MakeMapList( CsWorldMap* pZone )
{
	m_pAcceptButton->SetEnable( false );
	m_MapList.Delete();

	m_bRenderSelectMask_Map = m_bRenderMouseOnMask_Map = false;
	m_nSelectMaskIndex_Map = m_nMouseOnMaskIndex_Map = -1;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem );

	std::list< CsAreaMap* >* pList = pZone->GetAreaList();
	std::list< CsAreaMap* >::iterator it = pList->begin();
	std::list< CsAreaMap* >::iterator itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		if( g_pDataMng->GetMapRegion()->IsOpenedMap( ( *it )->GetInfo()->s_nMapID ) == false )
			continue;

		DWORD dwMapID = ( *it )->GetInfo()->s_nMapID;
		if( nsCsFileTable::g_pBaseMng->IsUseJumpBuster( m_nJumpBusterItemID, dwMapID ) == false )
			continue;			

		cString* pString = NiNew cString;
		ti.SetText( (*it)->GetInfo()->s_szName );
		pString->AddText( &ti );
		pString->SetValue1( dwMapID );
		pString->SetValue2( m_MapList.GetSize() );
		pString->SetValuePointer1( (*it) );

		m_MapList.AddTail( pString );
	}

	m_pMapScrollBar->SetRange( CsPoint( 0, m_MapList.GetSize() ) );
}

void cJumpBuster::Update(float const& fDeltaTime)
{
	// 전투중 종료
	if( g_pCharMng->GetDigimonUser( 0 )->IsBattle() == true )
	{
		Close( false );
		return;
	}
	// 엔피씨 이용중 종료
	if( g_pGameIF->IsOpenNpcWindow() == true )
	{
		Close( false );
		return;
	}

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cJumpBuster::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pAcceptButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			DWORD dwDestMapID = m_MapList.GetString( m_nSelectMaskIndex_Map )->GetValue1();
			if( dwDestMapID != nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )
			{
				std::wstring mapDiscrip = nsCsMapTable::g_pMapListMng->GetList( dwDestMapID )->GetInfo()->s_szMapDiscript;
				if( m_nJumpBusterItemID == 9801 && !m_bVipMemberShip )
				{
					cPrintMsg::PrintMsg( 30544, const_cast<TCHAR*>(mapDiscrip.c_str()) );
					cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
					pMBox->SetValue1( m_nItemInvenPos );
					pMBox->SetValue2( dwDestMapID );

					return muReturn;
				}

				cPrintMsg::PrintMsg( 30055, const_cast<TCHAR*>(mapDiscrip.c_str()));
				cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
				if( m_bVipMemberShip )
					pMBox->SetValue1( -1 );
				else
					pMBox->SetValue1( m_nItemInvenPos );
				pMBox->SetValue2( dwDestMapID );
			}
			else
			{
				if( m_nJumpBusterItemID == 9801 && !m_bVipMemberShip )
				{
					cPrintMsg::PrintMsg( 30545 );
					return muReturn;
				}
				cPrintMsg::PrintMsg( 30056 );
			}
		}
		return muReturn;
	default:
		return muReturn;
	}

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	m_pZoneScrollBar->Update_ForMouse();
	m_pMapScrollBar->Update_ForMouse();

	//마우스오버, 선택 마스크 및 스트링리스트 마우스업데이트 위치 셋팅
	int nArea_X = 9;
	int nZone_X = 243;
	int nMaskStartPosY = 56;

	int nActive1;
	CsPoint ptMouseOnMask = CsPoint( nArea_X, nMaskStartPosY ) + GetRootClient();
	switch( m_ZoneList.Update_ForMouse( nActive1, m_nMouseOnMaskIndex_Zone, ptMouseOnMask, IF_JUMPBUSTER_DELTA_STRING, CsPoint( 190, 25 ) ) )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		break;	
	case CURSOR::LBUTTON_UP:
		if( m_nSelectMaskIndex_Zone != m_nMouseOnMaskIndex_Zone )
		{
			_MakeMapList( (CsWorldMap*)m_ZoneList.GetString( m_nMouseOnMaskIndex_Zone )->GetValuePointer1() );
		}
		m_nSelectMaskIndex_Zone = m_nMouseOnMaskIndex_Zone;
		m_bRenderSelectMask_Zone = true;
		m_bRenderMouseOnMask_Zone = true;
		break;
	default:
		m_bRenderMouseOnMask_Zone = true;
		break;
	}

	ptMouseOnMask = CsPoint( nZone_X, nMaskStartPosY ) + GetRootClient();
	switch( m_MapList.Update_ForMouse( nActive1, m_nMouseOnMaskIndex_Map, ptMouseOnMask, IF_JUMPBUSTER_DELTA_STRING, CsPoint( 190, 25 ) ) )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		break;	
	case CURSOR::LBUTTON_UP:
		if( m_nSelectMaskIndex_Map != m_nMouseOnMaskIndex_Map )
		{
			m_pAcceptButton->SetEnable( true );			
		}
		m_nSelectMaskIndex_Map = m_nMouseOnMaskIndex_Map;
		m_bRenderSelectMask_Map = true;
		m_bRenderMouseOnMask_Map = true;
		break;
	default:
		m_bRenderMouseOnMask_Map = true;
		break;
	}

	return muReturn;
}

void cJumpBuster::Render()
{
	RenderScript();

	//마우스오버, 선택 마스크 및 스트링리스트 랜더링 위치 셋팅
	int nArea_X = 9;
	int nZone_X = 243;
	int nMaskStartPosY = 56;
	int nListStartPosY = nMaskStartPosY + 3;

	if( m_bRenderMouseOnMask_Zone )
	{
		int nPosY = m_ZoneList.GetStringPosY( nMaskStartPosY, m_nMouseOnMaskIndex_Zone, IF_JUMPBUSTER_DELTA_STRING );
		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( nArea_X, nPosY ) + GetRootClient();
			m_pMouseOnMask->Render( pos );
			m_bRenderMouseOnMask_Zone = false;
		}		
	}
	if( m_bRenderSelectMask_Zone )
	{
		int nPosY = m_ZoneList.GetStringPosY( nMaskStartPosY, m_nSelectMaskIndex_Zone, IF_JUMPBUSTER_DELTA_STRING );
		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( nArea_X, nPosY ) + GetRootClient();
			m_pSelectMask->Render( pos );
		}
	}
	if( m_bRenderMouseOnMask_Map )
	{
		int nPosY = m_MapList.GetStringPosY( nMaskStartPosY, m_nMouseOnMaskIndex_Map, IF_JUMPBUSTER_DELTA_STRING );
		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( nZone_X, nPosY ) + GetRootClient();
			m_pMouseOnMask->Render( pos );
			m_bRenderMouseOnMask_Map = false;
		}		
	}
	if( m_bRenderSelectMask_Map )
	{
		int nPosY = m_MapList.GetStringPosY( nMaskStartPosY, m_nSelectMaskIndex_Map, IF_JUMPBUSTER_DELTA_STRING );
		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( nZone_X, nPosY ) + GetRootClient();
			m_pSelectMask->Render( pos );
		}
	}

	m_ZoneList.Render( CsPoint( nArea_X + 5, nListStartPosY ) + GetRootClient(), IF_JUMPBUSTER_DELTA_STRING );
	m_MapList.Render( CsPoint( nZone_X + 5, nListStartPosY ) + GetRootClient(), IF_JUMPBUSTER_DELTA_STRING );
}

void cJumpBuster::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	m_ZoneList.ResetDevice();
	m_MapList.ResetDevice();
}

void cJumpBuster::ResetDevice()
{	
	ResetDeviceScript();

	m_ZoneList.ResetDevice();
	m_MapList.ResetDevice();
}

void cJumpBuster::SetJumpBusterItemInvenPos( int nInvenPos )
{
	m_nItemInvenPos = nInvenPos;
	g_pDataMng->ItemLock( TO_INVEN_SID( m_nItemInvenPos ) );
}

void cJumpBuster::SetVipMemberShip(bool bValue)
{
	m_bVipMemberShip = bValue;
}