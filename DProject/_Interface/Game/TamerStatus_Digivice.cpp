
#include "stdafx.h"
#include "TamerStatus.h"

#define IF_TAMER_DIGI_ICON_SIZE	CsPoint( 32, 32 )

void cTamerStatus::_DeleteDigivice()
{
	SAFE_NIDELETE( m_pDigiviceBG );


	if( g_pCharResMng )
	{
		for( int i=0; i<nLimit::Memory; ++i )
		{
			g_pCharResMng->CharImageResDelete( &m_pTacticsImage[ i ] );
		}
	}	
	SAFE_NIDELETE( m_pRenderDigiviceTex );	

	SAFE_NIDELETE( m_pMemoryIconBG );
	SAFE_NIDELETE( m_pMemoryIcon_Lock );
	SAFE_NIDELETE( m_pTSkillIconBG );
	SAFE_NIDELETE( m_pChipsetIconBG );

	for( int i = 0 ; i < 2; i++ )
	{
		SAFE_NIDELETE( m_pDigiviceText[ i ] );
	}
	DeleteDigimonDel();	
}

void cTamerStatus::DeleteDigimonDel()
{
	SAFE_NIDELETE( m_pCheckDeleteDigimon );
	SAFE_NIDELETE( m_pEditResistNumber );
	SAFE_NIDELETE( m_pDeleteDigimonCloseBtn );
	SAFE_NIDELETE( m_pDeleteDigimonOkBtn );
	m_pDelString.Delete();

	g_IME.SetNumberOnly( false );
}

void cTamerStatus::_CreateDigivice()
{
	CsPoint start( 18, 43 );

	for( int i=0; i<nLimit::Memory; ++i )
	{
		m_pTacticsImage[ i ] = NULL;
	}

	// 용병
	assert_cs( nLimit::Memory == 6 );		// 이값이 아니라면 아래 들어갈 위치 변경	
	for( int y=0; y<3; ++y )
	{
		for( int x=0; x<2; ++x )
		{
#ifdef UI_POSMOVE_RENEWAL_20190109
			m_IFIcon_vice_Memory[ y*2+x ].Init( CsRect( start + CsPoint( 11, 12 ), (CsSIZE)IF_TAMER_DIGI_ICON_SIZE ) );
			start.x += 165;
#else
			m_IFIcon_vice_Memory[ y*2+x ].Init( CsRect( start, (CsSIZE)IF_TAMER_DIGI_ICON_SIZE ) );
			start.x += 188;
#endif
		}
		start.x = 18;
#ifdef UI_POSMOVE_RENEWAL_20190109
		start.y += 60;
#else
		start.y += 49;
#endif
	}

	// 테이머 스킬
	start = CsPoint( 19, 205 );
	for( int x=0; x<nLimit::SkillSlot; ++x )
	{
#ifdef UI_POSMOVE_RENEWAL_20190109
		m_IFIcon_vice_TSkill[ x ].Init( CsRect( start + CsPoint( 10, 10 ), (CsSIZE)IF_TAMER_DIGI_ICON_SIZE ) );
		start.x += 59;
#else
		m_IFIcon_vice_TSkill[ x ].Init( CsRect( start, (CsSIZE)IF_TAMER_DIGI_ICON_SIZE ) );
		start.x += 46;
#endif
	}

	// 칩셋
#ifdef UI_POSMOVE_RENEWAL_20190109
	start = CsPoint( 21, 338 );
#else
	start = CsPoint( 42, 285 );
#endif
	for( int y=0; y<IF_TAMER_DIGI_ROW; ++y )
	{		
		for( int x=0; x<IF_TAMER_DIGI_COL; ++x )
		{
			m_IFIcon_vice_Chip[ y*IF_TAMER_DIGI_COL + x ].Init( CsRect( start, (CsSIZE)IF_TAMER_DIGI_ICON_SIZE ) );
#ifdef UI_POSMOVE_RENEWAL_20190109
			start.x += 59;
#else
			start.x += 47;
#endif
		}
#ifdef UI_POSMOVE_RENEWAL_20190109
		start.x += 19;
#else
		start.x = 42;
#endif
		start.y += 45;
	}
}

