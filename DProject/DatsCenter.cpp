#include "StdAfx.h"
#include "DatsCenter.h"

#include "Flow/Flow.h"
#include "Flow/FlowMgr.h"

CDatsCenter*	 g_pDatsCenter = NULL;

CDatsCenter::CDatsCenter(void)
{		
	m_vPos = NiPoint3::ZERO;
	m_pSprite1 = NULL;	
	m_pSprite2 = NULL;	
	//m_pSprTitle = NULL;
	
	m_pSprCardSuccess = NULL;
	m_pSprCardFail = NULL;	
	m_pSpriteNo = NULL;	
}

CDatsCenter::~CDatsCenter(void)
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void CDatsCenter::Start()
{
	m_fBackupDistMin = CAMERA_ST.GetDistRange_Min();
	m_fBackupDistMax = CAMERA_ST.GetDistRange_Max();

	m_bIsSound = g_pResist->IsSound();
	g_pResist->SetEnableSound( false );
	g_pSoundMgr->Play_Stop_Sound();

	CsC_AvObject::g_bEnableVoice = false;
	g_pMngCollector->SetSceneState( CMngCollector::eSCENE_DATS );	

	PlayMusic( "Enchant_Room.mp3" );
	g_pGameIF->GetMiniMap()->LoadDatsMiniMap( true );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SMALLMAP ) )	
		g_pGameIF->GetSmallMap()->LoadSmallMap();
	
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ZONEMAP ) )	
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ZONEMAP );
	
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TARGET_WINDOW ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TARGET_WINDOW );

	if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_TAMERSTATUS ) )
	{
		bool bSetTab = false;
		int nTabNo = 0;
		ContentsStream kTmp;
		kTmp << bSetTab << nTabNo;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CLOSE_TAMERSTATUS, &kTmp );
	}

	ContentsStream kTmp;
	UINT nFTID = 0, nObjType = 0;
	kTmp << nFTID << nObjType;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ACTIONKEY_NPC, &kTmp );

	// 강화실로 들어오면 채널 UI를 닫아줍니다.
	if(g_pGameIF && g_pGameIF->IsActiveWindow(cBaseWindow::WT_CHANNEL) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CHANNEL );

	m_eState = eSTATE_NOR;

	// 이동 중지	
	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();		
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();	

	NiPoint3 vPos;	
	vPos = NiPoint3( 0.0f, 0.0f, -100000.0f );

