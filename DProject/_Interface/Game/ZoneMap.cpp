
#include "stdafx.h"

#include "ZoneMap.h"

#define IF_ZONEMAP_TEX_SIZE		512.0f

#define RADER_SIZE	CsPoint( 120, 120 )
cZoneMap::cZoneMap() : m_pRegionIcon(NULL), m_RegionName(NULL), m_RegionShowText(NULL), m_CheckRegionShow(NULL), m_pZoneMap(NULL), m_bDungeon(false)
{
	m_Time.SetDeltaTime( 1200 );
	m_fRot = 0.0f;	
}

cZoneMap::~cZoneMap()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cZoneMap::Destroy()
{
	cBaseWindow::Delete();	
}

bool cZoneMap::Close( bool bSound /* = true  */ )
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}

	m_nRadioIndex = eType_Nor;
	DeleteResource();
	SetShowWindow( false );

	return true;
}

void cZoneMap::DeleteResource()
{
	DeleteScript();
	SAFE_NIDELETE( m_pMyZoneMask );

	SAFE_NIDELETE( m_pArrow );	
#ifdef ZONEMAP_CLICK_MOVE
	SAFE_NIDELETE( m_pArrivePoint );	
#endif
	SAFE_NIDELETE( m_pHelperMove );
	SAFE_NIDELETE( m_pRadioBtn );	
	SAFE_NIDELETE( m_pRadar );
	SAFE_NIDELETE( m_pRadarBack );	
	SAFE_NIDELETE( m_pCircle );
	SAFE_NIDELETE( m_pRegionIcon );
	
	SAFE_NIDELETE( m_RegionName );
	SAFE_NIDELETE( m_RegionShowText );	
	SAFE_NIDELETE( m_CheckRegionShow );
	
	SAFE_NIDELETE( m_pBtnBack );
	SAFE_NIDELETE( m_pExtendBtn );

	_DeleteZone();
	_DeleteArea();
	_DeleteWorld();
	_DeleteMainMap();
	_DeleteHelpMap();

	_DeleteMapInfo();
	_DeleteNpcInfo();		
}

void cZoneMap::_DeleteNpcInfo()
{
	std::list< cMiniMap::sNPC_INFO* >::iterator it = m_listNpcInfo.begin();
	std::list< cMiniMap::sNPC_INFO* >::iterator itEnd = m_listNpcInfo.end();
	for( ; it!=itEnd; ++it )
	{
		cMiniMap::sNPC_INFO::DeleteInstance( *it );
	}
	m_listNpcInfo.clear();
}

void cZoneMap::ResetMap()
{
	Close( false );
}

void cZoneMap::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() )
	{
		m_nRadioIndex = eType_Nor;
	}

	InitScript( NULL, _GetCenterZonePos(), CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE + 34 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );

	//m_pZoneMap = AddSprite(CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), NULL);

	m_pRadar = NULL;
	m_pRadarBack = NULL;
	m_pHelperMove = NULL;	
	m_pCircle = NULL;
	m_pTranslate = NULL;

	m_pCancelButton = AddButton( CsPoint( 472, 13 ), CsPoint( 24, 24 ), CsPoint( 0, 19 ), "QuestRev\\Quest_X_But.tga" );
	if( m_pCancelButton )
		m_pCancelButton->SetControlName( "CloseBtn" );
	m_pTranslate = AddButton( CsPoint( 438, 13 ), CsPoint( 24, 24 ), CsPoint( 0, 20 ), "System\\Translate.tga" );

	m_pBtnBack = NiNew cButton;
	m_pBtnBack->Init( m_pRoot, CsPoint( 13, 13 ), CsPoint( 26, 27 ), "QuestRev\\System_back.tga", false );
	m_pBtnBack->SetTexToken( CsPoint( 0, 22 ) );
	//AddChildControl(m_pBtnBack);

	m_pHelperMove = NiNew cButton;
	m_pHelperMove->Init( m_pRoot, CsPoint( 408, 518 ), CsPoint( 98, 23 ), cButton::IMAGE_NOR_3, UISTRING_TEXT( "ZONEMAP_BTN_JUMP BOOSTER" ).c_str(), cWindow::SD_Ba1 );	
	//AddChildControl(m_pHelperMove);

	m_pExtendBtn = NiNew cSprite;
	m_pExtendBtn->Init( m_pRoot, CsPoint( 0, (int)IF_ZONEMAP_TEX_SIZE + 1 ), CsPoint( (int)IF_ZONEMAP_TEX_SIZE, 33 ), "Map\\_ExpendWindow.tga", false );
	m_pExtendBtn->GetTexProp()->SetApplyMode( NiTexturingProperty::APPLY_REPLACE );
	//AddChildControl(m_pExtendBtn);
		
	m_pRadioBtn = NiNew cRadioButton;
	m_pRadioBtn->Init( m_pRoot, CsPoint( 10, 519 ), CsPoint( 70, 23 ), cCheckBox::IMAGE_RADIO_3, UISTRING_TEXT( "COMMON_TXT_NORMAL" ).c_str(), false );
	m_pRadioBtn->AddNode( CsPoint( 90, 519 ), CsPoint( 70, 23 ), cCheckBox::IMAGE_RADIO_3, UISTRING_TEXT( "COMMON_TXT_NPC" ).c_str() );
	if( m_pRadioBtn->GetNode( 1 ) )
		m_pRadioBtn->GetNode( 1 )->SetControlName( "FindNpcBtn" );
	m_pRadioBtn->AddNode( CsPoint( 170, 519 ), CsPoint( 70, 23 ), cCheckBox::IMAGE_RADIO_3, UISTRING_TEXT( "COMMON_TXT_MONSTER" ).c_str() );
	if( m_pRadioBtn->GetNode( 2 ) )
		m_pRadioBtn->GetNode( 2 )->SetControlName( "FindMonsterBtn" );
	m_pRadioBtn->AddNode( CsPoint( 250, 519 ), CsPoint( 70, 23 ), cCheckBox::IMAGE_RADIO_3, UISTRING_TEXT( "ZONEMAP_BTN_FIND" ).c_str() );
	//AddChildControl(m_pRadioBtn);

	m_pRadioBtn->AddNode( CsPoint( 330, 519 ), CsPoint( 70, 23 ), cCheckBox::IMAGE_RADIO_3, UISTRING_TEXT( "ZONEMAP_BTN_FIND_SHOP" ).c_str() );
	m_pRadioBtn->GetNode( eType_Pos )->SetEnable( false );
	m_pRadioBtn->GetNode( eType_Employment )->SetEnable( false );

	m_nLoadZoneMapID = m_nLoadAreaMapID = m_nLoadWorldMapID = -1;
	m_eState = CUR_ZONE;
	m_pZoneMap = NULL;
	m_pAreaMap = NULL;
	m_pWorldMap = NULL;
	m_pMainMap = NULL;
	m_pImage = NULL;
	m_pScroll = NULL;
	m_fRot = NI_PI;	

	m_pMyZoneMask = NiNew cSprite;
	m_pMyZoneMask->Init( m_pRoot, CsPoint::ZERO, CsPoint( 91, 79 ), "Map\\WorldMap\\Mask_MyZone.tga", false );
	//AddChildControl(m_pMyZoneMask);

	m_pArrow = NiNew cSprite;
	m_pArrow->Init( m_pRoot, CsPoint::ZERO, CsPoint( 32, 32 ), "MiniMap\\Arrow_01.tga", false );
	//AddChildControl(m_pArrow);

#ifdef ZONEMAP_CLICK_MOVE
	m_pArrivePoint = NiNew cSprite;	
	m_pArrivePoint->Init( m_pRoot, CsPoint(20,20), CsPoint( 16, 16 ), "Map\\SmallMap\\TamerPoint.tga", false );
	m_pArrivePoint->SetVisible(false);
	//AddChildControl(m_pArrivePoint);

	// 이동 중일 때 존맵이 열려있으면 도착 지점을 표시해줍니다.
	if( g_pCharMng->GetTamerUser()->GetProp_Path()->IsMove() )
	{
		NiPoint2 vArrivePos = g_pCharMng->GetTamerUser()->GetProp_Path()->GetArrivePos();
		CsPoint vArrivePoint = _CalZoneMapPos( NiPoint2(vArrivePos.x, vArrivePos.y), CsPoint::ZERO );
		vArrivePoint -= GetRootClient();
		m_pArrivePoint->SetVisible(true);
		m_pArrivePoint->SetPos( vArrivePoint - CsPoint(7,16) );
	}
#endif

	m_pRadar = NiNew cSprite;
	m_pRadar->Init( NULL, CsPoint::ZERO, CsPoint::ZERO, "Serch\\npcmark_01.tga", false );						
	m_pRadar->GetAlphaProp()->SetDestBlendMode( NiAlphaProperty::ALPHA_ONE );
	//AddChildControl(m_pRadar);

	m_pRadarBack = NiNew cSprite;
	m_pRadarBack->Init( NULL, CsPoint::ZERO, CsPoint::ZERO, "Serch\\npcmark_02.tga", false );		
	m_pRadarBack->GetAlphaProp()->SetDestBlendMode( NiAlphaProperty::ALPHA_ONE );
	//AddChildControl(m_pRadarBack);

	m_pCircle = NiNew cSprite;
	m_pCircle->Init( NULL, CsPoint::ZERO, CsPoint( 36, 36 ), "TargetWindow\\TacticsTarget.tga", false );
	//AddChildControl(m_pCircle);

	if( g_pHelpArrow->IsArrowRender() == false )
	{		
		_LoadZone();
		_Update_TamerPos();
	}
	else
	{
		m_Time.Reset();
		m_nRadioIndex = eType_Pos;
		_LoadZone( g_pHelpArrow->GetInfo()->s_dwMapID );
		_Update_TamerPos();		
	}

#ifdef MAP_REGION_SHOW

	cText::sTEXTINFO ti;		
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_9, FONT_GREEN );					
	ti.s_bOutLine = false;		
	ti.s_eTextAlign = DT_CENTER;

	m_RegionName = NiNew cText;
	m_RegionName->Init(NULL, CsPoint::ZERO, &ti, false);

	m_pRegionIcon = NiNew cSprite;
	m_pRegionIcon->Init( NULL, CsPoint::ZERO, CsPoint( 10, 10 ), "Map\\Map_Region.tga", false );

	// 지역명 보이기 / 숨기기
	m_RegionShowText = NiNew cText;
	m_RegionShowText->Init(m_pRoot, CsPoint::ZERO, &ti, false);
	m_RegionShowText->SetText( UISTRING_TEXT( "ZONEMAP_AREA_NAME_ON_OFF" ).c_str() );
	m_RegionShowText->SetPos( CsPoint(IF_ZONEMAP_TEX_SIZE/2, 490) );
	m_RegionShowText->SetColor(FONT_YELLOW);
	//AddChildControl(m_RegionShowText);

	// 체크 박스
	m_CheckRegionShow = NiNew cCheckBox;
	m_CheckRegionShow->Init( m_pRoot, CsPoint( IF_ZONEMAP_TEX_SIZE/2+m_RegionShowText->GetStringSize().x/2+10, 490 ), CsPoint( 14, 14 ), "System\\Check.tga", false );
	m_CheckRegionShow->SetTexToken( CsPoint( 0, 20 ) );
	m_CheckRegionShow->SetCheck(GetSystem()->IsRegionShow());
	//AddChildControl(m_CheckRegionShow);

#endif
}

