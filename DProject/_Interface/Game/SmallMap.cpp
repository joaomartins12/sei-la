#include "stdafx.h"

#include "SmallMap.h"

#define IF_SMALLMAP_TEX_SIZE		256.0f

cSmallMap::cSmallMap() : m_pRegionIcon(NULL), m_RegionName(NULL), m_pTamerPoint(NULL), m_bDungeon(false)
{	
}

void cSmallMap::Destroy()
{
	cBaseWindow::Delete();
}

void cSmallMap::DeleteResource()
{	
	_DeleteNpcInfo();
	DeleteScript();
	m_pTamerPoint = NULL;
	SAFE_NIDELETE( m_RegionName );
	SAFE_NIDELETE( m_pRegionIcon );
	
}

void cSmallMap::_DeleteNpcInfo()
{
	std::list< cMiniMap::sNPC_INFO* >::iterator it = m_listNpcInfo.begin();
	std::list< cMiniMap::sNPC_INFO* >::iterator itEnd = m_listNpcInfo.end();
	for( ; it!=itEnd; ++it )
	{
		cMiniMap::sNPC_INFO::DeleteInstance( *it );
	}
	m_listNpcInfo.clear();
}

bool cSmallMap::Close( bool bSound /* = true  */ )
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}

	DeleteResource();
	SetShowWindow( false );
	return true;
}

void cSmallMap::ResetDevice()
{
	ResetDeviceScript();
	if(m_RegionName)
		m_RegionName->ResetDevice();
	if(m_pRegionIcon)
		m_pRegionIcon->ResetDevice();
}

void cSmallMap::Create(int nValue /* = 0  */)
{	
	if( IsLive() == false )
	{
		SetRootClient( CsPoint( 19, 430 ) );	
		m_pMap = NULL;
		m_pCancelButton = NULL;		
		m_pTranslate = NULL;
		m_pMoveWindow = NULL;	
		m_pMinus = NULL;
		m_pPlus = NULL;		
		m_fSize = 1.0f;
	}			

	cBaseWindow::Init();	
 
	LoadSmallMap();
}


bool cSmallMap::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;
	return true;
}

void cSmallMap::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}


cBaseWindow::eMU_TYPE
cSmallMap::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
	{
		m_pCancelButton->Update_ForMouse();
		m_pTranslate->Update_ForMouse();
		m_pPlus->Update_ForMouse();
		m_pMinus->Update_ForMouse();
		m_pMoveWindow->Update_ForMouse();
		return muReturn;
	}

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

	switch( m_pTranslate->Update_ForMouse() )
	{	
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();		
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_ZONEMAP );
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pPlus->Update_ForMouse() )
	{	
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:		
		if( _ResetSize(1) )
		{
			LoadSmallMap();
			_Update_TamerPos();	
		}		
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pMinus->Update_ForMouse() )
	{	
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:	
		if( _ResetSize(-1) )
		{
			LoadSmallMap();
			_Update_TamerPos();	
		}		
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pMoveWindow->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_DOWN:
		break;
	case cButton::ACTION_PRESS:
		_UpdateMoveWindow_ForMouse();
		return muReturn;
		break;		
	default:
		return muReturn;
	}	

	return MUT_NONE;
}

void cSmallMap::Update(float const& fDeltaTime)
{	
	_UpdateMoveWindow();	
	_Update_TamerPos();	
}

void cSmallMap::Render()
{
	RenderScript();	
	if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_DATS )
	{
		_RenderNpc();
#ifdef MAP_REGION_SHOW
		if( m_bDungeon == false )	// 던전이면 지역정보 표시 안함.
			_RenderRegion();
#endif
	}
}

