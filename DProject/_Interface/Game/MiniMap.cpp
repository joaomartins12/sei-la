
#include "stdafx.h"
#include "MiniMap.h"

#define IF_MINIMAP_MASK_SIZE			176.0f

CSGBMEMPOOL_CPP( cMiniMap::sNPC_INFO );

#define IF_MINIMAP_TEX_SIZE		512.0f
#define IF_MINIMAP_MAX_SCALE	1.6f
#define IF_MINIMAP_MIN_SCALE	0.7f
#define IF_MINIMAP_IMAGE_POS			CsPoint( 2, 43 )

void cMiniMap::GlobalInit()
{
	CSGBMEMPOOL_INIT( cMiniMap::sNPC_INFO, 16 );
}

void cMiniMap::GlobalShotDown()
{
	CSGBMEMPOOL_DELETE( cMiniMap::sNPC_INFO );
}

cMiniMap::cMiniMap() : m_pArrow(NULL),m_pImage(NULL),m_pMiniMap(NULL),m_pMapText(NULL),m_pRegionText(NULL)
,m_pNpcFindBtn(NULL),m_pUpScaleBtn(NULL),m_pDownScaleBtn(NULL),m_pNpcCancel(NULL),m_pMapCancel(NULL),m_pToggleMapBtn(NULL)
,m_pPosFindCancelBtn(NULL)
{
	m_eType = CsQuestCompRequire::NPCTALK;
	ZeroMemory( &m_szMap, MAX_FILENAME);
	ZeroMemory( &m_szRegion, FT_MAP_REGION_DISC_LEN);

	m_bHeightHigh = false;
	m_dwOldMapID = 0;
	m_fBackupZoom = 1.0f;
	m_fZoom = 1.0f;

	m_eFieldType = CsAreaMap::FT_None;
	m_bRenderRegionText = true;

	m_ptOrgPos = CsPoint::ZERO;
	m_vTamerPos = NiPoint2::ZERO;
	m_fAspectiveHeight = 0.0f;
	m_Meter_To_Pixel  = 0.0f;
	m_fInvertImageSize  = 0.0f;
	m_fHalfDeltaSize  = 0.0f;
	m_fMaxSize = 0.0f;
	m_fTexselSize = 0.0f;
}

cMiniMap::~cMiniMap()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}


TCHAR * cMiniMap::Get_MapName()
{ 
	return m_szMap; 
}

void cMiniMap::Destroy()
{
	cBaseWindow::Delete();
	_DeleteNpcInfo();
}

void cMiniMap::DeleteResource()
{
	DeleteScript();
	SAFE_NIDELETE( m_pNpcFindBtn );
	SAFE_NIDELETE( m_pUpScaleBtn );
	SAFE_NIDELETE( m_pDownScaleBtn );
	SAFE_NIDELETE( m_pNpcCancel );
	SAFE_NIDELETE( m_pMapCancel );	
	SAFE_NIDELETE( m_pMiniMap );
	SAFE_NIDELETE( m_pRegionText );
	SAFE_NIDELETE( m_pMapText );

	g_pCharResMng->CharImageResDelete( &m_pImage );
}

void cMiniMap::_DeleteNpcInfo()
{
	std::list< sNPC_INFO* >::iterator it = m_listNpcInfo.begin();
	std::list< sNPC_INFO* >::iterator itEnd = m_listNpcInfo.end();
	for( ; it!=itEnd; ++it )
	{
		sNPC_INFO::DeleteInstance( *it );
	}
	m_listNpcInfo.clear();
}

