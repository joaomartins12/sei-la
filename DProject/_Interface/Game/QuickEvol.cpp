
#include "stdafx.h"
#include "QuickEvol.h"

#define IF_QUICK_SLOT_SIZE			261
#define IF_QUICK_SLOT_SIZE_RIDE		290

#ifdef _GIVE
	#define RIDE_COOLTIME				5.0f
#else
	#define RIDE_COOLTIME				0.01f
#endif

cQuickEvol::cQuickEvol()
{
	m_pTargetObject = NULL;
	m_nQuickEvolIndex = 0;
	m_dwDestDigimonID = 0;
}

void cQuickEvol::Destroy()
{
	cBaseWindow::Delete();
}

void cQuickEvol::DeleteResource()
{
	DeleteScript();

	for( int i=0; i<nLimit::EvoStep; ++i )
	{
		EvolveIcons[ i ].Delete();
	}
	SAFE_NIDELETE( m_pSlotBG );
	SAFE_NIDELETE( m_pRideBtn );
#ifdef SDM_TAMER_XGUAGE_20180628
	SAFE_NIDELETE( m_pXSlotBG );
#endif
}

void cQuickEvol::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_9 );
	ti.s_eTextAlign = DT_RIGHT;

	InitScript( NULL, CsPoint( 397, 768 - nValue*44 - 31 - 27 ), CsPoint( IF_QUICK_SLOT_SIZE, 29 ), false, IFREGION_X::LEFT, IFREGION_Y::BOTTOM );
	//윈도우 시작 위치 저장 - 위치 이동할때 써먹음
	m_ptRootPos = GetRootClient();
	//슬롯 배경
	m_pSlotBG = NiNew cSprite;
	m_pSlotBG->Init( NULL, CsPoint::ZERO, CsPoint( 29, 29 ), "QuickSlot\\Evol.tga", false );

#ifdef SDM_TAMER_XGUAGE_20180628
	m_pXSlotBG = NiNew cSprite;
	m_pXSlotBG->Init( NULL, CsPoint::ZERO, CsPoint( 29, 29 ), "QuickSlot\\Evol_X.tga", false );
#endif

	CsSIZE size( 21, 21 );

	for( int i=0; i<nLimit::EvoStep; ++i )
		EvolveIcons[ i ].s_IFIcon.Init( CsRect( CsPoint( ( 29*i ) + 4, 4 ), size ) );

	m_pRideBtn = NiNew cButton;
	m_pRideBtn->Init( m_pRoot, CsPoint( 29*nLimit::EvoStep, 0 ), CsPoint( 29, 29 ), "QuickSlot\\RideBtn.tga", false );
	m_pRideBtn->SetTexToken( CsPoint( 0, 29 ) );

	m_RideCoolTime.SetEndTime( RIDE_COOLTIME );
	m_EvolCoolTime.SetEndTime( 8.0f + 0.15f );
}

cBaseWindow::eMU_TYPE cQuickEvol::Update_ForMouse()
{
	if( g_pResist->m_CenterBar.s_bIsOpenQuick )//추가 퀵슬롯 오픈상태일 때
		m_pRoot->SetPos( m_ptRootPos - CsPoint( 0, 44 ) );
	else	//퀵슬롯 닫힌 상태
		m_pRoot->SetPos( m_ptRootPos );

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if( ( m_pTargetObject == NULL ) || ( m_pTargetObject->IsLoad() == false ) )
		return muReturn;
/*
#ifdef CROSSWARS_SYSTEM
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_CROSS || g_pDataMng->GetServerSync()->IsCrossChangeParter() ) {
		return muReturn;
	}
#endif
*/
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();
	switch( eCursor )
	{
	case CURSOR::LBUTTON_UP:
	case CURSOR::RBUTTON_UP:
	case CURSOR::BUTTON_NONE:
		{
			for( int i=0; i<nLimit::EvoStep; ++i )
			{
				if( EvolveIcons[ i ].s_bRender == false )
					continue;

				// 이미지가 없거나 노말 상태가 아닌것은 패스
				if( ( EvolveIcons[ i ].s_pImage == NULL )||( EvolveIcons[ i ].s_pImage->GetState() != cImage::NORMAL0 ) )
					continue;

				EvolveIcons[ i ].s_IFIcon.MouseOn( ptMouseLocalPos );				
			}
		}	
		break;

		// L, R 동작 같다
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			for( int i=0; i<nLimit::EvoStep; ++i )
			{
				if( EvolveIcons[ i ].s_bRender == false )
					continue;

				// 이미지가 없거나 노말 상태가 아닌것은 패스
				if( ( EvolveIcons[ i ].s_pImage == NULL )||( EvolveIcons[ i ].s_pImage->GetState() != cImage::NORMAL0 ) )
					continue;

				EvolveIcons[ i ].s_IFIcon.MouseLBtnClick( ptMouseLocalPos );					
			}
		}
		break;
	}

	// 툴팁용
	{
		for( int i=0; i<nLimit::EvoStep; ++i )
		{
			if( EvolveIcons[ i ].s_bRender == false )
				continue;

			// 이미지가 없으면 패쓰
			if( EvolveIcons[ i ].s_pImage == NULL )
				continue;

			if( !EvolveIcons[ i ].s_IFIcon.s_Rect.PtInRect( ptMouseLocalPos ) )
				continue;

			if( !GLOBALINPUT_ST.IsShift() )
				break;
			
			CsPoint pos = EvolveIcons[ i ].s_IFIcon.GetPos() + GetRootClient();
			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
			if( pDigimonUser == NULL || pDigimonUser->GetFTEvolCurObj() == NULL || pDigimonUser->GetFTEvol() == NULL )
				break;

			DWORD dwModelID = pDigimonUser->GetModelID();
			DWORD dwDestModelID = pDigimonUser->GetFTEvolCurObj()->m_nEvolutionList[ i ].dwDigimonID;
			CDigimonEvolveObj* pFTEvolObj = pDigimonUser->GetFTEvol()->GetEvolveObjByID( dwDestModelID );
			if( pFTEvolObj == NULL )
				break;

			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 21, 21 ), TOOLTIP_MAX_SIZE, cTooltip::QUICK_EVOL, cBaseWindow::WT_QUICKEVOL, pFTEvolObj->m_nEvoSlot, dwModelID, i );
			break;
		}
	}

	if( m_bEnableRide )
	{
		if( m_pRideBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			SetRideToggle();			
		}
	}

	return muReturn;
}

