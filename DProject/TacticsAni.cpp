#include "stdafx.h"
#include "TacticsAni.h"

#define ANI_SPEED 1.25f
#define CLON_NUM 4

cTacticsAni*		g_pTacticsAni = NULL;

void cTacticsAni::GlobalInit()
{
	assert_cs( g_pTacticsAni == NULL );
	g_pTacticsAni = NiNew cTacticsAni;	
	g_pTacticsAni->Init();
}

void cTacticsAni::GlobalShotDown()
{
	if( g_pTacticsAni != NULL )
		g_pTacticsAni->Delete();
	SAFE_NIDELETE( g_pTacticsAni );
}

void cTacticsAni::Delete()
{
	DeleteScript();

	SAFE_NIDELETE( m_pBG );
	SAFE_NIDELETE( m_pBGAlpha );
	SAFE_NIDELETE( m_Sprite );
	SAFE_NIDELETE( m_pSprite1 );
	SAFE_NIDELETE( m_pSprite2 );
	SAFE_NIDELETE( m_pSkip );

	if( m_pTranscendBG )
		SAFE_NIDELETE( m_pTranscendBG );
	if( m_pTranscendFrame )
		SAFE_NIDELETE( m_pTranscendFrame );
	if( m_pTranscendDigimon )
		SAFE_NIDELETE( m_pTranscendDigimon );
	if( m_pTranscendCanopy )
		SAFE_NIDELETE( m_pTranscendCanopy );
	if( m_pTranscendAni )
		SAFE_NIDELETE( m_pTranscendAni );
	if( m_pTranscendText )
		SAFE_NIDELETE( m_pTranscendText );
	m_bTranscend = false;

	m_StringList.Delete();
	//m_SkipStringList.Delete();

	for( int i = 0; i < m_nCount; i++ )
	{
		if( m_sDigimon[ i ] )
		{
			sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
			pUnit->s_pLoadedObject = (CsC_Object*)m_sDigimon[ i ]->s_pDigimon;
			g_pThread->DeleteChar( pUnit );	

			SAFE_NIDELETE( m_sDigimon[ i ] );
		}		
	}	
}

void cTacticsAni::Init()
{
	m_Sprite = NULL; 
	m_pSprite1 = NULL;
	m_pSprite2 = NULL;

	m_pBG = NULL;
	m_pBGAlpha = NULL;

	m_pSkip = NULL;	

	m_bAniProcess = false;	// Tactics 애니 시작 위치 변경을 위한 bool 변수

	m_pHatchingSound = NULL;

	SetRootClient( CsPoint::ZERO );
	m_eProcessType = eAni_None;

	for( int i=0; i<nLimit::EvoStep-1; ++i )
	{
		m_sDigimon[ i ] = NULL;
	}

	//2016-03-02-nova 디지몬 부화 카메라 설정
	GET_SUBCAMERA(CAMERA_11)->SetDeltaHeight(200.0f);
	GET_SUBCAMERA(CAMERA_11)->ReleaseDistRange();	
	GET_SUBCAMERA(CAMERA_11)->SetRotation( 0.0f, 0.0f );
	GET_SUBCAMERA(CAMERA_11)->SetDist( 30.0f );
	GET_SUBCAMERA(CAMERA_11)->_UpdateCamera();	

	m_pTranscendBG = NULL;
	m_pTranscendFrame = NULL;
	m_pTranscendDigimon = NULL;
	m_pTranscendCanopy = NULL;
	m_pTranscendAni = NULL;
	m_pTranscendText = NULL;
	m_bTranscend = false;
}


void cTacticsAni::End()
{		
	m_eProcessType = eAni_None;

	_EndAni();
	Delete();	

	// 디지몬 부화 후 카메라 위치가 위로 올라가는 버그로 아래 카메라 위치를 초기화 시키는 코드 추가
	CAMERA_ST.SetDeltaHeight( 0.0f );

	g_pCharMng->GetTamerUser()->PlayBGM();
	g_pMngCollector->ReleaseScene();

	if( m_pHatchingSound )
	{
		g_pSoundMgr->StopSound( m_pHatchingSound );
		m_pHatchingSound = NULL;
	}
}

