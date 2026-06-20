
#include "StdAfx.h"
#include "CrossEvent.h"
#ifdef CROSSWARS_SYSTEM
CCrossEvent*		g_pCrossEvent = NULL;

CCrossEvent::CCrossEvent(void)
{
	m_pBG = NULL;	
	m_sDigimon.clear();
	m_vecRenderTex.clear();
	m_vecSprite.clear();
	m_nDigimonCount = m_TargetDIgimon = 0;

	m_fScenePlayTime = (float)g_fAccumTime;
	m_fCutTime = 0.0f;

}

CCrossEvent::~CCrossEvent(void)
{
}
void CCrossEvent::Delete()
{
	Reset();
}
void CCrossEvent::Reset()
{	

	SAFE_NIDELETE( m_pBG );	

	{
		CCrossEvent::vecTex_it it = m_vecRenderTex.begin();
		CCrossEvent::vecTex_it it_End = m_vecRenderTex.end();
		for( ; it != it_End; ++it)
		{			
			SAFE_NIDELETE((*it));
		}
	}	
	{
		CCrossEvent::vecSprite_it it = m_vecSprite.begin();
		CCrossEvent::vecSprite_it it_End  = m_vecSprite.end();
		for( ; it != it_End; ++it)
		{
			(*it)->Delete();
			SAFE_NIDELETE((*it));

		}
	}



	for( u4 i = 0; i < m_sDigimon.size(); i++ )
	{
		if( m_sDigimon[ i ] )
		{
			sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
			pUnit->s_pLoadedObject = (CsC_Object*)m_sDigimon[ i ]->s_pDigimon;
			g_pThread->DeleteChar( pUnit );	

			SAFE_NIDELETE( m_sDigimon[ i ] );
		}		
	}	

	/*
	{
		CCrossEvent::vecDigimon_it it = m_sDigimon.begin();
		CCrossEvent::vecDigimon_it it_End  = m_sDigimon.end();
		for( ; it != it_End; ++it)
		{			
			if(NULL != (*it))
			{
				sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
				pUnit->s_pLoadedObject = (CsC_Object*)(*it)->s_pDigimon;
				g_pThread->DeleteChar( pUnit );	

				SAFE_NIDELETE( (*it));
			}
		}
	}
	*/

	m_pBG = NULL;	
	m_sDigimon.clear();
	m_vecRenderTex.clear();
	m_vecSprite.clear();

	m_nDigimonCount = m_TargetDIgimon = 0;
	m_fScenePlayTime = (float)g_fAccumTime;
	m_fCutTime = 0.0f;


}

void CCrossEvent::ResetDevice()
{
	CCrossEvent::vecTex_it it = m_vecRenderTex.begin();
	CCrossEvent::vecTex_it itEnd = m_vecRenderTex.end();
	for( ; it != itEnd; ++it)
	{
		(*it)->ResetDevice();
	}
}

void CCrossEvent::Init()
{	
	/*
	for(int i =0;i < 1; i++ )
	{
		cSprite* pSprite = NiNew cSprite;
		pSprite->Init( NULL, CsPoint( 0, 0), CsPoint( (int)g_nScreenWidth, (int)g_nScreenHeight ), "CrossWars\\background_X2_2.tga", false );
		m_vecSprite.push_back(pSprite);
	}	
*/


}

cTacticsObject* CCrossEvent::_AddDigimon( DWORD dwModelID, NiPoint3 vPos, float fRot )
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

void CCrossEvent::EventEnd()
{
	g_pGameIF->SetRenderIF( true );
	g_pMngCollector->ReleaseScene();	

	

	Reset();
}

void CCrossEvent::EventStart( int nDigimonID )
{

	m_TargetDIgimon = nDigimonID;

	
	// 이동 중지	
	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();		
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();	

	assert_csm( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_CROSS, L"이벤트 중복" );
	g_pMngCollector->SetSceneState( CMngCollector::eSCENE_CROSS );	
			

	m_sDigimon.clear();
	m_vecRenderTex.clear();
	m_vecSprite.clear();

	g_pGameIF->SetRenderIF( false );

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = OP_LIGHT;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	CsCrossScene::sINFO* pInfo = nsCsFileTable::g_pDigimonMng->GetCrossScene( m_TargetDIgimon )->GetInfo();

	m_pBG = NiNew cSprite;			
	m_pBG->Init( NULL, CsPoint( 0, 0), CsPoint( (int)g_nScreenWidth, (int)g_nScreenHeight ), pInfo->s_szBGPath, false );	


	for(int i =0; i < pInfo->s_nDigimonCount; i++)
	{

		cSprite* pBG  = NiNew cSprite;			
		pBG->Init( NULL, CsPoint( 0, 0), CsPoint( (int)g_nScreenWidth, (int)g_nScreenHeight ), pInfo->s_Scene[i].s_szRegionPath, false );
		m_vecSprite.push_back(pBG);
		
		/*
		CDigimon* pDigimon = g_pCharMng->AddTempDigimon( &prop, pInfo->s_Scene[i].s_dwID, NiPoint3::ZERO, 0.0f, NULL );
		pDigimon->GetProp_Alpha()->SetUseDistAlpha( false );
		pDigimon->SetViewSight( true );
		//pDigimon->SetPos(NiPoint3(10.0f,0,0));
		//m_sDigimon.push_back(pDigimon);	
*/
		sTacticsAni *sAni = NiNew sTacticsAni;

		sAni->s_pDigimon = _AddDigimon( pInfo->s_Scene[i].s_dwID, NiPoint3::ZERO, 0.0f );					
		sAni->s_pDigimon->SetAniOption( pInfo->s_Scene[i].m_dwAni, (float)pInfo->s_Scene[i].m_fStartTime, (float)pInfo->s_Scene[i].m_fEndTime );
		sAni->s_pDigimon->GetProp_Effect()->AddEffect( EFFECT_CROSSCOMBINE_COMPLATE, NiPoint3::ZERO, 6.0f );
		sAni->s_pDigimon->SetViewSight( true );
		m_sDigimon.push_back(sAni);	

		cRenderTex* pRenderTex = NiNew cRenderTex;
		if( pRenderTex->Init( CsPoint( (int)(g_nScreenWidth * pInfo->s_Scene[i].s_width), (int)(g_nScreenHeight * pInfo->s_Scene[i].s_height)) ) == false )
			SAFE_NIDELETE( pRenderTex );

		m_vecRenderTex.push_back(pRenderTex);
	}

	m_fScenePlayTime = (float)g_fAccumTime;
}