void cQuickEvol::Render()
{
	RenderScript();

	bool bCoolTime = m_EvolCoolTime.IsCoolTime();

	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pDUser );
	DWORD nCurDigimonID = pDUser->GetFTID();
	CDigimonUser::sBASE_STAT* pDigimonStat = pDUser->GetBaseStat();
	SAFE_POINTER_RET( pDigimonStat );
	int nLevel = pDigimonStat->GetLevel();

	CTamerUser* pTUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTUser );
	CTamerUser::sUSER_STAT* pTamerStat = dynamic_cast<CTamerUser::sUSER_STAT*>(pTUser->GetBaseStat());
	SAFE_POINTER_RET( pTamerStat );
	int nTamerDS = pTamerStat->GetDS();

	EvolveIcon* szEvolveIcon = NULL;
	CsPoint pos;
	CsPoint bgrenderpos;

	for( int i=0; i<nLimit::EvoStep; ++i )
	{
		if( g_pResist->m_CenterBar.s_bIsOpenQuick )//추가 퀵슬롯 오픈상태일 때
			bgrenderpos = m_ptRootPos + CsPoint( 29*i, -44 );
		else
			bgrenderpos = m_ptRootPos + CsPoint( 29*i, 0 );

		m_pSlotBG->Render( bgrenderpos );

		if( EvolveIcons[ i ].s_bRender == false )
			continue;

		szEvolveIcon = &EvolveIcons[ i ];
		pos = szEvolveIcon->s_IFIcon.GetPos() + m_ptRootPos;
		if( szEvolveIcon->s_pImage != NULL )
		{
			CDigimonEvolveObj* pFTEvolObj = pDUser->GetFTEvol()->GetEvolveObjByEvoSlot( szEvolveIcon->s_pEvolObjInfo->nSlot );
			if(pFTEvolObj == NULL)
				continue;
#ifdef SDM_TAMER_XGUAGE_20180628
			if(pFTEvolObj->m_nOpenQualification == NEED_QUALITICATION::_XAI_SYSTEM)
				m_pXSlotBG->Render( bgrenderpos );
#endif
			szEvolveIcon->s_pImage->Render( pos );

			// 쿨타임
			if( bCoolTime == true )
			{
				g_pIconMng->RenderTime( pos, szEvolveIcon->s_pImage->GetSize().x*0.5f, &m_EvolCoolTime );
			}

			// 마스크 있다면 마스크 랜더링
			if( szEvolveIcon->s_IFIcon.s_eState != sIFIcon::NONE )
			{			
				szEvolveIcon->s_IFIcon.RenderMask( pos, CsPoint( 21, 21 ) );
			}

#ifdef SDM_TAMER_XGUAGE_20180628
			if( szEvolveIcon->s_isXAnti )
			{
				if( FMDigimon::GetCostEvlove( nCurDigimonID, nLevel, szEvolveIcon->s_pEvolObjInfo->dwDigimonID ) > pTamerStat->GetCurrentXGuage() )				
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, CsPoint( 21, 21 ) );
				}
			}
			else
#endif
			if( FMDigimon::GetCostEvlove( nCurDigimonID, nLevel, szEvolveIcon->s_pEvolObjInfo->dwDigimonID ) > nTamerDS )				
			{
				g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, CsPoint( 21, 21 ) );
			}			

			if( pDUser->IsEvoPlag( szEvolveIcon->s_pEvolObjInfo->nSlot, CDigimonUser::eActive ) == true )
			{

				if( pFTEvolObj->m_nUseItem != 0 )
				{
					if( g_pDataMng->GetInven()->GetCount_Item_ID( pFTEvolObj->m_nUseItem ) < pFTEvolObj->m_nUseItemNum )
						g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, CsPoint( 21, 21 ) );
				}
				if( pFTEvolObj->m_nIntimacy != 0 )
				{
					if( g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetFriendShip() < pFTEvolObj->m_nIntimacy )	
						g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, CsPoint( 21, 21 ) );
				}
			}			
		}
	}

	if( m_bEnableRide )
	{
		m_pRideBtn->SetPos( CsPoint( 29*nLimit::EvoStep, 0 ) );
		m_pRideBtn->Render();

		// 쿨타임		
		g_pIconMng->RenderTime( m_pRideBtn->GetClient() + CsPoint( 3, 3 ) , (m_pRideBtn->GetSize().x-6)*0.5f, &m_RideCoolTime );
	}
}

