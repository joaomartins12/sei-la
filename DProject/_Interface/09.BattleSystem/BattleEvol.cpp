
#include "stdafx.h"
#include "BattleEvol.h"

#define IF_BATTLEEVOL_SIZE		221


cBattleEvol::cBattleEvol()
{
	m_pFT_BattleEvo = NULL;
}

void cBattleEvol::Destroy()
{
	cBaseWindow::Delete();
}

void cBattleEvol::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pBar );
	SAFE_NIDELETE( m_pBarCharge );
	SAFE_NIDELETE( m_pDeviceBar );
	SAFE_NIDELETE( m_pBtnMask );
	SAFE_NIDELETE( m_pAniBG );
	SAFE_NIDELETE( m_pAniInside );
	m_pFT_BattleEvo = NULL;

	_DeleteCharImageList();
	m_vpCharImage.Destroy();
}

void cBattleEvol::_DeleteCharImageList()
{
	m_nRenderIndex = 0;
	m_RenderTimeSeq.OffReset();

	int nCnt = m_vpCharImage.Size();
	for( int i=0; i<nCnt; ++i )
	{
		g_pCharResMng->CharImageResDelete( &m_vpCharImage[ i ] );
	}
	m_vpCharImage.ClearElement();
}

void cBattleEvol::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "QuickSlot\\BattleEvol_Win.tga", CsPoint( 795, 768 - nValue*44 - 31 - 66 ), CsPoint( IF_BATTLEEVOL_SIZE, 65 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );

	m_pBar = NiNew cProgressBar;
	m_pBar->Init( m_pRoot, cProgressBar::BAR_STATIC, CsPoint( 10, 29 ), CsPoint( 150, 23 ), CsPoint( 150, 23 ), "QuickSlot\\BattleEvol_Bar.tga", false );
	m_pBarCharge = NiNew cProgressBar;
	m_pBarCharge->Init( m_pRoot, cProgressBar::BAR_STATIC, CsPoint( 10, 29 ), CsPoint( 150, 23 ), CsPoint( 150, 23 ), "QuickSlot\\BattleEvol_BarCharge.tga", false );

	m_pDeviceBar = NiNew cSprite;
	m_pDeviceBar->Init( m_pBar, CsPoint::ZERO, CsPoint( 5, 12 ), "QuickSlot\\BattleEvol_DeviceBar.tga", false );

	m_pBtnMask = NiNew cButton;
	m_pBtnMask->Init( m_pRoot, CsPoint( 160, 5 ), CsPoint( 56, 56 ), "QuickSlot\\BattleEvol_Mask.tga", false );
	m_pBtnMask->SetTexToken( CsPoint( 54, 0 ) );
	//m_pBtnMask->SetEnable( false );

	m_pAniBG = NiNew cSpriteAni;
	m_pAniBG->Init( cSpriteAni::LOOP, m_pRoot, CsPoint( 124, -31 ), CsPoint( 128, 128 ), "QuickSlot\\ANI_BG\\BG", "tga", 20, false );
	m_pAniBG->SetAniTime( 0.1f );

	m_pAniInside = NiNew cSpriteAni;
	m_pAniInside->Init( cSpriteAni::LOOP, m_pRoot, CsPoint( 160, 5 ), CsPoint( 56, 56 ), "QuickSlot\\ANI_Inside\\Inside", "tga", 16, false );
	m_pAniInside->SetAniTime( 0.05f );

	m_RenderTimeSeq.SetDeltaTime( 3500 );
	ResetTarget();
}

void cBattleEvol::ResetTarget()
{
	m_pFT_BattleEvo = g_pCharMng->GetDigimonUser( 0 )->GetFTBattleEvo();

	CDigimonUser::sBASE_STAT* pBaseStat = g_pCharMng->GetDigimonUser( 0 )->GetBaseStat();

	int nCurEP = (int)pBaseStat->GetBattleEP();
	if( nCurEP > m_pFT_BattleEvo->s_nMaxEP )
		pBaseStat->SetBattleEP( (float)m_pFT_BattleEvo->s_nMaxEP );

	// 바 크기 책정
	m_pBar->SetRange( CsPoint( 0, m_pFT_BattleEvo->s_nMaxEP ) );
	m_pBarCharge->SetRange( CsPoint( 0, m_pFT_BattleEvo->s_nMaxEP ) );

	// 바 위치
	m_nDeviceBarCnt = 0;
	CDigimonEvolution::sBATTLE_EVO_INFO::sELEMENT* pElement = NULL;
	for( int i=0; i<FT_BATTLE_EVO_ELEMENT_CNT; ++i )
	{
		pElement = &m_pFT_BattleEvo->s_Element[ i ];
		if( pElement->s_bEnable == false )
		{
			break;
		}
		float fRate = pElement->s_nReqEvoEP / (float)m_pFT_BattleEvo->s_nMaxEP;

		m_ptDeviceBarPosArray[ i ].x = (int)( 150 * fRate );
		m_ptDeviceBarPosArray[ i ].y = 11;
		++m_nDeviceBarCnt;
	}

	ResetEPBar( (int)pBaseStat->GetBattleEP() );
}