void CCrossEvent::Update( float fDeltaTime )
{
	


	CsCrossScene::sINFO* pInfo = nsCsFileTable::g_pDigimonMng->GetCrossScene( m_TargetDIgimon )->GetInfo();
	
	// 하나씩 나오는 시간 = pInfo->s_nDigimonCount / pInfo->s_nCrossSceneTime;

	if((float)g_fAccumTime > m_fScenePlayTime + ((pInfo->s_nCrossSceneTime / pInfo->s_nDigimonCount) * m_nDigimonCount))
	{
		m_nDigimonCount++;		
	}

	

	for(int i =0; (i < m_nDigimonCount && i < pInfo->s_nDigimonCount); i++)
	{
		m_sDigimon[ i ]->s_pDigimon->Update( g_fDeltaTime * 1.25f );		
		//if( m_sDigimon[ i ]->s_pDigimon->IsEnd() == false )
			//break;
	}


	if((float)g_fAccumTime > (m_fScenePlayTime + pInfo->s_nCrossSceneTime) )
		EventEnd();	

	if( GLOBALINPUT_ST.IsEscape() )	//2016-02-24-nova
	{
		EventEnd();		
	}
}

void CCrossEvent::PostRender()
{
	g_pEngine->ScreenSpace();

	m_pBG->Render();	

	CsCrossScene::sINFO* pInfo = nsCsFileTable::g_pDigimonMng->GetCrossScene( m_TargetDIgimon )->GetInfo();

	for(int i =0; (i < m_nDigimonCount && i < pInfo->s_nDigimonCount); i++)
	{
		m_vecSprite[i]->Render();
		m_vecRenderTex[i]->BeginRender();
		m_vecRenderTex[i]->SetDistConstant(pInfo->s_Scene[i].s_dist);
		m_vecRenderTex[i]->SetHeightConstant(pInfo->s_Scene[i].s_high);
		m_vecRenderTex[i]->RenderObject( GET_SUBCAMERA(CAMERA_09), m_sDigimon[i]->s_pDigimon);	//2015-11-30-nova
		//m_sDigimon[i]->s_pDigimon->Render();
		// 렌더 텍스쳐의 좌표, 렌더 uv 제어값..
		//m_vecRenderTex[immn].EndRenderForSquare(CsPoint(200, 200 ), CsPoint(0, 0 ), CsPoint(0, 30 ), CsPoint(200, 20 ), CsPoint(200, 0 ) );
		m_vecRenderTex[i]->EndRenderForSquare( GET_SUBCAMERA(CAMERA_09), 
			CsPoint( (int)(g_nScreenWidth * pInfo->s_Scene[i].s_rx), (int)(g_nScreenHeight * pInfo->s_Scene[i].s_ry) ), 
			CsPoint( (int)(g_nScreenWidth * pInfo->s_Scene[i].s_v0.x), (int)(g_nScreenHeight * pInfo->s_Scene[i].s_v0.y) ), 
			CsPoint( (int)(g_nScreenWidth * pInfo->s_Scene[i].s_v1.x), (int)(g_nScreenHeight * pInfo->s_Scene[i].s_v1.y) ), 
			CsPoint( (int)(g_nScreenWidth * pInfo->s_Scene[i].s_v2.x), (int)(g_nScreenHeight * pInfo->s_Scene[i].s_v2.y) ), 
			CsPoint( (int)(g_nScreenWidth * pInfo->s_Scene[i].s_v3.x), (int)(g_nScreenHeight * pInfo->s_Scene[i].s_v3.y) ) );
	}



	CAMERA_ST._UpdateCamera();
}

void CCrossEvent::Render()
{	
}
#endif