void cTacticsAni::ResetDevice()
{
	if( m_Sprite )
		m_Sprite->ResetDevice();
	if( m_pSprite1 )
		m_pSprite1->ResetDevice();
	if( m_pSprite2 )
		m_pSprite2->ResetDevice();

	if( m_pBG )
		m_pBG->ResetDevice();
	if( m_pBGAlpha )
		m_pBGAlpha->ResetDevice();

	if( m_pSkip )
		m_pSkip->ResetDevice();

	if( m_pTranscendBG )
		m_pTranscendBG->ResetDevice();
	if( m_pTranscendFrame )
		m_pTranscendFrame->ResetDevice();
	if( m_pTranscendDigimon )
		m_pTranscendDigimon->ResetDevice();
	if( m_pTranscendCanopy )
		m_pTranscendCanopy->ResetDevice();
	if( m_pTranscendAni )
		m_pTranscendAni->ResetDevice();
	if( m_pTranscendText )
		m_pTranscendText->ResetDevice();
}

void cTacticsAni::Start( int nValue /*=0*/, CsC_AvObject* pTarget )
{	
	//2016-03-02-nova	부화시 카메라 이동을 캐릭터 위치 영향을 받지 않도록
	CAMERA_ST.SetDistLevel( 4, true );	
	CAMERA_ST._UpdateCamera();

	g_pMngCollector->SetSceneState( CMngCollector::eSCENE_TACTICSANI );
	g_pSoundMgr->DeleteMusic( false );

	if( m_pHatchingSound )
	{
		g_pSoundMgr->StopSound( m_pHatchingSound );
		m_pHatchingSound = NULL;
	}
	m_pHatchingSound = g_pSoundMgr->PlaySystemSound(  "System\\Tactics_movie.mp3" );

	assert_cs( pTarget );
	m_pTarget = pTarget;
	m_pTarget->SetAnimation( ANI::NPC_TACTICS_GET );

	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();

	InitScript( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );	

	g_pGameIF->SetRenderIF( false );
	if( m_bAniProcess == true )
		m_eProcessType = eAni_BG;	// 알부화 시 Tactics 애니 없이 바로 어떤 디지몬들이 나오는지 애니 보여줌
	else
		m_eProcessType = eAni_Start;
	m_fBright = 0.0f;
	m_nDeltaValue = 0;
	m_fCameraDist = 0.0f;
	m_fAccumTime = 0;
	m_nCount = 0;	
	m_StringList.Delete();
	m_pSkip = NiNew cButton;
	m_pSkip->Init( NULL, CsPoint( 10, (int)( g_nScreenHeight - 50 ) ), CsPoint( 85, 19 ), "Scene\\Skip.tga", false );					
	m_pSkip->SetTexToken( CsPoint( 67, 17 ) );

	NiPoint3 vPos = m_pTarget->GetPos();
	CAMERA_ST.SetTranslate( vPos );

	m_pBG = NiNew cSprite;	
	m_pBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), "Event\\background.tga", false );
	m_fCurAlpha = 0.0f;
	m_fConvert = 1.0f;
	m_pBGAlpha = NiNew cSprite;	
	m_pBGAlpha->Init( m_pRoot, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), "Event\\background2.tga", false );
	m_pBGAlpha->SetAlpha( m_fCurAlpha );

	m_Sprite = NiNew cSprite;	
	m_Sprite->Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), NiColorA::WHITE, false );
	m_Sprite->SetAlpha( 0.0f );

	float fViewSize = (float)g_nScreenHeight * 0.17f;
	m_pSprite1 = NiNew cSprite;			
	m_pSprite1->Init( NULL, CsPoint( (int)-g_nScreenWidth, (int)0 ), CsPoint( (int)g_nScreenWidth, (int)fViewSize ), false, NiColor::BLACK );				

	m_pSprite2 = NiNew cSprite;			
	m_pSprite2->Init( NULL, CsPoint( (int)g_nScreenWidth, (int)(g_nScreenHeight - fViewSize) ), CsPoint( (int)g_nScreenWidth, (int)fViewSize ), false, NiColor::BLACK );

	SetDigitama( nValue );

	if( m_bTranscend && m_pTranscendAni )
		m_pTranscendAni->PlayOn( true );
}
void cTacticsAni::Skip( int nValue, CsC_AvObject* pPickObject)
{
	SAFE_POINTER_RET( pPickObject );

	g_pTacticsAni->Start( nValue, pPickObject );

	m_eProcessType = eAni_BG;
	m_fAccumTime = 0.0f;
}