void cTamerStatus::_InitDigivice()
{
	_InitText_Digivice();

	m_pDigiviceBG = NiNew cSprite;
	float fPosY = 424;
#ifdef SDM_TAMER_XGUAGE_20180628
		fPosY += 22;
#endif
	m_pDigiviceBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( 255, fPosY ), "TamerStatus\\Vice_Win.tga", false );

	m_pMemoryIconBG = NiNew cSprite;
	m_pMemoryIconBG->Init( NULL, CsPoint::ZERO, CsPoint( 46, 43 ), "TamerStatus\\partner.tga", false );

	m_pMemoryIcon_Lock = NiNew cSprite;
	m_pMemoryIcon_Lock->Init( NULL, CsPoint::ZERO, CsPoint( 46, 43 ), "TamerStatus\\partner_Lock.tga", false );

	m_pTSkillIconBG = NiNew cSprite;
#ifdef UI_POSMOVE_RENEWAL_20190109
	m_pTSkillIconBG->Init( NULL, CsPoint::ZERO, CsPoint( 46, 43 ), "TamerStatus\\card.tga", false );
#else
	m_pTSkillIconBG->Init( NULL, CsPoint::ZERO, CsPoint( 40, 40 ), "TamerStatus\\card.tga", false );
#endif

	m_pChipsetIconBG = NiNew cSprite;
	m_pChipsetIconBG->Init( NULL, CsPoint::ZERO, CsPoint( 46, 43 ), "TamerStatus\\chipset.tga", false );

	m_pRenderDigiviceTex = NiNew cRenderTex;
	if( m_pRenderDigiviceTex->Init( CsPoint( 118, 138 ) ) == false )
		SAFE_NIDELETE( m_pRenderDigiviceTex );

	cData_PostLoad::sDATA* pTactics;
	for( int i=0; i<nLimit::Memory; ++i )
	{
		pTactics = g_pDataMng->GetTactics()->GetTactics( i );
		assert_cs( pTactics != NULL );
		if( pTactics->s_Type.GetType() == 0 )
		{
			m_pTacticsImage[ i ] = NULL;
			m_nTacticsDigimonID[ i ] = 0;
			continue;
		}
		m_nTacticsDigimonID[ i ] = pTactics->s_dwBaseDigimonID;
		g_pCharResMng->CharImageResDelete( &m_pTacticsImage[ i ] );

		CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pTactics->s_dwBaseDigimonID )->GetInfo();
		m_pTacticsImage[ i ] = g_pCharResMng->CharImageResLoad( pFTDigimon->s_dwModelID );
	}	
}

void cTamerStatus::_InitText_Digivice()
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_DIGIVICE_TAMERSKILL" ).c_str() );
	m_pDigiviceText[ 0 ] = NiNew cText;
#ifdef UI_POSMOVE_RENEWAL_20190109
	m_pDigiviceText[ 0 ]->Init( m_pRoot, CsPoint( 42, 172 ), &ti, false );
#else
	m_pDigiviceText[ 0 ]->Init( m_pRoot, CsPoint( 52, 180 ), &ti, false );
#endif


	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_CHIPSET_TEXT" ).c_str() );
	m_pDigiviceText[ 1 ] = NiNew cText;
#ifdef UI_POSMOVE_RENEWAL_20190109
	m_pDigiviceText[ 1 ]->Init( m_pRoot, CsPoint( 25, 306 ), &ti, false );
#else
	m_pDigiviceText[ 1 ]->Init( m_pRoot, CsPoint( 52, 261 ), &ti, false );
#endif
}

bool cTamerStatus::_UpdateIcon_ForMouse_Digivice()
{
	bool bCheck = false;
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	int nMemory = g_pDataMng->GetTactics()->_GetOpenSlot();
	int nTSkill = g_pDataMng->GetDigivice()->GetTamerSkillCount();
	int nChipset = g_pDataMng->GetDigivice()->GetChipsetCount();
#ifdef SIMPLE_TOOLTIP
	CsRect rcTemp = CsRect( CsPoint( 10, 190 ), CsSIZE( 235, 60 ) );
	if(rcTemp.PtInRect( ptMouseLocalPos ) == TRUE)
	{
		
		CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1041 );
	}	