void cQuickEvol::SetTargetObject( CsC_AvObject* pTarget )
{
	m_pTargetObject = pTarget;	
	assert_cs( m_pTargetObject != NULL );
	assert_cs( m_pTargetObject->IsKindOf( RTTI_DIGIMON_USER ) );
	CDigimonUser* pUser = (CDigimonUser*)m_pTargetObject;

	CDigimonEvolveInfo* pFTEvolInfo = pUser->GetFTEvol();
	CDigimonEvolveObj* pFTEvolObj;
	CDigimonEvolveObj::SEvolutionInfo* pFTEvolObjInfo;

	pFTEvolObj = pUser->GetFTEvolCurObj();
	int nLastEvoStep = nLimit::EvoStep;
	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
	{
		nLastEvoStep = nLimit::EvoStep - 1;
		EvolveIcons[ nLastEvoStep ].Delete();
	}

	for( int i=0; i<nLastEvoStep; ++i )
	{
		pFTEvolObjInfo = &pFTEvolObj->m_nEvolutionList[ i ];
		EvolveIcons[ i ].Delete();

		// ID가 존재 하는 값일 경우에
		if( pFTEvolObjInfo->nSlot == CDigimonEvolveObj::SEvolutionInfo::INVAIDE )
			continue;

		CDigimonEvolveObj* pStepEvolObj = pFTEvolInfo->GetEvolveObjByID( pFTEvolObjInfo->dwDigimonID );
		if( pStepEvolObj == NULL )
			continue;		
		
		if( pStepEvolObj->m_nEnableSlot == 0 )
		{
			BHPRT( "디지몬ID [%d] 슬롯번호 [%d] 사용할 수 없는 슬롯 m_nEnableSlot [%d]", pFTEvolObjInfo->dwDigimonID, pFTEvolObjInfo->nSlot, pStepEvolObj->m_nEnableSlot   );
			continue;
		}

		// 값 설정 및 포인터 연결
		EvolveIcons[ i ].s_pEvolObjInfo = pFTEvolObjInfo;

		// 진화 이미지 이름 찾기
		assert_cs( EvolveIcons[ i ].s_pImage == NULL );

		assert_csm1( EvolveIcons[ i ].s_pEvolObjInfo->dwDigimonID != 0, _T( "디지몬진화 테이블 문제 - 기본체 ID : %d" ), g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID() );

		CsDigimon* pkCsDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( EvolveIcons[ i ].s_pEvolObjInfo->dwDigimonID );
		if(pkCsDigimon == NULL)
			continue;

		CsDigimon::sINFO* pFTInfo = pkCsDigimon->GetInfo();
		std::string strPath = g_pModelDataMng->GetEvoModelIconFile( pFTInfo->s_dwModelID );	
	
		// 진화 이미지 로드
		EvolveIcons[ i ].s_pImage = NiNew cImage;
		EvolveIcons[ i ].s_pImage->Init( NULL, CsPoint::ZERO, CsPoint( 21, 21 ), strPath.c_str(), false, false );
		EvolveIcons[ i ].s_pImage->SetTexToken( CsPoint( 32, 0 ) );

		EvolveIcons[ i ].s_bRender = true;

		//NEWEVO

		DWORD EvolValue = pUser->GetEvoValue( pFTEvolObjInfo->nSlot, CDigimonUser::eActive_Mask );
		BHPRT( "Digimon ID [%d] Evo Slot [%d] EolValue [%d]", 
			pFTEvolObjInfo->dwDigimonID, pFTEvolObjInfo->nSlot, EvolValue  );
		switch( EvolValue)
		{
		case CDigimonUser::eActive:
			break;
		case CDigimonUser::eInActive:						
			EvolveIcons[ i ].s_pImage->SetState( cImage::NORMAL2 );	
			break;					
		case CDigimonUser::eClosed:				
		case CDigimonUser::eNone:
			EvolveIcons[ i ].s_bRender = false;
			break;
		default:
			assert_csm2( false, _T( "디지몬ID %d\n슬롯번호 %d" ), pFTEvolObjInfo->dwDigimonID, pFTEvolObjInfo->nSlot );
			break;
		}		
#ifdef SDM_TAMER_XGUAGE_20180628
		EvolveIcons[ i ].s_isXAnti = pkCsDigimon->isX_Anti();
#endif	

		if( pUser->IsEvoPlag( pFTEvolObjInfo->nSlot, CDigimonUser::eInActive ) )
		{
			// 퀵슬롯 캡슐진화체 렌더 셋 관련
			//캡슐진화체는 진화슬롯 번호 순서대로 들어간게 아니라서 바로앞에놈 검사는 무의미하다
			if( pStepEvolObj->m_nEvolutionTree != CDigimonUser::TREE_CAPSULE )
			{
				if( pUser->IsEvoPlag( pFTEvolObjInfo->nSlot - 1, CDigimonUser::eActive ) == false )
					EvolveIcons[ i ].s_bRender = false;
			}
		}
	}
	
	m_bEnableRide = pUser->IsEvoPlag( pFTEvolObj->m_nEvoSlot, CDigimonUser::eRide ) &&
					nsCsFileTable::g_pDigimonMng->IsRide( ( (CDigimon*)m_pTargetObject )->GetFTID() );
	if( m_bEnableRide == true )
		m_pRoot->SetSize( CsPoint( IF_QUICK_SLOT_SIZE_RIDE, 29 ) );
	else
		m_pRoot->SetSize( CsPoint( IF_QUICK_SLOT_SIZE, 29 ) );

	TOOLTIPMNG_STPTR->GetTooltip()->ReleaseRenderToolTip( cTooltip::QUICK_EVOL );
}