void cMiniMap::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "MiniMap\\MiniMapImage.tga", CsPoint( DEFAULT_SCREEN_WIDTH - 182, 2 ), CsPoint( 180, 221 ), false, IFREGION_X::RIGHT, IFREGION_Y::TOP );

	m_ptOrgPos = CsPoint( 90, 131 );

	m_pMiniMap = NiNew cSprite;
	m_pMiniMap->Init( m_pRoot, IF_MINIMAP_IMAGE_POS, CsPoint( 176, 176 ), false );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;
	// 맵이름
	ti.s_Color = FONT_WHITE;
	m_pMapText = NiNew cText;
	m_pMapText->Init( m_pRoot, CsPoint( 90, 5 ), &ti, false );	

	// 지역이름
	ti.s_Color = NiColor( 0, 1, 0 );
	m_pRegionText = NiNew cText;
	m_pRegionText->Init( m_pRoot, CsPoint( 90, 22 ), &ti, false );	

	// 맵 토글버튼
	m_pToggleMapBtn = AddButton( CsPoint( 2, 43 ), CsPoint( 26, 26 ), CsPoint( 0, 33 ), "MiniMap\\ToggleMapBtn.tga" );
	if( m_pToggleMapBtn )
		m_pToggleMapBtn->SetControlName( "ToggleZoneMapBtn" );

	// 축소
	m_pDownScaleBtn = NiNew cButton;
	m_pDownScaleBtn->Init( m_pRoot, CsPoint( 126, 43 ), CsPoint( 26, 26 ), "MiniMap\\DownScaleBtn.tga", false );
	m_pDownScaleBtn->SetTexToken( CsPoint( 0, 33 ) );

	// 확대
	m_pUpScaleBtn = NiNew cButton;
	m_pUpScaleBtn->Init( m_pRoot, CsPoint( 152, 43 ), CsPoint( 26, 26 ), "MiniMap\\UpScaleBtn.tga", false );
	m_pUpScaleBtn->SetTexToken( CsPoint( 0, 33 ) );

	// 엔피씨 찾기
	m_pNpcFindBtn = NiNew cButton;
	m_pNpcFindBtn->Init( m_pRoot, CsPoint( 2, 192 ), CsPoint( 26, 26 ), "Minimap\\NpcFindBtn.tga", false );
	m_pNpcFindBtn->SetTexToken( CsPoint( 33, 0 ) );

	// 위치찾기 취소버튼
	m_pNpcCancel = NiNew cButton;
	m_pNpcCancel->Init( m_pRoot, CsPoint( 145, 185 ), CsPoint( 33, 33 ), "Serch\\SerchCancel_01.tga", false );
	m_pNpcCancel->SetTexToken( CsPoint( 33, 0 ) );

	m_szMap[ 0 ] = NULL;
	m_szRegion[ 0 ] = NULL;

	if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_DATS )
		m_pArrow = AddSprite( m_ptOrgPos, CsPoint( 32, 32 ), "MiniMap\\Arrow_01.tga" );


	m_pMapCancel = NiNew cButton;
	m_pMapCancel->Init( m_pRoot, CsPoint( 145, 185 ), CsPoint( 33, 33 ), "Serch\\SerchCancel_02.tga", false );
	m_pMapCancel->SetTexToken( CsPoint( 33, 0 ) );

	m_pPosFindCancelBtn = NULL;

	m_fBackupZoom = m_fZoom = 1.0f;
	m_dwOldMapID = 0;

	m_bRenderRegionText = true;
}

void cMiniMap::LoadDatsMiniMap( bool bSet )
{
	// 닷트 본부 내부 (작전지휘실)일 경우에 화살표 안그림.
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS && m_pArrow )
	{
		RemoveChildControl(m_pArrow);
		m_pArrow = NULL;
	}
	else if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_DATS && m_pArrow == NULL)
		m_pArrow = AddSprite( m_ptOrgPos, CsPoint( 32, 32 ), "MiniMap\\Arrow_01.tga" );

	DWORD dwCurID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

#ifndef _GIVE
	if( nsCsFileTable::g_pWorldMapMng->IsArea( dwCurID ) == false )
		return;