void cTacticsAni::SetDigitama( int nDigitama )
{	
	CDigimonEvolveInfo* pFTEvol = NULL;
	if(NULL ==  nsCsFileTable::g_pTacticsMng->IsTactics( nDigitama ))
	{
		m_nDigimonID = nDigitama;
		pFTEvol = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_nDigimonID ); 
	}	
	else
	{

		m_nDigimonID = nsCsFileTable::g_pTacticsMng->GetTactics( nDigitama )->GetInfo()->s_nDigimonID;
		pFTEvol = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_nDigimonID ); 
	}
	
	for( int i=1; i<nLimit::EvoUnit; ++i )
	{
		CDigimonEvolveObj* pFTObj = pFTEvol->GetEvolveObjByEvoSlot( i );		
		SAFE_POINTER_CON( pFTObj );
		if(4 <= m_nCount) // 4마리 까지 나오게 하자.
			break;

		if( pFTObj != NULL )
		{	
			if( nItem::Chipset == pFTObj->m_nChipsetType ) // 진화 테이블에 칩셋 조건이 있으면  조그레스 (현재 기준)
				continue;

			DWORD nDigimonID = pFTObj->m_dwID;

			if( nsCsFileTable::g_pDigimonMng->IsDigimon( nDigimonID ) )
			{
				DWORD DigimonModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo()->s_dwModelID;
				if( g_pModelDataMng->IsData( DigimonModelID ) && pFTObj->m_bRender )
				{					
					m_sDigimon[ m_nCount ] = NiNew sTacticsAni;					

					m_sDigimon[ m_nCount ]->s_vStartPos.x = (float)pFTObj->m_TargetStartPos.x;
					m_sDigimon[ m_nCount ]->s_vStartPos.y = (float)pFTObj->m_TargetStartPos.y;
					m_sDigimon[ m_nCount ]->s_vStartPos.z = (float)pFTObj->m_nStartHegiht;
					m_sDigimon[ m_nCount ]->s_vStartPos += g_pCharMng->GetTamerUser()->GetPos();

					m_sDigimon[ m_nCount ]->s_vEndPos.x = (float)pFTObj->m_TargetEndPos.x;
					m_sDigimon[ m_nCount ]->s_vEndPos.y = (float)pFTObj->m_TargetEndPos.y;
					m_sDigimon[ m_nCount ]->s_vEndPos.z = (float)pFTObj->m_nEndHegiht;
					m_sDigimon[ m_nCount ]->s_vEndPos += g_pCharMng->GetTamerUser()->GetPos();

					m_sDigimon[ m_nCount ]->s_fStartRot = (float)pFTObj->m_nStartRot * ( NI_PI/ 180.0f );
					m_sDigimon[ m_nCount ]->s_fEndRot = (float)pFTObj->m_nEndRot * ( NI_PI/ 180.0f );
					m_sDigimon[ m_nCount ]->s_nUpdateSpeed = pFTObj->m_nSpeed;				

					NiPoint3 vPos =  m_sDigimon[ m_nCount ]->s_vStartPos;				
					m_sDigimon[ m_nCount ]->s_pDigimon = _AddDigimon( DigimonModelID, vPos, m_sDigimon[ m_nCount ]->s_fStartRot );					
					m_sDigimon[ m_nCount ]->s_pDigimon->SetAniOption( pFTObj->m_dwAni, (float)pFTObj->m_fStartTime, (float)pFTObj->m_fEndTime );		

					m_nCount++;
				}
			}
		}
	}	

	if( m_sDigimon[ 0 ] == NULL )
		End();
}