void cQuickEvol::OnLButtonUp( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
	sIFIcon* pIFIcon;
	for( int i=0; i<nLimit::EvoStep; ++i )
	{
		pIFIcon = &EvolveIcons[ i ].s_IFIcon;
		if( ( pIFIcon->s_eState == sIFIcon::MOUSE_LCLICK )&&( pIFIcon->s_Rect.PtInRect( ptLocal ) ) )
		{
			assert_cs( ( EvolveIcons[ i ].s_pImage != NULL )&&( EvolveIcons[ i ].s_pImage->GetState() == cImage::NORMAL0 ) );
			// 동작
			Req_ActivateEvolve( i );
		}
		pIFIcon->s_eState = sIFIcon::NONE;
	}
}

void cQuickEvol::OnRButtonUp( CsPoint pos )
{
	OnLButtonUp( pos );	
}

void cQuickEvol::ResetDevice()
{
	ResetDeviceScript();

	for( int i=0; i<nLimit::EvoStep; ++i )
	{
		if( EvolveIcons[ i ].s_pImage != NULL )
			EvolveIcons[ i ].s_pImage->ResetDevice();
	}

	m_pRideBtn->ResetDevice();
}

bool cQuickEvol::OnMacroKey(const MSG& p_kMsg)
{
	switch(p_kMsg.message)
	{
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			
			for( int i = 0 ; i <= (DMKEY::EVOL_5-DMKEY::EVOL_0)  ; ++i )
			{
				if( GLOBALINPUT_ST.GetEqualSnKey(p_kMsg.wParam, DMKEY::EVOL_0+i) && GLOBALINPUT_ST.GetEqualModifier(DMKEY::EVOL_0+i) )
				{
					if( i == 5 )
						i = 8;

					Req_ActivateEvolve( i );
					return true;
				}
			}
			if( GLOBALINPUT_ST.GetEqualSnKey(p_kMsg.wParam, DMKEY::RIDE_TOGGLE) && GLOBALINPUT_ST.GetEqualModifier(DMKEY::RIDE_TOGGLE) )
			{
				if( m_bEnableRide )
				{
					SetRideToggle();
					return true;
				}
			}
		}
		break;
	}
	return false;
}
void cQuickEvol::Send_Evolvution(int EvolveIdx)
{
	int TargetUniqID = m_pTargetObject->GetUniqID();
	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CHANGE_KFM, 0, 
		&TargetUniqID, &EvolveIdx, 
		&EvolveIcons[ EvolveIdx ].s_pEvolObjInfo->dwDigimonID );	// SendEvolution
	BHPRT("TRY EVOLUTION XANTI DIGIMON  : Send_Evolvution TagetUniqID[%d] EvolIndx[%d] DwordID[%d]",
		TargetUniqID, EvolveIdx, EvolveIcons[ EvolveIdx ].s_pEvolObjInfo->dwDigimonID);
}


void cQuickEvol::Req_ActivateEvolve( int nIndex, bool bAutoEvo /*=false*/ )
{	
	CheckingEvolutionConditions(nIndex, bAutoEvo);
}
void cQuickEvol::CheckingEvolutionConditions(const int nIndex, bool bAutoEvo)
{
	SAFE_POINTER_RET( m_pTargetObject);
	SAFE_POINTER_RET( EvolveIcons[ nIndex ].s_pEvolObjInfo);
	SAFE_POINTER_RET( EvolveIcons[ nIndex ].s_pImage);
	DWORD TargetDigimonID = EvolveIcons[ nIndex ].s_pEvolObjInfo->dwDigimonID;

	if(CheckCommonCondition(nIndex) == false) 
		return;

	if(CheckEvolAbleArea(nIndex, bAutoEvo) == false)
		return;

	if(CheckEvolAbleAnimState() == false)
		return;

#ifdef SDM_TAMER_XGUAGE_20180628
	if(isXAntiDigimon(TargetDigimonID) == true)
	{
		BHPRT("TRY EVOLUTION XANTI DIGIMON ");
		if(isSettedXAISystem() == false)
			return;
		if(hasEnoughXGage(nIndex) == false)
			return;
		if(isXEvolutionOfJogress(nIndex) == true)
		{
			if( CheckingXEvolutionOfJogress(nIndex) == false)
				return;
		}
	}
	else
#endif
	{
		if(CheckHasEvolItem(nIndex, bAutoEvo) == false)
			return;

		if(CheckingDs(nIndex) == false)
			return;

		if(CheckingEvolAboutJogress(nIndex, bAutoEvo) == false)
			return;
	}

	Send_Evolvution(nIndex);
}
bool	cQuickEvol::CheckingDs(int nIndex)
{
	// DS 체크
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_BATTLE:
		{
			CDigimonUser* pUser = (CDigimonUser*)m_pTargetObject;
			CDigimonEvolution::sBATTLE_EVO_INFO* pFTBE = pUser->GetFTBattleEvo();
			int nEvoSlot = nsCsFileTable::g_pDigimonMng->GetDigimon( EvolveIcons[ nIndex ].s_pEvolObjInfo->dwDigimonID )->GetInfo()->s_eEvolutionType;

			// Battle Evolution Point Check
			bool bFind = false;
			for( int i=0; i<nLimit::BattleEvo_Element; ++i )
			{
				if( pFTBE->s_Element[ i ].s_bEnable == false )
					continue;

				if( pFTBE->s_Element[ i ].s_nEvolStep != nEvoSlot )
					continue;


				if( pFTBE->s_Element[ i ].s_nReqEvoEP > pUser->GetBaseStat()->GetBattleEP() )
				{
					// 포인트 모자람					
					return false;
				}
				bFind = true;
				break;
			}

			if( bFind == false )
			{
				// 정보 못 찾음
				return false;
			}
		}
		break;
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		{			
			int nCostDS = FMDigimon::GetCostEvlove( m_pTargetObject->GetFTID(), m_pTargetObject->GetBaseStat()->GetLevel(), EvolveIcons[ nIndex ].s_pEvolObjInfo->dwDigimonID );
			if( nCostDS > g_pCharMng->GetTamerUser()->GetBaseStat()->GetDS() )
			{
				// DS 부족
				cPrintMsg::PrintMsg( 13001 );
				return false;
			}
		}
		break;
	default:
		{
			assert_cs( false );
			return false;
		}
		break;
	}	
	return true;

}