void cZoneMap::_CalValue( DWORD dwMapID )
{
	CsMapList::sINFO* pMapInfo = nsCsMapTable::g_pMapListMng->GetList( dwMapID )->GetInfo();
	
	m_bHeightHigh = ( pMapInfo->s_nHeight >= pMapInfo->s_nWidth );

	if( m_bHeightHigh )
	{
		m_fHalfDeltaSize = ( pMapInfo->s_nHeight - pMapInfo->s_nWidth )*0.5f;
		m_fMaxSize = (float)pMapInfo->s_nHeight;
	}
	else
	{
		m_fHalfDeltaSize = ( pMapInfo->s_nWidth - pMapInfo->s_nHeight )*0.5f;
		m_fMaxSize = (float)pMapInfo->s_nWidth;
	}

	// 필요한값 계산	
	m_fAspectiveHeight = (float)pMapInfo->s_nHeight / (float)pMapInfo->s_nWidth;

	if( (int)dwMapID == 105 )	// 요코하마 마을Renewal Map
		m_Meter_To_Pixel = IF_ZONEMAP_TEX_SIZE / 55919;
	else
		m_Meter_To_Pixel = IF_ZONEMAP_TEX_SIZE / m_fMaxSize;
}

void cZoneMap::_UpperMapLevel()
{
	switch( m_eState )
	{
	case CUR_ZONE:
	case OTHER_ZONE:		
		{	
			assert_cs( m_nLoadZoneMapID != -1 );
			int nAreaType = nsCsFileTable::g_pWorldMapMng->GetArea( m_nLoadZoneMapID )->GetInfo()->s_nAreaType;
			_LoadArea( nAreaType );
		}
		break;
	case AREAMAP:
		{			
			_LoadWorld( nsCsFileTable::g_pWorldMapMng->GetWorld( m_nLoadAreaMapID )->GetInfo()->s_nWorldType );
		}
		break;
	case WORLDMAP:
		{
			_LoadMainMap();
		}
		break;
	case MAINMAP:
		break;
	default:
		assert_cs( false );
	}
}

void cZoneMap::Update(float const& fDeltaTime)
{
	// 전투중 종료
	if( m_pHelperMove )
	{
		m_pHelperMove->SetEnable( true );
		// 전투중
		if( g_pCharMng->GetDigimonUser( 0 )->IsBattle() == true )
			m_pHelperMove->SetEnable( false );
		// 엔피씨 이용중 , 거래중
		else if( g_pGameIF->IsOpenNpcWindow() == true )
			m_pHelperMove->SetEnable( false );

		//else if( m_pHelperMove->IsEnable() == true && IsOpenMap() == false )
		else if( IsOpenMap() == false )
			m_pHelperMove->SetEnable( false );
	}	

	switch( m_eState )
	{
	case CUR_ZONE:
		_Update_TamerPos();	
		break;
	}	
	switch( m_nRadioIndex )
	{
	case eType_Nor:
	case eType_Mon:
	case eType_Npc:
		break;

	case eType_Pos:
	case eType_Employment:
		_Update_Helper();
		break;

	default:
		assert_cs( false );
	}

	//_UpdateMoveWindow();
}

void cZoneMap::_Update_TamerPos()
{
	CsPoint vRenderPos = _CalZoneMapPos( g_pCharMng->GetTamerUser()->GetPos2D(), CsPoint::ZERO );
	vRenderPos -= GetRootClient();
	m_pArrow->SetPos( vRenderPos );

	// 카메라 방향
	NiPoint3 vCameraDir = CAMERA_ST.GetWorldDirection();
	vCameraDir.z = 0.0f;
	vCameraDir.Unitize();
	float fRot = NiACos( NiPoint3( 0.0f, -1.0f, 0.0f ).Dot( vCameraDir ) );
	if( NiPoint3( 0.0f, -1.0f, 0.0f ).Cross( vCameraDir ).z > 0 )
		fRot = -fRot;
	m_pArrow->SetRotation( fRot, CsPoint( 16, 16 ) );

#ifdef ZONEMAP_CLICK_MOVE
	if( g_pCharMng->GetTamerUser()->GetProp_Path()->IsMove() )
	{
		m_pArrivePoint->SetVisible(true);
		_UpdatePath();
	}

	else
	{
		m_PathList.clear();
		m_pArrivePoint->SetVisible(false);
	}
#endif
}


bool cZoneMap::IsOpenMap()
{
	cData_MapRegion* pRegionMap = g_pDataMng->GetMapRegion();
	SAFE_POINTER_RETVAL( pRegionMap, false );

	if( m_nLoadZoneMapID == -1 )
		return false;
	if( pRegionMap->IsOpenedMap( m_nLoadZoneMapID ) == false )
		return false;

	return true;
}
bool cZoneMap::IsMonster(int nMapID)
{
	if( nMapID == -1 )	
		nMapID = (int)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;					

	CsVectorPB< DWORD > vpInsert;
	CsMapMonsterGroup* pFtMonGroup = nsCsMapTable::g_pMapMonsterMng->GetGroup( nMapID );
	
	SAFE_POINTER_RETVAL(pFtMonGroup, false);

	// 몬스터가 없거나.
	if( pFtMonGroup->GetMonsterList()->size() == 0 )
		return false;

	// 표시할 몬스터가 없는 경우.
	CsMapMonster::LIST_IT it = pFtMonGroup->GetMonsterList()->begin();
	CsMapMonster::LIST_IT itEnd = pFtMonGroup->GetMonsterList()->end();
	for( ; it != itEnd; it++ )
	{
		int nSize = vpInsert.Size();
		bool bFind = false;
		for( int i = 0; i < nSize; i++ )
		{
			if( vpInsert[ i ] == (*it)->GetInfo()->s_dwMonsterTableID )
			{				
				bFind = true;
				break;
			}
		}

		if( bFind )
			continue;
		else
			vpInsert.PushBack( (*it)->GetInfo()->s_dwMonsterTableID );	

		CsMonster* pFtMon = nsCsFileTable::g_pMonsterMng->GetMonster( (*it)->GetInfo()->s_dwMonsterTableID );	
		if( pFtMon->IsBossType() ) //보스몹 패스!!
			continue;

		return true;
	}

	return false;
}

cBaseWindow::eMU_TYPE
cZoneMap::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;	

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_DOWN:
		return muReturn;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;
	}

	switch( m_pTranslate->Update_ForMouse() )
	{
	case cButton::ACTION_DOWN:
		return muReturn;
	case cButton::ACTION_CLICK:
		Close();
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_SMALLMAP );
		return muReturn;
	}

	switch( m_pBtnBack->Update_ForMouse() )
	{
	case cButton::ACTION_DOWN:
		return muReturn;
	case cButton::ACTION_CLICK:
		_UpperMapLevel();
		return muReturn;
	}

	switch( m_eState )
	{
	case CUR_ZONE:
	case OTHER_ZONE:
		{
			if( m_pHelperMove->Update_ForMouse() == cButton::ACTION_CLICK )			
			{				
				if( m_nLoadZoneMapID != nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )
				{					
					std::list< DWORD > pList;
					nsCsFileTable::g_pBaseMng->GetJumpBusterItemID( &pList, m_nLoadZoneMapID );													
					if( (int)pList.size() > 0 )
					{
						std::list< DWORD >::iterator it = pList.begin();
						bool bUseJump = false;
						for( ; it != pList.end(); it++ )
						{
							int nSlot = g_pDataMng->GetInven()->GetFirstSlot_Item_ID( *it );
							if( nSlot != cData_Inven::INVALIDE_INVEN_INDEX )
							{
								std::wstring wsMapDiscript = nsCsMapTable::g_pMapListMng->GetList( m_nLoadZoneMapID )->GetInfo()->s_szMapDiscript;
								cPrintMsg::PrintMsg( 30055, const_cast<TCHAR*>(wsMapDiscript.c_str()) );
								cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
								pMBox->SetValue1( nSlot );
								pMBox->SetValue2( m_nLoadZoneMapID );
								bUseJump = true;
								break;
							}														
						}						

						if( !bUseJump )
						{
							std::list< DWORD >::iterator itStart = pList.begin();

#ifdef VERSION_TH
	#ifdef VERSION_TH
								cPrintMsg::PrintMsg( 11040, nsCsFileTable::g_pItemMng->GetItem( *itStart )->GetInfo()->s_szName );
	#elif VERSION_USA
								cPrintMsg::PrintMsg( 11040, nsCsFileTable::g_pItemMng->GetItem( *itStart )->GetInfo()->s_szName );
	#else
								cPrintMsg::PrintMsg( 30116, nsCsFileTable::g_pItemMng->GetItem( *itStart )->GetInfo()->s_szName );
	#endif
#elif VERSION_TW
						cPrintMsg::PrintMsg( 11040, nsCsFileTable::g_pItemMng->GetItem( *itStart )->GetInfo()->s_szName );
#elif VERSION_HK
						cPrintMsg::PrintMsg( 11040, nsCsFileTable::g_pItemMng->GetItem( *itStart )->GetInfo()->s_szName );
#else
#ifndef VERSION_USA
							cPrintMsg::PrintMsg( 30116, nsCsFileTable::g_pItemMng->GetItem( *itStart )->GetInfo()->s_szName );
#else
							cPrintMsg::PrintMsg( 11040, nsCsFileTable::g_pItemMng->GetItem( *itStart )->GetInfo()->s_szName );
#endif
#endif
						}
					}
				}
				else
				{
					cPrintMsg::PrintMsg( 30056 );	
				}	
				return muReturn;
			}						

			if( m_pRadioBtn->Update_ForMouse() == cCheckBox::ACTION_CLICK )
			{				
				m_nRadioIndex = (eRadio_Type)m_pRadioBtn->GetCheckIndex();
				_LoadZone( m_nLoadZoneMapID );			
				return muReturn;
			}

			if( m_pScroll )
				m_pScroll->Update_ForMouse();
		}
		break;
	default:
		break;
	}	

	switch( m_eState )
	{
	case CUR_ZONE:
	case OTHER_ZONE:
		_Update_ForMouse_Zone();
//		return MUT_OUT_WINDOW;
		break;

	case AREAMAP:
		_Update_ForMouse_Area();
		break;

	case WORLDMAP:
		_Update_ForMouse_World();
		break;

	case MAINMAP:
		_Update_ForMouse_MainMap();
		break;
	}

	if( GLOBALINPUT_ST.GetModifier() == DMKEYBOARD::KMOD_LCONTROL )
	{
		CsPoint ptRender;
		CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();
		switch( eCursor )
		{	
		case CURSOR::LBUTTON_DOWN:	// GM 전용 존맵 텔레포트
			_GMTeleport();
			break;

		case CURSOR::RBUTTON_DOWN:	// 유저 미공개 존맵 클릭 길찾기
#ifdef ZONEMAP_CLICK_MOVE
			_ClickMapMove();
#endif
			break;

		}
	}

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	//if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
	//	_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cZoneMap::Render()
{
	switch( m_eState )
	{
	case CUR_ZONE:
	case OTHER_ZONE:
		_Render_Zone();
		break;
	case AREAMAP:	_Render_Area();			break;
	case WORLDMAP:	_Render_World();		break;
	case MAINMAP:	_Render_MainMap();		break;
	default:		assert_cs( false );
	}

	RenderScript();	
}

void cZoneMap::ResetDevice()
{
	SetPosSize(_GetCenterZonePos(), CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE + 34 ));

	ResetDeviceScript();

	if( m_pMyZoneMask )
		m_pMyZoneMask->ResetDevice();

	if( m_pBtnBack )
		m_pBtnBack->ResetDevice();

	if( m_pArrow )
		m_pArrow->ResetDevice();	

#ifdef ZONEMAP_CLICK_MOVE
	if( m_pArrivePoint )
		m_pArrivePoint->ResetDevice();