void cBattleEvol::ResetEPBar( int nCurEP )
{
	m_ptRangeEP = CsPoint::ZERO;
	m_bMaxEP = false;
	int nEnableEvolStep = 0;
	_DeleteCharImageList();

	CDigimonEvolution::sBATTLE_EVO_INFO::sELEMENT* pElement = NULL;
	for( int i=0; i<FT_BATTLE_EVO_ELEMENT_CNT; ++i )
	{		
		pElement = &m_pFT_BattleEvo->s_Element[ i ];
		if( pElement->s_bEnable == false )
		{
			break;
		}
		
		if( ( pElement->s_nReqEvoEP != 0 )&&( nCurEP < pElement->s_nReqEvoEP ) )
		{
			m_ptRangeEP.y = pElement->s_nReqEvoEP;
			break;
		}

		m_ptRangeEP.x = pElement->s_nReqEvoEP;
		nEnableEvolStep = pElement->s_nEvolStep;
	}

	if( nCurEP >= m_pFT_BattleEvo->s_nMaxEP )
	{
		m_bMaxEP = true;
	}
	
	m_pBar->SetBarPos( nCurEP );
	m_pBarCharge->SetBarPos( m_ptRangeEP.x );

	//===============================================================================================================================
	//
	//		얼굴 이미지
	//
	//===============================================================================================================================
	// 스텝 1. 진화 가능한 개체중 최고등급 이미지
	// 스텝 2. 없다면, 진화 가능한 바로 위 등급이 있는지 검색 - 없다면 - 내얼굴 디스에이블
	// 스텝 3. 없다면, 진화 가능한 바로 위 등급이 있는지 검색 - 있다면 - 해당 얼굴 디스에이블

	int nStep1_Evo = -1;
	int nStep2_Evo = 1000;
	std::list< DWORD > listEnable;
	std::list< DWORD > listUpper;
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	CDigimonEvolveInfo* pFTEvolInfo = pUser->GetFTEvol();
	CDigimonEvolveObj* pFTEvolObj = pUser->GetFTEvolCurObj();
	CDigimonEvolveObj::SEvolutionInfo* pFTEvolObjInfo;
	for( int i=0; i<nLimit::EvoStep-1; ++i )
	{
		pFTEvolObjInfo = &pFTEvolObj->m_nEvolutionList[ i ];

		// ID가 존재 하는 값일 경우에
		if( pFTEvolObjInfo->nSlot == CDigimonEvolveObj::SEvolutionInfo::INVAIDE )
			continue;

		CDigimonEvolveObj* pStepEvolObj = pFTEvolInfo->GetEvolveObjByID( pFTEvolObjInfo->dwDigimonID );
		if( pStepEvolObj == NULL )
			continue;		

		if( pStepEvolObj->m_nEnableSlot == 0 )
			continue;

		if( pUser->IsEvoPlag( pFTEvolObjInfo->nSlot, CDigimonUser::eActive ) == false )
			continue;
				
		CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObjInfo->dwDigimonID )->GetInfo();
		int nEvoStep = pFTDigimon->s_eEvolutionType;

		if( nStep2_Evo > nEvoStep )
		{
			nStep2_Evo = nEvoStep;
			listUpper.clear();
			listUpper.push_back( pFTDigimon->s_dwModelID );
		}
		else if( nStep2_Evo == nEvoStep )
		{
			listUpper.push_back( pFTDigimon->s_dwModelID );
		}
		
		if( nEvoStep <= nEnableEvolStep )
		{
			if( nStep1_Evo < nEvoStep )
			{
				nStep1_Evo = nEvoStep;
				listEnable.clear();
				listEnable.push_back( pFTDigimon->s_dwModelID );
			}
			else if( nStep1_Evo == nEvoStep )
			{
				listEnable.push_back( pFTDigimon->s_dwModelID );
			}
		}
	}

	if( listEnable.empty() == false )
	{
		std::list< DWORD >::iterator it = listEnable.begin();
		std::list< DWORD >::iterator itEnd = listEnable.end();
		for( ; it!=itEnd; ++it )
		{
			m_vpCharImage.PushBack( g_pCharResMng->CharImageResLoad( *it ) );
		}
		m_pBtnMask->SetEnable( true );
	}
	else if( listUpper.empty() == false )
	{
		std::list< DWORD >::iterator it = listUpper.begin();
		std::list< DWORD >::iterator itEnd = listUpper.end();
		for( ; it!=itEnd; ++it )
		{
			m_vpCharImage.PushBack( g_pCharResMng->CharImageResLoad( *it ) );
		}
		m_pBtnMask->SetEnable( false );
	}
	else
	{		
		m_vpCharImage.PushBack( g_pCharResMng->CharImageResLoad( pUser->GetFTDigimon()->GetInfo()->s_dwModelID ) );
		m_pBtnMask->SetEnable( false );
	}
}