#endif
	switch( eCursor )
	{	
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int i=0; i<nMemory; ++i )
				m_IFIcon_vice_Memory[ i ].MouseLBtnClick( ptMouseLocalPos );

			for( int i=0; i<nTSkill; ++i )
				m_IFIcon_vice_TSkill[ i ].MouseLBtnClick( ptMouseLocalPos );

			for( int i=0; i<nChipset; ++i )
				m_IFIcon_vice_Chip[ i ].MouseLBtnClick( ptMouseLocalPos );
		}
		break;	
	case CURSOR::LBUTTON_PRESS:
		{
			_OnMouseOutWindow_Digivice();

			for( int i=0; i<nMemory; ++i )
			{
				if( m_IFIcon_vice_Memory[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					m_IFIcon_vice_Memory[ i ].MouseOn( ptMouseLocalPos );
				}
			}

			for( int i=0; i<nTSkill; ++i )
			{
				if( m_IFIcon_vice_TSkill[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					m_IFIcon_vice_TSkill[ i ].MouseOn( ptMouseLocalPos );
				}
			}

			for( int i=0; i<nChipset; ++i )
			{
				if( m_IFIcon_vice_Chip[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					m_IFIcon_vice_Chip[ i ].MouseOn( ptMouseLocalPos );
				}
			}
		}
		break;
	case CURSOR::LBUTTON_UP:
		{
			for( int i=0; i<nMemory; ++i )
				m_IFIcon_vice_Memory[ i ].ReleaseState();

			for( int i=0; i<nTSkill; ++i )
				m_IFIcon_vice_TSkill[ i ].ReleaseState();

			for( int i=0; i<nChipset; ++i )
				m_IFIcon_vice_Chip[ i ].ReleaseState();
		}
		break;
	case CURSOR::RBUTTON_DOWN:
		{
			for( int i=0; i<nMemory; ++i )
				m_IFIcon_vice_Memory[ i ].MouseRBtnClick( ptMouseLocalPos );
			for( int i=0; i<nTSkill; ++i )
				m_IFIcon_vice_TSkill[ i ].MouseRBtnClick( ptMouseLocalPos );
			for( int i=0; i<nChipset; ++i )
				m_IFIcon_vice_Chip[ i ].MouseRBtnClick( ptMouseLocalPos );
		}		
		break;
	case CURSOR::RBUTTON_PRESS:
		{
			for( int i=0; i<nMemory; ++i )
			{
				if( m_IFIcon_vice_Memory[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
					m_IFIcon_vice_Memory[ i ].MouseOn( ptMouseLocalPos );
			}

			for( int i=0; i<nTSkill; ++i )
			{
				if( m_IFIcon_vice_TSkill[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
					m_IFIcon_vice_TSkill[ i ].MouseOn( ptMouseLocalPos );
			}
			for( int i=0; i<nChipset; ++i )
			{
				if( m_IFIcon_vice_Chip[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
					m_IFIcon_vice_Chip[ i ].MouseOn( ptMouseLocalPos );
			}
		}		
		break;
	case CURSOR::RBUTTON_UP:
		{
			for( int i=0; i<nMemory; ++i )
				m_IFIcon_vice_Memory[ i ].ReleaseState();

			for( int i=0; i<nTSkill; ++i )
				m_IFIcon_vice_TSkill[ i ].ReleaseState();

			for( int i=0; i<nChipset; ++i )
				m_IFIcon_vice_Chip[ i ].ReleaseState();
		}
		break;	
	}

	// 툴팁용
	{
		for( int i=0; i<nMemory; ++i )
		{
			if( m_IFIcon_vice_Memory[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				cData_PostLoad::sDATA* pData = g_pDataMng->GetTactics()->GetTactics( i );
				if( pData->s_Type.GetType() == 0 )
				{
#ifdef SIMPLE_TOOLTIP				
					CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1042 );
#endif
					continue;
				}

				CsPoint pos = m_IFIcon_vice_Memory[ i ].GetPos() + GetRootClient();
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 140, cTooltip::TACTICS, i, pData->s_Type.GetType() );
			}			
		}
#ifdef SIMPLE_TOOLTIP
		for( int i=0; i<nChipset; ++i )
		{
			if( m_IFIcon_vice_Chip[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				cItemInfo* pData = g_pDataMng->GetDigivice()->GetChipset( i );
				if( pData->IsEnable() == false )
				{				

					CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1043 );
					continue;
				}
				CsPoint pos = m_IFIcon_vice_Chip[ i ].GetPos() + GetRootClient();
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_DIGI_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->GetType(), cBaseWindow::WT_TAMERSTATUS, 0, 0, pData );
				break;
			}
		}
#else
		for( int i=0; i<nChipset; ++i )
		{
			cItemInfo* pData = g_pDataMng->GetDigivice()->GetChipset( i );

			if( pData->IsEnable() == false )
				continue;

			if( m_IFIcon_vice_Chip[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				CsPoint pos = m_IFIcon_vice_Chip[ i ].GetPos() + GetRootClient();
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_DIGI_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->GetType(), cBaseWindow::WT_TAMERSTATUS, 0, 0, pData );
				break;
			}
		}
#endif
	}

	return bCheck;
}

void cTamerStatus::_RenderDigivice()
{
	m_pDigiviceBG->Render();

	if( m_pRenderDigiviceTex != NULL )
	{
		if( g_pDataMng->GetTEquip()->GetDigiviceItem()->IsEnable() )
		{
			CsC_AvObject* pDigivice = g_pNpcMng->GetDigivice();

			m_pRenderDigiviceTex->SetHeightConstant( 1.8f );
			m_pRenderDigiviceTex->SetDistConstant( 0.3f );
			m_pRenderDigiviceTex->BeginRender();
			pDigivice->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 1.0f );
			pDigivice->GetProp_Alpha()->SetUseDistAlpha( false );
			pDigivice->CsC_AvObject::Update( g_fDeltaTime );
			m_pRenderDigiviceTex->RenderObject( GET_SUBCAMERA(CAMERA_07), pDigivice );	//2015-11-30-nova
			m_pRenderDigiviceTex->EndRender( GET_SUBCAMERA(CAMERA_07), CsPoint( 69, 37 ) + GetRootClient() );
		}		
	}

	cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	int nMemory = pTactics->GetTacticsCount();
	int nTSkill = pDigivice->GetTamerSkillCount();
	int nChipset = pDigivice->GetChipsetCount();

	cItemInfo* pItem;
	CsPoint pos;
	cData_PostLoad::sDATA* pTacticsData;

	for( int i = 0 ; i<nMemory ; ++i )
	{
		// 위치
		pos = m_IFIcon_vice_Memory[ i ].GetPos() + GetRootClient();
#ifdef UI_POSMOVE_RENEWAL_20190109
		CsPoint interval = CsPoint( -7, -4 );
#endif

		if( i >= g_pDataMng->GetTactics()->_GetOpenSlot() )
		{
#ifdef UI_POSMOVE_RENEWAL_20190109
			m_pMemoryIcon_Lock->Render( pos + interval );
#else
			m_pMemoryIcon_Lock->Render( pos + CsPoint( -7, -6 ) );
#endif
			continue;
		}
		else
#ifdef UI_POSMOVE_RENEWAL_20190109
			m_pMemoryIconBG->Render( pos + interval );
#else
			m_pMemoryIconBG->Render( pos + CsPoint( -7, -6 ) );
#endif

		pTacticsData = pTactics->GetTactics( i );

		if( pTacticsData->s_Type.GetType() == 0 )
		{
			m_nTacticsDigimonID[ i ] = 0;
			g_pCharResMng->CharImageResDelete( &m_pTacticsImage[ i ] );
			continue;
		}
		else if( m_pTacticsImage[ i ] == NULL )
		{
			m_nTacticsDigimonID[ i ] = pTacticsData->s_dwBaseDigimonID;

			CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pTacticsData->s_dwBaseDigimonID )->GetInfo();
			m_pTacticsImage[ i ] = g_pCharResMng->CharImageResLoad( pFTDigimon->s_dwModelID );
		}
		else if( m_nTacticsDigimonID[ i ] != pTacticsData->s_dwBaseDigimonID )
		{
			m_nTacticsDigimonID[ i ] = pTacticsData->s_dwBaseDigimonID;

			g_pCharResMng->CharImageResDelete( &m_pTacticsImage[ i ] );

			CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pTacticsData->s_dwBaseDigimonID )->GetInfo();
			m_pTacticsImage[ i ] = g_pCharResMng->CharImageResLoad( pFTDigimon->s_dwModelID );
		}
		
		if( m_pTacticsImage[ i ] )
		{
			m_pTacticsImage[ i ]->Render_S( pos, IF_TAMER_DIGI_ICON_SIZE );
		}

		if( g_pDataMng->IsItemLock( TO_MEMORY_SID( i ) ) == true )
		{
			g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_TAMER_DIGI_ICON_SIZE );
		}

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_vice_Memory[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_vice_Memory[ i ].RenderMask( pos, IF_TAMER_DIGI_ICON_SIZE );
		}
	}

	cItemInfo* pItemArray = pDigivice->GetTamerSkillArray();
	for( int i=0; i<nTSkill; ++i )
	{
		pItem = &pItemArray[ i ];

		// 위치
		pos = m_IFIcon_vice_TSkill[ i ].GetPos() + GetRootClient();
#ifdef UI_POSMOVE_RENEWAL_20190109
		m_pTSkillIconBG->Render( pos + CsPoint( -7, -5 ) );
#else
		m_pTSkillIconBG->Render( pos + CsPoint( -4, -4 ) );
#endif

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_vice_TSkill[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_vice_TSkill[ i ].RenderMask( pos, IF_TAMER_DIGI_ICON_SIZE );
		}
	}

	pItemArray = pDigivice->GetChipsetArray();
	for( int i=0; i<nChipset; ++i )
	{
		pItem = &pItemArray[ i ];

		// 위치
		pos = m_IFIcon_vice_Chip[ i ].GetPos() + GetRootClient();
#ifdef UI_POSMOVE_RENEWAL_20190109
		m_pChipsetIconBG->Render( pos + CsPoint( -7, -5 ) );
#else
		m_pChipsetIconBG->Render( pos + CsPoint( -7, -6 ) );
#endif
		if( pItem->IsEnable() == true )
		{
			// 아이템 랜더링
			g_pIconMng->RenderItem( pItem, pos, false );
	
			if( g_pDataMng->IsItemLock( TO_CHIPSET_SID( i ) ) == true )
			{
				g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_TAMER_DIGI_ICON_SIZE );
			}
		}		

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_vice_Chip[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_vice_Chip[ i ].RenderMask( pos, IF_TAMER_DIGI_ICON_SIZE );
		}
	}

	for( int i = 0 ; i < 2; i++ )
	{
		m_pDigiviceText[ i ]->Render();
	}	
}