#endif
	_ResetDevice_Zone();
	_ResetDevice_Area();
	_ResetDevice_World();
	_ResetDevice_MainMap();
	_ResetDevice_HelpMap();

	if(m_RegionName) 
		m_RegionName->ResetDevice();

	if(m_pRegionIcon)
		m_pRegionIcon->ResetDevice();

	if(m_RegionShowText)
		m_RegionShowText->ResetDevice();

	if(m_CheckRegionShow)
		m_CheckRegionShow->ResetDevice();

	std::list< sMap_INFO* >::iterator it = m_listMapInfo.begin();
	std::list< sMap_INFO* >::iterator itEnd = m_listMapInfo.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->ResetDevice();
	}
}

bool cZoneMap::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;
	return true;
}

void cZoneMap::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

//===============================================================================================================================================
//
//		Zone
//
//===============================================================================================================================================

CsPoint cZoneMap::_GetCenterZonePos()
{
	CsPoint CenterPos = CsPoint( (int)( ( g_nScreenWidth/2 - IF_ZONEMAP_TEX_SIZE/2 ) ), (int)( ( g_nScreenHeight/2 - IF_ZONEMAP_TEX_SIZE/2 ) ) );
	return CenterPos;
}

void cZoneMap::_DeleteZone()
{
	SAFE_NIDELETE( m_pZoneMap );	
}

void cZoneMap::_ResetDevice_Zone()
{
	if( m_pZoneMap )
		m_pZoneMap->ResetDevice();	

	if( m_pRadioBtn )
		m_pRadioBtn->ResetDevice();	

	if( m_pExtendBtn )
		m_pExtendBtn->ResetDevice();	

	if( m_pHelperMove )
		m_pHelperMove->ResetDevice();
	if( m_pScroll )
		m_pScroll->ResetDevice();
}

void cZoneMap::_LoadZone( int nMapID /*=-1*/ )
{
	if( nMapID == -1 )
	{
		nMapID = (int)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;					
	}	

	if( g_pHelpArrow->IsArrowRender() )
	{
		switch( g_pHelpArrow->GetInfo()->s_nFindWayType )
		{
			case cQuestFindWay::FindWay_Npc:
			case cQuestFindWay::FindWay_Monster:
			case cQuestFindWay::FindWay_Region:
			case cQuestFindWay::FindWay_Map:
				m_pRadioBtn->GetNode( eType_Pos )->SetEnable( true );
				break;
			case cQuestFindWay::FindWay_Employment:
				break;
			default:
				assert_cs( false );
		}
	}

	if(IsMonster(nMapID))
		m_pRadioBtn->GetNode( eType_Mon )->SetEnable( true );
	else
		m_pRadioBtn->GetNode( eType_Mon )->SetEnable( false );

	int iMapID = 0;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_GET_CHAR_MAPID, &iMapID);

	if( iMapID != 0 )
	{
		m_pRadioBtn->GetNode( eType_Employment )->SetEnable( true );
	}

	m_pRadioBtn->SetCheckIndex( m_nRadioIndex );
	m_pBtnBack->SetEnable( true );

	switch( m_nRadioIndex )
	{
	case eType_Nor:			
		break;
	case eType_Pos:
		assert_cs( g_pHelpArrow->IsArrowRender() );
		nMapID = g_pHelpArrow->GetInfo()->s_dwMapID;
		m_pBtnBack->SetEnable( false );
		break;
	case eType_Npc:
		//if( g_pResist->IsTutorial() == false )
			_SetNpcInfo( nMapID );
// 		else
// 			_SetNpcInfo_Tutorial( nMapID );
		break;
	case eType_Mon:				
		_SetMonInfo( nMapID );				
		break;
	case eType_Employment:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_GET_CHAR_MAPID, &nMapID);
		m_pBtnBack->SetEnable( false );
		break;
	default:
		assert_cs( false );
	}

	// 현재 맵과 같은지
	if( nMapID == (int)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )
	{
		m_eState = CUR_ZONE;
		_CalValue( nMapID );
	}
	else
	{
		m_eState = OTHER_ZONE;
		_CalValue( nMapID );
	}	

	_DeleteZone();
	m_nLoadZoneMapID = nMapID;	

	switch( m_nRadioIndex )
	{
	case eType_Nor:
	case eType_Mon:
	case eType_Npc:
	case eType_Pos:
		break;
	case eType_Employment:
		g_pHelpArrow->SetArrowPoint( 0, nMapID, cQuestFindWay::FindWay_Employment, UISTRING_TEXT( "ZONEMAP_BTN_FIND_SHOP" ).c_str() );
		break;
	default:
		assert_cs( false );
	}

	CsAreaMap::sINFO* pAreaMap = nsCsFileTable::g_pWorldMapMng->GetArea( m_nLoadZoneMapID )->GetInfo();	
	char cZonemap[ MAX_PATH ] = {0, };

	switch( pAreaMap->s_nFieldType )
	{
	case CsAreaMap::FT_None:
		{
			m_bDungeon = false;
			char cZonemapName[ MAX_PATH ] = {0, };	
			nsCsGBTerrain::g_pCurRoot->GetZonemapName( m_nLoadZoneMapID, cZonemapName, MAX_PATH );
#ifdef VERSION_USA
			sprintf_s( cZonemap, MAX_PATH, "Map\\__Map_Eng\\%s", cZonemapName );
			if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
				sprintf_s( cZonemap, MAX_PATH, "Map\\__Map_Eng\\%s", "DATS_IN_Zone.tga" );		
#elif VERSION_TH
			sprintf_s( cZonemap, MAX_PATH, "Map\\__Map_THAI\\%s", cZonemapName );
			if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
				sprintf_s( cZonemap, MAX_PATH, "Map\\__Map_THAI\\%s", "DATS_IN_Zone.tga" );		
#elif VERSION_TW
			sprintf_s( cZonemap, MAX_PATH, "Map\\__Map_Taiwan\\%s", cZonemapName );
			if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
				sprintf_s( cZonemap, MAX_PATH, "Map\\__Map_Taiwan\\%s", "DATS_IN_Zone.tga" );
#elif VERSION_HK
			sprintf_s( cZonemap, MAX_PATH, "Map\\__Map_HongKong\\%s", cZonemapName );
			if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
				sprintf_s( cZonemap, MAX_PATH, "Map\\__Map_HongKong\\%s", "DATS_IN_Zone.tga" );
#else
			sprintf_s( cZonemap, MAX_PATH, "Map\\%s", cZonemapName );
			if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
				sprintf_s( cZonemap, MAX_PATH, "Map\\%s", "DATS_IN_Zone.tga" );
#endif

		}
		break;
	case CsAreaMap::FT_Dungeon:
		{
			m_bDungeon = true;
#ifdef VERSION_USA
			strcpy_s( cZonemap, MAX_PATH, "Map\\__Map_Eng\\_EmptyZone.tga" );			
#elif VERSION_TH
			strcpy_s( cZonemap, MAX_PATH, "Map\\__Map_THAI\\_EmptyZone.tga" );
#elif VERSION_TW
			strcpy_s( cZonemap, MAX_PATH, "Map\\__Map_Taiwan\\_EmptyZone.tga" );
#elif VERSION_HK
			strcpy_s( cZonemap, MAX_PATH, "Map\\__Map_HongKong\\_EmptyZone.tga" );
#else
			strcpy_s( cZonemap, MAX_PATH, "Map\\_EmptyZone.tga" );
#endif
		}
		break;

	default:
		m_bDungeon = false;
		assert_cs( false );
	}


	// 이미지 로드
	assert_cs( m_pZoneMap == NULL );
	m_pZoneMap = NiNew cSprite;
	m_pZoneMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), cZonemap, false );
	//m_pZoneMap->ChangeTexture(CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), NiSourceTexture::Create( cZonemap ));

	m_pZoneMap->GetAlphaProp()->SetAlphaBlending( false );

	// 엔피씨 로드 - 존맵상의 이미지
	if( m_eState == CUR_ZONE )
	{
		_DeleteNpcInfo();

		switch( pAreaMap->s_nFieldType )
		{
		case CsAreaMap::FT_None:
			{
				CsMapNpcGroup* pGroup = nsCsMapTable::g_pMapNpcMng->GetGroup( nMapID );
				MAP_MAPNPC* pMap = pGroup->GetNpcMap();
				MAP_MAPNPC_IT it = pMap->begin();
				MAP_MAPNPC_IT itEnd = pMap->end();
				for( ; it!=itEnd; ++it )
				{
					cMiniMap::sNPC_INFO* pNpcInfo = cMiniMap::sNPC_INFO::NewInstance();
					pNpcInfo->s_pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( it->second->GetInfo()->s_dwNpcID );
					pNpcInfo->s_vPos = NiPoint2( (float)it->second->GetInfo()->s_nInitPosX, (float)it->second->GetInfo()->s_nInitPosY );
					pNpcInfo->s_pNpc = g_pNpcMng->GetNpc( it->second->GetInfo()->s_dwNpcID );

					// 퀘스트 오너 없다면 리턴
					if( pNpcInfo->s_pNpc->GetQuestOwner() == NULL )
					{
						cMiniMap::sNPC_INFO::DeleteInstance( pNpcInfo );
						continue;
					}

					m_listNpcInfo.push_back( pNpcInfo );
				}
			}
			break;
		case CsAreaMap::FT_Dungeon:
			break;
		default:
			assert_cs( false );
		}		
	}

	if( g_pHelpArrow->IsArrowRender() )
		_LoadHelpMap();
}

void cZoneMap::_Update_ForMouse_Zone()
{
	_Update_ForMouseMapInfo();
	_Update_ForMouseParty();

#ifdef MAP_REGION_SHOW
	// 지역 표시 보이기/숨기기
	if( m_CheckRegionShow->Update_ForMouse() == cCheckBox::ACTION_CLICK )
		GetSystem()->SetRegionShow(m_CheckRegionShow->IsCheck());
#endif
}

void cZoneMap::_Update_ForMouseParty()
{
	SPartyInfo const* pMemberList = GetSystem()->GetPartyInfo();
	SAFE_POINTER_RET( pMemberList );

	if( !pMemberList->IsParty() )
		return;

	CsPoint ptMouse = CURSOR_ST.GetPos();

	LIST_MEMBER_INFO const& pMembers = pMemberList->GetList();
	LIST_MEMBER_INFO_CIT it = pMembers.begin();
	for( ; it != pMembers.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );

		if( (*it)->m_nMapNo != m_nLoadZoneMapID )
			continue;

		NiPoint2 vTamerPos((*it)->m_tPos.m_nX, (*it)->m_tPos.m_nY);
		if( vTamerPos == NiPoint2::ZERO )
			continue;

// 		NiPoint2 vTamerPos((*it)->m_tPos.m_nX, (*it)->m_tPos.m_nY);
// 		CsC_AvObject* pPartyTamer = g_pMngCollector->GetObject( (*it)->m_TamerInfo.m_nUID );
// 		if( pPartyTamer != NULL )
// 			vTamerPos = pPartyTamer->GetPos2D();
// 		else
// 		{
// 			if( vTamerPos == NiPoint2::ZERO )
// 				continue;
// 		}

		CsPoint vRenderPos = _CalZoneMapPos( vTamerPos, CsPoint( 22, 22 ) );
		CsRect rc( vRenderPos, CsSIZE( 22, 22 ) );
		if( rc.PtInRect( ptMouse ) == TRUE )
		{
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( vRenderPos, CsPoint( 22, 22 ), 240, cTooltip::PARTY, (*it)->m_nSlotNo );
			return;
		}		
	}
}