#define Dats_Center		"Data/etcobject/DatsCenter/F_DATS_base.nif"
	NiStream kStream;
	if( !kStream.Load( Dats_Center ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", Dats_Center );
		return;
	}

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_DatsCenter.SetNiObject( pNode, CGeometry::Normal );	
	m_DatsCenter.m_pNiNode->SetTranslate( vPos );

	NiTimeController::StartAnimations( m_DatsCenter.m_pNiNode, (float)g_fAccumTime );
	//nsCSGBFUNC::Set_Emittance( m_DatsCenter.m_pNiNode, NiColor( 1.0f, 1.0f, 1.0f ) );
	m_DatsCenter.m_pNiNode->UpdateEffects();
	m_DatsCenter.m_pNiNode->Update(0.0f);

#define C_Gradation		"Data/etcobject/DatsCenter/C_Gradation_0001.nif"	
	if( !kStream.Load( C_Gradation ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", C_Gradation );
		return;
	}

	pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_PickInchantObj.SetNiObject( pNode, CGeometry::Normal );	
	m_PickInchantObj.m_pNiNode->SetTranslate( vPos );

	NiTimeController::StartAnimations( m_PickInchantObj.m_pNiNode, (float)g_fAccumTime );
	nsCSGBFUNC::Set_Emittance( m_PickInchantObj.m_pNiNode, NiColor( 1.0f, 1.0f, 1.0f ) );


#define D_Gradation		"Data/etcobject/DatsCenter/D_Gradation_0001.nif"	
	if( !kStream.Load( D_Gradation ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", D_Gradation );
		return;
	}

	pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_PickExitObj.SetNiObject( pNode, CGeometry::Normal );	
	m_PickExitObj.m_pNiNode->SetTranslate( vPos );

	NiTimeController::StartAnimations( m_PickExitObj.m_pNiNode, (float)g_fAccumTime );
	nsCSGBFUNC::Set_Emittance( m_PickExitObj.m_pNiNode, NiColor( 1.0f, 1.0f, 1.0f ) );	

	// 페이드 아웃	
//	SAFE_DELETE( g_pFade );
//	g_pFade = NiNew CFade( FADE_IN, 0.8f );		

	// 카메라 설정			
	CAMERA_ST.SetTranslate( NiPoint3( -200.0f, -1400.0f, -99700.0f ) );			
	CAMERA_ST.SetRotation( 0.27f, -0.02f );
	CAMERA_ST.SetDistRange( 300.0f, 2000.01f );	
	CAMERA_ST.SetDeltaHeight( 120.0f );
	CAMERA_ST.SetDist(2000.0f);
	CAMERA_ST._UpdateCamera();
	_SetDefaultCamera();	
	
	m_nPicked = 0;

	NiPoint3 vCharPos;
	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = OP_ABVWIRE | OP_LIGHT;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	NiAVObject* pBox1Node = m_DatsCenter.m_pNiNode->GetObjectByName( "Digimon_Box01" );
	float fBoxHeight = 330.0f;
	if( pBox1Node )
		fBoxHeight = pBox1Node->GetWorldBound().GetRadius() * 2.0f;

	vCharPos = NiPoint3( vPos + pBox1Node->GetTranslate() ); 	
	vCharPos.z -= 150.0f;

	DWORD dwID = g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID();
	m_pDigimon = g_pCharMng->AddTempDigimon( &prop, dwID, NULL, vCharPos, 0.0f, NULL );	

	float digimonScale = fBoxHeight / m_pDigimon->CsC_AvObject::GetToolHeight();

	m_pDigimon->SetScale( digimonScale );
	m_pDigimon->SetAnimation( ANI::IDLE_NORMAL ); 	

	prop.s_eInsertGeomType = CGeometry::GetCharOutLineGrayType();

	NiAVObject* pBox2Node = m_DatsCenter.m_pNiNode->GetObjectByName( "Digimon_Box02" );
	vCharPos = NiPoint3( vPos + pBox2Node->GetTranslate() );	
	vCharPos.z -= 150.0f;

	m_pDigimonClon = g_pCharMng->AddTempDigimon( &prop, dwID, NULL, vCharPos, 0.0f, NULL );
	m_pDigimonClon->SetScale( digimonScale );
	m_pDigimonClon->SetAnimation( ANI::IDLE_NORMAL );
}

void CDatsCenter::End()
{
	g_pMngCollector->ReleaseScene();

	// 페이드 아웃	
//	SAFE_DELETE( g_pFade );
//	g_pFade = NiNew CFade( FADE_IN, 0.4f );

	sCAMERAINFO ci;	
	ci.s_fDist = 1000.0f;
	ci.s_fInitRoll = NI_PI;
	ci.s_fInitPitch += NI_PI*0.4f;
	ci.s_fFarPlane = 200000.0f;
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.SetDistRange( m_fBackupDistMin, m_fBackupDistMax );
	CAMERA_ST.SetRotationLimit( -CsD2R( 35 ), CsD2R( 70 ) );
	//CAMERA_ST.SetDeltaHeight( 120.0f );
	CAMERA_ST.SetDistAccel( 1.5f );

	g_pResist->SetEnableSound( m_bIsSound );
	CsC_AvObject::g_bEnableVoice = g_pResist->m_Global.s_bEnableVoice;
	g_pCharMng->GetTamerUser()->PlayBGM();	

	// 디지몬 이펙트 변경
	g_pCharMng->GetDigimonUser( 0 )->CheckEnchantEffect();

	g_pCharMng->DeleteTempChar( m_pDigimon );
	m_pDigimon = NULL;

	g_pCharMng->DeleteTempChar( m_pDigimonClon );
	m_pDigimonClon = NULL;		

	g_pGameIF->GetMiniMap()->LoadDatsMiniMap( false );	

	PlayMusic( _LoadMusic() );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SMALLMAP ) )	
	{
		if( g_pTerrainMng )
		{
			DWORD nMap = g_pTerrainMng->GetBackupMapID();
			g_pGameIF->GetSmallMap()->LoadSmallMap2( nMap );	// 닷츠본부내부에서 나올때 스몰맵이 Active상태이면 재로드
		}
		else
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SMALLMAP );
	}
}
	

void CDatsCenter::Delete()
{	
	m_PickInchantObj.Delete();
	m_PickExitObj.Delete();

	m_DatsCenter.Delete();
	SAFE_NIDELETE( m_pSprite1 );
	SAFE_NIDELETE( m_pSprite2 );
	//SAFE_NIDELETE( m_pSprTitle );	
	SAFE_NIDELETE( m_pSprCardSuccess );
	SAFE_NIDELETE( m_pSprCardFail );	
	SAFE_NIDELETE( m_pSpriteNo );
	SAFE_NIDELETE( m_pAniCard );
	SAFE_NIDELETE( m_pAniCardSuccess );
	SAFE_NIDELETE( m_pAniCardFail );
	SAFE_NIDELETE( m_pAniCardSelect );
	SAFE_NIDELETE( m_pAniSuccess );
	SAFE_NIDELETE( m_pAniFail );
} 

