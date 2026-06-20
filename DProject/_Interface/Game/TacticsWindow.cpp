
#include "stdafx.h"
#include "TacticsWindow.h"

cTacticsWindow::cTacticsWindow()
#ifdef SDM_TAMER_XGUAGE_20180628 
:m_bisXAnti(false)
#endif
{
	m_pTargetImgae = NULL;
}

void cTacticsWindow::Destroy()
{
	cBaseWindow::Delete();	
}

void cTacticsWindow::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pTargetImgae );
	SAFE_NIDELETE( m_pBattleMask );
	SAFE_NIDELETE( m_pTactics_Lock );
#ifdef SDM_TAMER_XGUAGE_20180628 
	SAFE_NIDELETE(m_pkBG);
#endif
}

void cTacticsWindow::Create( int nValue /*=0*/ )
{
	if( cBaseWindow::Init() == false )
		return;

	assert_cs( m_pTargetImgae == NULL );
	m_nTacticsIndex = nValue;

	CsPoint pos;
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	if (nValue >= 4)
	{
		pos = CsPoint(40 + ((nValue - 4) * 36), 200);
	}
	else
	{
		pos = CsPoint(40 + (nValue * 36), 160);
	}
#else
	pos = CsPoint( 76 + ( nValue*36 ), 158 );
#endif

#ifdef SDM_TAMER_XGUAGE_20180628 
	m_pkBG = NiNew cSprite;
	m_pkBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( 36, 36 ), "TargetWindow\\TacticsTarget_X.tga", false );
#endif

	InitScript( "TargetWindow\\TacticsTarget.tga", pos, CsPoint( 36, 36 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
	m_pBattleMask = NiNew cSprite;
	m_pBattleMask->Init( m_pRoot, CsPoint::ZERO, CsPoint( 36, 36 ), "TargetWindow\\Digimon_Sub_Mask.tga", false );
	m_pTactics_Lock = NiNew cSprite;
	m_pTactics_Lock->Init( m_pRoot, CsPoint::ZERO, CsPoint( 36, 36 ), "TargetWindow\\TacticsTarget_Lock.tga", false );
	m_bClickMouse = false;
	m_bDisable	=	false;
}

void cTacticsWindow::Update(float const& fDeltaTime)
{
}

cBaseWindow::eMU_TYPE
cTacticsWindow::Update_ForMouse()
{
	m_bClickMouse = false;
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	CURSOR::eTYPE mt = CURSOR_ST.GetButtonState();
	switch( mt )
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
	case CURSOR::LBUTTON_PRESS:
	case CURSOR::RBUTTON_PRESS:
		m_bClickMouse = true;
	}
	int nOpenSlot = g_pDataMng->GetTactics()->_GetOpenSlot();// 현재 오픈된 용병 슬롯 수
	if( ( m_nTacticsIndex ) >= nOpenSlot )	//오픈된 애보다 뒤에 아이들은 무시
		return muReturn;
	cData_PostLoad::sDATA* pData = g_pDataMng->GetTactics()->GetTactics( m_nTacticsIndex );
	if( pData->s_Type.m_nType != 0 )
	{
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient(), CsPoint( 36, 36 ), 140, cTooltip::TACTICS, m_nTacticsIndex, pData->s_Type.m_nType );
	}

	return muReturn;
}

void cTacticsWindow::Render()
{
	RenderScript();
	int nOpenSlot = g_pDataMng->GetTactics()->_GetOpenSlot();// 현재 오픈된 용병 슬롯 수
	if( ( m_nTacticsIndex ) >= nOpenSlot )//오픈된 애보다 뒤에 아이들은 회색으로
	{
		m_pTactics_Lock->Render();
		return;
	}

#ifdef SDM_TAMER_XGUAGE_20180628 
	if(m_bisXAnti)
		m_pkBG->Render(GetRootClient());
#endif

	if( m_pTargetImgae != NULL )
	{
		m_pTargetImgae->Render( GetRootClient() + CsPoint( 2, 2 ) );
	}

	// 전투중 교체 아이템 ( 배틀태그 ) 존재시 마스크 랜더링 안함
	if( g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( nItem::BattleTag, 0 ) == cData_Inven::INVALIDE_INVEN_INDEX )
	{
		if( g_pGameIF->GetMainDigimonWindow()->IsBattle() == true )
			m_pBattleMask->Render();				
	}		
}