void cZoneMap::_Update_ForMouseMapInfo()
{
	if( m_nRadioIndex != eType_Mon && m_nRadioIndex != eType_Npc )
		return;

	std::list< sMap_INFO* >::iterator it = m_listMapInfo.begin();
	std::list< sMap_INFO* >::iterator itEnd = m_listMapInfo.end();
	if( m_listMapInfo.size() > 10 )
	{
		int nIdx = m_pScroll->GetCurPosIndex();
		for( int i = 0; i < nIdx; i++ )
			++it;	
		for( int j = 0; j < 10; j++ )
		{
			if( it==itEnd )
				break;

			if( (*it)->Update_ForMouse() == cBaseWindow::MUT_NONE )
			{				
				m_nRadioIndex = eType_Pos;
				m_pRadioBtn->SetCheckIndex( m_nRadioIndex );
				_LoadZone( g_pHelpArrow->GetInfo()->s_dwMapID );								
			}

			++it;
		}	
	}
	else
	{
		for( ;it != itEnd; ++it )
		{
			if( (*it)->Update_ForMouse() == cBaseWindow::MUT_NONE )
			{			
				m_nRadioIndex = eType_Pos;
				m_pRadioBtn->SetCheckIndex( m_nRadioIndex );
				_LoadZone( g_pHelpArrow->GetInfo()->s_dwMapID );			
			}
		}
	}
}

void cZoneMap::_Render_Zone()
{
	m_pZoneMap->Render();
	m_pBtnBack->Render();
	m_pExtendBtn->Render();
	m_pHelperMove->Render();	
	m_pRadioBtn->Render();

	if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_DATS )
	{
		_Render_Party();	

		if( m_eState == CUR_ZONE )
		{
			m_pArrow->Render();
			_RenderNpc();
#ifdef ZONEMAP_CLICK_MOVE
			_PathLineRender();				// Cell을 연결한 라인을 존맵에 그린다.
			m_pArrivePoint->Render();
#endif
		}		
	}

#ifdef MAP_REGION_SHOW
	if( m_bDungeon == false )	// 던전이면 지역정보 표시 안함.
		_RenderRegion();
#endif

	switch( m_nRadioIndex )
	{
	case eType_Pos:
		_Render_HelperMap();
		break;
	case eType_Npc:
		_Render_NpcInfo();
		break;
	case eType_Mon:	
		_Render_MonInfo();
		break;
	case eType_Nor:
		break;
	case eType_Employment:
		_Render_EmploymentMap();
		break;
	default:
		assert_cs( false );
	}
}

void cZoneMap::_Render_Party()
{
	SPartyInfo const* pMemberList = GetSystem()->GetPartyInfo();
	SAFE_POINTER_RET( pMemberList );

	if( !pMemberList->IsParty() )
		return;

	LIST_MEMBER_INFO const& pMembers = pMemberList->GetList();
	LIST_MEMBER_INFO_CIT it = pMembers.begin();
	for( ; it != pMembers.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );

		if( (*it)->m_nMapNo != m_nLoadZoneMapID )
			continue;

		NiPoint2 vTamerPos((*it)->m_tPos.m_nX, (*it)->m_tPos.m_nY);
		if( vTamerPos == NiPoint2::ZERO )
			continue;

// 		NiPoint2 vTamerPos((*it)->m_tPos.m_nX, (*it)->m_tPos.m_nY);
// 		CsC_AvObject* pPartyTamer = g_pMngCollector->GetObject( (*it)->m_TamerInfo.m_nUID );
// 		if( pPartyTamer != NULL )
// 			vTamerPos = pPartyTamer->GetPos2D();
// 		else
// 		{
// 			if( vTamerPos == NiPoint2::ZERO )
// 				continue;
// 		}

		CsPoint vRenderPos = _CalZoneMapPos( vTamerPos, CsPoint( 22, 22 ) );
		(*it)->m_DigimonInfo.RenderImg( vRenderPos, CsPoint( 22, 22 ) );
		//pPartyInfo->s_DigimonStat.s_pImage->Render_S( vRenderPos, CsPoint( 22, 22 ) );
	}
}

void cZoneMap::_RenderNpc()
{
	std::list< cMiniMap::sNPC_INFO* >::iterator it = m_listNpcInfo.begin();
	std::list< cMiniMap::sNPC_INFO* >::iterator itEnd = m_listNpcInfo.end();
	for( ; it!=itEnd; ++it )
	{
		// 테이머 위치
		int nRenderIcon = -1;

		// 이동엔피씨는 표현 안하겠다
		if( (*it)->s_pFTNpc->GetInfo()->s_eMoveType == nsCsNpcTable::MT_MOVE )
			continue;

		// 안보이는 엔피씨면 패스
		if( (*it)->s_pNpc->IsEnableUse() == false )
			continue;

		switch( (*it)->s_pNpc->GetQuestViewType() )
		{
		case CNpc::QV_NONE:				nRenderIcon = -1;			break;
		case CNpc::QV_NPCTALK:			nRenderIcon = 2;			break;
		default:						assert_cs( false );
		}

		if( nRenderIcon == -1 )
		{
			if( (*it)->s_pNpc->GetQuestOwner() )
			{
				switch( (*it)->s_pNpc->GetQuestOwner()->GetDisplayRev() )
				{
				case cData_QuestOwner::sNpcOwner::REQUITE:
					nRenderIcon = 1;
					break;
				case cData_QuestOwner::sNpcOwner::ENABLE_REV_MAIN:
					nRenderIcon = 10;
					break;
				case cData_QuestOwner::sNpcOwner::ENABLE_REV_SUB:
					nRenderIcon = 0;
					break;
				case cData_QuestOwner::sNpcOwner::ENABLE_REV_REPEAT:
					nRenderIcon = 11;
					break;
				case cData_QuestOwner::sNpcOwner::ENABLE_REV_EVENTREPEAT:
					nRenderIcon = 12;
					break;
				}
			}
		}		

		// 랜더링
		if( nRenderIcon != -1 )
		{
			g_pIconMng->RenderIcon( ICONITEM::MiniMap, _CalZoneMapPos( ( *it )->s_vPos, CsPoint( 16, 22 ) ), CsPoint( 22, 16 ), nRenderIcon );
		}		
	}
}

void cZoneMap::_RenderRegion()
{
	if( nsCsMapTable::g_pMapListMng->IsList( m_nLoadZoneMapID ) == false )
		return;
	CsMapRegionGroup* pGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup(m_nLoadZoneMapID);
	SAFE_POINTER_RET(pGroup);

	CsMapRegion::LIST*	pMRList = pGroup->GetRegionList();
	SAFE_POINTER_RET(pMRList);
	for(CsMapRegion::LIST_IT it = pMRList->begin(); it != pMRList->end(); ++it)
	{
		CsMapRegion* pRegion = *it;

		USHORT	Typ = pRegion->GetInfo()->s_nFatigue_Type;
		int		mCX	= pRegion->GetInfo()->s_nCenterX;
		int		mCY	= pRegion->GetInfo()->s_nCenterY;

		CsPoint mPos = _CalZoneMapPos( NiPoint2(mCX, mCY), CsPoint( 10, 10 ) );

		if(m_pRegionIcon)
			m_pRegionIcon->Render(mPos);

	}
	for(CsMapRegion::LIST_IT it = pMRList->begin(); it != pMRList->end(); ++it)
	{
		CsMapRegion* pRegion = *it;

		TCHAR*	Scr = pRegion->GetInfo()->s_szDiscript;
		int		mCX	= pRegion->GetInfo()->s_nCenterX;
		int		mCY	= pRegion->GetInfo()->s_nCenterY;

		CsPoint mPos = _CalZoneMapPos( NiPoint2(mCX, mCY), CsPoint( 10, 10 ) );		

		if(GetSystem()->IsRegionShow() && m_RegionName)
		{
			m_RegionName->SetText(Scr);
			m_RegionName->SetColor(FONT_GREEN);
			m_RegionName->Render(mPos-CsPoint(0, 12)/*-CsPoint(m_RegionName->GetStringSize().x/2-10/2, 12)*/);
		}

		int result = CURSOR_ST.CheckClickBox( mPos.x, mPos.y, 10, 10 );
		if( result != CURSOR::BUTTON_OUTWINDOW )
		{			
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint( 15, 0 ), Scr);
		}
	}
	if(m_RegionShowText)	m_RegionShowText->Render();
	if(m_CheckRegionShow)	m_CheckRegionShow->Render();
}
CsPoint cZoneMap::_CalZoneMapPos( NiPoint2 vPos, CsPoint ptSize )
{
	CsPoint ptRender;
	if( m_bHeightHigh )
	{
		int nMapID = 0;
		if( g_pHelpArrow->IsArrowRender() )
			nMapID = g_pHelpArrow->GetInfo()->s_dwMapID;
		else
			nMapID = m_nLoadZoneMapID;//nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

		if( nMapID == 105 )	// 요코하마 맵일 경우만
		{
			ptRender.x = (int)( ( vPos.x - 1264 ) * m_Meter_To_Pixel ) - ptSize.x / 2;
			ptRender.y = (int)( ( m_fMaxSize - 22048 - vPos.y ) * m_Meter_To_Pixel ) - ptSize.y / 2;
		}
		else
		{
			ptRender.x = (int)( ( vPos.x + m_fHalfDeltaSize ) * m_Meter_To_Pixel ) - ptSize.x / 2;
			ptRender.y = (int)( ( m_fMaxSize - vPos.y ) * m_Meter_To_Pixel ) - ptSize.y / 2;
		}
	}
	else
	{
		ptRender.x = (int)( vPos.x * m_Meter_To_Pixel ) - ptSize.x / 2;
		ptRender.y = (int)( ( m_fMaxSize - vPos.y - m_fHalfDeltaSize ) * m_Meter_To_Pixel ) - ptSize.y / 2;
	}	
	ptRender += GetRootClient();
	return ptRender;
}


//===============================================================================================================================================
//
//		Area
//
//===============================================================================================================================================
void cZoneMap::_DeleteArea()
{
	SAFE_NIDELETE( m_pAreaMap );

	std::list< sBTN_INFO* >::iterator it = m_listAreaBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listAreaBtn.end();
	for( ; it!=itEnd; ++it )
	{
		SAFE_NIDELETE( *it );
	}
	m_listAreaBtn.clear();
}

void cZoneMap::_ResetDevice_Area()
{
	if( m_pAreaMap )
		m_pAreaMap->ResetDevice();

	std::list< sBTN_INFO* >::iterator it = m_listAreaBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listAreaBtn.end();
	for( ; it!=itEnd; ++it )
	{
		( *it )->s_pButton->ResetDevice();
	}
}