cTacticsObject* cTacticsAni::_AddDigimon( DWORD dwModelID, NiPoint3 vPos, float fRot )
{
	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, dwModelID );

	// 초기화 - Prop 초기화 포함
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE | PR_ATTACK_ENABLE;
	pUint->s_Prop.s_dwOptionPlag = OP_LIGHT;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	std::string strPath = g_pModelDataMng->GetKfmPath( dwModelID );
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );		

	//신규캐릭터 생성
	cTacticsObject* pObject = NiNew cTacticsObject;
	pObject->Init( dwModelID, vPos, fRot, 0, &pUint->s_Prop );				

	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)pObject;	
	//우선로드 설정
	pUint->SetLoadType( sTCUnit::eFast );

	// 쓰레드 로드		
	g_pThread->LoadChar( pUint );				

	return pObject;
}

void cTacticsAni::Update()
{
	if( IsPlay() == false )
		return;

	switch( m_eProcessType )
	{
	case eAni_None:
		return;
		break;
	case eAni_Start:
		_UpdateStart();
		break;
	case eAni_BG:
		_UpdateBG();
		break;
	case eAni_Play:
		_UpdatePlay();
		break;	
	case eAni_End:	
		break;
	default:
		assert_cs( false );
		break;
	}

	if( IsPlay() == true )
	{
		if( m_pSkip->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			End();
		}

		if( GLOBALINPUT_ST.IsEscape() && m_eProcessType == eAni_Play )	//2016-02-24-nova
		{		
			End();		
		}
		_SetBGAlpha();

		if( m_bTranscend && m_pTranscendAni )
			m_pTranscendAni->Update( g_fDeltaTime );
	}	
}

void cTacticsAni::PostRender()
{
	if( m_eProcessType == eAni_None )
		return;		
	
	if( m_eProcessType < eAni_Play )
	{	
		m_Sprite->Render( GetRootClient() );
		m_pSprite1->Render();
		m_pSprite2->Render();			
	}
	else
	{
		RenderScript();
		m_pBG->Render();
		m_pBGAlpha->Render();
		m_pSprite1->Render();
		m_pSprite2->Render();	

		for( int i = m_nCount - 1; i >= 0 ; i-- )
		{
			if(NULL == m_sDigimon[ i ])
				continue;

			CsC_AvObject* pObject = m_sDigimon[ i ]->s_pDigimon;
			if( pObject == NULL )
				continue;

			// 오브젝트 위치 설정	
			CsNodeObj* pCsNode = pObject->GetCsNode();
			if( pCsNode == NULL )
				continue;			

			// ZBuffer만 클리어
			g_pEngine->m_spRenderer->EndUsingRenderTargetGroup();	
			g_pEngine->m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ZBUFFER );			

			// 카메라 설정			

			NiPoint3 vCameraPos = g_pCharMng->GetTamerUser()->GetPos() + NiPoint3( 0, -30, 0 );
			GET_SUBCAMERA(CAMERA_11)->SetTranslate( vCameraPos );
			GET_SUBCAMERA(CAMERA_11)->_UpdateCamera();

			g_pEngine->m_spRenderer->SetCameraData( GET_SUBCAMERA(CAMERA_11)->GetCameraObj() );	

			// 쉐이더 - 케쉬 사용 안시키기 위해
			NiSkinInstance* pSkin;
			int nCount = pCsNode->m_vpGeom.Size();	
			for( int j=0; j<nCount; ++j )
			{
				if( pCsNode->m_vpGeom[ j ]->IsPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE ) )
					continue;

				pSkin = pCsNode->m_vpGeom[ j ]->m_pGeometry->GetSkinInstance();
				if( pSkin == NULL )
					continue;
				pSkin->SetBoneMatrixInfo( 0, 0 );
			}					

			// 케릭터 랜더링
			pCsNode->RenderAbsolute();	

			g_pEngine->m_spRenderer->SetScreenSpaceCameraData();
		}

		if( m_bTranscend )
		{
			if( m_pTranscendBG )
				m_pTranscendBG->Render();
			if( m_pTranscendAni )
				m_pTranscendAni->Render();
			if( m_pTranscendFrame )
				m_pTranscendFrame->Render();
			if( m_pTranscendDigimon )
				m_pTranscendDigimon->Render();
			if( m_pTranscendCanopy )
				m_pTranscendCanopy->Render();
			if( m_pTranscendText )
				m_pTranscendText->Render();
		}
		else
			m_StringList.Render( CsPoint( 20, 25 ), 5, DT_LEFT );

		m_pSkip->Render();
	}		
}
void cTacticsAni::Render()
{
	
}