bool  cQuickEvol::CheckHasEvolItem(int nIndex, bool bAutoEvo)
{
	// 소모아이템 있는지 체크
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	CDigimonEvolveObj* pFTEvolObj = pDUser->GetFTEvol()->GetEvolveObjByEvoSlot( EvolveIcons[ nIndex ].s_pEvolObjInfo->nSlot );
	if( bAutoEvo == false )
	{
		if( pFTEvolObj->m_nUseItem != 0 )
		{
			if( g_pDataMng->GetInven()->GetCount_Item_ID( pFTEvolObj->m_nUseItem ) < pFTEvolObj->m_nUseItemNum )
			{
				cPrintMsg::PrintMsg( 13005 );
				return false;
			}
		}	
		if( pFTEvolObj->m_nIntimacy != 0 )
		{
			if( g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetFriendShip() < pFTEvolObj->m_nIntimacy )	
			{
				cPrintMsg::PrintMsg( 13005 );
				return false;
			}
		}
	}
	return true;
}
bool cQuickEvol::CheckEvolAbleArea(int nIndex, bool bAutoEvo)
{
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	CDigimonEvolveObj* pFTEvolObj = pDUser->GetFTEvol()->GetEvolveObjByEvoSlot( EvolveIcons[ nIndex ].s_pEvolObjInfo->nSlot );
	// 진화가 가능한 지역인가
	int dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	// 진화 제한 구역 체크
	//캡슐진화체는 진화제한구역 무시
	bool bIsCapsule = g_pCharMng->GetDigimonUser( 0 )->IsCapsuleTree( pFTEvolObj->m_nEvolutionTree );//진화 하려는 디지몬의 진화트리가 캡슐진화체인지

	u4 nBaseD = g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID();

	if( nBaseD != pFTEvolObj->m_dwID	// 성장기로 돌아가는거면 그냥 돌려주자( 체크 안함 )
		&& !bIsCapsule					// 캡슐진화체라면 진화 가능 ( 체크안함 )
		&& nsCsMapTable::g_pMapRegionMng->IsLimitEvoGroup( dwMapID ) == true )
	{
		CsMap_LimitEvo::LIST* pList = nsCsMapTable::g_pMapRegionMng->GetLimitEvoGroup( dwMapID )->GetList();
		CsMap_LimitEvo::LIST_IT it = pList->begin();
		CsMap_LimitEvo::LIST_IT itEnd = pList->end();
		CsMap_LimitEvo::sINFO* pLimitEvoInfo = NULL;
		for( ; it!=itEnd; ++it )
		{
			pLimitEvoInfo = (*it)->GetInfo();
			NiPoint2 vLE( (float)pLimitEvoInfo->s_nCenterX, (float)pLimitEvoInfo->s_nCenterY );
			if( ( g_pCharMng->GetDigimonUser( 0 )->GetPos2D() - vLE ).Length() < pLimitEvoInfo->s_nRadius )
			{
				if( bAutoEvo == false )
				{
					cPrintMsg::PrintMsg( 30110 );
				}				
				return false;
			}
		}
	}
	return true;
}
bool cQuickEvol::CheckEvolAbleAnimState(void)
{
	//진화가 가능한지 체크
	if( m_pTargetObject->GetProp_Animation()->IsEnableEvol() == false )
	{
		cPrintMsg::PrintMsg( 13005 );
		return false;
	}	
	return true;
}
bool cQuickEvol::CheckPacketSync(void)
{
	// 스킬을 사용패킷을 보낸 상태인지 체크
	if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::USE_DIGIMON_SKILL ) == false )
	{
		cPrintMsg::PrintMsg( 13005 );
		return false;
	}

	// 서버에 패킷을 보낸상태인가
	if( g_pDataMng->GetServerSync()->IsChageDigimon() )	
	{
		cPrintMsg::PrintMsg( 13005 );
		return false;
	}
	return true;
}
bool cQuickEvol::CheckingEvolAboutJogress(int nIndex, bool bAutoEvo)
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pDUser, false );

	CDigimonEvolveInfo* pFTEvolInfo = pDUser->GetFTEvol(); 
	SAFE_POINTER_RETVAL( pFTEvolInfo, false );
	CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( EvolveIcons[ nIndex ].s_pEvolObjInfo->nSlot );
	SAFE_POINTER_RETVAL( pFTEvolObj, false );

	// 진화나 퇴화시 조그레스에 대한 조건.
	if( pFTEvolObj->m_nChipsetType != 0 && ( pFTEvolObj->m_nOpenQuest != 0 || pFTEvolObj->m_nEvolutionTree == CDigimonUser::TREE_JOGRESS ) )  // 진화트리가 조그레스 인지 조건 추가(2)
	{		
		// 조그레스 상대방 용병중 슬롯이 뚫려있고 가장 낮은 레벨을 불러온다
		cData_PostLoad::sDATA*	pMercenary = NULL;
		int		pGetError = 0;	// 0값부터 시작
		int		pSlotIndex = 0;
		cData_Tactics* pTactics = g_pDataMng->GetTactics();
		SAFE_POINTER_RETVAL( pTactics, false );
		pMercenary = pTactics->GetJoGressMercenaries(
			pFTEvolObj->m_dwID,						// 진화 하려는 조그레스 ID
			pFTEvolObj->m_nJoGressesNum,			//조그레스 합체에 필요한 용병수
			pFTEvolObj->m_nJoGress_Tacticses,		//조그레스 합체에 필요한 용병 Base Digimon ID
			pSlotIndex, 
			pGetError);

		//assert_cs(NULL != pMercenary);	

		if(NULL == pMercenary)
		{
			switch(pGetError)
			{
			case 0:	// 조건 1 합체하기위한 상대 디지몬이 없다.
				{
					cPrintMsg::PrintMsg( 30509 );	// 조그레스 진화 상대 디지몬을 소유하고 있지 않습니다
					return false;
				}
				break;
			case 1:	// 조건 2 용병들중 슬롯이 뚫린것이 없다.
				{
					cPrintMsg::PrintMsg( 30510 );	// 조그레스 진화 상대 디지몬의 슬롯이 열려있지 않습니다
					return false;
				}
				break;
			}

			return false;
		}		

		// 조건3 디지바이스 장착 칩셋 확인
		if( !CheckDigiviceChipset( pFTEvolObj ) )
		{
			cPrintMsg::PrintMsg( 30508 );	// 진화하기 위해 칩셋이 필요합니다
			return false;
		}

		// 조그레스 진화시 파트너(리더)와 상대 디지몬 레벨 비교 상대가 낮으면 체인지 파트너
		if(pDUser->GetBaseStat()->GetLevel() > pMercenary->s_nLevel )
		{

			// pMercenary->s_szName 으로 진화해야합니다. 

			cPrintMsg::PrintMsg( 30516, pMercenary->s_szName );
			return false;			
		}

		//////////////////////////////////////////////////////

		// 조그레스 진화 시도. 소모 칩셋을 파괴하기위해 여기에 설정
		pDUser->SetJointProgressFlag(pFTEvolObj->m_nEvoSlot);
	}
	return true;
}