void cZoneMap::_LoadArea( int nType )
{
	m_eState = AREAMAP;

	if( m_nLoadAreaMapID == nType )
		return;

	_DeleteArea();
	assert_cs( m_pAreaMap == NULL );
	assert_cs( m_listAreaBtn.empty() == true );

	m_nLoadAreaMapID = nType;

	char cTex[ 64 ];

#ifdef VERSION_USA
	sprintf_s( cTex, 64, "Map\\__Map_Eng\\AreaMap\\Area%d.tga", nType );	
#elif VERSION_TH
	sprintf_s( cTex, 64, "Map\\__Map_THAI\\AreaMap\\Area%d.tga", nType );	
#elif VERSION_TW
	sprintf_s( cTex, 64, "Map\\__Map_Taiwan\\AreaMap\\Area%d.tga", nType );	
#elif VERSION_HK
	sprintf_s( cTex, 64, "Map\\__Map_HongKong\\AreaMap\\Area%d.tga", nType );
#else
	sprintf_s( cTex, 64, "Map\\AreaMap\\Area%d.tga", nType );
#endif

	m_pAreaMap = NiNew cSprite;
	m_pAreaMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), cTex, false );

	cData_MapRegion* pMapRegion = g_pDataMng->GetMapRegion();
	SAFE_POINTER_RET( pMapRegion );

	// 던젼
	std::list< CsAreaMap::sINFO* > listFTArea;
	std::map< DWORD, std::list< CsAreaMap::sINFO* > > mapDetail;

	// 버튼 로드
	{
		std::list< CsAreaMap* >* pList = nsCsFileTable::g_pWorldMapMng->GetWorld( nType )->GetAreaList();
		std::list< CsAreaMap* >::iterator it = pList->begin();
		std::list< CsAreaMap* >::iterator itEnd = pList->end();
		CsAreaMap::sINFO* pFTArea = NULL;
		for( ; it!=itEnd; ++it )
		{
			pFTArea = (*it)->GetInfo();

			if( pFTArea->s_nUI_X == 0 && pFTArea->s_nUI_Y == 0  )	// 맵 좌표가 있어야 한다.
				continue;

			// 던젼 인가
			if( pFTArea->s_nFieldType == CsAreaMap::FT_Dungeon )
			{
				mapDetail[ pFTArea->s_nFTDetail ].push_back( pFTArea );
				continue;
			}

			sBTN_INFO* pBtn = NiNew sBTN_INFO;
			pBtn->s_pButton = NiNew cButton;
			pBtn->s_pButton->Init( m_pRoot, CsPoint( pFTArea->s_nUI_X, pFTArea->s_nUI_Y ), CsPoint( 91, 79 ), "Map\\WorldMap\\Smallmask.tga", false );
			pBtn->s_pButton->SetTexToken( CsPoint( 91, 0 ) );
			pBtn->s_nID = pFTArea->s_nMapID;

			if( pMapRegion->IsOpenedMap( pFTArea->s_nMapID ) == false )
			{
				pBtn->s_pButton->SetEnable( false );
			}
			m_listAreaBtn.push_back( pBtn );
		}
	}	

	// 던젼만 따로 관리
	{
		std::map< DWORD, std::list< CsAreaMap::sINFO* > >::iterator itmap = mapDetail.begin();
		std::map< DWORD, std::list< CsAreaMap::sINFO* > >::iterator itmapEnd = mapDetail.end();
		for( ; itmap != itmapEnd; ++itmap )
		{
			std::list< CsAreaMap::sINFO* >::iterator itlist = itmap->second.begin();
			std::list< CsAreaMap::sINFO* >::iterator itlistEnd = itmap->second.end();				
			for( ; itlist != itlistEnd; ++itlist )
			{
				if( (*itlist )->s_nUI_X == 0 && (*itlist )->s_nUI_Y == 0  )	// 맵 좌표가 있어야 한다.
					continue;
				
				sBTN_INFO* pBtn = NiNew sBTN_INFO;
				pBtn->s_pButton = NiNew cButton;

				bool bOpen = pMapRegion->IsOpenedMap( ( *itlist )->s_nMapID );					
				pBtn->s_pButton->Init( m_pRoot, CsPoint(  (*itlist )->s_nUI_X,  (*itlist )->s_nUI_Y ), CsPoint( 91, 79 ), "Map\\WorldMap\\Smallmask.tga", false );
				pBtn->s_pButton->SetTexToken( CsPoint( 91, 0 ) );
				pBtn->s_nID = ( *itlist )->s_nMapID;
				pBtn->s_pButton->SetEnable( bOpen );
				m_listAreaBtn.push_back( pBtn );
			}
		}
	}
}

void cZoneMap::_Update_ForMouse_Area()
{
	std::list< sBTN_INFO* >::iterator it = m_listAreaBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listAreaBtn.end();
	sBTN_INFO* pBtn = NULL;
	for( ; it!=itEnd; ++it )
	{
		pBtn = *it;
		switch( pBtn->s_pButton->Update_ForMouse() )
		{
		case cButton::ACTION_CLICK:
			_LoadZone( pBtn->s_nID );
			return;
		case cButton::ACTION_NONE:
			break;
		default:
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pBtn->s_pButton->GetClient(), pBtn->s_pButton->GetSize(), TOOLTIP_MAX_SIZE, cTooltip::WORLDMAP, cTooltip::WORLDMAP_AREA, pBtn->s_nID );
		}
	}
}

void cZoneMap::_Render_Area()
{
	m_pAreaMap->Render();
	m_pBtnBack->Render();

	std::list< sBTN_INFO* >::iterator it = m_listAreaBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listAreaBtn.end();
	for( ; it!=itEnd; ++it )
	{		
		( *it )->s_pButton->Render();

		if( (*it)->s_nID == nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )
			m_pMyZoneMask->Render( (*it)->s_pButton->GetDefaultPos() );
	}
}


//===============================================================================================================================================
//
//		World
//
//===============================================================================================================================================
void cZoneMap::_DeleteWorld()
{
	SAFE_NIDELETE( m_pWorldMap );

	std::list< sBTN_INFO* >::iterator it = m_listWorldBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listWorldBtn.end();
	for( ; it!=itEnd; ++it )
	{
		SAFE_NIDELETE( *it );
	}
	m_listWorldBtn.clear();
}

void cZoneMap::_ResetDevice_World()
{
	if( m_pWorldMap )
		m_pWorldMap->ResetDevice();

	std::list< sBTN_INFO* >::iterator it = m_listWorldBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listWorldBtn.end();
	for( ; it!=itEnd; ++it )
	{
		( *it )->s_pButton->ResetDevice();
	}
}

void cZoneMap::_LoadWorld( int nType )
{
	m_eState = WORLDMAP;

	if( m_nLoadWorldMapID == nType )
		return;

	_DeleteWorld();
	assert_cs( m_pWorldMap == NULL );
	assert_cs( m_listWorldBtn.empty() == true );

	std::list< CsWorldMap* >* pList = NULL;
	m_pWorldMap = NiNew cSprite;

	switch( nType )
	{
	case 1:
#ifdef VERSION_USA
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_Eng\\WorldMap\\RealWorld.tga", false );	
#elif VERSION_TH
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_THAI\\WorldMap\\RealWorld.tga", false );	
#elif VERSION_TW
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_Taiwan\\WorldMap\\RealWorld.tga", false );
#elif VERSION_HK
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_HongKong\\WorldMap\\RealWorld.tga", false );
#else
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\WorldMap\\RealWorld.tga", false );		
#endif
		pList = nsCsFileTable::g_pWorldMapMng->GetRealWorldList();
		break;
	case 2:
#ifdef VERSION_USA
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_Eng\\WorldMap\\DigitallWorld.tga", false );	
#elif VERSION_TH
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_THAI\\WorldMap\\DigitallWorld.tga", false );
#elif VERSION_TW
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_Taiwan\\WorldMap\\DigitallWorld.tga", false );
#elif VERSION_HK
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_HongKong\\WorldMap\\DigitallWorld.tga", false );
#else
		m_pWorldMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\WorldMap\\DigitallWorld.tga", false );		
#endif
		pList = nsCsFileTable::g_pWorldMapMng->GetDigitalWorldList();
		break;
	default:
		assert_cs( false );
	}
	// 버튼 로드
	std::list< CsWorldMap* >::iterator it = pList->begin();
	std::list< CsWorldMap* >::iterator itEnd = pList->end();
	CsWorldMap::sINFO* pFTWorld = NULL;
	for( ; it!=itEnd; ++it )
	{
		pFTWorld = (*it)->GetInfo();
		sBTN_INFO* pBtn = NiNew sBTN_INFO;
		pBtn->s_pButton = NiNew cButton;
		pBtn->s_pButton->Init( m_pRoot, CsPoint( pFTWorld->s_nUI_X, pFTWorld->s_nUI_Y ), CsPoint( 91, 79 ), "Map\\WorldMap\\smallmask.tga", false );
		pBtn->s_pButton->SetTexToken( CsPoint( 91, 0 ) );
		pBtn->s_nID = pFTWorld->s_nID;

		// 해당 내부 맵중에 열린게 없다면 디스에이블
		if( g_pDataMng->GetMapRegion()->IsOpenedWorld( pFTWorld->s_nID ) == false )
			pBtn->s_pButton->SetEnable( false );

		m_listWorldBtn.push_back( pBtn );
	}
}

void cZoneMap::_Update_ForMouse_World()
{
	std::list< sBTN_INFO* >::iterator it = m_listWorldBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listWorldBtn.end();
	sBTN_INFO* pBtn = NULL;
	for( ; it!=itEnd; ++it )
	{
		pBtn = *it;		

		switch( pBtn->s_pButton->Update_ForMouse() )
		{
		case cButton::ACTION_CLICK:
			_LoadArea( pBtn->s_nID );
			return;
		case cButton::ACTION_NONE:
			break;
		default:
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pBtn->s_pButton->GetClient(), pBtn->s_pButton->GetSize(), TOOLTIP_MAX_SIZE, cTooltip::WORLDMAP, cTooltip::WORLDMAP_WORLD, pBtn->s_nID );
		}
	}
}

void cZoneMap::_Render_World()
{
	m_pWorldMap->Render();
	m_pBtnBack->Render();

	std::list< sBTN_INFO* >::iterator it = m_listWorldBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listWorldBtn.end();
	for( ; it!=itEnd; ++it )
	{
		( *it )->s_pButton->Render();
	}
}



//===============================================================================================================================================
//
//		MainMap
//
//===============================================================================================================================================

void cZoneMap::_DeleteMainMap()
{
	SAFE_NIDELETE( m_pMainMap );

	std::list< sBTN_INFO* >::iterator it = m_listMainMapBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listMainMapBtn.end();
	for( ; it!=itEnd; ++it )
	{
		SAFE_NIDELETE( *it );
	}
	m_listMainMapBtn.clear();
}

void cZoneMap::_ResetDevice_MainMap()
{
	if( m_pMainMap )
		m_pMainMap->ResetDevice();

	std::list< sBTN_INFO* >::iterator it = m_listMainMapBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listMainMapBtn.end();
	for( ; it!=itEnd; ++it )
	{
		( *it )->s_pButton->ResetDevice();
	}
}

void cZoneMap::_LoadMainMap()
{
	m_eState = MAINMAP;

	if( m_pMainMap )
		return;

	assert_cs( m_pMainMap == NULL );
	assert_cs( m_listMainMapBtn.empty() == true );

	m_pMainMap = NiNew cSprite;
#ifdef VERSION_USA
	m_pMainMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_Eng\\WorldMap\\World.tga", false );	
#elif VERSION_TH
	m_pMainMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_THAI\\WorldMap\\World.tga", false );	
#elif VERSION_TW
	m_pMainMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_Taiwan\\WorldMap\\World.tga", false );	
#elif VERSION_HK
	m_pMainMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\__Map_HongKong\\WorldMap\\World.tga", false );	
#else
	m_pMainMap->Init( m_pRoot, CsPoint::ZERO, CsPoint( (int)IF_ZONEMAP_TEX_SIZE, (int)IF_ZONEMAP_TEX_SIZE ), "Map\\WorldMap\\World.tga", false );	
#endif

	sBTN_INFO* pBtn = NiNew sBTN_INFO;
	pBtn->s_pButton = NiNew cButton;
	pBtn->s_pButton->Init( m_pRoot, CsPoint( 89, 129 ), CsPoint( 193, 167 ), "Map\\WorldMap\\Bigmask.tga", false );
	pBtn->s_pButton->SetTexToken( CsPoint( 193, 0 ) );
	pBtn->s_nID = 1;
	m_listMainMapBtn.push_back( pBtn );

	pBtn = NiNew sBTN_INFO;
	pBtn->s_pButton = NiNew cButton;
	pBtn->s_pButton->Init( m_pRoot, CsPoint( 238, 215 ), CsPoint( 193, 167 ), "Map\\WorldMap\\Bigmask.tga", false );
	pBtn->s_pButton->SetTexToken( CsPoint( 193, 0 ) );
	pBtn->s_nID = 2;
	m_listMainMapBtn.push_back( pBtn );
}