#endif

	m_bHeightHigh = ( nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight() >= nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth() );	

	char cMinimapName[ MAX_PATH ] = {0, };
	char cMinimap[ MAX_PATH ] = {0, };

	if( bSet == true ) 
	{
		// 이미지 로드
		nsCsGBTerrain::g_pCurRoot->GetMinimapName( cMinimapName, MAX_PATH );

#ifdef VERSION_USA
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_Eng\\%s", "DATS_IN_Mini.tga" );			
#elif VERSION_TH
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_THAI\\%s", "DATS_IN_Mini.tga" );
#elif VERSION_TW
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_Taiwan\\%s", "DATS_IN_Mini.tga" );
#elif VERSION_HK
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_HongKong\\%s", "DATS_IN_Mini.tga" );
#else
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\%s", "DATS_IN_Mini.tga" );	
#endif

		NiSourceTexturePtr pTexture = NiSourceTexture::Create( cMinimap );
		NiTexturingProperty* pkTexturing = (NiTexturingProperty*)m_pMiniMap->GetScreen()->GetProperty(NiProperty::TEXTURING);		
		NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( pTexture, 0 );
		pkTexturing->SetBaseMap( pMap );
		pkTexturing->SetBaseClampMode( NiTexturingProperty::CLAMP_S_CLAMP_T );
	}
	else
	{		
		nsCsGBTerrain::g_pCurRoot->GetMinimapName( cMinimapName, MAX_PATH );

#ifdef VERSION_USA
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_Eng\\%s", cMinimapName );
#elif VERSION_TH
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_THAI\\%s", cMinimapName );
#elif VERSION_TW
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_Taiwan\\%s", cMinimapName );
#elif VERSION_HK
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_HongKong\\%s", cMinimapName );
#else
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\%s", cMinimapName );		
#endif			

		NiSourceTexturePtr pTexture = NiSourceTexture::Create( cMinimap );
		NiTexturingProperty* pkTexturing = (NiTexturingProperty*)m_pMiniMap->GetScreen()->GetProperty(NiProperty::TEXTURING);		
		NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( pTexture, 0 );
		pkTexturing->SetBaseMap( pMap );
		pkTexturing->SetBaseClampMode( NiTexturingProperty::CLAMP_S_CLAMP_T );
	}

	CsAreaMap::sINFO* pAreaMap = nsCsFileTable::g_pWorldMapMng->GetArea( dwCurID )->GetInfo();
	m_eFieldType = (CsAreaMap::eFIELD_TYPE)pAreaMap->s_nFieldType;

	switch( m_eFieldType )
	{
	case CsAreaMap::FT_None:
		m_fZoom = m_fBackupZoom;
		break;
	case CsAreaMap::FT_Dungeon:
		m_fBackupZoom = m_fZoom;
		m_fZoom = IF_MINIMAP_MAX_SCALE;
		break;
	default:
		assert_cs( false );
	}
}

void cMiniMap::LoadMiniMap()
{
	DWORD dwCurID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	if( m_dwOldMapID == dwCurID )
		return;
	m_dwOldMapID = dwCurID;

	if( nsCsFileTable::g_pWorldMapMng->IsArea( dwCurID ) == false )
		return;

	// 이미지 로드
	{
		char cMinimapName[ MAX_PATH ] = {0, };
		char cMinimap[ MAX_PATH ] = {0, };
		nsCsGBTerrain::g_pCurRoot->GetMinimapName( cMinimapName, MAX_PATH );

	#ifdef VERSION_USA
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_Eng\\%s", cMinimapName );
	#elif VERSION_TH
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_THAI\\%s", cMinimapName );
	#elif VERSION_TW
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_Taiwan\\%s", cMinimapName );
	#elif VERSION_HK
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\__Map_HongKong\\%s", cMinimapName );
	#else
		sprintf_s( cMinimap, MAX_PATH, "Data\\Interface\\Map\\%s", cMinimapName );		
	#endif

		m_bHeightHigh = ( nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight() >= nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth() );

		NiSourceTexturePtr pTexture = NiSourceTexture::Create( cMinimap );
		NiTexturingProperty* pkTexturing = (NiTexturingProperty*)m_pMiniMap->GetScreen()->GetProperty(NiProperty::TEXTURING);		
		NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( pTexture, 0 );
		pkTexturing->SetBaseMap( pMap );
		pkTexturing->SetBaseClampMode( NiTexturingProperty::CLAMP_S_CLAMP_T );

		CsAreaMap::sINFO* pAreaMap = nsCsFileTable::g_pWorldMapMng->GetArea( dwCurID )->GetInfo();
		m_eFieldType = (CsAreaMap::eFIELD_TYPE)pAreaMap->s_nFieldType;

		switch( m_eFieldType )
		{
		case CsAreaMap::FT_None:
			m_fZoom = m_fBackupZoom;
			break;
		case CsAreaMap::FT_Dungeon:
			m_fBackupZoom = m_fZoom;
			m_fZoom = IF_MINIMAP_MAX_SCALE;
			break;
		default:
			assert_cs( false );
		}

	}	

	// 엔피씨 로드
	{
		_DeleteNpcInfo();

		CsMapNpcGroup* pGroup = nsCsMapTable::g_pMapNpcMng->GetGroup( m_dwOldMapID );
		MAP_MAPNPC* pMap = pGroup->GetNpcMap();
		MAP_MAPNPC_IT it = pMap->begin();
		MAP_MAPNPC_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			sNPC_INFO* pNpcInfo = sNPC_INFO::NewInstance();
			pNpcInfo->s_pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( it->second->GetInfo()->s_dwNpcID );
			pNpcInfo->s_vPos = NiPoint2( (float)it->second->GetInfo()->s_nInitPosX, (float)it->second->GetInfo()->s_nInitPosY );
			pNpcInfo->s_pNpc = g_pNpcMng->GetNpc( it->second->GetInfo()->s_dwNpcID );

			m_listNpcInfo.push_back( pNpcInfo );
		}
	}	

	_tcscpy_s( m_szMap, MAX_FILENAME, nsCsMapTable::g_pMapListMng->GetList( dwCurID )->GetInfo()->s_szMapDiscript.c_str() );

	m_pMapText->SetText( m_szMap );

	_CalMapValue();
}