void CDatsCenter::ResetDevice()
{	
	if( m_pSprite1 )
		m_pSprite1->ResetDevice();
	if( m_pSprite2 )
		m_pSprite2->ResetDevice();		
	//if( m_pSprTitle )
	//	m_pSprTitle->ResetDevice();		
	if( m_pSprCardSuccess )
		m_pSprCardSuccess->ResetDevice();	
	if( m_pSprCardFail )
		m_pSprCardFail->ResetDevice();	
	if( m_pSpriteNo )
		m_pSpriteNo->ResetDevice();	
	if( m_pAniCard )
		m_pAniCard->ResetDevice();
	if( m_pAniCardSelect )
		m_pAniCardSelect->ResetDevice();
	if( m_pAniCardSuccess )
		m_pAniCardSuccess->ResetDevice();
	if( m_pAniCardFail )
		m_pAniCardFail->ResetDevice();
	if( m_pAniSuccess )
		m_pAniSuccess->ResetDevice();
	if( m_pAniFail )
		m_pAniFail->ResetDevice();
}

void CDatsCenter::Init()
{
	Start();	

	// 메인 바 생성			
	float fViewSize = (float)g_nScreenHeight * 0.28f;
	m_pSprite1 = NiNew cSprite;			
	m_pSprite1->Init( NULL, CsPoint( 0, 10 ), CsPoint( (int)g_nScreenWidth, (int)fViewSize ), "EnchantTactics\\card_base.tga", false );
	m_pSprite1->SetAlpha( 0.8f );

	fViewSize = (float)g_nScreenHeight * 0.24f;
	m_pSprite2 = NiNew cSprite;								
	m_pSprite2->Init( NULL, CsPoint( 0, (int)(g_nScreenHeight - fViewSize - 10) ), CsPoint( (int)g_nScreenWidth, (int)fViewSize ), "EnchantTactics\\card_base2.tga", false );
	m_pSprite2->SetAlpha( 0.8f );	

	//m_pSprTitle = NiNew cSprite;								
	//m_pSprTitle->Init( NULL, CsPoint::ZERO, CsPoint( 358, 32 ), "EnchantTactics\\card_title.tga", false );

	m_pSpriteNo = NiNew cSprite;								
	m_pSpriteNo->Init( NULL, CsPoint::ZERO, CsPoint( 375, 178 ), "EnchantTactics\\NO.tga", false );
	m_pSpriteNo->SetColor( NiColor( 1, 1, 0 ) );

	m_pSprCardSuccess = NiNew cSprite;								
	m_pSprCardSuccess->Init( NULL, CsPoint::ZERO, CsPoint( 130, 190 ), "EnchantTactics\\CardSuccess\\Succes_R_20.tga", false );

	m_pSprCardFail = NiNew cSprite;								
	m_pSprCardFail->Init( NULL, CsPoint::ZERO, CsPoint( 130, 190 ), "EnchantTactics\\CardFail\\Fail_R_20.tga", false );	

	m_pAniCard = NiNew cSpriteAni;
	m_pAniCard->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( 130, 190 ), "EnchantTactics\\CardAni\\Card_R_", "tga", 20, false );		
	m_pAniCard->SetAniTime( 0.05f );	

	m_pAniCardSelect = NiNew cSpriteAni;
	m_pAniCardSelect->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( 130, 190 ), "EnchantTactics\\SelectAni\\Card_C_", "tga", 20, false );		
	m_pAniCardSelect->SetAniTime( 0.05f );	

	m_pAniCardSuccess = NiNew cSpriteAni;
	m_pAniCardSuccess->Init( cSpriteAni::ONCE, NULL, CsPoint::ZERO, CsPoint( 130, 190 ), "EnchantTactics\\CardSuccess\\Succes_R_", "tga", 20, false );		
	m_pAniCardSuccess->SetAniTime( 0.03f );	

	m_pAniCardFail = NiNew cSpriteAni;
	m_pAniCardFail->Init( cSpriteAni::ONCE, NULL, CsPoint::ZERO, CsPoint( 130, 190 ), "EnchantTactics\\CardFail\\Fail_R_", "tga", 20, false );		
	m_pAniCardFail->SetAniTime( 0.03f );	

	m_pAniSuccess = NiNew cSpriteAni;
	m_pAniSuccess->Init( cSpriteAni::ONCE, NULL, CsPoint::ZERO, CsPoint( 1078, 300 ), "EnchantTactics\\SuccessAni\\Succes_", "tga", 30, false );		
	m_pAniSuccess->SetAniTime( 0.15f );	

	m_pAniFail = NiNew cSpriteAni;
	m_pAniFail->Init( cSpriteAni::ONCE, NULL, CsPoint::ZERO, CsPoint( 1078, 300 ), "EnchantTactics\\FailAni\\Fail_", "tga", 30, false );		
	m_pAniFail->SetAniTime( 0.15f );	
	
}