void cZoneMap::_Update_ForMouse_MainMap()
{
	std::list< sBTN_INFO* >::iterator it = m_listMainMapBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listMainMapBtn.end();
	sBTN_INFO* pBtn = NULL;
	for( ; it!=itEnd; ++it )
	{
		pBtn = *it;

		switch( pBtn->s_pButton->Update_ForMouse() )
		{
		case cButton::ACTION_CLICK:
			_LoadWorld( pBtn->s_nID );
			return;
		case cButton::ACTION_NONE:
			break;
		default:
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pBtn->s_pButton->GetClient(), pBtn->s_pButton->GetSize(), TOOLTIP_MAX_SIZE, cTooltip::WORLDMAP, cTooltip::WORLDMAP_MAIN, pBtn->s_nID );
		}
	}
}

void cZoneMap::_Render_MainMap()
{
	m_pMainMap->Render();
	std::list< sBTN_INFO* >::iterator it = m_listMainMapBtn.begin();
	std::list< sBTN_INFO* >::iterator itEnd = m_listMainMapBtn.end();
	for( ; it!=itEnd; ++it )
	{
		( *it )->s_pButton->Render();
	}
}

//===============================================================================================================================================
//
//		HelpMap
//
//===============================================================================================================================================

void cZoneMap::_DeleteHelpMap()
{	
	m_fStep = 0.0f;	
	g_pCharResMng->CharImageResDelete( &m_pImage );
}

void cZoneMap::_LoadHelpMap()
{
	m_fStep = 0.0f;	
	cHelpArrow::sPosFind* sInfo = g_pHelpArrow->GetInfo();
	SAFE_POINTER_RET( sInfo );
	switch( sInfo->s_nFindWayType )
	{
	case cQuestFindWay::FindWay_Npc:
		{
			CsNpc* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( sInfo->s_nFileID ); 
			assert_csm1( pFTNpc != NULL, _T( " Npc정보 없음 번호 : %d " ), sInfo->s_nFileID );
			CsMapNpc* pFTMapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( sInfo->s_nFileID );
			assert_csm1( pFTMapNpc != NULL, _T( " MapNpc정보 없음 번호 : %d " ), sInfo->s_nFileID );	

			SAFE_POINTER_RET(pFTMapNpc->GetInfo());

			sInfo->s_vArrivePos.x = pFTMapNpc->GetInfo()->s_nInitPosX;
			sInfo->s_vArrivePos.y = pFTMapNpc->GetInfo()->s_nInitPosY;
			
			m_HelperRenderPos = _CalZoneMapPos( NiPoint2( (float)sInfo->s_vArrivePos.x, (float)sInfo->s_vArrivePos.y ), CsPoint( 22, 22 ) );										
			m_HelperRenderPos += CsPoint( 20, 12 );		

			m_pRadarBack->SetSize( RADER_SIZE );

			g_pCharResMng->CharImageResDelete( &m_pImage );	
			m_pImage = g_pCharResMng->CharImageResLoad( pFTNpc->GetInfo()->s_dwModelID );													
		}
		break;

	case cQuestFindWay::FindWay_Monster:
		{
			CsMonster* pFTMon = nsCsFileTable::g_pMonsterMng->GetMonster( sInfo->s_nFileID );
			assert_csm1( pFTMon != NULL, _T( " Mon정보 없음 번호 : %d " ), sInfo->s_nFileID );		

			CsMonster::sINFO* pMonsterInfo = pFTMon->GetInfo();
			SAFE_POINTER_RET( pMonsterInfo );

			CsMapMonsterMng::sMapMonster* pMapMonster = nsCsMapTable::g_pMapMonsterMng->GetMonster( sInfo->s_nFileID );
			SAFE_POINTER_RET( pMapMonster );

			std::map< DWORD, CsMapMonster::LIST >* pMonsterListInMap = pMapMonster->GetMonsterList();
			SAFE_POINTER_RET( pMonsterListInMap );

			std::map< DWORD, CsMapMonster::LIST >::iterator mapiter = pMonsterListInMap->find( sInfo->s_dwMapID );
			if( mapiter == pMonsterListInMap->end() )
				return;
			
			if( mapiter->second.empty() )
				return;

			float fShortRange = FLT_MAX;
			NiPoint3 vPosTamer = g_pCharMng->GetTamerUser()->GetPos();
			vPosTamer.z = 0.0f;		

			CsMapMonster::LIST_IT subit = mapiter->second.begin();
			for( ; subit != mapiter->second.end(); ++subit )
			{
				CsMapMonster::sINFO * pInfo = (*subit)->GetInfo();
				SAFE_POINTER_CON( pInfo );

				NiPoint3 vPos( (float)pInfo->s_nCenterX, (float)pInfo->s_nCenterY, 0.0f );					 
				NiPoint3 vLen = vPos - vPosTamer;
				vLen.z = 0.0f;

				if( vLen.Length() < fShortRange )
				{
					fShortRange = vLen.Length();
					sInfo->s_vArrivePos.x = (float)pInfo->s_nCenterX;
					sInfo->s_vArrivePos.y = (float)pInfo->s_nCenterY;
				}
			}

			m_HelperRenderPos = _CalZoneMapPos( NiPoint2( (float)sInfo->s_vArrivePos.x, (float)sInfo->s_vArrivePos.y ), CsPoint( 22, 22 ) );										
			m_HelperRenderPos += CsPoint( 20, 12 );	

			m_pRadarBack->SetSize( RADER_SIZE );

			g_pCharResMng->CharImageResDelete( &m_pImage );	
			m_pImage = g_pCharResMng->CharImageResLoad( pFTMon->GetInfo()->s_dwModelID );													
		}
		break;
	case cQuestFindWay::FindWay_Region:
		{
			m_HelperRenderPos = _CalZoneMapPos( NiPoint2( (float)sInfo->s_vArrivePos.x, (float)sInfo->s_vArrivePos.y ), CsPoint( 22, 22 ) );										
			m_HelperRenderPos += CsPoint( 20, 12 );	

			m_pRadarBack->SetSize( RADER_SIZE );

			g_pCharResMng->CharImageResDelete( &m_pImage );	
			//m_pImage = g_pCharResMng->CharImageResLoad( pFTMon->GetInfo()->s_dwModelID );	 // 지역은 현재 아이콘이 없다
		}break;

	case cQuestFindWay::FindWay_Employment:
		{
			m_HelperRenderPos = _CalZoneMapPos( NiPoint2( (float)sInfo->s_vArrivePos.x, (float)sInfo->s_vArrivePos.y ), CsPoint( 22, 22 ) );
			m_HelperRenderPos += CsPoint( 20, 12 );	

			int nSize = ( g_pHelpArrow->GetInfo()->s_nRad / 100 ) + 22;
			m_pRadarBack->SetSize( CsPoint( nSize, nSize ) );

			g_pCharResMng->CharImageResDelete( &m_pImage );	

			int iModelID = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_GET_CHAR_MODELID, &iModelID);
			m_pImage = g_pCharResMng->CharImageResLoad( iModelID );
		}
		break;

	case cQuestFindWay::FindWay_Map:				
	
		break;
	default:
		assert_cs( false );
		break;		
	}			
}

void cZoneMap::_ResetDevice_HelpMap()
{
	if( g_pHelpArrow->IsArrowRender() && m_nLoadZoneMapID == g_pHelpArrow->GetInfo()->s_dwMapID )
	{
		SAFE_POINTER_RET( g_pHelpArrow->GetInfo() );
		switch( g_pHelpArrow->GetInfo()->s_nFindWayType )
		{		
		case cQuestFindWay::FindWay_Npc:
		case cQuestFindWay::FindWay_Monster:
		case cQuestFindWay::FindWay_Employment:
		case cQuestFindWay::FindWay_Region:
			{
				if( m_pImage )
					m_pImage->ResetDevice();				
				if( m_pRadar )
					m_pRadar->ResetDevice();
				if( m_pRadarBack )
					m_pRadarBack->ResetDevice();				

				m_HelperRenderPos = _CalZoneMapPos( NiPoint2( (float)g_pHelpArrow->GetInfo()->s_vArrivePos.x, 
					(float)g_pHelpArrow->GetInfo()->s_vArrivePos.y ), CsPoint( 22, 22 ) );										
				m_HelperRenderPos += CsPoint( 20, 12 );						
			}
			break;	
		case cQuestFindWay::FindWay_Map:
			break;
		default:
			assert_cs( false );
		}
	}	
}

void cZoneMap::_Update_Helper()
{
	if( g_pHelpArrow->IsArrowRender() && m_nLoadZoneMapID == g_pHelpArrow->GetInfo()->s_dwMapID )
	{	
		SAFE_POINTER_RET( g_pHelpArrow->GetInfo() );
		switch( g_pHelpArrow->GetInfo()->s_nFindWayType )
		{		
		case cQuestFindWay::FindWay_Npc:
		case cQuestFindWay::FindWay_Monster:
		case cQuestFindWay::FindWay_Region:
		case cQuestFindWay::FindWay_Employment:
			{			
				m_pRadarBack->SetPos( m_HelperRenderPos - RADER_SIZE/2 );				

				m_fRot += g_fDeltaTime;
				while( m_fRot > NI_PI )		m_fRot -= NI_PI*2;
				while( m_fRot < -NI_PI )	m_fRot += NI_PI*2;

				m_pRadar->SetPos( m_HelperRenderPos );
				m_pRadar->SetRotation( m_fRot, RADER_SIZE/2 );	

				if( m_Time.IsEnable() == true )
					cWindow::PlaySound( "System\\Interface_button_02.wav" );
			}break;
		default:
			assert_cs( false );
		}		
	}	
}

void cZoneMap::_Render_HelperMap()
{		
	if( g_pHelpArrow->IsArrowRender() && m_nLoadZoneMapID == g_pHelpArrow->GetInfo()->s_dwMapID )
	{	
		SAFE_POINTER_RET(g_pHelpArrow->GetInfo());
		cQuestFindWay::eFindWayDestType	eType	= g_pHelpArrow->GetInfo()->s_nFindWayType;
		switch( eType )
		{		
		case cQuestFindWay::FindWay_Npc:
		case cQuestFindWay::FindWay_Monster:
		case cQuestFindWay::FindWay_Region:
			{
				int nSizeX = CsFloat2Int( CsMax( 72 * m_fStep * 0.7f, 24 ) );
				int nSizeY = CsFloat2Int( CsMax( 60 * m_fStep * 0.7f, 22 ) );				

				m_fStep += g_fDeltaTime;				

				if(eType == cQuestFindWay::FindWay_Npc)
				{
					m_pRadarBack->SetColor( NiColor(0.0f, 155.0f / 255.0f, 0.0f) );
					m_pRadar->SetColor( NiColor(57.0f / 255.0f, 1.0f, 0.0f) );
				}
				else if(eType == cQuestFindWay::FindWay_Monster)
				{
					m_pRadarBack->SetColor( NiColor(1.0f, 0.0f, 0.0f) );
					m_pRadar->SetColor( NiColor(1.0f, 70.0f / 255.0f, 0.0f) );
				}
				else
				{
					m_pRadarBack->SetColor( NiColor(0.0f, 155.0f / 255.0f, 0.0f) );
					m_pRadar->SetColor( NiColor(57.0f / 255.0f, 1.0f, 0.0f) );
				}

				m_pRadarBack->Render();
				m_pRadar->Render();			

				assert_cs( m_pImage );
				m_pImage->Render_L( m_HelperRenderPos - CsPoint( nSizeX / 2, nSizeY / 2 ) , CsPoint( nSizeX, nSizeY ) );

				if( 95 * m_fStep > 95 )	
					m_fStep = 0.0f;	
			}
			break;
		case cQuestFindWay::FindWay_Map:
			break;
		case cQuestFindWay::FindWay_Employment:
			break;
		default:
			assert_cs( false );
		}
	}		
}