void cMiniMap::SetRegion( TCHAR const* szRegion )
{
	if( g_pEngine->m_FontSystem.IsInitialize() == false )
		return;

	TCHAR szStr[ FT_MAP_REGION_DISC_LEN ];
	_tcscpy_s( szStr, FT_MAP_REGION_DISC_LEN, szRegion );

	if( _tcscmp( szRegion, m_pMapText->GetTextInfo()->GetText() ) == 0 )
	{
		m_bRenderRegionText = false;
		return;
	}

	_tcscpy_s( m_szRegion, FT_MAP_REGION_DISC_LEN, szRegion );
	m_bRenderRegionText = true;

	FT_Face face = g_pEngine->m_FontSystem.GetFace( CFont::FS_10 );
	int nLen = _tcslen( m_szRegion );
	int nMaxX = 0;
	int nCurX;

	for( int i=0; i<nLen; ++i )
	{
		nCurX = cText::GetCharWidth( face, m_szRegion[ i ] );
		if( (nCurX + nMaxX) > 135 )
		{
			_tcsncpy_s( szStr, FT_MAP_REGION_DISC_LEN, m_szRegion, i-1 );
			_tcscat_s( szStr, FT_MAP_REGION_DISC_LEN, _T( "..." ) );
			break;
		}
		nMaxX += nCurX;
	}

	m_pRegionText->SetText( szStr );
}

CsPoint cMiniMap::_CalZoneMapPos( float fZoom, NiPoint2 vPos, CsPoint ptImageSize )
{
	NiPoint2 deltaPos = vPos - m_vTamerPos;
	deltaPos.y *= m_fAspectiveHeight;

	CsPoint ptRender( 0, 0 );
	ptRender.x = m_ptOrgPos.x + (int)( ( deltaPos.x ) * m_Meter_To_Pixel - ptImageSize.x*fZoom*0.5f );
	ptRender.y = m_ptOrgPos.y + (int)( ( -deltaPos.y ) * m_Meter_To_Pixel - ptImageSize.y*fZoom*0.5f );
	ptRender -= IF_MINIMAP_IMAGE_POS;

	if( ( ptRender.x < 0 )||( ptRender.y < 0 ) )
		return CsPoint::ZERO;

	if( ( ptRender.x + ptImageSize.x*fZoom > IF_MINIMAP_MASK_SIZE )||( ptRender.y + ptImageSize.y*fZoom > IF_MINIMAP_MASK_SIZE ) )
		return CsPoint::ZERO;

	ptRender += GetRootClient() + IF_MINIMAP_IMAGE_POS;
	return ptRender;
}

void cMiniMap::_CalMapValue()
{	
	float fMapWidth = nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth();
	float fMapHeight = nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight();

	if( m_bHeightHigh )
	{
		m_fHalfDeltaSize = ( fMapHeight - fMapWidth )*0.5f;
		m_fMaxSize = fMapHeight;
	}
	else
	{
		m_fHalfDeltaSize = ( fMapWidth - fMapHeight )*0.5f;
		m_fMaxSize = fMapWidth;
	}
	
	// 필요한값 계산	
	m_fAspectiveHeight = fMapHeight / fMapWidth;
	m_Meter_To_Pixel = ( IF_MINIMAP_TEX_SIZE*m_fZoom ) / m_fMaxSize;

	m_fInvertImageSize = 1.0f/( IF_MINIMAP_TEX_SIZE*m_fZoom );
	m_fTexselSize = m_fInvertImageSize * IF_MINIMAP_MASK_SIZE;
}