void cTacticsAni::_EndAni()
{
//	SAFE_DELETE( g_pFade );
//	g_pFade = NiNew CFade( FADE_IN, 0.6f );	

	g_pGameIF->SetRenderIF( true );

	// 카메라 복귀		
	sCAMERAINFO ci;	
}

void cTacticsAni::_UpdateStart()
{	
	GET_SUBCAMERA(CAMERA_11)->SetDeltaHeight(200.0f); //2016-03-02-nova 디지몬 부화 카메라 설정

	CAMERA_ST.ReleaseRemainDist();
	CAMERA_ST.SetTranslate( m_pTarget->GetPos() );
	CAMERA_ST.SetDeltaHeight( 200.0f );
	CAMERA_ST.SetRotation( m_pTarget->GetCurRot(), 0 );
	CAMERA_ST._UpdateCamera();		

	m_fAccumTime += g_fDeltaTime;	

	if( m_fAccumTime < 2.0f )
	{
		CAMERA_ST.SetDist( 450.0f );	
	}
	else	
	{
		CAMERA_ST.SetDeltaDist( -2.5f );	

		if( m_fAccumTime > 4.0f )
		{
			if( m_fBright < 1.0f )
			{
				m_Sprite->SetAlpha( m_fBright );
				m_fBright += g_fDeltaTime * 0.5f;		
			}		
			else
			{
				m_eProcessType = eAni_BG;		
				m_fAccumTime = 0.0f;
			}
		}		
	}	
}

void cTacticsAni::_UpdateBG()
{
	float fViewSize = (float)g_nScreenHeight * 0.15f;
	m_nDeltaValue += int( g_fDeltaTime * 3000 );

	if( m_nDeltaValue < g_nScreenWidth )
	{
		CsPoint ptPos = GetRootClient() + CsPoint( m_nDeltaValue, 0 );		
		m_pSprite1->SetPos( CsPoint( (int)-g_nScreenWidth, (int)0 ) + ptPos );
		m_pSprite2->SetPos( CsPoint( g_nScreenWidth, (int)(g_nScreenHeight - fViewSize) ) - ptPos );			
	}
	else
	{
		m_pSprite1->SetPos( CsPoint::ZERO );
		m_pSprite2->SetPos( CsPoint( 0, (int)(g_nScreenHeight - fViewSize) ) );

		m_eProcessType = eAni_Play;
		m_fAccumTime = 0.0f;
	}	
}	

