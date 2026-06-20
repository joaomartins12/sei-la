#include "stdafx.h"
#include "Digimon_Boss.h"

cBossScene::cBossScene()
{

}

void cBossScene::Delete()
{
	SAFE_NIDELETE( m_pBG );
	SAFE_NIDELETE( m_pBright );
	SAFE_NIDELETE( m_pAniWarning );
	SAFE_NIDELETE( m_pAniEmergency );

	std::map< DWORD, sBossInfo* >::iterator it = m_mapBossInfo.begin();
	std::map< DWORD, sBossInfo* >::iterator itEnd = m_mapBossInfo.end();
	for( ;it != itEnd; ++it )
	{			
		g_pCharResMng->CharImageResDelete( &it->second->s_pImage );
		SAFE_NIDELETE( it->second->s_strCount );
		SAFE_NIDELETE( it->second );
	}	
	m_mapBossInfo.clear();

	DeleteBossMon();
}

void cBossScene::DeleteBossMon()
{
	if( m_pBossMon )
	{	
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = (CsC_Object*)m_pBossMon;
		g_pThread->DeleteChar( pUnit );
		m_pBossMon = NULL;
	}
}

void cBossScene::PreResetMap()
{
	std::map< DWORD, sBossInfo* >::iterator it = m_mapBossInfo.begin();
	std::map< DWORD, sBossInfo* >::iterator itEnd = m_mapBossInfo.end();
	for( ;it != itEnd; ++it )
	{			
		g_pCharResMng->CharImageResDelete( &it->second->s_pImage );
		SAFE_NIDELETE( it->second->s_strCount );
		SAFE_NIDELETE( it->second );
	}	
	m_mapBossInfo.clear();

	DeleteBossMon();
}

void cBossScene::ResetDevice()
{	
	std::map< DWORD, sBossInfo* >::iterator it = m_mapBossInfo.begin();
	std::map< DWORD, sBossInfo* >::iterator itEnd = m_mapBossInfo.end();
	for( ;it != itEnd; ++it )
	{
		it->second->s_pImage->ResetDevice(); 
		it->second->s_strCount->ResetDevice();
	}
}

void cBossScene::Init()
{
	m_nFlag = 1;
	m_fAccumTime = 0.0f;
	m_fAlpa = 0.0f;	
	m_fBossAlpa = 0.0f;
	m_eType = eNONE;
	m_bIncreaseZoomAlpha = false;
	m_pBossMon = NULL;	
	m_mapBossInfo.clear();

	m_pBG = NiNew cSprite;	
	m_pBG->Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), "Boss\\RedBackLight.tga", false );		
	m_pBG->SetAlpha( 0.0f );	
	
	m_pAniWarning = NiNew cSpriteAni;
	m_pAniWarning->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( 600, 320 ), "Boss\\warning\\Warning_", "tga", 20, false );
	m_pAniWarning->SetAniTime( 0.05f );	

	m_pAniEmergency = NiNew cSpriteAni;
	m_pAniEmergency->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( 600, 320 ), "Boss\\Emergency\\Emergency_", "tga", 20, false );
	m_pAniEmergency->SetAniTime( 0.05f );	

	m_pBright = NiNew cSprite;
	m_pBright->Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), true, NiColor::BLACK);
	m_pBright->SetAlpha( 0.0f );
}

void cBossScene::Update(float const& fDeltaTime)
{
	if( m_eType > eNONE )
	{
		m_fAccumTime += g_fDeltaTime;
		m_fAlpa += g_fDeltaTime * m_nFlag * 1.2f;
		if( m_fAlpa < 0.0f )
		{
			m_nFlag = 1;
			m_fAlpa = 0.0f;
		}
		if( m_fAlpa > 1.0f )
		{
			m_nFlag = -1;
			m_fAlpa = 1.0f;
		}

		m_pBG->SetAlpha( m_fAlpa );

		switch( m_eType )
		{
		case eWARNING:
			m_pAniWarning->Update( fDeltaTime );	
			break;
		case eEMERGENCY:
			m_pAniEmergency->Update( fDeltaTime );	
			break;		
		}		

		if( m_fAccumTime > 6.0f )
		{
			EndWarning();						
		}

		if( m_pBossMon )
		{		
			m_pBossMon->Update( fDeltaTime );
		}
	}		
}