void CDatsCenter::_SetDefaultCamera()
{
	m_fCameraRot = 0.22f;
	m_fCameraPitch = 0.02f;
	m_fCameraDist = 2000.0f;
	CAMERA_ST.SetCameraDist( m_fCameraDist );
}

void CDatsCenter::_SetEnchentCamera()
{
	m_fCameraRot = 0.22f;
	m_fCameraPitch = 0.02f;
	m_fCameraDist = 300.0f;	
	CAMERA_ST.SetCameraDist( m_fCameraDist );
}

char const* CDatsCenter::_LoadMusic()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	if( nsCsMapTable::g_pMapListMng->IsList( dwMapID ) == false )
		return NULL;

	CsMapRegion::sINFO* pInfo = NULL;
	float fLength;
	NiPoint2 ptSrc = NiPoint2( g_pCharMng->GetTamerUser()->GetPos().x, g_pCharMng->GetTamerUser()->GetPos().y );
	CsMapRegionGroup* pGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( dwMapID );
	CsMapRegion::LIST* pList = pGroup->GetRegionList();
	CsMapRegion::LIST_IT it = pList->begin();
	for( ; it!=pList->end(); ++it )
	{
		pInfo = (*it)->GetInfo();
		fLength = ( ptSrc - NiPoint2( (float)pInfo->s_nCenterX, (float)pInfo->s_nCenterY ) ).Length();
		if( fLength < pInfo->s_nRadius )
		{
			if( pInfo->s_cBGSound[ 0 ] != NULL )
				return pInfo->s_cBGSound;
			break;
		}
	}

	CsMapList::sINFO* pListInfo = nsCsMapTable::g_pMapListMng->GetList( dwMapID )->GetInfo();	
	if( pListInfo )
		return pListInfo->s_cBGSound.c_str();
	return NULL;
}