void cTacticsAni::_UpdatePlay()
{
	int i;
	for( i = 0; i < m_nCount; i++ )
	{	
		if(NULL == m_sDigimon[ i ])
			continue;
		if( m_sDigimon[ i ]->s_nUpdateSpeed > 0 )
		{
			NiPoint3 vDir = m_sDigimon[ i ]->s_vEndPos - m_sDigimon[ i ]->s_vStartPos;
			vDir.Unitize();

			NiPoint3 vCurPos = m_sDigimon[ i ]->s_vEndPos - m_sDigimon[ i ]->s_pDigimon->GetPos();
			vCurPos.Unitize();

			NiPoint3 vPos;
			if( vCurPos.Dot( vDir ) > 0 )
			{
				vPos = m_sDigimon[ i ]->s_pDigimon->GetPos() + ( vDir * (float)m_sDigimon[ i ]->s_nUpdateSpeed * g_fDeltaTime );		
			}
			else
			{
				vPos = m_sDigimon[ i ]->s_vEndPos;
			}			
			m_sDigimon[ i ]->s_pDigimon->SetPos( vPos );
		}		

		m_sDigimon[ i ]->s_fDigimonAlpha += g_fDeltaTime;
		if( m_sDigimon[ i ]->s_fDigimonAlpha > 1.0f )
			m_sDigimon[ i ]->s_fDigimonAlpha = 1.0f;	

		// 오브젝트 알파 설정
		if( m_sDigimon[ i ]->s_fDigimonAlpha < 1.0f )
		{
			m_sDigimon[ i ]->s_pDigimon->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, m_sDigimon[ i ]->s_fDigimonAlpha );
			m_sDigimon[ i ]->s_pDigimon->GetProp_Alpha()->SetUseDistAlpha( false );			
		}			

		if( m_sDigimon[ i ]->s_fEndRot != 0.0f )
		{	
			float fRot = m_sDigimon[ i ]->s_fEndRot * g_fDeltaTime;			
			m_sDigimon[ i ]->s_pDigimon->SetRotation( m_sDigimon[ i ]->s_pDigimon->GetCurRot() + fRot );
		}

		m_sDigimon[ i ]->s_pDigimon->Update( g_fDeltaTime * ANI_SPEED );
		if( m_sDigimon[ i ]->s_pDigimon->IsEnd() == false )
			break;
	}	

	if( i == m_nCount )
		m_fAccumTime += g_fDeltaTime;	

	if( m_fAccumTime > 0.5f  )
	{	
		if( m_StringList.GetSize() < 1 )
		{
			_SetText( nsCsFileTable::g_pTacticsMng->GetTacticsExplain( m_nDigimonID )->GetInfo()->s_szTacticsName, CFont::FS_24 );
			_SetText( nsCsFileTable::g_pTacticsMng->GetTacticsExplain( m_nDigimonID )->GetInfo()->s_szTacticsExplain, CFont::FS_16 );
			m_StringList.SetAlpha( 0.0f );		
		}		
	}		

	if( m_fAccumTime < 2.0f )
	{
		m_StringList.SetAlpha( m_fAccumTime / 2.0f );

		if( m_pTranscendBG )
			m_pTranscendBG->SetAlpha( m_fAccumTime / 2.0f );
		if( m_pTranscendAni )
			m_pTranscendAni->SetAlpha( m_fAccumTime / 2.0f );
		if( m_pTranscendFrame )
			m_pTranscendFrame->SetAlpha( m_fAccumTime / 2.0f );
		if( m_pTranscendDigimon )
			m_pTranscendDigimon->SetAlpha( m_fAccumTime / 2.0f );
		if( m_pTranscendCanopy )
			m_pTranscendCanopy->SetAlpha( m_fAccumTime / 2.0f );
		if( m_pTranscendText )
			m_pTranscendText->SetAlpha( m_fAccumTime / 2.0f );
	}
	else
	{
		m_StringList.SetAlpha( 1.0f );
		if( m_pTranscendBG )
			m_pTranscendBG->SetAlpha( 1.0f );
		if( m_pTranscendAni )
			m_pTranscendAni->SetAlpha( 1.0f );
		if( m_pTranscendFrame )
			m_pTranscendFrame->SetAlpha( 1.0f );
		if( m_pTranscendDigimon )
			m_pTranscendDigimon->SetAlpha( 1.0f );
		if( m_pTranscendCanopy )
			m_pTranscendCanopy->SetAlpha( 1.0f );
		if( m_pTranscendText )
			m_pTranscendText->SetAlpha( 1.0f );
	}	

	if( m_fAccumTime > 10.0f )
	{	
		m_eProcessType = eAni_End;
		m_pTarget->SetAnimation( ANI::NPC_OPEN_IDLE2 );
		End();
	}
}