void cBattleEvol::Update(float const& fDeltaTime)
{
	m_pAniBG->Update( fDeltaTime );
	m_pAniInside->Update( fDeltaTime );

	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	int nCurEP = (int)pDigimonUser->GetBaseStat()->GetBattleEP();

	// 맥스치 도달 안했다면
	if( m_bMaxEP == false )
	{
		// 변화폭을 벋어 났다면
		if( ( nCurEP < m_ptRangeEP.x )||( nCurEP >= m_ptRangeEP.y ) )
		{
			ResetEPBar( nCurEP );
		}
		else
		{
			m_pBar->SetBarPos( nCurEP );
		}
	}
	// 맥스치
	else
	{		
		// 변화
		if( nCurEP < m_pFT_BattleEvo->s_nMaxEP )
		{
			ResetEPBar( nCurEP );
		}
	}	
}

cBaseWindow::eMU_TYPE cBattleEvol::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if( m_pBtnMask->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		DWORD nModelID = m_vpCharImage[ m_nRenderIndex ]->s_nID;
		CDigimonEvolveObj* pCurFTEvol = g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj();
		for( int i=0; i<MAX_EVOLUTION; ++i )
		{
			DWORD nSrcDigimonID = pCurFTEvol->m_nEvolutionList[ i ].dwDigimonID;
			if( nSrcDigimonID == CDigimonEvolveObj::SEvolutionInfo::INVAIDE )
				continue;

			if( nsCsFileTable::g_pDigimonMng->IsDigimon( nSrcDigimonID ) == false )
				continue;

			if( nsCsFileTable::g_pDigimonMng->GetDigimon( nSrcDigimonID )->GetInfo()->s_dwModelID != nModelID )
				continue;

			g_pGameIF->GetQuickEvol()->Req_ActivateEvolve( i );
		}
		return muReturn;
	}

	return muReturn;
}

void cBattleEvol::Render()
{
	RenderScript();
	m_pBar->Render();
	m_pBarCharge->Render();

	// 디바이스 바
	for( int i=0; i<m_nDeviceBarCnt-1; ++i )
	{
		if( m_ptDeviceBarPosArray[ i ].x == 0 )
			continue;

		m_pDeviceBar->Render( m_ptDeviceBarPosArray[ i ] );
	}


	// 얼굴 이미지	
	assert_cs( m_vpCharImage.Size() );	
	m_vpCharImage[ m_nRenderIndex ]->Render_L( GetRootClient() + CsPoint( 160, 3 ), CsPoint( 57, 57 ) );
	if( ( m_vpCharImage.Size() > 1 )&&( m_RenderTimeSeq.IsEnable() ) )
	{
		++m_nRenderIndex;
		if( m_nRenderIndex >= m_vpCharImage.Size() )
		{
			m_nRenderIndex = 0;
		}
	}

	if( m_pBtnMask->IsEnable() )
	{
		m_pAniBG->Render();
		m_pAniInside->Render();
	}	

	m_pBtnMask->Render();
}

void cBattleEvol::ResetDevice()
{
	ResetDeviceScript();

	m_pBar->ResetDevice();
	m_pBarCharge->ResetDevice();
	m_pDeviceBar->ResetDevice();
	m_pBtnMask->ResetDevice();
	m_pAniBG->ResetDevice();
	m_pAniInside->ResetDevice();
}