void cSmallMap::LoadSmallMap()
{
	DeleteResource();
	InitScript( NULL, m_ptRootClient, CsPoint( (int)(IF_SMALLMAP_TEX_SIZE * m_fSize), (int)(IF_SMALLMAP_TEX_SIZE * m_fSize) ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );	

	//현재 맵 체크
	DWORD nMap = g_pTerrainMng->GetBackupMapID();
	CsAreaMap::sINFO* pAreaMap = nsCsFileTable::g_pWorldMapMng->GetArea( nMap )->GetInfo();	

	char cZonemap[ MAX_PATH ] = {0, };

	switch( pAreaMap->s_nFieldType )
	{
	case CsAreaMap::FT_None:
		{
			m_bDungeon = false;
			char cZonemapName[ MAX_PATH ] = {0, };	
			nsCsGBTerrain::g_pCurRoot->GetZonemapName( nMap, cZonemapName, MAX_PATH );

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

	float fBtnSize = 20;

	// 이미지 로드	
	m_pMap = AddSprite( CsPoint::ZERO, CsPoint( (int)(IF_SMALLMAP_TEX_SIZE * m_fSize), (int)(IF_SMALLMAP_TEX_SIZE * m_fSize) ), cZonemap );
	m_pMap->GetAlphaProp()->SetAlphaBlending( true );	
	m_pMoveWindow = AddButton( CsPoint( 1, 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 0, 22 ), "System\\MoveWindow.tga" );
	m_pMoveWindow->SetAlpha( 0.7f );
	m_pMoveWindow->AddEvent( cButton::BUTTON_LBDOWN_EVENT, this, &cSmallMap::_MoveUi );	//2016-03-15-nova

	m_pCancelButton = AddButton( CsPoint( (int)(IF_SMALLMAP_TEX_SIZE * m_fSize - fBtnSize), 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 0, 19 ), "QuestRev\\Quest_X_But.tga" );
	m_pCancelButton->SetAlpha( 0.7f );
	m_pTranslate = AddButton( CsPoint( (int)(IF_SMALLMAP_TEX_SIZE * m_fSize - ( fBtnSize * 2.2f )), 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 0, 20 ), "System\\Translate.tga" );
	m_pTranslate->SetAlpha( 0.7f );
	if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_DATS )
		m_pTamerPoint = AddSprite( CsPoint::ZERO, CsPoint( 17, 17 ), "Map\\SmallMap\\TamerPoint.tga" );	
	m_pPosPoint = AddSprite( CsPoint::ZERO, CsPoint( 17, 17 ), "Map\\SmallMap\\PosPoint.tga" );	

	m_pPlus = AddButton( CsPoint( (int)(fBtnSize * 1.2f), 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 33, 33 ), "MiniMap\\UpScaleBtn.tga" );
	m_pPlus->SetAlpha( 0.7f );
	m_pMinus = AddButton( CsPoint( (int)(fBtnSize * 2.3f), 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 33, 33 ), "MiniMap\\DownScaleBtn.tga" );
	m_pMinus->SetAlpha( 0.7f );	

#ifdef MAP_REGION_SHOW
	cText::sTEXTINFO ti;		
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_8, FONT_GREEN );					
	ti.s_bOutLine = false;		
	ti.s_eTextAlign = DT_CENTER;

	m_RegionName = NiNew cText;
	m_RegionName->Init(NULL, CsPoint::ZERO, &ti, false);

	m_pRegionIcon = NiNew cSprite;
	m_pRegionIcon->Init( NULL, CsPoint::ZERO, CsPoint( 10, 10 ), "Map\\Map_Region.tga", false );

#endif

	_DeleteNpcInfo();
	
	// 엔피씨 로드	
	switch( pAreaMap->s_nFieldType )
	{
	case CsAreaMap::FT_None:
		{
			CsMapNpcGroup* pGroup = nsCsMapTable::g_pMapNpcMng->GetGroup( nMap );
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
	

	_CalValue( nMap );	
	_Update_TamerPos();
}

bool cSmallMap::_ResetSize( int nNum )
{
	float nfSize = m_fSize + ( nNum * 0.3f );
	if( !( nfSize < 1.6f && nfSize > 0.6f ) )
		return false;

	m_fSize += nNum * 0.3f;
	return true;	
}

void cSmallMap::_CalValue( DWORD dwMapID )
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
		m_Meter_To_Pixel = ( IF_SMALLMAP_TEX_SIZE * m_fSize ) / 55919;
	else
		m_Meter_To_Pixel = ( IF_SMALLMAP_TEX_SIZE * m_fSize ) / m_fMaxSize;
}

CsPoint cSmallMap::_CalMapPos( NiPoint2 vPos, CsPoint ptSize )
{
	CsPoint ptRender;
	if( m_bHeightHigh )
	{
		int nMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
		if( nMapID == 105 )
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

void cSmallMap::_Update_TamerPos()
{
	NiPoint2 vTamerPos;
	if( g_pCharMng->GetTamerUser() )
	{
		vTamerPos = g_pCharMng->GetTamerUser()->GetPos2D();
	}
	else
	{
		nSync::Pos pos = g_pDataMng->GetPostLoad()->GetPos();
		vTamerPos.x = (float)pos.m_nX;
		vTamerPos.y = (float)pos.m_nY;
	}

	CsPoint vRenderPos = _CalMapPos( vTamerPos, CsPoint::ZERO );
	vRenderPos -= GetRootClient();	
	if(m_pTamerPoint)
		m_pTamerPoint->SetPos( vRenderPos );

	if( g_pHelpArrow->IsArrowRender() )
	{
		if( g_pHelpArrow->GetInfo()->s_dwMapID == g_pTerrainMng->GetBackupMapID() )
		{
			NiPoint2 pos( g_pHelpArrow->GetInfo()->s_vArrivePos.x, g_pHelpArrow->GetInfo()->s_vArrivePos.y );
			vRenderPos = _CalMapPos( pos, CsPoint::ZERO );
			vRenderPos -= GetRootClient();				
						
			m_pPosPoint->SetAlpha( 1.0f );
			m_pPosPoint->SetPos( vRenderPos );
			CsPoint ptMouse = CURSOR_ST.GetPos();	
			CsRect rc( vRenderPos + GetRootClient(), CsSIZE( 18, 18 ) );
			if( rc.PtInRect( ptMouse ) == TRUE )
			{
				vRenderPos.y -= 13;
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( vRenderPos + GetRootClient(), CsPoint::ZERO, g_pHelpArrow->GetInfo()->s_szName, CFont::FS_8 );
			}
		}		
		else
			m_pPosPoint->SetAlpha( 0.0f );
	}
	else
		m_pPosPoint->SetAlpha( 0.0f );		
	

	// 카메라 방향
	NiPoint3 vCameraDir = CAMERA_ST.GetWorldDirection();
	vCameraDir.z = 0.0f;
	vCameraDir.Unitize();
	float fRot = NiACos( NiPoint3( 0.0f, -1.0f, 0.0f ).Dot( vCameraDir ) );
	if( NiPoint3( 0.0f, -1.0f, 0.0f ).Cross( vCameraDir ).z > 0 )
		fRot = -fRot;
	if(m_pTamerPoint)
		m_pTamerPoint->SetRotation( fRot, CsPoint( 8, 8 ) );	
}

void cSmallMap::_RenderNpc()
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
			g_pIconMng->RenderIcon( ICONITEM::MiniMap, _CalMapPos( ( *it )->s_vPos, CsPoint( 14, 11 ) ), CsPoint( 14, 11 ), nRenderIcon );
		}		
	}
}

void cSmallMap::_MoveUi( void* pSender, void* pData )
{
	_UpdateMoveWindow_ForMouse();
}

void cSmallMap::_RenderRegion()
{
	int MapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;;
	if( nsCsMapTable::g_pMapListMng->IsList( MapID ) == false )
		return;
	CsMapRegionGroup* pGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup(MapID);
	SAFE_POINTER_RET(pGroup);

	CsMapRegion::LIST*	pMRList = pGroup->GetRegionList();
	SAFE_POINTER_RET(pMRList);
	for(CsMapRegion::LIST_IT it = pMRList->begin(); it != pMRList->end(); ++it)
	{
		CsMapRegion* pRegion = *it;		
		int		mCX	= pRegion->GetInfo()->s_nCenterX;
		int		mCY	= pRegion->GetInfo()->s_nCenterY;

		CsPoint mPos = _CalMapPos( NiPoint2(mCX, mCY), CsPoint( 10, 10 ) );
		if(m_pRegionIcon)
			m_pRegionIcon->Render(mPos);

	}

	for(CsMapRegion::LIST_IT it = pMRList->begin(); it != pMRList->end(); ++it)
	{
		CsMapRegion* pRegion = *it;
	
		TCHAR*	Scr = pRegion->GetInfo()->s_szDiscript;	//TCHAR*	Sen = pRegion->GetInfo()->s_szDiscript_Eng;
		int		mCX	= pRegion->GetInfo()->s_nCenterX;
		int		mCY	= pRegion->GetInfo()->s_nCenterY;

		CsPoint mPos = _CalMapPos( NiPoint2(mCX, mCY), CsPoint( 10, 10 ) );

		if(GetSystem()->IsRegionShow() && m_RegionName)
		{
			m_RegionName->SetText(Scr);
			m_RegionName->SetColor(FONT_GREEN);
			m_RegionName->Render(mPos - CsPoint(0, 12)/*-CsPoint(m_RegionName->GetStringSize().x/2-10/2, 12)*/);
		}

		int result = CURSOR_ST.CheckClickBox( mPos.x, mPos.y, 10, 10 );
		if( result != CURSOR::BUTTON_OUTWINDOW )
		{			
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint( 15, 0 ), Scr);
		}
	}
}