void cMiniMap::Update(float const& fDeltaTime)
{
	m_pUpScaleBtn->SetEnable( true );
	m_pDownScaleBtn->SetEnable( true );
	if( m_fZoom == IF_MINIMAP_MAX_SCALE )
	{
		m_pUpScaleBtn->SetEnable( false );
	}
	else if( m_fZoom == IF_MINIMAP_MIN_SCALE )
	{
		m_pDownScaleBtn->SetEnable( false );
	}

	_Update_TamerPos();

	// 카메라 방향
	NiPoint3 vCameraDir = CAMERA_ST.GetWorldDirection();
	vCameraDir.z = 0.0f;
	vCameraDir.Unitize();
	float fRot = NiACos( NiPoint3( 0.0f, -1.0f, 0.0f ).Dot( vCameraDir ) );
	if( NiPoint3( 0.0f, -1.0f, 0.0f ).Cross( vCameraDir ).z > 0 )
		fRot = -fRot;
	if(m_pArrow)
		m_pArrow->SetRotation( fRot, CsPoint( 16, 16 ) );
	m_pNpcFindBtn->SetEnable( ( g_pCharMng->GetDigimonUser( 0 )->IsBattle() == false ) );
}

void cMiniMap::_Update_TamerPos()
{
  	m_vTamerPos.x = g_pCharMng->GetTamerUser()->GetPos().x;
  	m_vTamerPos.y = g_pCharMng->GetTamerUser()->GetPos().y;

	if( m_bHeightHigh )
	{
		float x = ( m_vTamerPos.x + m_fHalfDeltaSize ) * m_Meter_To_Pixel;
		float y = ( m_fMaxSize - m_vTamerPos.y ) * m_Meter_To_Pixel;	
		float px = x * m_fInvertImageSize - m_fTexselSize*0.5f;
		float py = y * m_fInvertImageSize - m_fTexselSize*0.5f;
		m_pMiniMap->GetScreen()->SetTextures( 0, 0, px, py, px+m_fTexselSize, py+m_fTexselSize );
	}
	else
	{
		float x = m_vTamerPos.x * m_Meter_To_Pixel;
		float y = ( m_fMaxSize - m_vTamerPos.y - m_fHalfDeltaSize ) * m_Meter_To_Pixel;	
		float px = x * m_fInvertImageSize - m_fTexselSize*0.5f;
		float py = y * m_fInvertImageSize - m_fTexselSize*0.5f;
		m_pMiniMap->GetScreen()->SetTextures( 0, 0, px, py, px+m_fTexselSize, py+m_fTexselSize );
	}
}

cBaseWindow::eMU_TYPE
cMiniMap::Update_ForMouse()
{
cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;
#ifdef SIMPLE_TOOLTIP
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	
	CsRect rcTemp = CsRect( CsPoint( 2, 43 ), CsSIZE( 33, 33 ) );
	if(rcTemp.PtInRect( ptMouseLocalPos ) == TRUE)
	{
		CsTalk_Tooltip::sINFO* pInfo = nsCsFileTable::g_pTalkMng->GetTalk_Tooltip( 1051 )->GetInfo();
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1051 );	//2017-01-19-nova 마우스 좌표로 툴팁표시
	}
	rcTemp.SetRect( CsPoint( 2, 185 ), CsSIZE( 33, 33 ) );
	if(rcTemp.PtInRect( ptMouseLocalPos ) == TRUE)
	{
		CsTalk_Tooltip::sINFO* pInfo = nsCsFileTable::g_pTalkMng->GetTalk_Tooltip( 1052 )->GetInfo();
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1052 );	//2017-01-19-nova 마우스 좌표로 툴팁표시		
	}	
	rcTemp.SetRect( CsPoint( 145, 43 ), CsSIZE( 33, 33 ) );
	if(rcTemp.PtInRect( ptMouseLocalPos ) == TRUE)
	{
		CsTalk_Tooltip::sINFO* pInfo = nsCsFileTable::g_pTalkMng->GetTalk_Tooltip( 1053 )->GetInfo();
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1053 );	//2017-01-19-nova 마우스 좌표로 툴팁표시
	}	
	rcTemp.SetRect( CsPoint( 112, 43 ), CsSIZE( 33, 33 ) );
	if(rcTemp.PtInRect( ptMouseLocalPos ) == TRUE)
	{
		CsTalk_Tooltip::sINFO* pInfo = nsCsFileTable::g_pTalkMng->GetTalk_Tooltip( 1054 )->GetInfo();
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1054 );	//2017-01-19-nova 마우스 좌표로 툴팁표시	
	}	