void cBossScene::Render()
{
	if( m_eType > eNONE )
	{
		m_pBG->Render();
		switch( m_eType )
		{
		case eWARNING:
			m_pAniWarning->Render( CsPoint( (g_nScreenWidth - 580) / 2, 240 ) );
			break;
		case eEMERGENCY:
			m_pBright->Render();
			m_pAniEmergency->Render( CsPoint( (g_nScreenWidth - 580) / 2, 240 ) );
			break;		
		}
	}		

	if( m_pBossMon == NULL )
		return;
	
	if( m_pBossMon->IsLoad() == false )
		return;		

	// 오브젝트 위치 설정	
	CsNodeObj* pCsNode = m_pBossMon->GetCsNode();
	if( pCsNode == NULL )
		return;	
	// ZBuffer만 클리어
	g_pEngine->m_spRenderer->EndUsingRenderTargetGroup();	
	g_pEngine->m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ZBUFFER );

	// 오브젝트 알파 설정
	if( m_bIncreaseZoomAlpha == true )
	{		
		if( (m_fBossAlpa += g_fDeltaTime/2.0f) > 1.0f )
		{
			m_fBossAlpa = 1.0f;	
			m_pBossMon->SetAnimation( ANI::IDLE_CHAR );			
		}				
	}		
	else
	{
		if( m_fBossAlpa > 0.3f )
		{
			m_pBright->SetAlpha( 0.3f );
		}
		else
		{
			m_pBright->SetAlpha( m_fBossAlpa );
		}

		if( (m_fBossAlpa -= g_fDeltaTime/1.2f) < 0.0f )
		{	
			DeleteBossMon();
			return;
		}		
	}

	if( m_fBossAlpa < 1.0f )
	{
		m_pBossMon->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, m_fBossAlpa );
		m_pBossMon->GetProp_Alpha()->SetUseDistAlpha( false );
		m_pBossMon->GetProp_Alpha()->Update( 0.0f );		

		if( m_fBossAlpa > 0.3f )
		{
			m_pBright->SetAlpha( 0.3f );
		}
		else
		{
			m_pBright->SetAlpha( m_fBossAlpa );
		}
	}			

	float fHeight = m_pBossMon->GetToolHeight();
	float fWidth = m_pBossMon->GetToolWidth();
	float fCameraDist;
	float fCameraHeight;	
		
	if( fHeight < 120  )					
	{
		fCameraDist = 400.0f;
		fCameraHeight = 100.0f;
	}	
	else if( fHeight < 250 )					
	{
		fCameraDist = 800.0f;
		fCameraHeight = 150.0f;
	}
	else if( fHeight < 500 )					
	{
		fCameraDist = 800.0f;
		fCameraHeight = 170.0f;
	}
	else if( fHeight < 700 )
	{
		fCameraDist = 1500.0f;
		fCameraHeight = 400.0f;
	}
	else if( fHeight < 900 )
	{
		fCameraDist = 1800.0f;
		fCameraHeight = 500.0f;
	}
	else if( fHeight < 1800 )
	{
		fCameraDist = 2200.0f;
		fCameraHeight = 600.0f;
	}
	else 
	{
		fCameraDist = 2400.0f;
		fCameraHeight = 900.0f;
	}

	float fMoveX = tan( 0.27f )*fCameraDist + fWidth*0.55f;

	// 해상도 배율에 따라서 fMoveX에 비율을 곱해준다
	// 기준은 1680/1050
	fMoveX *= ( (g_nScreenWidth/(float)g_nScreenHeight)*(1050/1680.0f) );

	// 시야에 보이지 않으면 애니메이션 업데이트 풀로 다해준다.
	float fAniTime = m_pBossMon->GetAniTime();
	m_pBossMon->GetProp_Animation()->GetActorMng()->Update( fAniTime );
	pCsNode->m_pNiNode->Update( fAniTime );		

	// 카메라 설정		

	GET_SUBCAMERA(CAMERA_13)->SetDeltaHeight( 0.0f );
	GET_SUBCAMERA(CAMERA_13)->SetRotation( m_pBossMon->GetCurRot() - 0.7f, -0.05f );	
	GET_SUBCAMERA(CAMERA_13)->_UpdateCamera();
	GET_SUBCAMERA(CAMERA_13)->SetDist( fCameraDist );

	NiPoint3 vCameraPos = m_pBossMon->GetPos() + NiPoint3( 0, 0, fCameraHeight + 10.0f ) - GET_SUBCAMERA(CAMERA_13)->GetWorldRightVector()*fMoveX;	
	GET_SUBCAMERA(CAMERA_13)->SetTranslate( vCameraPos );
	GET_SUBCAMERA(CAMERA_13)->_UpdateCamera();		

	g_pEngine->m_spRenderer->SetCameraData( GET_SUBCAMERA(12)->GetCameraObj() );

	// 쉐이더 - 케쉬 사용 안시키기 위해
	NiSkinInstance* pSkin;
	int nCount = pCsNode->m_vpGeom.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( pCsNode->m_vpGeom[ i ]->IsPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE ) )
			continue;

		pSkin = pCsNode->m_vpGeom[ i ]->m_pGeometry->GetSkinInstance();
		if( pSkin == NULL )
			continue;
		pSkin->SetBoneMatrixInfo( 0, 0 );
	}

	// 케릭터 랜더링
	pCsNode->Render_GeomData_ExceptPlag( 0 );	//2016-03-07-nova	

	if( m_pBossMon->IsProperty( CsC_Property::EFFECT ) )
	{
		m_pBossMon->GetProp_Effect()->RenderAbsolute();
	}	

	g_pEngine->m_spRenderer->SetScreenSpaceCameraData();		
}