void CDatsCenter::Update( float fDeltaTime, cGlobalInput::eMOUSE_INPUT mi )
{	
	switch( m_eState ) 
	{
	case eSTATE_NOR:
		{
			if( mi == cGlobalInput::MOUSEINPUT_PICK )
			{
				POINT pt;
				GetCursorPos( &pt );
				ScreenToClient( GAMEAPP_ST.GetHWnd(), &pt );	

				CsGBPick pick( false, NiPick::NO_SORT );
				float fLenth = FLT_MAX;
				NiPoint3 origin, dir;
				CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );

				if( pick.Pick( m_DatsCenter.m_pNiNode->GetObjectByName( "ehchant_terminal" ), origin, dir, false ) == true )	
				{
					m_nPicked = 1;	
					m_PickInchantObj.m_pNiNode->Update( (float)g_fAccumTime );
					if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
					{
						_SetEnchentCamera();
						m_eState = eSTATE_ENCHENT;						
						m_nPicked = 0;
						m_bOpenUI = false;
					}
				}
				else if( pick.Pick( m_DatsCenter.m_pNiNode->GetObjectByName( "exit" ), origin, dir, false ) == true )	
				{
					m_nPicked = 2;	
					m_PickExitObj.m_pNiNode->Update( (float)g_fAccumTime );
					if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
					{
						g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ENCHANT_TACTICS ); 
						GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_INVENTORY, NULL );		
						m_nPicked = 0;
						//g_pMngCollector->EndDatsCenter();
						FLOWMGR_ST.PopFlow( Flow::CFlow::FLW_DATSCENTER );
						return;
					}
				}
				else
				{
					m_nPicked = 0;
				}	
			}

			if( CAMERA_ST.GetDist() < m_fCameraDist )
			{
// 				if( m_fCameraDist - CAMERA_ST.GetDist() > 1.0f ) 2015-11-30-nova 삭제 예정
// 				{
// 					float fDelta = (m_fCameraDist - CAMERA_ST.GetDist()) * fDeltaTime * 1.8f; 
// 					CAMERA_ST.SetDistRange( CAMERA_ST.GetDist() + fDelta, CAMERA_ST.GetDist() + fDelta + 0.01f );					
// 				}
// 				else
// 				{
// 					CAMERA_ST.SetDistRange( m_fCameraDist, m_fCameraDist + 0.01f );										
// 				}		
				float fRot = CAMERA_ST.GetCurRoll() + ( m_fCameraRot * fDeltaTime * 1.8f );
				if( fRot > 0.27f )
					fRot = 0.27f;

				float fPitch = CAMERA_ST.GetCurPitch() + ( m_fCameraPitch * fDeltaTime * 1.8f );
				if( fPitch > -0.02f )
					fPitch = -0.02f;

				CAMERA_ST.SetRotation( fRot, fPitch );
			}						
		}		
		break;
	case eSTATE_ENCHENT:	
		{	
			if( CAMERA_ST.GetDist() > m_fCameraDist )
			{
// 				if( CAMERA_ST.GetDist() - m_fCameraDist > 1.0f )	2015-11-30-nova 삭제 예정
// 				{
// 					float fDelta = (CAMERA_ST.GetDist() - m_fCameraDist) * fDeltaTime * 1.8f;
// 					CAMERA_ST.SetDistRange( CAMERA_ST.GetDist() - fDelta, CAMERA_ST.GetDist() - fDelta + 0.01f );				
// 				}
// 				else
// 				{
// 					CAMERA_ST.SetDistRange( m_fCameraDist, m_fCameraDist + 0.01f );
// 				}		

				float fRot = CAMERA_ST.GetCurRoll() - ( m_fCameraRot * fDeltaTime * 1.8f );
				if( fRot < 0.05f )
					fRot = 0.05f;

				float fPitch = CAMERA_ST.GetCurPitch() - ( m_fCameraPitch * fDeltaTime * 1.8f );
				if( fPitch < 0.02f )
					fPitch = 0.02f;

				CAMERA_ST.SetRotation( fRot, fPitch );
			}				
			if( m_bOpenUI == false )
			{
				if( (int)(m_fCameraDist + 100.0f) > (int)CAMERA_ST.GetDist() )
				{
					g_pGameIF->GetDynamicIF( cBaseWindow::WT_ENCHANT_TACTICS );
					bool bSound = false;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );
					m_bOpenUI = true;
				}
			}
		}		
		break;
	case eSTATE_SELECT_ANI:
		{
			bool bEnd = true;
			for( int i = 0; i < MAX_CARD_NUM; i++ )
			{
				if( m_Card[ i ].s_rcCard.GetPos().x != (int)(m_Card[ i ].s_ptPos.x) )
					bEnd = false;
				
				switch( i % (MAX_CARD_NUM / 2) )
				{
				case 0:				
				case 1:					
				case 2:
					{
						m_Card[ i ].s_ptPos.x += ( m_Card[ i ].s_rcCard.GetPos().x - (g_nScreenWidth/2 - 65) ) / 30.0f;						
						if( m_Card[ i ].s_ptPos.x < m_Card[ i ].s_rcCard.GetPos().x )
							m_Card[ i ].s_ptPos = NiPoint2( (float)m_Card[ i ].s_rcCard.GetPos().x, (float)m_Card[ i ].s_rcCard.GetPos().y );
					}
					break;
				case 3:
					m_Card[ i ].s_ptPos = NiPoint2( (float)m_Card[ i ].s_rcCard.GetPos().x, (float)m_Card[ i ].s_rcCard.GetPos().y );
					break;
				case 4:					
				case 5:					
				case 6:
					{						
						m_Card[ i ].s_ptPos.x += ( m_Card[ i ].s_rcCard.GetPos().x - (g_nScreenWidth/2 - 65) ) / 30.0f;						
						if( m_Card[ i ].s_ptPos.x > m_Card[ i ].s_rcCard.GetPos().x )
							m_Card[ i ].s_ptPos = NiPoint2( (float)m_Card[ i ].s_rcCard.GetPos().x, (float)m_Card[ i ].s_rcCard.GetPos().y );
					}					
					break;
				}
				
			}

			m_pAniCard->Update( fDeltaTime );

			if( bEnd == true )
				SetSelectMode();
		}
		break;
	case eSTATE_SELECT:
		{	
			if( m_SelectTime.RemainDeltaTime() == 0 )
			{			
				m_nSelect = rand() % MAX_CARD_NUM;
				SetSelectCard( false );				
			}
			else
			{
				m_nOver = -1;
				for( int i = 0; i < MAX_CARD_NUM; i++ )
				{				
					if( m_Card[ i ].s_rcCard.PtInRect( CURSOR_ST.GetPos() ) == TRUE )
					{
						m_nOver = i;
						if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
						{
							m_nSelect = i;
							SetSelectCard( true );
						}
					}
				}
			}			

			m_pAniCard->Update( fDeltaTime );
			m_pAniCardSelect->Update( fDeltaTime );
		}
		break;
	case eSTATE_SELECT_END:
		{
			m_pAniSuccess->Update( fDeltaTime );			
			m_pAniFail->Update( fDeltaTime );
			m_pAniCardSuccess->Update( fDeltaTime );
			m_pAniCardFail->Update( fDeltaTime );
			m_pAniCardSelect->Update( fDeltaTime );

			if( m_SelectTime.RemainDeltaTime() == 0 )
			{
				EndSelectCard();
			}
		}
	}		
	
	CAMERA_ST._UpdateCamera();
	CAMERA_ST.SetDefaultFarFrustumPlane();	

	m_DatsCenter.m_pNiNode->Update( (float)g_fAccumTime );		
	m_pDigimon->CsC_AvObject::Update( g_fDeltaTime );	

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCHANT_TACTICS ) == true )
	{
		if( g_pGameIF->GetEnchantTactics()->IsDigiClon() == true )
			m_pDigimonClon->CsC_AvObject::Update( g_fDeltaTime );	
	}
}