void cZoneMap::_Render_EmploymentMap()
{		
	if( g_pHelpArrow->IsArrowRender() && m_nLoadZoneMapID == g_pHelpArrow->GetInfo()->s_dwMapID )
	{
		SAFE_POINTER_RET(g_pHelpArrow->GetInfo());
		assert_cs( g_pHelpArrow->GetInfo()->s_nFindWayType == cQuestFindWay::FindWay_Employment );

		int nSizeX = CsFloat2Int( CsMax( 72 * m_fStep * 0.7f, 24 ) );
		int nSizeY = CsFloat2Int( CsMax( 60 * m_fStep * 0.7f, 22 ) );

		m_fStep += g_fDeltaTime;				

		m_pRadarBack->SetColor( NiColor(0.0f, 155.0f / 255.0f, 0.0f) );
		m_pRadarBack->Render();
		m_pRadar->SetColor( NiColor(57.0f / 255.0f, 1.0f, 0.0f) );
		m_pRadar->Render();			

		assert_cs( m_pImage );
		m_pImage->Render_L( m_HelperRenderPos - CsPoint( nSizeX / 2, nSizeY / 2 ) , CsPoint( nSizeX, nSizeY ) );

		if( 95 * m_fStep > 95 )	
			m_fStep = 0.0f;	
	}
}

//===============================================================================================================================================
//
//		MapInfo
//
//===============================================================================================================================================

void cZoneMap::sMap_INFO::Delete()
{
	SAFE_NIDELETE( s_pBtn );	
	SAFE_NIDELETE( s_pName );
	SAFE_NIDELETE( s_pTitle );
	SAFE_NIDELETE( s_pEtc );
	g_pCharResMng->CharImageResDelete( &s_pImage );
}

void cZoneMap::sMap_INFO::Init( cZoneMap::sMap_INFO::eType eType, cSprite* pSprite, DWORD dwFileID /*= -1*/, DWORD dwMapID /*= -1*/ )
{	
	assert_cs( dwFileID > 0 );	

	assert_cs( s_pImage == NULL );
	assert_cs( s_pBtn == NULL );
	assert_cs( s_pName == NULL );
	assert_cs( s_pTitle == NULL );
	assert_cs( s_pEtc == NULL );

	s_pCircle = pSprite;

	s_pBtn = NiNew cButton;	
	s_pBtn->Init( NULL, CsPoint::ZERO, CsPoint( 250, 36 ), "System\\Btn_Radio1.tga", false );
	s_pBtn->SetTexToken( CsPoint( 0, 22 ) );

	s_eType = eType;
	switch( eType )
	{
	case sMap_INFO::Type_Npc:
		{
			CsNpc* pFtNpc = nsCsFileTable::g_pNpcMng->GetNpc( dwFileID );
			s_dwMapID = pFtNpc->GetInfo()->s_dwMapID;
			s_dwFileID = pFtNpc->GetInfo()->s_dwNpcID;
			_tcscpy_s( s_szName, NPC_NAME_LEN, pFtNpc->GetInfo()->s_szName );

			g_pCharResMng->CharImageResDelete( &s_pImage );	
			s_pImage = g_pCharResMng->CharImageResLoad( pFtNpc->GetInfo()->s_dwModelID );			
			
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem );		
			s_pName = NiNew cString;			
			s_pTitle = NiNew cString;

			ti.SetText( pFtNpc->GetInfo()->s_szPenName );
			s_pTitle->AddText( &ti );	
			
			ti.SetText( pFtNpc->GetInfo()->s_szName );			
			s_pName->AddText( &ti );								
		}
		break;
	case sMap_INFO::Type_Monster:
		{
			CsMonster* pFtMon = nsCsFileTable::g_pMonsterMng->GetMonster( dwFileID );	
			s_dwMapID = dwMapID;
			s_dwFileID = pFtMon->GetInfo()->s_dwMonsterID;
			_tcscpy_s( s_szName, NPC_NAME_LEN, pFtMon->GetInfo()->s_szName );
			
			g_pCharResMng->CharImageResDelete( &s_pImage );	
			s_pImage = g_pCharResMng->CharImageResLoad( pFtMon->GetInfo()->s_dwModelID );	

			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem );		
			s_pName = NiNew cString;						
			s_pEtc = NiNew cString;			

			if( _tcslen( pFtMon->GetInfo()->s_szPenName ) > 0 )
			{
				s_pTitle = NiNew cString;				

				ti.SetText( pFtMon->GetInfo()->s_szPenName );
				s_pTitle->AddText( &ti );

				CsPoint ptPos;
				int nBase;
				cText::GetStringSize( &ti, ptPos, nBase, pFtMon->GetInfo()->s_szPenName );
				s_pTitle->TailAddSizeX( 150 - ptPos.x );
			}			

			ti.SetText( pFtMon->GetInfo()->s_szName );
			s_pName->AddText( &ti );

			ti.SetText( UISTRING_TEXT( "COMMON_TXT_LV" ).c_str() );
			ti.AddText( L" " );
			s_pEtc->AddText( &ti );
			ti.SetText( pFtMon->GetInfo()->s_nLevel );
			s_pEtc->AddText( &ti )->s_ptSize.x = 24;
			
			CsDigimon::sINFO* pFTDigimonInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pFtMon->GetInfo()->s_dwModelID )->GetInfo();
			int nDigimonAtt = pFTDigimonInfo->s_eAttributeType - 1;		// 0기반 인덱스로
			s_pEtc->AddIcon( CsPoint( 24, 24 ), ICONITEM::DAttribute, cIconMng::DAttribute, nDigimonAtt, CsPoint( 0, -5 ) );
		}
		break;
	default:
		assert_cs( false );
	}		
}

void cZoneMap::sMap_INFO::Render( CsPoint ptPos )
{	
	s_pBtn->SetPos( ptPos );
	s_pBtn->Render();			
	if( s_pTitle )
	{
		s_pName->Render( ptPos + CsPoint( 45, 21 ) );
		s_pTitle->Render( ptPos + CsPoint( 45, 5 ) );	

		if( s_pEtc )
			s_pEtc->Render( ptPos + CsPoint( 175, 13 ) );
	}
	else
	{		
		s_pName->Render( ptPos + CsPoint( 45, 13 ) );

		if( s_pEtc )
			s_pEtc->Render( ptPos + CsPoint( 175, 13 ) );
	}

	assert_cs( s_pCircle );
	s_pCircle->Render( ptPos );
	s_pImage->Render_L( ptPos, CsPoint( 35, 35 ) );
}

void cZoneMap::sMap_INFO::ResetDevice()
{
	s_pBtn->ResetDevice();
	s_pImage->ResetDevice();

	if( s_pName )
		s_pName->ResetDevice();
	
	if( s_pTitle )
		s_pTitle->ResetDevice();

	if( s_pEtc )
		s_pEtc->ResetDevice();
}

cBaseWindow::eMU_TYPE cZoneMap::sMap_INFO::Update_ForMouse()
{
	if( s_pBtn->Update_ForMouse() != cCheckBox::ACTION_CLICK )
		return cBaseWindow::MUT_OUT_WINDOW;

	if(  ( (s_dwMapID == 4)/*튜토리얼 예외처리*/ || (nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_DUNGEON) ) ) 
	{
		switch( s_eType )
		{
		case sMap_INFO::Type_Npc:
			if( g_pHelpArrow->SetArrowPoint( s_dwFileID, s_dwMapID, cQuestFindWay::FindWay_Npc, s_szName ) )
				return cBaseWindow::MUT_NONE;			
			break;
		case sMap_INFO::Type_Monster:
			if( g_pHelpArrow->SetArrowPoint( s_dwFileID, s_dwMapID, cQuestFindWay::FindWay_Monster, s_szName ) )
				return cBaseWindow::MUT_NONE;
			break;
		default:
			assert_cs( false );
			break;
		}		
	}	

	return cBaseWindow::MUT_OUT_WINDOW;
}



void cZoneMap::_DeleteMapInfo()
{
	if( m_pScroll )
	{
		m_pScroll->Delete();
		SAFE_NIDELETE( m_pScroll );
	}

	std::list< sMap_INFO* >::iterator it = m_listMapInfo.begin();
	std::list< sMap_INFO* >::iterator itEnd = m_listMapInfo.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		SAFE_NIDELETE( *it );
	}
	m_listMapInfo.clear();		
}

void cZoneMap::_SetNpcInfo( int nMapID /*=-1*/ )
{
	_DeleteMapInfo();

	if( nMapID == -1 )	
		nMapID = (int)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	CsMapNpcGroup* pFTMapNpc = nsCsMapTable::g_pMapNpcMng->GetGroup( nMapID );
	if( pFTMapNpc )
	{
		MAP_MAPNPC_IT it = pFTMapNpc->GetNpcMap()->begin();
		MAP_MAPNPC_IT itEnd = pFTMapNpc->GetNpcMap()->end();
		for( ; it != itEnd; it++ )
		{
			DWORD dwNpcID = it->second->GetInfo()->s_dwNpcID;
			
			// 테이블에 존재 안한다면 패스
			if( nsCsFileTable::g_pNpcMng->IsNpc( dwNpcID ) == false )
				continue;

			// 이동 엔피씨인경우 존재를 표현 안해주겠다.
			if( nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID )->GetInfo()->s_eMoveType == nsCsNpcTable::MT_MOVE )
				continue;

			if( ( nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID )->GetInfo()->s_nDisplayPlag & CsNpc::DP_INFOMATION ) == 0 )
				continue;

			// 보이지 않는 npc이면 패스
			if( g_pNpcMng->IsNpc( dwNpcID ) == true )
			{
				CNpc* pNpc = g_pNpcMng->GetNpc( dwNpcID );
				if( pNpc->IsEnableUse() == false )
					continue;
			}

			sMap_INFO* Info = NiNew sMap_INFO;
			Info->Init( sMap_INFO::Type_Npc, m_pCircle, dwNpcID );
			m_listMapInfo.push_back( Info );			
		}	
	}	

	assert_cs( m_pScroll == NULL );
	m_pScroll = NiNew cScrollBar;

	m_pScroll->Init( cScrollBar::TYPE_1, m_pRoot, CsPoint( 263, 48 ), CsPoint( 16, 217 ), CsPoint( 16, 15 ), 
	CsRect( CsPoint( 13, 48 ), CsPoint( 290, 277 ) ), 6, false );

	m_pScroll->SetRange( CsPoint( 0, (int)m_listMapInfo.size() ) );			

}