void cTacticsAni::_SetText( TCHAR* szText, CFont::eFACE_SIZE eSize )
{
	cText::sTEXTINFO ti;		
	ti.Init( &g_pEngine->m_FontSystem, eSize, FONT_WHITE );	
	ti.s_eTextAlign = DT_LEFT;
	ti.s_bOutLine = false;
	ti.SetText( szText );	

	g_pStringAnalysis->Cut( &m_StringList, g_nScreenWidth - 200, szText, &ti );				
}

void cTacticsAni::SetTranscendAni(bool bAni, std::string strFile, std::wstring strName)
{
	if( strFile.empty() )
	{
		m_bTranscend = false;
		return;
	}

	m_bTranscend = bAni;
	if( m_bTranscend )
	{
		m_pTranscendBG = NiNew cSprite;
		m_pTranscendBG->Init( NULL, CsPoint( g_nScreenWidth / 2 - 477, g_nScreenHeight / 2 - 364 ), CsPoint( 954, 294 ), "TacticsHouse\\TranscendenceBG.tga", false );

		m_pTranscendFrame = NiNew cSprite;
		m_pTranscendFrame->Init( m_pTranscendBG, CsPoint( 396, 22 ), CsPoint( 159, 168 ), "TamerStatus_New\\TamerStatus_Digimon_X.tga", false );

		m_pTranscendDigimon = NiNew cSprite;
		m_pTranscendDigimon->Init( m_pTranscendBG, CsPoint( 433, 63 ), CsPoint( 85, 85 ), strFile.c_str(), false, NiColor::WHITE, false );

		m_pTranscendCanopy = NiNew cSprite;
		m_pTranscendCanopy->Init( m_pTranscendBG, CsPoint( 433, 63 ), CsPoint( 85, 85 ), "TamerStatus_New\\TamerStatus_Canopy_X.tga", false );

		m_pTranscendAni = NiNew cSpriteAni;
		m_pTranscendAni->Init( cSpriteAni::LOOP, m_pTranscendBG, CsPoint( 954/2 - 255, -93 ), CsPoint( 504, 452 ), "GiftBox\\Unique_Effect_Box_", "dds", 20, false );
		m_pTranscendAni->SetAniTime( 0.05f );
		m_pTranscendAni->PlayOff();

		{	// 초월 성공
			std::wstring wsSuccess = UISTRING_TEXT( "TACTICSANI_TRANSCENDENCE_SUCCESS_TEXT" );
			DmCS::StringFn::Replace( wsSuccess, L"#DigimonName#", strName );

			cText::sTEXTINFO ti;
			ti.Init( CFont::FS_20, NiColor::WHITE );
			ti.s_eTextAlign = DT_CENTER;
			ti.SetText( wsSuccess.c_str() );

			m_pTranscendText = NiNew cText;
			m_pTranscendText->Init( m_pTranscendBG, CsPoint( 477, 179 ), &ti, false );
		}
	}
}

void cTacticsAni::_SetBGAlpha()
{
	SAFE_POINTER_RET( m_pBGAlpha );
	
	m_fCurAlpha += g_fDeltaTime * m_fConvert;
	if( m_fCurAlpha > 1.0f )
	{
		m_fCurAlpha = 1.0f;
		m_fConvert = -1.0f;
	}
	else if( m_fCurAlpha < 0.0f )
	{
		m_fCurAlpha = 0.0f;
		m_fConvert = 1.0f;
	}

	m_pBGAlpha->SetAlpha( m_fCurAlpha );
}