void CDatsCenter::Render()
{
	switch( m_nPicked )
	{
	case 1:
		{			
			m_PickInchantObj.Render();
		}
		break;
	case 2:
		{			
			m_PickExitObj.Render();
		}
		break;		
	}	
	
	CsNodeObj* pNode;
	pNode = m_pDigimon->GetCsNode();
	if( pNode )
	{			
		m_pDigimon->CsC_AvObject::Render();
	}	

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCHANT_TACTICS ) == true )
	{
		if( g_pGameIF->GetEnchantTactics()->IsDigiClon() == true )
		{
			pNode = m_pDigimonClon->GetCsNode();
			if( pNode )
			{			
				m_pDigimonClon->CsC_AvObject::Render();
			}	
		}			
	}

	m_DatsCenter.Render();		

	g_pEngine->ScreenSpace();

	switch( m_eState )
	{
	case eSTATE_NOR:
	case eSTATE_ENCHENT:
		break;
	case eSTATE_SELECT_ANI:
		{
			m_pSprite1->Render();
			m_pSprite2->Render();

			for( int i = 0; i < MAX_CARD_NUM; i++ )
			{
				m_pAniCard->Render( CsPoint( (int)m_Card[ i ].s_ptPos.x , (int)m_Card[ i ].s_ptPos.y ) );				
			}			
		}
		break;
	case eSTATE_SELECT:
		{
			m_pSprite1->Render();
			m_pSprite2->Render();
			//m_pSprTitle->Render(); 

			for( int i = 0; i < MAX_CARD_NUM; i++ )			
			{
				if( m_nOver == i )
				{					
					m_pAniCard->Render( m_Card[ i ].s_rcCard.GetPos() );
				}
				else
				{				
					m_pAniCard->Render( m_Card[ i ].s_rcCard.GetPos() );
				}
			}

			if( m_nOver != -1 )
				m_pAniCardSelect->Render( m_Card[ m_nOver ].s_rcCard.GetPos() );
						
			int nSize = ( m_SelectTime.RemainDeltaTime() % 1000 ) / 3;
			int nNum = m_SelectTime.RemainDeltaTime() / 1000;	
			if( nNum != m_nOldTime )
			{
				m_nOldTime = nNum;
				if( nNum > 3 )
					PlaySystemSound( "game\\Card_Count.mp3" );
				else
					PlaySystemSound( "game\\Card_Count_B.mp3" );
			}

			if( nNum > 4 )			
			{
				m_pSpriteNo->Render( CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 2 ) - CsPoint( nSize / 2 , nSize / 2 ), 
				CsPoint( nSize, nSize ), NiPoint2( nNum/5.0f, 0.5f ), NiPoint2( ( nNum + 1 )/5.0f, 1.0f ) );				
			}
			else
			{
				m_pSpriteNo->Render( CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 2 ) - CsPoint( nSize / 2, nSize / 2 ), 
				CsPoint( nSize, nSize ), NiPoint2( nNum/5.0f, 0 ), NiPoint2( ( nNum + 1 )/5.0f, 0.5f ) );
			}			
		}
		break;
	case eSTATE_SELECT_END:
		{
			m_pSprite1->Render();
			m_pSprite2->Render();
			//m_pSprTitle->Render(); 

			for( int i = 0; i < MAX_CARD_NUM; i++ )
			{
				if( m_Card[ i ].s_bSuccess == false )
				{
					if( m_pAniCardFail->IsPlay() == true )
						m_pAniCardFail->Render( m_Card[ i ].s_rcCard.GetPos() );
					else
						m_pSprCardFail->Render( m_Card[ i ].s_rcCard.GetPos() );
				}
				else
				{
					if( m_pAniCardSuccess->IsPlay() == true )
						m_pAniCardSuccess->Render( m_Card[ i ].s_rcCard.GetPos() );
					else
						m_pSprCardSuccess->Render( m_Card[ i ].s_rcCard.GetPos() );
				}
			}

			if( m_nSelect != -1 )
				m_pAniCardSelect->Render( m_Card[ m_nSelect ].s_rcCard.GetPos() );		

			if( m_bResult == true )
				m_pAniSuccess->Render( CsPoint( g_nScreenWidth / 2 - 1078 / 2, g_nScreenHeight / 2 + (int)(g_nScreenHeight * 0.005f) ) );
			else
				m_pAniFail->Render( CsPoint( g_nScreenWidth / 2 - 1078 / 2, g_nScreenHeight / 2 + (int)(g_nScreenHeight * 0.005f) ) );
		}
		break;
	}
	g_pEngine->ResetRendererCamera();
}