// void cZoneMap::_SetNpcInfo_Tutorial( int nMapID /*=-1*/ )
// {
// 	if( m_eState != CUR_ZONE )
// 	{
// 		_SetNpcInfo( nMapID );
// 		return;
// 	}	
// 
// 	if( nMapID == -1 )	
// 		nMapID = (int)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
// 
// 	if( nMapID != 1 )
// 	{
// 		_SetNpcInfo( nMapID );
// 		return;
// 	}
// 
// 	_DeleteMapInfo();
// 	CsMapNpcGroup* pFTMapNpc = nsCsMapTable::g_pMapNpcMng->GetGroup( nMapID );
// 	if( pFTMapNpc )
// 	{
// 		MAP_MAPNPC_IT it = pFTMapNpc->GetNpcMap()->begin();
// 		MAP_MAPNPC_IT itEnd = pFTMapNpc->GetNpcMap()->end();
// 		for( ; it != itEnd; it++ )
// 		{
// 			DWORD dwNpcID = it->second->GetInfo()->s_dwNpcID;
// 
// 			switch( dwNpcID )
// 			{
// 			case 94015:
// 				break;
// 			default:
// 				continue;
// 			}
// 
// 			// 테이블에 존재 안한다면 패스
// 			if( nsCsFileTable::g_pNpcMng->IsNpc( dwNpcID ) == false )
// 				continue;
// 
// 			// 이동 엔피씨인경우 존재를 표현 안해주겠다.
// 			if( nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID )->GetInfo()->s_eMoveType == nsCsNpcTable::MT_MOVE )
// 				continue;
// 
// 			if( ( nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID )->GetInfo()->s_nDisplayPlag & CsNpc::DP_INFOMATION ) == 0 )
// 				continue;
// 
// 			// 보이지 않는 npc이면 패스
// 			if( g_pNpcMng->IsNpc( dwNpcID ) == true )
// 			{
// 				CNpc* pNpc = g_pNpcMng->GetNpc( dwNpcID );
// 				if( pNpc->IsEnableUse() == false )
// 					continue;
// 			}			
// 
// 			sMap_INFO* Info = NiNew sMap_INFO;
// 			Info->Init( sMap_INFO::Type_Npc, m_pCircle, dwNpcID );
// 			m_listMapInfo.push_back( Info );			
// 		}	
// 	}	
// 
// 	assert_cs( m_pScroll == NULL );
// 	m_pScroll = NiNew cScrollBar;
// 	m_pScroll->Init( cScrollBar::TYPE_1, m_pRoot, CsPoint( 263, 48 ), CsPoint( 16, 217 ), CsPoint( 16, 15 ), 
// 	CsRect( CsPoint( 13, 48 ), CsPoint( 290, 277 ) ), 6, false );
// 	m_pScroll->SetRange( CsPoint( 0, (int)m_listMapInfo.size() ) );			
// 
// }

void cZoneMap::_Render_NpcInfo()
{	
	if( m_pScroll )
	{
		if( m_pScroll->IsEnableScroll() == true )
			m_pScroll->Render();
	}
	CsPoint ptPos( GetRootClient() + CsPoint( 13, 48 ) );

	std::list< sMap_INFO* >::iterator it = m_listMapInfo.begin();
	std::list< sMap_INFO* >::iterator itEnd = m_listMapInfo.end();
	
	int nIdx = m_pScroll->GetCurPosIndex();
	for( int i = 0; i < nIdx; i++ )
		++it;	
	
	
	for( int j = 0; j < 6; j++ )
	{
		if( it==itEnd )
			break;

		(*it)->Render( ptPos );	
		ptPos.y += 36;		
		it++;
	}		
}

void cZoneMap::_SetMonInfo( int nMapID /* = -1  */ )
{
	_DeleteMapInfo();

	if( nMapID == -1 )	
		nMapID = (int)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;					

	CsVectorPB< DWORD > vpInsert;
	CsMapMonsterGroup* pFtMonGroup = nsCsMapTable::g_pMapMonsterMng->GetGroup( nMapID );
	if( pFtMonGroup )
	{	
		CsMapMonster::LIST_IT it = pFtMonGroup->GetMonsterList()->begin();
		CsMapMonster::LIST_IT itEnd = pFtMonGroup->GetMonsterList()->end();
		for( ; it != itEnd; it++ )
		{
			int nSize = vpInsert.Size();
			bool bFind = false;
			for( int i = 0; i < nSize; i++ )
			{
				if( vpInsert[ i ] == (*it)->GetInfo()->s_dwMonsterTableID )
				{				
					bFind = true;
					break;
				}
			}

			if( bFind )
				continue;
			else
				vpInsert.PushBack( (*it)->GetInfo()->s_dwMonsterTableID );	

			CsMonster* pFtMon = nsCsFileTable::g_pMonsterMng->GetMonster( (*it)->GetInfo()->s_dwMonsterTableID );	
			if( pFtMon->IsBossType() ) //보스몹 패스!!
				continue;			

			sMap_INFO* Info = NiNew sMap_INFO;
			Info->Init( sMap_INFO::Type_Monster, m_pCircle, (*it)->GetInfo()->s_dwMonsterTableID, (*it)->GetInfo()->s_dwMapID );	
			m_listMapInfo.push_back( Info );	
		}
	}	

	assert_cs( m_pScroll == NULL );
	m_pScroll = NiNew cScrollBar;
	m_pScroll->Init( cScrollBar::TYPE_1, m_pRoot, CsPoint( 263, 48 ), CsPoint( 16, 217 ), CsPoint( 16, 15 ), 
	CsRect( CsPoint( 13, 48 ), CsPoint( 290, 277 ) ), 6, false );
	m_pScroll->SetRange( CsPoint( 0, (int)m_listMapInfo.size() ) );		
	
}

void cZoneMap::_Render_MonInfo()
{
	if( m_pScroll )
	{
		if( m_pScroll->IsEnableScroll() == true )
			m_pScroll->Render();
	}
	CsPoint ptPos( GetRootClient() + CsPoint( 13, 48 ) );

	std::list< sMap_INFO* >::iterator it = m_listMapInfo.begin();
	std::list< sMap_INFO* >::iterator itEnd = m_listMapInfo.end();
	
	int nIdx = m_pScroll->GetCurPosIndex();
	for( int i = 0; i < nIdx; i++ )	
		++it;	


	for( int j = 0; j < 6; j++ )
	{
		if( it==itEnd )
			break;

		(*it)->Render( ptPos );	
		ptPos.y += 36;
		it++;
	}	
}

CsPoint cZoneMap::_GetMoveArrivePos()
{
	CsPoint ptRender(0,0);
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );

	if(m_bHeightHigh)
	{
		if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID == 105 ) // 요코하마 리뉴얼 맵
		{
			ptRender.x = LONG( (ptLocal.x / m_Meter_To_Pixel) + 1264 );
			ptRender.y = LONG( -((ptLocal.y / m_Meter_To_Pixel) - m_fMaxSize) - 22048);
		}
		else
		{
			ptRender.x = LONG( (ptLocal.x / m_Meter_To_Pixel) - m_fHalfDeltaSize );
			ptRender.y = LONG( -((ptLocal.y / m_Meter_To_Pixel) - m_fMaxSize) );
		}
	}
	else
	{
		ptRender.x = LONG( ptLocal.x / m_Meter_To_Pixel);
		ptRender.y = LONG(-((ptLocal.y / m_Meter_To_Pixel - m_fHalfDeltaSize) - m_fMaxSize));
	}

	return ptRender;
}

void cZoneMap::_GMTeleport()
{
	CsPoint ptRender = _GetMoveArrivePos();
	if( net::game )
		net::game->SendJump( ptRender.x, ptRender.y );
}

#ifdef ZONEMAP_CLICK_MOVE
void cZoneMap::RefreshArrivePoint( CsPoint ptMouse )
{
	// 픽킹에 성공한 월드 Pos를가져온다.
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_Terrain( CAMERA_ST.GetCameraObj(), ptMouse.x, ptMouse.y );
	if( pRecord == NULL )
		return;

	NiPoint3 vPickPos = pRecord->GetIntersection();
	vPickPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPickPos );

	CsPoint vRenderPos = _CalZoneMapPos( NiPoint2(vPickPos.x, vPickPos.y), CsPoint::ZERO );
	vRenderPos -= GetRootClient();

	// 존맵에 도착 예정 지역 갱신
	m_pArrivePoint->SetVisible(true);
	m_pArrivePoint->SetPos( CsPoint( vRenderPos.x - 7, vRenderPos.y - 16) );

	m_PathList.clear();
	_UpdatePath();
}

void cZoneMap::_ClickMapMove()
{
	CsPoint ptRender = _GetMoveArrivePos();
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );

	if( nsCsGBTerrain::g_pCurRoot->IsValidEmr2(ptRender.x, ptRender.y) == false )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = UISTRING_TEXT( "SMALLMAP_NOT_ALLOWED_AREA" );
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return ;
	}

	// 존맵에 도착 예정 지역 갱신
	m_pArrivePoint->SetVisible(true);
	m_pArrivePoint->SetPos( ptLocal - CsPoint(7,16) );
	g_pServerMoveOwner->SetPos_FromZoneMap( ptRender, 0.0f, true, false );
}

void cZoneMap::_UpdatePath()
{
	m_PathList.clear();
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	if( pTamer->GetProp_Path()->IsMove() )
	{
		int nPathSize = pTamer->GetProp_Path()->GetPath()->m_iPath->size();

		for(int i = 0 ; i < nPathSize; ++i )
		{
			cPosition NaviPos = pTamer->GetProp_Path()->GetPath()->m_iPath->position(i);
			CsPoint vPathPos = _CalZoneMapPos( NiPoint2((float)NaviPos.x, (float)NaviPos.y), CsPoint::ZERO );
			vPathPos -= GetRootClient();

			m_PathList.push_back( vPathPos );
		}

		CsPoint vPathPos = _CalZoneMapPos( pTamer->GetProp_Path()->GetArrivePos(), CsPoint::ZERO );
		vPathPos -= GetRootClient();
		m_PathList.push_back( vPathPos );
	}
}

void cZoneMap::_PathLineRender()
{
	if( m_PathList.size() > 2 )
	{
		std::list<CsPoint>::iterator iter_b = m_PathList.begin();
		++iter_b;

		g_pEngine->m_pD3DXLine->Begin();
		for(std::list<CsPoint>::iterator iter = m_PathList.begin(); iter != m_PathList.end(); ++iter, ++iter_b)
		{
			if(iter_b == m_PathList.end())
				break;

			D3DXVECTOR2 vLine[2];
			vLine[0] = D3DXVECTOR2( (float)m_ptRootClient.x + (*iter).x , (float)m_ptRootClient.y + (*iter).y );
			vLine[1] = D3DXVECTOR2( (float)m_ptRootClient.x + (*iter_b).x , (float)m_ptRootClient.y + (*iter_b).y );

			g_pEngine->m_pD3DXLine->Draw(vLine, 2, 0xffffffff);
		}
		g_pEngine->m_pD3DXLine->End();
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 때문에 만든 하드 코딩
//////////////////////////////////////////////////////////////////////////
cWindow* cZoneMap::GetChildControl( std::string const& sControlName )
{
	cWindow* pFindControl = cScriptUI::GetChildControl( sControlName );
	if( pFindControl )
		return pFindControl;
	if( 0 == sControlName.compare( "FindNpcBtn" ) )
	{
		SAFE_POINTER_RETVAL( m_pRadioBtn, NULL );
		return m_pRadioBtn->GetNode( eType_Npc );
	}
	else if( 0 == sControlName.compare( "FindMonsterBtn" ) )
	{
		SAFE_POINTER_RETVAL( m_pRadioBtn, NULL );
		return m_pRadioBtn->GetNode( eType_Mon );
	} 
	return NULL;
}

cWindow* cZoneMap::GetMapObjectControl( int const& nType, DWORD const& dwTargetIDX )
{
	if( nType != m_nRadioIndex )
		return NULL;

	std::list< sMap_INFO* >::iterator it = m_listMapInfo.begin();
	for(  int n = 0; it != m_listMapInfo.end(); ++it, ++n )
	{
		if( (*it)->s_dwFileID != dwTargetIDX )
			continue;

		if( m_pScroll )
			m_pScroll->SetCurPosIndex( n );

		return (*it)->GetButtonControl();
	}

	return NULL;
}