#endif
	// 토글맵
	if( m_pToggleMapBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ZONEMAP ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ZONEMAP );
		else
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_ZONEMAP );
	}

	if( m_eFieldType != CsAreaMap::FT_Dungeon )
	{
		// 축소
		if( m_pDownScaleBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			if( ( m_fZoom -= 0.3f ) < IF_MINIMAP_MIN_SCALE )
				m_fZoom = IF_MINIMAP_MIN_SCALE;

			m_fBackupZoom = m_fZoom;
			_CalMapValue();
			_Update_TamerPos();		
		}

		// 확대
		if( m_pUpScaleBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			if( ( m_fZoom += 0.3f ) > IF_MINIMAP_MAX_SCALE )
				m_fZoom = IF_MINIMAP_MAX_SCALE;

			m_fBackupZoom = m_fZoom;
			_CalMapValue();
			_Update_TamerPos();		
		}
	}	

	// NPC찾기
	if( g_pNpcMng->GetNpcVector()->size() && (nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_DUNGEON))
	{
		if( m_pNpcFindBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_NPCFIND ) == false )
				g_pGameIF->GetDynamicIF( cBaseWindow::WT_NPCFIND );
			else
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_NPCFIND );
		}
	}

	//위치찾기 취소버튼
	if( g_pHelpArrow->IsArrowRender() )
	{
		assert_cs( m_pPosFindCancelBtn );
		switch( m_pPosFindCancelBtn->Update_ForMouse() )
		{	
		case cButton::ACTION_ON:		
			{				
				CsPoint pos = GetRootClient() + m_pPosFindCancelBtn->GetDefaultPos() + CsPoint( 33, 33 );				
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( pos, CsPoint::ZERO, g_pHelpArrow->GetInfo()->s_szName );
			}			
			break;
		case cButton::ACTION_CLICK :
			{
				cPrintMsg::PrintMsg( 30113 );
				cWindow::PlaySound( cWindow::SD_Wi1 );
			}
			break;			
		}
	}	

	_Update_ForMouse_Party();

	
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );

	if( ptLocal.y < 21 )
	{
		if( m_szMap[ 0 ] != NULL )
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint( 15, 0 ), m_szMap );
	}
	else if( ptLocal.y < 42 )
	{
		if( m_szRegion[ 0 ] != NULL )
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint( 15, 0 ), m_szRegion );
	}

	return muReturn;
}

void cMiniMap::_Update_ForMouse_Party()
{
	SPartyInfo const* pMemberList = GetSystem()->GetPartyInfo();
	SAFE_POINTER_RET( pMemberList );
	
	if( !pMemberList->IsParty() )
		return;

	float fZoom = sqrt( m_fZoom );
	CsPoint ptMouse = CURSOR_ST.GetPos();
	DWORD nMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	LIST_MEMBER_INFO const& pMembers = pMemberList->GetList();
	LIST_MEMBER_INFO_CIT it = pMembers.begin();
	for( ; it != pMembers.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		if( (*it)->m_nMapNo != nMapID )
			continue;

		NiPoint2 vTamerPos((*it)->m_tPos.m_nX, (*it)->m_tPos.m_nY);
		CsC_AvObject* pPartyTamer = g_pMngCollector->GetObject( (*it)->m_TamerInfo.m_nUID );
		if( pPartyTamer != NULL )
			vTamerPos = pPartyTamer->GetPos2D();
		else
		{
			if( vTamerPos == NiPoint2::ZERO )
				continue;
		}

		CsPoint vRenderPos = _CalZoneMapPos( fZoom, vTamerPos, CsPoint( 22, 22 ) );
		if( vRenderPos != CsPoint::ZERO )
		{
			CsRect rc( vRenderPos, CsSIZE( (int)( 22*fZoom ), (int)( 22*fZoom ) ) );
			if( rc.PtInRect( ptMouse ) )
			{
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( vRenderPos, CsPoint( (int)( 22*fZoom ), (int)( 22*fZoom ) ), 240, cTooltip::PARTY, (*it)->m_nSlotNo );
				return;
			}
		}		
	}	
}