bool cQuickEvol::CheckTamerForEvolution()
{
	// 테이머 상태 - 상점 오픈, 상점 준비
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTamer, false );
	cCondition* pCondition = pTamer->GetCondition();
	SAFE_POINTER_RETVAL( pCondition, false );

	if( pCondition->IsCondition( nSync::Shop ) || pCondition->IsCondition( nSync::ShopReady ) )
		return false;

	return true;
}

bool	cQuickEvol::isXAntiDigimon(DWORD BaseDigimonID)
{
#ifdef SDM_TAMER_XGUAGE_20180628
	CsDigimon* pkCsDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( BaseDigimonID );
	if(pkCsDigimon != NULL)
	{

		const CsDigimon::sINFO* pkDigimonInfo = pkCsDigimon->GetInfo();
		if(pkDigimonInfo != NULL)
		{
			if(nsCsDigimonTable::ET_X_ROOKIE <= pkDigimonInfo->s_eEvolutionType  &&
				pkDigimonInfo->s_eEvolutionType <= nsCsDigimonTable::ET_X_JOGRESS)
			return true;
		}
	}
	else
	{
		BHPRT("Cannot Find in isXAntiDigimon BASEDIGIMONID : [%d]", BaseDigimonID);
	}
#endif
	return false;
}
void cQuickEvol::SetRideToggle()
{
	if( g_pCharMng->GetDigimonUser( 0 )->IsRide() )
		SetRide( false );
	else
		SetRide( true );
}