void cBossScene::SetWarning( DWORD dwMonType, DWORD dwSeedMon, int nCount )
{	
	sBossInfo* pInfo = _SetCount( dwMonType, nCount ); 
	if( pInfo != NULL )
	{		
		return;
	}

	SetBossInfo( dwMonType, 0, dwSeedMon, nCount );

	if( g_pResist->m_Global.s_bBossWarning == TRUE && 
		nsCsFileTable::g_pMonsterMng->GetMonster( dwMonType )->IsBossType() == true )
	{
		m_eType = eWARNING;
		m_fAccumTime = 0.0f;
		m_fAlpa = 0.0f;	

		m_fBackupMusic = g_pResist->m_Global.s_fMusic; 
		g_pSoundMgr->Set_BGM_Volume( CsMin( m_fBackupMusic, 0.1f ) );
		g_pSoundMgr->PlaySystemSound( "System\\Warning.wav" );
	}	
}

void cBossScene::SetWarning2( float fLifeTime )
{

	m_eType = eWARNING;
	fLifeTime = fLifeTime / 1000;
	m_fAccumTime = 0.0f + (6.0f - fLifeTime);
	m_fAlpa = 0.0f;	

	m_fBackupMusic = g_pResist->m_Global.s_fMusic; 
	g_pSoundMgr->Set_BGM_Volume( CsMin( m_fBackupMusic, 0.1f ) );
	g_pSoundMgr->PlaySystemSound( "System\\Warning.wav" );

}

void cBossScene::EndWarning()
{
	BHPRT( "%f", m_fAccumTime );
	m_eType = eNONE;	
	m_bIncreaseZoomAlpha = false;
	m_fAlpa = 0.0f;		
	m_pBG->SetAlpha( 0.0f );	

	g_pSoundMgr->Set_BGM_FadeVolume( m_fBackupMusic );
}

void cBossScene::SetEmergency( DWORD dwMonType, int nCount )
{	
	sBossInfo* pInfo = _SetCount( dwMonType, 0 );

	if( pInfo == NULL ) // 카운트 몹이 1마리인경우 바로 출현이라 정보가 없다.
	{
		SetBossInfo( dwMonType, nCount, 0, 0 );

		std::map< DWORD, sBossInfo* >::iterator it = m_mapBossInfo.find( dwMonType );
		std::map< DWORD, sBossInfo* >::iterator itEnd = m_mapBossInfo.end(); 
		assert_cs( it != itEnd );
		pInfo = it->second;
	}

	assert_cs( pInfo != NULL );
	if( pInfo->s_bRender == false )
		return;
 
	m_fBackupMusic = g_pResist->m_Global.s_fMusic;
	g_pSoundMgr->Set_BGM_Volume( CsMin( m_fBackupMusic, 0.1f ) );
	g_pSoundMgr->PlaySystemSound( "System\\Warning.wav" );
	g_pSoundMgr->PlayMusic( "Real_Factory_special_battle.mp3" );
	
	if( m_pBossMon )
	{
		DeleteBossMon();
	}	

	if( g_pResist->m_Global.s_bBossWarning == TRUE )
		m_eType = eEMERGENCY;

	m_bIncreaseZoomAlpha = true;
	m_fAccumTime = 0.0f;
	m_fAlpa = 0.0f;
	m_fBossAlpa = 0.0f;	
	
	// 등장보스 로드
	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, dwMonType );

	// 초기화 - Prop 초기화 포함
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	pUint->s_Prop.s_dwOptionPlag = OP_STAT | OP_LIGHT;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	m_pBossMon = CMonster::NewInstance();
	m_pBossMon->Init( 0, dwMonType, NiPoint3::ZERO, 0.0f, &pUint->s_Prop, CMonster::MONSTER_NORMAL );

	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)m_pBossMon;	

	std::string strPath = m_pBossMon->GetModelPath();
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	DWORD dwFlag = nsEFFECT::LIVE_ONCE | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER;
	float fScale = m_pBossMon->GetScale() * 0.7f;
	m_pBossMon->GetProp_Effect()->AddEffect( "system\\Boss_Digimon_gate.nif", fScale, dwFlag );	
	m_pBossMon->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BOSS, "system\\LevelEffect\\BossFinalityEffect.nif" );		
}