void cMiniMap::Render()
{
	m_pMiniMap->Render();

//#if (defined VERSION_TW || defined VERSION_HK)
	RenderScript();
//#endif

	m_pMapText->Render();
	if( m_bRenderRegionText )
		m_pRegionText->Render();

	if( g_pNpcMng->GetNpcVector()->size() && (nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_DUNGEON))
	{
		m_pNpcFindBtn->Render();
	}

	if( m_eFieldType != CsAreaMap::FT_Dungeon )
	{
		m_pDownScaleBtn->Render();
		m_pUpScaleBtn->Render();
	}

	//위치찾기 취소버튼
	if( g_pHelpArrow->IsArrowRender() )
	{
		assert_cs( m_pPosFindCancelBtn );
		m_pPosFindCancelBtn->Render();
		CsPoint ptPos = GetRootClient() + CsPoint( 147, 185 );

		switch( g_pHelpArrow->GetInfo()->s_nFindWayType )
		{
		case cQuestFindWay::FindWay_Npc:
		case cQuestFindWay::FindWay_Monster:
		case cQuestFindWay::FindWay_Employment:
			if( m_pImage )
				m_pImage->Render_L( ptPos, CsPoint( 30, 30 ) );
			break;
		case cQuestFindWay::FindWay_Map:
		case cQuestFindWay::FindWay_Region:
			break;
		default:
			assert_cs( false );
		}
	}	

	if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_DATS )
	{
		_RenderParty();
		_RenderNpc();
	}
}

void cMiniMap::_RenderNpc()
{
	float fZoom = sqrt( m_fZoom );

	CsPoint ptRender;
	std::list< sNPC_INFO* >::iterator it = m_listNpcInfo.begin();
	std::list< sNPC_INFO* >::iterator itEnd = m_listNpcInfo.end();
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
		case CNpc::QV_NONE:
			nRenderIcon = -1;
			break;
		case CNpc::QV_NPCTALK:
			nRenderIcon = 2;
			break;
		default:						assert_cs( false );
		}

		// 퀘스트 관련 아이콘
		if( ( nRenderIcon == -1 )&&( (*it)->s_pNpc->GetQuestOwner() ) )
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

		// NPC 고유 아이콘
		if( nRenderIcon == -1 )
		{
			switch( (*it)->s_pFTNpc->GetInfo()->s_eType )
			{
			case nsCsNpcTable::NT_NONE:				
			case nsCsNpcTable::NT_TRADE:
			case nsCsNpcTable::NT_EVENT_STORE:
			case nsCsNpcTable::NT_DATS_PORTAL:
			case nsCsNpcTable::NT_MASTERS_MATCHING:
			case nsCsNpcTable::NT_SPIRIT_EVO:
			case nsCsNpcTable::NT_SPECIAL_EVENT:
				continue;

			case nsCsNpcTable::NT_DIGITAMA_TRADE:
				nRenderIcon = 3;
				break;

			case nsCsNpcTable::NT_PORTAL:
				nRenderIcon = 6;
				break;

			case nsCsNpcTable::NT_MAKE_TACTICS:
				nRenderIcon = 4;
				break;

			case nsCsNpcTable::NT_ELEMENT_ITEM:
				nRenderIcon = 5;
				break;

			case nsCsNpcTable::NT_WAREHOUSE:
				nRenderIcon = 7;
				break;

			case nsCsNpcTable::NT_TACTICSHOUSE:
				nRenderIcon = 13;
				break;		

			case nsCsNpcTable::NT_GUILD:
				nRenderIcon = 14;
				break;		

			case nsCsNpcTable::NT_DIGICORE:
				nRenderIcon = 3;
				break;

			case nsCsNpcTable::NT_CAPSULE_MACHINE:
				nRenderIcon = 15;
				break;

			case  nsCsNpcTable::NT_SKILL:
				nRenderIcon = 16;
				break;	

			case nsCsNpcTable::NT_PROPERTY_STORE:
				nRenderIcon = 3;
				break;
			case nsCsNpcTable::NT_MYSTERY_MACHINE:
			case nsCsNpcTable::NT_GOTCHA_MACHINE:
				nRenderIcon = 15;
				break;
			case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
				nRenderIcon = ICONMINIMAP::ITEM_PRODUCTION_NPC;
				break;
			case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
				nRenderIcon = ICONMINIMAP::ITEM_PRODUCTION_NPC;
				break;
			/*default:
				assert_cs( false );*/
			}
		}

		NiPoint2	mpos = ( *it )->s_vPos;

		ptRender = _CalZoneMapPos( fZoom, mpos, CsPoint( 22, 16 ) );

		if( ptRender != CsPoint::ZERO && nRenderIcon != -1 )
		{
			// 랜더링
			g_pIconMng->RenderIcon( ICONITEM::MiniMap, ptRender, CsPoint( (int)( 22.0f*fZoom ), (int)( 16.0f*fZoom ) ), nRenderIcon );
		}
	}
}