void cQuickEvol::SetRide( bool bRide )
{
	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_DUNGEON )
	{
		// 던전안이면..
		return;
	}

	if( g_pDataMng->GetServerSync()->IsChageDigimon() == true )
	{
		// 경고문 표시
		return;
	}

	CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();

	bool bCurRide = pDigimon->IsRide();
	if( bCurRide == bRide )
	{		
		return;
	}

	// 죽은 상태인가
	if( pDigimon->IsServerDie() == true )
	{
		return;
	}

	// 쿨타임 체크
	if( bRide == true )
	{
		if( m_RideCoolTime.IsCoolTime() == true )
		{
			cPrintMsg::PrintMsg( 13006 );
			return;
		}
	}
	
	// 예외 처리 => 태워야 할때
	if( bRide == true )
	{
		// 스킬을 사용패킷을 보낸 상태인지 체크
		if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::USE_DIGIMON_SKILL ) == false )
		{
			cPrintMsg::PrintMsg( 30017 );
			return;
		}

		// 스킬이나 평타 시전중
		if( pDigimon->GetProp_Animation()->IsEnableRide() == false )
		{
			cPrintMsg::PrintMsg( 30017 );
			return;
		}
		// 전투중
		if( pDigimon->IsBattle() == true )
		{
			cPrintMsg::PrintMsg( 30017 );
			return;
		}
		// 테이머 못움직이는 상태 - ( 특정 행동중 )
		if( pTamer->IsPause( CsC_AvObject::PAUSE_PATH ) == true )
		{
			cPrintMsg::PrintMsg( 30017 );
			return;
		}
		// 디지몬과의 거리체크
		NiPoint2 DigiPos = pDigimon->GetPos2D();
		NiPoint2 TamerPos = pTamer->GetPos2D();

		if( ( TamerPos - DigiPos ).Length() > 400.0f )
		{
			cPrintMsg::PrintMsg( 30042 );
			return;
		}

		// 라이딩 가능지역인가
		int dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
		if( nsCsMapTable::g_pMapRegionMng->IsLimitEvoGroup( dwMapID ) == true )
		{
			CsMap_LimitEvo::LIST* pList = nsCsMapTable::g_pMapRegionMng->GetLimitEvoGroup( dwMapID )->GetList();
			CsMap_LimitEvo::LIST_IT it = pList->begin();
			CsMap_LimitEvo::LIST_IT itEnd = pList->end();
			CsMap_LimitEvo::sINFO* pLimitEvoInfo = NULL;
			for( ; it!=itEnd; ++it )
			{
				pLimitEvoInfo = (*it)->GetInfo();
				NiPoint2 vLE( (float)pLimitEvoInfo->s_nCenterX, (float)pLimitEvoInfo->s_nCenterY );
				//캡슐 진화 디지몬은 설정상 다크타워 근처에서도 진화가 유지된다. 20170208
				if( ( g_pCharMng->GetDigimonUser( 0 )->GetPos2D() - vLE ).Length() < pLimitEvoInfo->s_nRadius &&
					pDigimon->GetFTDigimon()->GetInfo()->s_eEvolutionType != nsCsDigimonTable::ET_CAPSULE)
				{
					cPrintMsg::PrintMsg( 30016 );
					return;
				}
			}
		}
	}
	// 예외처리 => 내려야 할때
	else
	{

	}

	SAFE_POINTER_RET(g_pDataMng);
	
	cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
	SAFE_POINTER_RET(pServerSync);

	if( pServerSync->IsEmptyRefCount(cData_ServerSync::RIDE_ON) && bRide == true)	// 한번만 라이딩 해제기능 보내도록 검사.
	{
		m_RideCoolTime.Start();
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::RIDE_ON, 0 );
	}
	else if( pServerSync->IsEmptyRefCount(cData_ServerSync::RIDE_OFF) && bRide == false)	// 한번만 라이딩 해제기능 보내도록 검사.
	{
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::RIDE_OFF, 0 );
	}		
}

bool cQuickEvol::CheckDigiviceChipset(CDigimonEvolveObj* pFTEvolObj)
{
	SAFE_POINTER_RETVAL( pFTEvolObj, false );
	cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
	SAFE_POINTER_RETVAL( pDigivice, false );
	//if(NULL == g_pDataMng->GetDigivice()->GetChipset_TypeL(pFTEvolObj->m_nChipsetType))
	if( !pDigivice->DoYouHaveJointProgressChipset() )
		return false;

	// 칩셋 소분류 타입 확인
	// 1. 기간형 칩셋 존재
	int nRequireIdx = pDigivice->GetChipsetIndex_TypeLT( pFTEvolObj->m_nChipsetType, pFTEvolObj->m_nChipsetTypeP );
	if( cData_Digivice::INVALIDE_DIGIVICE_INDEX != nRequireIdx )
		return true;

	// 2. 소모형 칩셋 존재
	nRequireIdx = pDigivice->GetChipsetIndex_TypeLT( pFTEvolObj->m_nChipsetType, pFTEvolObj->m_nChipsetTypeC );
	if( cData_Digivice::INVALIDE_DIGIVICE_INDEX == nRequireIdx )
		return false;

	cItemInfo* pChipset = pDigivice->GetChipset( nRequireIdx );
	SAFE_POINTER_RETVAL( pChipset, false );
	if( !pChipset->IsEnable() )
		return false;

	// 필요 개수
	if( pChipset->GetCount() < pFTEvolObj->m_nChipsetNum )
		return false;

	return true;
}

void cQuickEvol::Update(float const& fDeltaTime)
{
	if( g_pResist->m_CenterBar.s_bIsOpenQuick )//추가 퀵슬롯 오픈상태일 때
	{
		m_pRoot->SetPos( m_ptRootPos - CsPoint( 0 , 44 ) );
		CsSIZE size( 21, 21 );
		for( int i=0; i<nLimit::EvoStep; ++i )
			EvolveIcons[ i ].s_IFIcon.SetPos( CsRect( CsPoint( ( 29*i ) + 4, 4 - 44 ), size ) );
	}
	else	//퀵슬롯 닫힌 상태
	{
		m_pRoot->SetPos( m_ptRootPos );
		CsSIZE size( 21, 21 );
		for( int i=0; i<nLimit::EvoStep; ++i )
			EvolveIcons[ i ].s_IFIcon.SetPos( CsRect( CsPoint( ( 29*i ) + 4, 4 ), size ) );
	}
}