void CDatsCenter::SetSelectCard( bool bUserSelect )
{
	if( net::game == NULL )
	{
		//EnchantSuccess( 1 );
		EnchantFail( 1 );
	}
	else
	{
		assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCHANT_TACTICS ) == true );
		if( g_pGameIF->GetEnchantTactics()->SendEnchantServer() == false )
		{
			if( bUserSelect == true )
				cPrintMsg::PrintMsg( 10004 );						
		}
	}		
}

void CDatsCenter::SetSelectMode()
{
	m_eState = eSTATE_SELECT;	
	
	m_nOver = m_nSelect = -1;	
	m_SelectTime.SetDeltaTime( 1000*10 );
	m_SelectTime.OffReset();
	m_SelectTime.SetEnableTime( true );
}

void CDatsCenter::StartSelectCard()
{
	m_eState = eSTATE_SELECT_ANI;
	g_pGameIF->SetRenderIF( false );

	m_nOver = m_nSelect = -1;	
	m_SelectTime.SetDeltaTime( 1000*10 );
	m_SelectTime.OffReset();
	m_SelectTime.SetEnableTime( true );

	float fViewSize = (float)g_nScreenHeight * 0.24f;
	for( int i = 0; i < MAX_CARD_NUM; i++ )
	{
		if( i < MAX_CARD_NUM/2 )
		{
			int nDelta = (int)(g_nScreenHeight * 0.04f);
			m_Card[ i ].s_rcCard = CsRect( CsPoint( g_nScreenWidth/2 - 65 + (140 * (i-3)), nDelta ), CsSIZE( 130, 190 ) ); 
			m_Card[ i ].s_ptPos = NiPoint2( (float)(g_nScreenWidth / 2 - 65), (float)nDelta );
		}
		else
		{
			int nDelta = (int)(g_nScreenHeight * 0.005f);
			m_Card[ i ].s_rcCard = CsRect( CsPoint( g_nScreenWidth/2 - 65 + (140 * (i-10)), (int)(g_nScreenHeight - fViewSize + nDelta ) ), CsSIZE( 130, 190 ) ); 
			m_Card[ i ].s_ptPos = NiPoint2( (float)(g_nScreenWidth / 2 - 65), (float)(g_nScreenHeight - fViewSize + nDelta ) );
		}				
	}	

	PlaySystemSound( "game\\Card_Select.mp3" );
}

void CDatsCenter::EndSelectCard()
{
	m_eState = eSTATE_ENCHENT;
	g_pGameIF->SetRenderIF( true );
}

void CDatsCenter::EnchantEnd()
{
	m_eState = eSTATE_NOR;
	_SetDefaultCamera();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_INVENTORY, NULL );
}