void cSmallMap::LoadSmallMap2( DWORD nMap )
{
	DeleteResource();
	InitScript( NULL, m_ptRootClient, CsPoint( (int)(IF_SMALLMAP_TEX_SIZE * m_fSize), (int)(IF_SMALLMAP_TEX_SIZE * m_fSize) ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );	

	//현재 맵 체크
	//DWORD nMap = g_pTerrainMng->GetBackupMapID();
	CsAreaMap::sINFO* pAreaMap = nsCsFileTable::g_pWorldMapMng->GetArea( nMap )->GetInfo();	

	char cZonemap[ MAX_PATH ] = {0, };

	switch( pAreaMap->s_nFieldType )
	{
	case CsAreaMap::FT_None:
		{
			char cZonemapName[ MAX_PATH ] = {0, };	
			nsCsGBTerrain::g_pCurRoot->GetZonemapName( nMap, cZonemapName, MAX_PATH );

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
		assert_cs( false );
	}

	float fBtnSize = 20;

	// 이미지 로드	
	m_pMap = AddSprite( CsPoint::ZERO, CsPoint( (int)(IF_SMALLMAP_TEX_SIZE * m_fSize), (int)(IF_SMALLMAP_TEX_SIZE * m_fSize) ), cZonemap );
	m_pMap->GetAlphaProp()->SetAlphaBlending( true );	
	m_pMoveWindow = AddButton( CsPoint( 1, 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 0, 22 ), "System\\MoveWindow.tga" );
	m_pMoveWindow->SetAlpha( 0.7f );
	m_pCancelButton = AddButton( CsPoint( (int)(IF_SMALLMAP_TEX_SIZE * m_fSize - fBtnSize), 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 0, 19 ), "QuestRev\\Quest_X_But.tga" );
	m_pCancelButton->SetAlpha( 0.7f );
	m_pTranslate = AddButton( CsPoint( (int)(IF_SMALLMAP_TEX_SIZE * m_fSize - ( fBtnSize * 2.2f )), 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 0, 20 ), "System\\Translate.tga" );
	m_pTranslate->SetAlpha( 0.7f );	

	if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_DATS )
	{
		if( NULL == m_pTamerPoint )
			m_pTamerPoint = AddSprite( CsPoint::ZERO, CsPoint( 17, 17 ), "Map\\SmallMap\\TamerPoint.tga" );	
	}
	else
	{
		if( m_pTamerPoint )
		{
			RemoveChildControl( m_pTamerPoint );
			m_pTamerPoint = NULL;
		}
	}

	m_pPosPoint = AddSprite( CsPoint::ZERO, CsPoint( 17, 17 ), "Map\\SmallMap\\PosPoint.tga" );	

	m_pPlus = AddButton( CsPoint( (int)(fBtnSize * 1.2f), 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 33, 33 ), "MiniMap\\UpScaleBtn.tga" );
	m_pPlus->SetAlpha( 0.7f );
	m_pMinus = AddButton( CsPoint( (int)(fBtnSize * 2.3f), 1 ), CsPoint( (int)fBtnSize, (int)fBtnSize ), CsPoint( 33, 33 ), "MiniMap\\DownScaleBtn.tga" );
	m_pMinus->SetAlpha( 0.7f );	

	_DeleteNpcInfo();

	// 엔피씨 로드	
	switch( pAreaMap->s_nFieldType )
	{
	case CsAreaMap::FT_None:
		{
			CsMapNpcGroup* pGroup = nsCsMapTable::g_pMapNpcMng->GetGroup( nMap );
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

	{
		cText::sTEXTINFO ti;		
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_8, FONT_GREEN );					
		ti.s_bOutLine = false;		
		ti.s_eTextAlign = DT_CENTER;

		m_RegionName = NiNew cText;
		m_RegionName->Init(NULL, CsPoint::ZERO, &ti, false);

		m_pRegionIcon = NiNew cSprite;
		m_pRegionIcon->Init( NULL, CsPoint::ZERO, CsPoint( 10, 10 ), "Map\\Map_Region.tga", false );
	}

	_CalValue( nMap );	
	_Update_TamerPos();
}

#ifdef ZONEMAP_CLICK_MOVE
void cSmallMap::_GMTeleport()
{
	CsPoint ptRender = _GetMoveArrivePos();
	if( net::game )
		net::game->SendJump( ptRender.x, ptRender.y );
}

void cSmallMap::_ClickMapMove()
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
	g_pServerMoveOwner->SetPos_FromZoneMap( ptRender, 0.0f, true, false );
}

CsPoint cSmallMap::_GetMoveArrivePos()
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
#endif