void cTacticsWindow::OnLButtonUp( CsPoint pos )
{
	if( m_bClickMouse == false )
		return;

	if( m_pTargetImgae == NULL )
		return;

	int nOpenSlot = g_pDataMng->GetTactics()->_GetOpenSlot();// 현재 오픈된 용병 슬롯 수
	if( ( m_nTacticsIndex ) >= nOpenSlot )	//오픈된 애보다 뒤에 아이들은 무시
 		return;

	g_pGameIF->GetPopup()->SetPopup( GetRootClient(), m_pRoot->GetSize() + CsPoint( 120, 0 ) , cPopUpWindow::TACTICS, m_nTacticsIndex );
}

void cTacticsWindow::OnRButtonUp( CsPoint pos )
{
	OnLButtonUp( pos );
}

void cTacticsWindow::SetTargetObject( int nDigimonFTID )
{
	m_nTargetDigimonFTID = nDigimonFTID;
	SAFE_NIDELETE( m_pTargetImgae );

	if( nDigimonFTID == 0 )
	{
#ifdef SDM_TAMER_XGUAGE_20180628
		m_bisXAnti = false;
#endif
		return;
	}
	m_bDisable	=	false;
	CsDigimon* pkFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( m_nTargetDigimonFTID );
	if(pkFTDigimon != NULL)
	{
		// 큰 이미지
		DWORD dwModelID = pkFTDigimon->GetInfo()->s_dwModelID;
		std::string strPath = g_pModelDataMng->GetLargeModelIconFile( dwModelID );	
		m_pTargetImgae = NiNew cSprite;
		CsPoint ImageSize = CsPoint( 32, 32 );
		m_pTargetImgae->Init( NULL, CsPoint::ZERO, ImageSize, strPath.c_str(), true, FONT_WHITE, false );

	}
#ifdef SDM_TAMER_XGUAGE_20180628
	m_bisXAnti = false;
	//// 진화 트리에 X안티 가 있는가?
	//CDigimonEvolveInfo* pkEvolveInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_nTargetDigimonFTID );
	//if(pkEvolveInfo != NULL)
	//{
	//	bool isXAntiTree = false;
	//	for( int i =0; i < nLimit::EvoUnit; i++ )
	//	{
	//		CDigimonEvolveObj* pFTEvolObj = pkEvolveInfo->GetEvolveObjByEvoSlot( i );
	//		if(pFTEvolObj != NULL)
	//		{
	//			if(pFTEvolObj->m_nOpenQualification == NEED_QUALITICATION::_XAI_SYSTEM)
	//			{
	//				isXAntiTree = true;
	//				break;
	//			}
	//		}
	//	}
	//	if(isXAntiTree == true)
	//		m_bisXAnti = true;
	//	else
	//		m_bisXAnti = false;

	//}
#endif
}

void cTacticsWindow::ResetDevice()
{
	ResetDeviceScript();
	if( m_pBattleMask )
		m_pBattleMask->ResetDevice();
	if( m_pTactics_Lock )
		m_pTactics_Lock->ResetDevice();
	if( m_pTargetImgae )
		m_pTargetImgae->ResetDevice();

#ifdef SDM_TAMER_XGUAGE_20180628 
	if(m_pkBG )
		m_pkBG->ResetDevice();
#endif

}

void cTacticsWindow::SetDisable()
{
	m_bDisable = true;
	m_pTargetImgae->SetColor( FONT_GLAY );
}