void cBossScene::SetBossInfo( DWORD dwMonType, int nCount, DWORD dwSeedMon, int nSeedCount )
{
	sBossInfo* pInfo = _SetCount( dwMonType, nSeedCount );
	if( pInfo == NULL )
	{
		std::map< DWORD, sBossInfo* >::iterator it = m_mapBossInfo.find( dwMonType );
		std::map< DWORD, sBossInfo* >::iterator itEnd = m_mapBossInfo.end(); 
		if( it != itEnd )
		{
			g_pCharResMng->CharImageResDelete( &it->second->s_pImage );	
			SAFE_NIDELETE( it->second->s_strCount );
			SAFE_NIDELETE( it->second );

			assert_csm( false, _T( "똑같은 보스디지몬이 2마리 출현 헐!!!" ) );
		}		

		CsMonster::sINFO* pMon = nsCsFileTable::g_pMonsterMng->GetMonster( dwMonType )->GetInfo();

#if COMPAT_487_2
		if (!pMon) {
			return;
		}
#endif
		assert_cs( pMon != NULL );
		DWORD dwID = pMon->s_dwModelID;

		sBossInfo* pInfo = NiNew sBossInfo;	
		g_pCharResMng->CharImageResDelete( &pInfo->s_pImage );	
		pInfo->s_pImage = g_pCharResMng->CharImageResLoad( dwID );
		pInfo->s_dwSeedMon = dwSeedMon;
		pInfo->s_bRender = nsCsFileTable::g_pMonsterMng->GetMonster( dwMonType )->IsBossType();

		cText::sTEXTINFO ti;		
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );	
		ti.s_eTextAlign = DT_CENTER;
		ti.s_Color = NiColor( 1.0f, 1.0f, 0.0f );
		ti.s_bOutLine = true;	
		ti.SetText( nSeedCount );	

		pInfo->s_strCount = NiNew cText;
		pInfo->s_strCount->Init( NULL, CsPoint::ZERO, &ti, false );

		m_mapBossInfo[ dwMonType ] = pInfo;
		_SetCount( dwMonType, nSeedCount );			
	}		
}

void cBossScene::SetBossDie( DWORD dwID )
{
	std::map< DWORD, sBossInfo* >::iterator it = m_mapBossInfo.find( dwID );
	if( it == m_mapBossInfo.end() )
		return;

	SAFE_POINTER_RET( it->second );

	g_CsLastError.m_pValue1 = it->second;

	g_pCharResMng->CharImageResDelete( &it->second->s_pImage );			
	SAFE_NIDELETE( it->second->s_strCount );		
	SAFE_NIDELETE( it->second );		
	m_mapBossInfo.erase( it );

	if( g_pCharMng->GetTamerUser() )
		g_pCharMng->GetTamerUser()->PlayBGM();
}

void cBossScene::RenderBossInfo()
{
	if( m_mapBossInfo.size() < 1 )
		return;

	CsPoint vPos( g_nScreenWidth - 230, 180 );
	std::map< DWORD, sBossInfo* >::iterator it = m_mapBossInfo.begin();
	std::map< DWORD, sBossInfo* >::iterator itEnd = m_mapBossInfo.end();
	for( ;it != itEnd ;++it )
	{
		if( it->second->s_nSeedCount > 0 )
		{
			it->second->s_pImage->Render_B( vPos );
			it->second->s_strCount->Render( vPos + CsPoint( 17, 35 ), DT_CENTER );
			vPos.x -= 37;
		}
	}	
}

cBossScene::sBossInfo* cBossScene::_SetCount( DWORD dwMonType, int nCount )
{
	std::map< DWORD, sBossInfo* >::iterator it = m_mapBossInfo.find( dwMonType );
	std::map< DWORD, sBossInfo* >::iterator itEnd = m_mapBossInfo.end(); 
	if( it == itEnd )
		return NULL;

	if( it->second->s_nSeedCount != nCount )
	{
		it->second->s_nSeedCount = nCount;
		if( nCount == 0 )
			it->second->s_strCount->SetText( UISTRING_TEXT( "DIGIMON_BOSS_APPEAR" ).c_str() );	
		else
			it->second->s_strCount->SetText( nCount );	
	}	
	return it->second;
}

bool cBossScene::IsBoss()
{
	std::map< DWORD, sBossInfo* >::iterator it = m_mapBossInfo.begin();
	std::map< DWORD, sBossInfo* >::iterator itEnd = m_mapBossInfo.end();
	for( ;it != itEnd; ++it )
	{			
		if( it->second->s_nSeedCount == 0 )
			return true;
	}

	return false;	
}