void cMiniMap::_RenderParty()
{
	SPartyInfo const* pMemberList = GetSystem()->GetPartyInfo();
	SAFE_POINTER_RET( pMemberList );

	if( !pMemberList->IsParty() )
		return;

 	float fZoom = sqrt( m_fZoom );
	DWORD dwCurMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	LIST_MEMBER_INFO const& pMembers = pMemberList->GetList();
	LIST_MEMBER_INFO_CIT it = pMembers.begin();
	for( ; it != pMembers.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		if( (*it)->m_nMapNo != dwCurMapID )
			continue;

		NiPoint2 vTamerPos((*it)->m_tPos.m_nX, (*it)->m_tPos.m_nY);
		CsC_AvObject* pPartyTamer = g_pMngCollector->GetObject( (*it)->m_TamerInfo.m_nUID );
		if( pPartyTamer != NULL )
			vTamerPos = pPartyTamer->GetPos2D();
		else
		{
			if( vTamerPos == NiPoint2::ZERO )
				continue;
		}

		CsPoint vRenderPos = _CalZoneMapPos( fZoom, vTamerPos, CsPoint( 22, 22 ) );
		if( vRenderPos != CsPoint::ZERO )
			(*it)->m_DigimonInfo.RenderImg( vRenderPos, CsPoint( (int)( 22*fZoom ), (int)( 22*fZoom ) ) );
			//pPartyInfo->s_DigimonStat.s_pImage->Render_S( vRenderPos, CsPoint( (int)( 22*fZoom ), (int)( 22*fZoom ) ) );
	}
}

void cMiniMap::ResetDevice()
{
	ResetDeviceScript();

	m_pNpcFindBtn->ResetDevice();
	m_pDownScaleBtn->ResetDevice();
	m_pUpScaleBtn->ResetDevice();
	m_pNpcCancel->ResetDevice();
	m_pMapCancel->ResetDevice();
	if( m_pImage )
		m_pImage->ResetDevice();
	m_pMiniMap->ResetDevice();
	m_pMapText->ResetDevice();
	m_pRegionText->ResetDevice();
}

void cMiniMap::SetPosFind()
{
	assert_cs( g_pHelpArrow->IsArrowRender() );
	switch( g_pHelpArrow->GetInfo()->s_nFindWayType )
	{
	case cQuestFindWay::FindWay_Npc:	
		{
			m_pPosFindCancelBtn = m_pNpcCancel;

			CsNpc* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( g_pHelpArrow->GetInfo()->s_nFileID ); 
			assert_csm1( pFTNpc != NULL, _T( " Npc정보 없음 번호 : %d " ), g_pHelpArrow->GetInfo()->s_nFileID );

			if( pFTNpc != NULL )
			{
				g_pCharResMng->CharImageResDelete( &m_pImage );	
				m_pImage = g_pCharResMng->CharImageResLoad( pFTNpc->GetInfo()->s_dwModelID );					
			}				
		}break;
	case cQuestFindWay::FindWay_Monster:
		{
			m_pPosFindCancelBtn = m_pNpcCancel;

			CsMonster* pFTMon = nsCsFileTable::g_pMonsterMng->GetMonster( g_pHelpArrow->GetInfo()->s_nFileID );
			assert_csm1( pFTMon != NULL, _T( " Mon정보 없음 번호 : %d " ), g_pHelpArrow->GetInfo()->s_nFileID );

			if( pFTMon != NULL )
			{			
				g_pCharResMng->CharImageResDelete( &m_pImage );	
				m_pImage = g_pCharResMng->CharImageResLoad( pFTMon->GetInfo()->s_dwModelID );				
			}
		}break;
	case cQuestFindWay::FindWay_Map:
		m_pPosFindCancelBtn = m_pMapCancel;
		break;

	case cQuestFindWay::FindWay_Employment:
		{
			m_pPosFindCancelBtn = m_pNpcCancel;
			g_pCharResMng->CharImageResDelete( &m_pImage );

			int iModelID = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_GET_CHAR_MODELID, &iModelID);
			m_pImage = g_pCharResMng->CharImageResLoad( iModelID );				
		}break;

	case cQuestFindWay::FindWay_Region:
		{

		}break;
	default:
		assert_cs(false);
	}
}


bool cMiniMap::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;
	return true;
}

void cMiniMap::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}