void CDatsCenter::EnchantSuccess( int nSelect )
{
	m_bResult = true;
	m_pAniFail->PlayOn( true );
	m_pAniSuccess->PlayOn( true );
	m_pAniCardFail->PlayOn( true );
	m_pAniCardSuccess->PlayOn( true );
	m_pDigimon->GetProp_Effect()->AddEffect( "System\\Tactics_success.nif", 2.0f, nsEFFECT::POS_CHARPOS );	
	PlaySystemSound( "game\\Enchant_Success.mp3" );

	m_eState = eSTATE_SELECT_END;

	m_SelectTime.SetDeltaTime( 1000*5 );
	m_SelectTime.OffReset();
	m_SelectTime.SetEnableTime( true );

	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pDUser );
	CDigimonUser::sENCHENT_STAT* pEStat = pDUser->GetEnchantStat();
	SAFE_POINTER_RET( pEStat );
	int nLevel = 0;	
	switch( nSelect )
	{
	case 0:
		nLevel = pEStat->GetEnchantLv( ET_AT );
		break;
	case 1:
		nLevel = pEStat->GetEnchantLv( ET_BL );
		break;
	case 2:
		nLevel = pEStat->GetEnchantLv( ET_CR );
		break;	
	case 3:
		nLevel = pEStat->GetEnchantLv( ET_EV );
		break;	
	case 4:
		nLevel = pEStat->GetEnchantLv( ET_HP );
		break;
		/*case 5:
		nAbility = ET_AS;
		break;*/
	default:
		assert_csm( false, _T( "잘못된 능력치 선택" ) );
	}	

	int nSuccessCount = 0;
	if( nLevel < 4 )
		nSuccessCount = 11;
	else if( nLevel < 7 )
		nSuccessCount = 8;
	else if( nLevel < 10 )
		nSuccessCount = 5;
	else
		nSuccessCount = 2;	

	for( int i = 0; i < MAX_CARD_NUM; i++ )
		m_Card[ i ].s_bSuccess = false;

	m_Card[ m_nSelect ].s_bSuccess = true;
	while( nSuccessCount != 0 )
	{
		int nIdx = rand() % MAX_CARD_NUM;
		if( nIdx == m_nSelect ||
			m_Card[ nIdx ].s_bSuccess == true )
			continue;
		else
		{
			m_Card[ nIdx ].s_bSuccess = true;
			nSuccessCount--;
		}
	}

	assert_cs( nSuccessCount == 0 );
}

void CDatsCenter::EnchantFail( int nSelect )
{
	m_bResult = false;
	m_pAniFail->PlayOn( true );
	m_pAniSuccess->PlayOn( true );
	m_pAniCardFail->PlayOn( true );
	m_pAniCardSuccess->PlayOn( true );
	m_pDigimon->GetProp_Effect()->AddEffect( "System\\Tactics_enchant_fail.nif", 2.0f, nsEFFECT::POS_CHARPOS );
	PlaySystemSound( "game\\Enchant_Fail.mp3" );

	m_eState = eSTATE_SELECT_END;

	m_SelectTime.SetDeltaTime( 1000*5 );
	m_SelectTime.OffReset();
	m_SelectTime.SetEnableTime( true );

	int nLevel = 0;	
	switch( nSelect )
	{
	case 0:
		nLevel = ((CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat()->GetEnchantLv( ET_AT );
		break;
	case 1:
		nLevel = ((CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat()->GetEnchantLv( ET_BL );
		break;
	case 2:
		nLevel = ((CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat()->GetEnchantLv( ET_CR );
		break;	
	case 3:
		nLevel = ((CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat()->GetEnchantLv( ET_EV );
		break;		
	case 4:
		nLevel = ((CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat()->GetEnchantLv( ET_HP );
		break;
		/*case 5:
		nAbility = ET_AS;
		break;*/
	default:
		assert_csm( false, _T( "잘못된 능력치 선택" ) );
	}	

	int nSuccessCount = 0;
	if( nLevel < 4 )
		nSuccessCount = 12;
	else if( nLevel < 7 )
		nSuccessCount = 9;
	else if( nLevel < 10 )
		nSuccessCount = 6;
	else
		nSuccessCount = 3;

	for( int i = 0; i < MAX_CARD_NUM; i++ )
		m_Card[ i ].s_bSuccess = false;
	
	m_Card[ m_nSelect ].s_bSuccess = false;
	while( nSuccessCount != 0 )
	{
		int nIdx = rand() % MAX_CARD_NUM;
		if( nIdx == m_nSelect ||
			m_Card[ nIdx ].s_bSuccess == true )
			continue;
		else
		{
			m_Card[ nIdx ].s_bSuccess = true;
			nSuccessCount--;
		}
	}

	assert_cs( nSuccessCount == 0 );
}

void CDatsCenter::WeakSuccess()
{
	m_pDigimon->GetProp_Effect()->AddEffect( "System\\Tactics_enchant_fail.nif", 2.0f, nsEFFECT::POS_CHARPOS );	
	PlaySystemSound( "\\Tactics_Fail.wav" );	
}




bool CDatsCenter::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CDatsCenter::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}