bool cQuickEvol::isSettedXAISystem(void)
{
#ifdef SDM_TAMER_XGUAGE_20180628
	bool bisSettedXAISystem= g_pDataMng->GetTEquip()->IsExistItem(nTamer::eParts::XAI);
	BHPRT("PROCESSING EVOLUTION");

	if(bisSettedXAISystem == true)
	{
		BHPRT("PROCESSING EVOLUTION : isSettedXAISystem SUCCESS %d", (int)bisSettedXAISystem);
	}
	else
	{
		cPrintMsg::PrintMsg(32001);
	}
	return bisSettedXAISystem;
#endif
	return false;
}
bool cQuickEvol::hasEnoughXGage(int nIndex)
{
#ifdef SDM_TAMER_XGUAGE_20180628
	bool bhasEnoughXGage = true;
	BHPRT("PROCESSING EVOLUTION : hasEnoughXGage %d", (int)bhasEnoughXGage);
	CTamerUser* pkTamerUser = g_pCharMng->GetTamerUser();
	if(pkTamerUser != NULL)
	{
		CDigimonUser* pDigimonUser = (CDigimonUser*)m_pTargetObject;
		CDigimonEvolution::sBATTLE_EVO_INFO* pFTBE = pDigimonUser->GetFTBattleEvo();
		int nEvoSlot = nsCsFileTable::g_pDigimonMng->GetDigimon( EvolveIcons[ nIndex ].s_pEvolObjInfo->dwDigimonID )->GetInfo()->s_eEvolutionType;

		int NeedXGuage = FMDigimon::GetCostEvlove( m_pTargetObject->GetFTID(), m_pTargetObject->GetBaseStat()->GetLevel(), EvolveIcons[ nIndex ].s_pEvolObjInfo->dwDigimonID );

		CTamerUser* pUser = g_pCharMng->GetTamerUser();
		CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
		unsigned int CurrXGuage = pUserState->GetCurrentXGuage();
		if(CurrXGuage < NeedXGuage)
		{
			cPrintMsg::PrintMsg( 32002 );
			return false;
		}
	}
	return bhasEnoughXGage;
#endif
	return false;
}
bool cQuickEvol::isXEvolutionOfJogress(int nIndex)
{
#ifdef SDM_TAMER_XGUAGE_20180628
	SAFE_POINTER_RETVAL(EvolveIcons[ nIndex ].s_pEvolObjInfo, false);

	CsDigimon* pkCsDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( EvolveIcons[ nIndex ].s_pEvolObjInfo->dwDigimonID );
	SAFE_POINTER_RETVAL(pkCsDigimon, false);
	const CsDigimon::sINFO* pkDigimonInfo = pkCsDigimon->GetInfo();
	if(pkDigimonInfo != NULL)
	{
		if(pkDigimonInfo->s_eEvolutionType == nsCsDigimonTable::ET_X_JOGRESS)
			return true;
	}
#endif
	return false;
}
bool cQuickEvol::CheckingXEvolutionOfJogress(int nIndex)
{
	bool bCheckingXEvolutionOfJogress = true;
	BHPRT("PROCESSING EVOLUTION : CheckingXEvolutionOfJogress %d", (int)bCheckingXEvolutionOfJogress);
#ifdef SDM_TAMER_XGUAGE_20180628

/////////////////////////////////////////////////////////////////////////////////
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL(pDUser, false);
	CDigimonEvolveObj* pFTEvolObj = pDUser->GetFTEvol()->GetEvolveObjByEvoSlot( EvolveIcons[ nIndex ].s_pEvolObjInfo->nSlot );
	SAFE_POINTER_RETVAL(pFTEvolObj, false);

	{		
		// 조그레스 상대방 용병중 조그레스 및 X 조그레스 슬롯이 뚫려있고 가장 낮은 레벨을 불러온다
		cData_PostLoad::sDATA*	pMercenary = NULL;
		int		pGetError = 0;	// 0값부터 시작
		int		pSlotIndex = 0;
		pMercenary = g_pDataMng->GetTactics()->GetXJogressMercenaries(
			pFTEvolObj->m_dwID,
			pFTEvolObj->m_nJoGressesNum,			//조그레스 합체에 필요한 용병수
			pFTEvolObj->m_nJoGress_Tacticses,		//조그레스 합체에 필요한 용병 Base Digimon ID
			pSlotIndex, 
			pGetError);

		if(pMercenary != NULL)
		{
			// 조건3 칩셋이 착용 되있나?
			if(g_pDataMng->GetDigivice()->DoYouHaveJointProgressChipset() == NULL)
			{
				cPrintMsg::PrintMsg( 30508 );	// 진화하기위해 칩셋이 필요합니다
				return false;
			}		
			// 조그레스 진화시 파트너(리더)와 상대 디지몬 레벨 비교 상대가 낮으면 체인지 파트너
			if(pDUser->GetBaseStat()->GetLevel() > pMercenary->s_nLevel )
			{
				cPrintMsg::PrintMsg( 30516, pMercenary->s_szName );
				return false;			
			}
			//////////////////////////////////////////////////////
			// 조그레스 진화 시도. 소모 칩셋을 파괴하기위해 여기에 설정
			pDUser->SetJointProgressFlag(pFTEvolObj->m_nEvoSlot);
			return true;
		}
	}
	return false;
#endif
	return false;
}
bool cQuickEvol::CheckCommonCondition(int iEvolutionIdx)
{
	assert_csm( ( net::game == NULL ) || ( EvolveIcons[ iEvolutionIdx ].s_pEvolObjInfo->nSlot != 0 ), _T( "진화관련오류") );

	SAFE_POINTER_RETVAL(m_pTargetObject, false);

	if( m_pTargetObject->IsServerDie() == true )
		return false;

	if( CheckTamerForEvolution() == false )
		return false;

	if(CheckPacketSync() == false)
		return false;

	if(isCoolTimeNow() == true)
		return false;

	if( nLimit::EvoStep <= iEvolutionIdx )
		return false;

	if( EvolveIcons[ iEvolutionIdx ].s_pEvolObjInfo == NULL )
		return false;

	return true;
}

bool cQuickEvol::isCoolTimeNow(void)
{
	// 쿨타임 중인가	
	if( m_EvolCoolTime.IsCoolTime() )
	{
		// 쿨타임중
		cPrintMsg::PrintMsg( 13002 );
		return true;
	}
	return false;
}