void cTamerStatus::_UpdateDigiviceMemory(CsPoint pos, sIFIcon::eSTATE	check, BOOL MouseOn)
{
	CsPoint ptLocal = MousePosToWindowPos( pos );

	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RET(pTactics);
	int nMemory = pTactics->GetTacticsCount();
	for( int i=0; i<nMemory; ++i )
	{
		if( m_IFIcon_vice_Memory[ i ].s_eState == check	&&
			m_IFIcon_vice_Memory[ i ].s_Rect.PtInRect( ptLocal ) == MouseOn )
		{
			if( pTactics->GetTactics( i )->s_Type.GetType() != 0 )
			{
				if( g_pDataMng->IsItemLock( TO_MEMORY_SID( i ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					return;
				}

				CsPoint mypos = m_IFIcon_vice_Memory[ i ].GetPos() + GetRootClient();
				g_pGameIF->GetPopup()->SetPopup( mypos, IF_TAMER_DIGI_ICON_SIZE + CsPoint( 120, 0 ), cPopUpWindow::DIGIVICE_TACTICS, i );
			}
			//if(MouseOn==TRUE)	m_IFIcon_vice_Memory[ i ].ReleaseState();
			break;
		}		
	}
}
void cTamerStatus::_UpdateDigiviceChipset(CsPoint pos, sIFIcon::eSTATE	check, BOOL MouseOn)
{
	CsPoint ptMouseLocalPos = MousePosToWindowPos( pos );

	cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
	SAFE_POINTER_RET(pDigivice);
	int nChipset = pDigivice->GetChipsetCount();
	for( int i=0; i<nChipset; ++i )
	{
		if( pDigivice->GetChipset( i )->IsEnable() == false )
			continue;

		if( m_IFIcon_vice_Chip[ i ].s_eState == check )
		{
			if( m_IFIcon_vice_Chip[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == MouseOn )
			{
				if( g_pDataMng->IsItemLock( TO_CHIPSET_SID( i ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					return;
				}

				// 동작
				if( check == sIFIcon::MOUSE_RCLICK && g_pDataMng->IsItemUse( TO_CHIPSET_SID( i ) ) == true )
					g_pDataMng->SendItemMoveInven( TO_CHIPSET_SID( i ) );
				
				else if( check == sIFIcon::MOUSE_LCLICK )	// 이경우에만 아이콘 만들어준다.
					CURSOR_ST.SetIcon( CURSOR_ICON::CI_CHIPSET, i, 1, &m_IFIcon_vice_Chip[ i ] );
			}
			//if(MouseOn==TRUE)	m_IFIcon_vice_Chip[ i ].ReleaseState();
			break;
		}
	}
}

void cTamerStatus::_OnMouseOutWindow_Digivice()
{
	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
		return;
	if( CURSOR_ST.IsIcon() == true )
		return;

	_UpdateDigiviceChipset(CURSOR_ST.GetPos(), sIFIcon::MOUSE_LCLICK, FALSE);
}

void cTamerStatus::_OnLButtonUp_Digivice( CsPoint pos )
{
	_UpdateDigiviceMemory(pos, sIFIcon::MOUSE_LCLICK, TRUE);

	_UpdateDigiviceChipset(pos, sIFIcon::MOUSE_LCLICK, TRUE);
}

void cTamerStatus::_OnRButtonUp_Digivice( CsPoint pos )
{
	_UpdateDigiviceMemory(pos, sIFIcon::MOUSE_RCLICK, TRUE);

	_UpdateDigiviceChipset(pos, sIFIcon::MOUSE_RCLICK, TRUE);
}

bool cTamerStatus::_CursorIconLBtnUp_Digivice( CURSOR_ICON::eTYPE eIconType, int nIconSlot )
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	cData_Digivice* pDigivice = g_pDataMng->GetDigivice();

	int nChipset = g_pDataMng->GetDigivice()->GetChipsetCount();

	for( int i=0; i<nChipset; ++i )
	{
		if( m_IFIcon_vice_Chip[ i ].s_Rect.PtInRect( ptLocal ) )
		{
			switch( eIconType )
			{
			case CURSOR_ICON::CI_INVEN:
				// 칩셋일 경우에만
				if( nsCsFileTable::g_pItemMng->GetItem( g_pDataMng->GetInven()->GetData( TO_ID( nIconSlot ) )->GetType() )->GetInfo()->s_nType_L == nItem::Chipset )
				{
					if( g_pDataMng->IsItemUse( nIconSlot ) == true )
					{
						g_pDataMng->SendItemMove( nIconSlot, TO_CHIPSET_SID( i ) );
					}
				}
				break;
			case CURSOR_ICON::CI_CHIPSET:
				if( g_pDataMng->IsItemUse( nIconSlot ) == true )
				{
					g_pDataMng->SendItemMove( nIconSlot, TO_CHIPSET_SID( i ) );
				}
				break;
			case CURSOR_ICON::CI_WAREHOUSE:
				cPrintMsg::PrintMsg( 11018 );
				break;
			default:
				cPrintMsg::PrintMsg( 11014 );
			}
			CURSOR_ST.ReleaseIcon();
			return true;
		}		
	}
	return false;
}

void cTamerStatus::_ResetDevice_Digivice()
{
	if( m_pDigiviceBG )
		m_pDigiviceBG->ResetDevice();

	if( m_pRenderDigiviceTex )
	{
		m_pRenderDigiviceTex->ResetDevice();
	}
	for( int i = 0 ; i < 2; i++ )
	{
		if( m_pDigiviceText[ i ] )
			m_pDigiviceText[ i ]->ResetDevice();
	}
	if( m_pCheckDeleteDigimon )
		m_pCheckDeleteDigimon->ResetDevice();

	if( m_pEditResistNumber )
		m_pEditResistNumber->ResetDevice();

	if( m_pDeleteDigimonCloseBtn )
		m_pDeleteDigimonCloseBtn->ResetDevice();

	if( m_pDeleteDigimonOkBtn )
		m_pDeleteDigimonOkBtn->ResetDevice();
	
	m_pDelString.ResetDevice();
}

void cTamerStatus::InitDeleteDigimon( int nIdx )
{
	assert_cs( m_pCheckDeleteDigimon == NULL );
	if( m_pCheckDeleteDigimon != NULL )
		return;		

	m_nDigimonIdx = nIdx;

	switch( GLOBALDATA_ST.Get2ndPassType() )
	{
	case GData::eEmail:
		{
			CsPoint pos;
			pos.x = ( g_nScreenWidth - 383 ) / 2;
			pos.y = 150;

			m_pCheckDeleteDigimon = NiNew cSprite;
			m_pCheckDeleteDigimon->Init( NULL, pos, CsPoint( 383, 159 ), "CharSelect\\DeleteChar.tga", false );	

			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.SetText( _T( "" ) );

			// PW
			m_pEditResistNumber = NiNew cEditBox;	
			ti.s_eTextAlign = DT_LEFT;
			m_pEditResistNumber->Init( m_pCheckDeleteDigimon, CsPoint( 72, 98 ), CsPoint( 280, 20 ), &ti, false );
			m_pEditResistNumber->SetFontLength( 64 );

			m_pEditResistNumber->EnableUnderline();
			m_pEditResistNumber->SetEnableSound( true );	
			m_pEditResistNumber->SetFocus();	

			m_pDeleteDigimonCloseBtn = NiNew cButton;
			m_pDeleteDigimonCloseBtn->Init( m_pCheckDeleteDigimon, CsPoint( 361, 7 ), CsPoint( 16, 16 ), "System\\Ch_Close.tga", false );
			m_pDeleteDigimonCloseBtn->SetTexToken( CsPoint( 0, 16 ) );

			m_pDeleteDigimonOkBtn = NiNew cButton;
			m_pDeleteDigimonOkBtn->Init( m_pCheckDeleteDigimon, CsPoint( 150, 125 ), CsPoint( 84, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str(), false );	
//			m_pDeleteDigimonOkBtn->Init( m_pCheckDeleteDigimon, CsPoint( 150, 125 ), CsPoint( 84, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "TAMERSTATUS_WINDOW_DIGIVICE_TAMERSKILL_TEXT" ).c_str(), false );	
			m_pDeleteDigimonOkBtn->SetEnable( false );

		}
		break;
	case GData::eAccountPass:	
	case GData::eStringPass:
	case GData::e2ndNumberPass:
		{
			CsPoint pos;
			pos.x = ( g_nScreenWidth - 283 ) / 2;
			pos.y = 150;

			m_pCheckDeleteDigimon = NiNew cSprite;
			m_pCheckDeleteDigimon->Init( NULL, pos, CsPoint( 283, 159 ), "CharSelect\\DeleteChar.tga", false );	

			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;
			ti.SetText( _T( "" ) );

			// PW
			m_pEditResistNumber = NiNew cEditBox;	
			m_pEditResistNumber->Init( m_pCheckDeleteDigimon, CsPoint( 65, 98 ), CsPoint( 150, 20 ), &ti, false );
			m_pEditResistNumber->SetFontLength( 20 );
			m_pEditResistNumber->SetEnableSound( true );	
			m_pEditResistNumber->SetFocus();	

			m_pDeleteDigimonCloseBtn = NiNew cButton;
			m_pDeleteDigimonCloseBtn->Init( m_pCheckDeleteDigimon, CsPoint( 261, 7 ), CsPoint( 16, 16 ), "System\\Ch_Close.tga", false );
			m_pDeleteDigimonCloseBtn->SetTexToken( CsPoint( 0, 16 ) );

			m_pDeleteDigimonOkBtn = NiNew cButton;
			m_pDeleteDigimonOkBtn->Init( m_pCheckDeleteDigimon, CsPoint( 100, 125 ), CsPoint( 84, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str(), false );	
			m_pDeleteDigimonOkBtn->SetEnable( false );
		}
		break;
	}

	switch( GLOBALDATA_ST.Get2ndPassType() )
	{
	case GData::eEmail:			g_IME.SetNumberOnly( false );	break;
	case GData::eStringPass:	g_IME.SetNumberOnly( false );	break;
	case GData::eAccountPass:	g_IME.SetNumberOnly( false );	m_pEditResistNumber->GetText()->SetMark( L'*' );	break;
#ifdef SDM_SECONDPASSWORD_REINFORCE_20180330
	case GData::e2ndNumberPass:	g_IME.SetNumberOnly( false );	m_pEditResistNumber->GetText()->SetMark( L'*' );	break;
#else
	case GData::e2ndNumberPass:	g_IME.SetNumberOnly( true );	m_pEditResistNumber->GetText()->SetMark( L'*' );	break;
#endif
	}

	g_pGameIF->GetChatMain()->OnEscapeKey();

	std::wstring title;
	std::wstring msg;

	if( GLOBALDATA_ST.Is2ndPassType(GData::eEmail) )
	{
		title = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_EMAIL_INPUT_TITLE" );
		msg = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_EMAIL_INPUT_MSG" );
	}
	else if( GLOBALDATA_ST.Is2ndPassType(GData::eStringPass) )
	{
		title = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_WORD_INPUT_TITLE" );
		msg = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_WORD_INPUT_MSG" );

		std::wstring deleteKey;
		DmCS::StringFn::From( deleteKey, DeleteDigimonSteamSecondPass );
		DmCS::StringFn::ReplaceAll( msg, L"#Value#", deleteKey );
	}
	else if( GLOBALDATA_ST.Is2ndPassType( GData::e2ndNumberPass ) )
	{
		title = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_2nd_PASSWORD_INPUT_TITLE" );
		msg = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_2nd_PASSWORD_INPUT_MSG" );
	} 
	else if( GLOBALDATA_ST.Is2ndPassType( GData::eAccountPass ) )
	{
		title = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_PASSWORD_INPUT_TITLE" );
		msg = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_PASSWORD_INPUT_TITLE" );
	} 
	
	{
		cText::sTEXTINFO windowTitle;
		windowTitle.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		//windowTitle.s_eTextAlign = DT_CENTER;
		windowTitle.SetText( title.c_str() );

		cString* pTitle = NiNew cString;	
		pTitle->AddText( &windowTitle );
		m_pDelString.AddHead( pTitle );
	}

	{
		cText::sTEXTINFO windowMsg;
		windowMsg.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		//windowMsg.s_eTextAlign = DT_CENTER;
		windowMsg.SetText( msg.c_str() );

		cString* pMsg = NiNew cString;
		pMsg->AddText( &windowMsg );
		m_pDelString.AddTail( pMsg );
	}
}

void cTamerStatus::_RenderDeleteDigimon()
{
	if( m_pCheckDeleteDigimon == NULL )
		return;

	m_pCheckDeleteDigimon->Render();
	m_pEditResistNumber->Render();
	m_pDeleteDigimonCloseBtn->Render();
	m_pDeleteDigimonOkBtn->Render();
	m_pDelString.Render( CsPoint( g_nScreenWidth / 2, 163 ) , 30, DT_CENTER );
}

bool cTamerStatus::_UpdateDigimonDel()
{
	if( m_pCheckDeleteDigimon == NULL )
		return false;

	char szResistNumber[ 64 ] = {0, };
	W2M( szResistNumber, m_pEditResistNumber->GetTextAll(), 64 );
	
	m_pDeleteDigimonOkBtn->SetEnable( ( strlen( szResistNumber ) >= 4 ) );

	if( GLOBALINPUT_ST.IsEscape() )	//2016-02-16-nova
	{		
		DeleteDigimonDel();
		return false;
	}

	if( m_pDeleteDigimonCloseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		DeleteDigimonDel();
		return false;
	}

	if( m_pDeleteDigimonOkBtn->IsEnable() )
	{
		if( m_pDeleteDigimonOkBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::DELETE_TACTICS, 0 ) == false )
			{
				cPrintMsg::PrintMsg( 10004 );									
			}
			else
			{
				g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::DELETE_TACTICS, 0 );
				if( net::game )
					net::game->SendDeletePartner( m_nDigimonIdx + 1, szResistNumber );			
			}

			DeleteDigimonDel();	
			return false;
		}
	}	

	return true;
}

#ifdef UI_DIGIMON_DELETE
bool cTamerStatus::OnMacroKey(const MSG& p_kMsg)
{
	SAFE_POINTER_RETVAL( m_pCheckDeleteDigimon, false );
	SAFE_POINTER_RETVAL( g_pDataMng, false );

	cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
	SAFE_POINTER_RETVAL( pServerSync, false );

	switch(p_kMsg.message)
	{
	case WM_KEYUP:
		{
			switch(p_kMsg.wParam)
			{
			case VK_RETURN:
				{
					char szResistNumber[ 64 ] = {0, };
					W2M( szResistNumber, m_pEditResistNumber->GetTextAll(), 64 );

					if( !m_pDeleteDigimonOkBtn->IsEnable() )
						return false;

					if( pServerSync->IsEmptyRefCount( cData_ServerSync::DELETE_TACTICS, 0 ) == false )
					{
						cPrintMsg::PrintMsg( 10004 );
						return false;
					}
					else
					{
						pServerSync->SendServer( cData_ServerSync::DELETE_TACTICS, 0 );
						if( net::game )
							net::game->SendDeletePartner( m_nDigimonIdx + 1, szResistNumber );			
					}

					DeleteDigimonDel();
					return true;
				}
				break;			
			}
		}
		break;
	}
	
	return false;
}
#endif