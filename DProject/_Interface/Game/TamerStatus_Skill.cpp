
#include "stdafx.h"
#include "TamerStatus.h"
#define IF_TAMER_SKILL_ICON_SIZE		CsPoint(32, 32)
#define ICON_HALF		16

void cTamerStatus::_DeleteSkill()
{
	SAFE_NIDELETE( m_pSkillBG );

	for( int i = 0; i < 3; i++ )
	{
		SAFE_NIDELETE( m_pSkillText[ i ] );
	}

	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
		SAFE_NIDELETE( m_pDeleteButton[i] );
}

void cTamerStatus::_CreateSkill()
{
#ifdef UI_POSMOVE_RENEWAL_20190109
	CsPoint start = CsPoint( 23, 69 );
	const int intervalX = 59;
	const int intervalY = 52;			// 위치 맞추기
	CsPoint pos = start;

	for(int i = 0; i < IF_TAMER_BASE_SKILL_COUNT; ++i)
	{
		m_IFIcon_BaseSkill[ i ].Init( CsRect( pos, (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
		pos.x += intervalX;

		if(i % IF_TAMER_DIGI_COL == IF_TAMER_DIGI_COL - 1 )
		{
			pos.x = start.x;
			pos.y += intervalY;
		}
	}

	pos.x = start.x;
	pos.y += 31;
	for(int i = 0; i < IF_TAMER_ACTIVE_SKILL_COUNT; ++i)
	{
		m_IFIcon_ActiveSkill[ i ].Init( CsRect( pos, (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
		pos.x += intervalX;
	}

	pos.x = start.x;
	pos.y += 135;
	for(int i = 0; i < IF_TAMER_PASIVE_SKILL_COUNT; ++i)
	{
		m_IFIcon_PasiveSkill[ i ].Init( CsRect( pos, (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
		pos.x += intervalX;
	}	
#else
	m_IFIcon_BaseSkill[ 0 ].Init( CsRect( CsPoint( 42, 90 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_BaseSkill[ 1 ].Init( CsRect( CsPoint( 89, 90 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_BaseSkill[ 2 ].Init( CsRect( CsPoint( 136, 90 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_BaseSkill[ 3 ].Init( CsRect( CsPoint( 183, 90 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_BaseSkill[ 4 ].Init( CsRect( CsPoint( 42, 136 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_BaseSkill[ 5 ].Init( CsRect( CsPoint( 89, 136 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_BaseSkill[ 6 ].Init( CsRect( CsPoint( 136, 136 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_BaseSkill[ 7 ].Init( CsRect( CsPoint( 183, 136 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );

	m_IFIcon_ActiveSkill[ 0 ].Init( CsRect( CsPoint( 42, 220 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_ActiveSkill[ 1 ].Init( CsRect( CsPoint( 89,  220 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_ActiveSkill[ 2 ].Init( CsRect( CsPoint( 136, 220 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_ActiveSkill[ 3 ].Init( CsRect( CsPoint( 183, 220 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );

	m_IFIcon_PasiveSkill[ 0 ].Init( CsRect( CsPoint( 42, 345 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_PasiveSkill[ 1 ].Init( CsRect( CsPoint( 89, 345 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_PasiveSkill[ 2 ].Init( CsRect( CsPoint( 136, 345 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
	m_IFIcon_PasiveSkill[ 3 ].Init( CsRect( CsPoint( 183, 345 ), (CsSIZE)IF_TAMER_SKILL_ICON_SIZE ) );
#endif

	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
		m_pDeleteButton[i] = NULL;
}

void cTamerStatus::_InitSkill()
{
	_InitText_Skill();

	m_pSkillBG = NiNew cSprite;
	float fPosY = 424;
#ifdef SDM_TAMER_XGUAGE_20180628
	fPosY += 22;
#endif
	m_pSkillBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( 255, fPosY ), "TamerStatus\\Skill_Win.tga", false );

	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
	{
		// 위치
		CsPoint pos = m_IFIcon_ActiveSkill[i].GetPos() + CsPoint( 10, 38 );

		m_pDeleteButton[i] = NiNew cButton;
		m_pDeleteButton[i]->Init( m_pRoot, pos, CsPoint( 13, 13 ), "System\\Ch_SkillDel.tga", false );
		m_pDeleteButton[i]->SetTexToken( CsPoint( 0, 13 ) );
	}
}

void cTamerStatus::_InitText_Skill()
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;

	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_BASICSKILL_TEXT" ).c_str() );
	m_pSkillText[ 0 ] = NiNew cText;
#ifdef UI_POSMOVE_RENEWAL_20190109
	m_pSkillText[ 0 ]->Init( m_pRoot, CsPoint( 40, 37 ), &ti, false );	
#else
	m_pSkillText[ 0 ]->Init( m_pRoot, CsPoint( 50, 61 ), &ti, false );	
#endif

	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_ACTIVESKILL_TEXT" ).c_str() );
	m_pSkillText[ 1 ] = NiNew cText;
#ifdef UI_POSMOVE_RENEWAL_20190109
	m_pSkillText[ 1 ]->Init( m_pRoot, CsPoint( 43, 172 ), &ti, false );	
#else
	m_pSkillText[ 1 ]->Init( m_pRoot, CsPoint( 50, 191 ), &ti, false );	
#endif

	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_PASSIVESKILL_TEXT" ).c_str() );
	m_pSkillText[ 2 ] = NiNew cText;
#ifdef UI_POSMOVE_RENEWAL_20190109
	m_pSkillText[ 2 ]->Init( m_pRoot, CsPoint( 43, 307 ), &ti, false );	
#else
	m_pSkillText[ 2 ]->Init( m_pRoot, CsPoint( 50, 317 ), &ti, false );	
#endif
}

bool cTamerStatus::_UpdateIcon_ForMouse_Skill()
{
	bool bCheck = false;
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
	{
		switch( m_pDeleteButton[i]->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				cSkill::sINFO* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerActiveSkill( i );

				if( pSkill == NULL )
					break;

				if( pSkill->s_nLifeTime == 0 )
					break;

				CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill->s_pFTSkill->GetInfo()->s_dwID );
				CsSkill* pTotalSkill = nsCsFileTable::g_pSkillMng->GetSkill(pTamerSkill->Get_TSkill_Info()->s_dwSkillCode );

				cPrintMsg::PrintMsg( 30583, pTotalSkill->GetInfo()->s_szName );	// 스킬을 삭제 하시겠습니까?
				cMessageBox::GetFirstMessageBox()->SetValue1( pTamerSkill->Get_TSkill_Info()->s_nIndex );	// 스킬 인덱스 넘김
			}
			break;
		default:
			break;
		}
	}

	switch( eCursor )
	{	
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
			{
				CsSkill* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetFTSkill( i );
				if( pSkill != NULL )
				{
					if( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pSkill->GetInfo()->s_dwID ) )
						continue;
				}

				if( m_IFIcon_BaseSkill[ i ].MouseLBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					m_IFIcon_BaseSkill[ i ].ReleaseState();
			}
			for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_ActiveSkill[ i ].MouseLBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					m_IFIcon_ActiveSkill[ i ].ReleaseState();
			}			
		}
		break;	
	case CURSOR::LBUTTON_PRESS:
		{
			_OnMouseOutWindow_Skill();
			for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
			{
				CsSkill* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetFTSkill( i );
				if( pSkill != NULL )
				{
					if( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pSkill->GetInfo()->s_dwID ) )
						continue;
				}	

				if( m_IFIcon_BaseSkill[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( m_IFIcon_BaseSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						m_IFIcon_BaseSkill[ i ].ReleaseState();
				}
			}
			for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_ActiveSkill[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( m_IFIcon_ActiveSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						m_IFIcon_ActiveSkill[ i ].ReleaseState();
				}
			}			
		}
		break;
	case CURSOR::RBUTTON_DOWN:
		{
			for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
			{
				CsSkill* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetFTSkill( i );
				if( pSkill != NULL )
				{
					if( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pSkill->GetInfo()->s_dwID ) )
						continue;
				}	

				if( m_IFIcon_BaseSkill[ i ].MouseRBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					m_IFIcon_BaseSkill[ i ].ReleaseState();
			}
			for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_ActiveSkill[ i ].MouseRBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					m_IFIcon_ActiveSkill[ i ].ReleaseState();
			}			
		}
		break;
	case CURSOR::RBUTTON_PRESS:
		{
			for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
			{
				CsSkill* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetFTSkill( i );
				if( pSkill != NULL )
				{
					if( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pSkill->GetInfo()->s_dwID ) )
						continue;
				}	

				if( m_IFIcon_BaseSkill[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( m_IFIcon_BaseSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						m_IFIcon_BaseSkill[ i ].ReleaseState();
				}
			}
			for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_ActiveSkill[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( m_IFIcon_ActiveSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						m_IFIcon_ActiveSkill[ i ].ReleaseState();
				}
			}			
		}
		break;
	default:
		for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
		{
			CsSkill* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetFTSkill( i );
			if( pSkill != NULL )
			{
				if( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pSkill->GetInfo()->s_dwID ) )
					continue;
			}	

			if( m_IFIcon_BaseSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
				bCheck = true;
			else
				m_IFIcon_BaseSkill[ i ].ReleaseState();
		}
		for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
		{
			if( m_IFIcon_ActiveSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
				bCheck = true;
			else
				m_IFIcon_ActiveSkill[ i ].ReleaseState();
		}		
	}

// 	for( int i=0; i<IF_TAMER_PASIVE_SKILL_COUNT; ++i )
// 	{
// 		if( m_IFIcon_PasiveSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
// 			bCheck = true;
// 		else
// 			m_IFIcon_PasiveSkill[ i ].ReleaseState();
// 	}


	// 툴팁용
	{
		for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
		{
			if( m_IFIcon_BaseSkill[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				CsSkill* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetFTSkill( i );
				if( pSkill != NULL )
				{
					if( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pSkill->GetInfo()->s_dwID ) )
						continue;
				}	

				if( g_pCharMng->GetTamerUser()->GetSkillMng()->IsSkill( i ) )
				{
					CsPoint pos = m_IFIcon_BaseSkill[ i ].GetPos() + GetRootClient();				
#ifdef VERSION_USA
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_SKILL_ICON_SIZE, 360, cTooltip::SKILL, i, 0, cBaseWindow::WT_TAMERSTATUS );
#else
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_SKILL_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::SKILL, i, 0, cBaseWindow::WT_TAMERSTATUS );
#endif
				}
				break;
			}
		}
		for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
		{
			if( m_IFIcon_ActiveSkill[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				int nIndex = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerActiveSkillIndex( i );
				if( nIndex != -1 )
				{
					CsPoint pos = m_IFIcon_ActiveSkill[ i ].GetPos() + GetRootClient();
#ifdef VERSION_USA
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_SKILL_ICON_SIZE, 360, cTooltip::SKILL, nIndex, 0, cBaseWindow::WT_TAMERSTATUS );
#else
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_SKILL_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::SKILL, nIndex, 0, cBaseWindow::WT_TAMERSTATUS );
#endif
				}				
				break;
			}
		}
		for( int i=0; i<IF_TAMER_PASIVE_SKILL_COUNT; ++i )
		{
			if( m_IFIcon_PasiveSkill[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				int nIndex = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerPasiveSkillIndex( i );
				if( nIndex != -1 )
				{
					CsPoint pos = m_IFIcon_PasiveSkill[ i ].GetPos() + GetRootClient();	
#ifdef VERSION_USA
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_SKILL_ICON_SIZE, 360, cTooltip::SKILL, nIndex, 0, cBaseWindow::WT_TAMERSTATUS );
#else
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_SKILL_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::SKILL, nIndex, 0, cBaseWindow::WT_TAMERSTATUS );
#endif
				}				
				break;
			}
		}
	}	

	return bCheck;
}

void cTamerStatus::_RenderSkill()
{
	assert_cs( g_pCharMng->GetTamerUser() );

	m_pSkillBG->Render();

	cSkill::sINFO* pSkill;
	CsPoint pos;

	cSkill* pSkillMng = g_pCharMng->GetTamerUser()->GetSkillMng();	

	for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
	{		
		pSkill = pSkillMng->GetSkill( i );

		if( pSkill == NULL )
			break;

		if( pSkill->s_pFTSkill->IsActive() || pSkill->s_pFTSkill->IsPasive()
			|| pSkill->s_pFTSkill->IsEmotionSkill() )
			continue;		

		// 위치
		pos = m_IFIcon_BaseSkill[ i ].GetPos() + GetRootClient();

		// 아이템 랜더링
		g_pIconMng->RenderSkill( pSkill, pos );

		// 쿨타임 랜더링
		if( pSkill->s_CoolTime.IsCoolTime() )
			g_pIconMng->RenderTime( pos, ICON_HALF, &pSkill->s_CoolTime );
		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_BaseSkill[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_BaseSkill[ i ].RenderMask( pos, IF_TAMER_SKILL_ICON_SIZE );
		}
	}

	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
	{
		pSkill = pSkillMng->GetTamerActiveSkill( i );		

		if( pSkill == NULL )
			break;

		// 위치
		pos = m_IFIcon_ActiveSkill[ i ].GetPos() + GetRootClient();

		// 아이템 랜더링
		g_pIconMng->RenderSkill( pSkill, pos );

		// 쿨타임 랜더링
		if( pSkill->s_CoolTime.IsCoolTime() )
			g_pIconMng->RenderTime( pos, ICON_HALF, &pSkill->s_CoolTime );
		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_ActiveSkill[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_ActiveSkill[ i ].RenderMask( pos, IF_TAMER_SKILL_ICON_SIZE );
		}
	}

	for( int i=0; i<IF_TAMER_PASIVE_SKILL_COUNT; ++i )
	{
		pSkill = pSkillMng->GetTamerPasiveSkill( i );		

		if( pSkill == NULL )
			break;

		// 위치
		pos = m_IFIcon_PasiveSkill[ i ].GetPos() + GetRootClient();

		// 아이템 랜더링
		g_pIconMng->RenderSkill( pSkill, pos );		

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_PasiveSkill[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_PasiveSkill[ i ].RenderMask( pos, IF_TAMER_SKILL_ICON_SIZE );
		}
	}

	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
	{
		cSkill::sINFO* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerActiveSkill(i);
		if( pSkill == NULL )
			continue;

		if( pSkill->s_nLifeTime == 0 )
			continue;

		m_pDeleteButton[i]->Render();
	}

	for( int i = 0; i < 3; i++ )
		m_pSkillText[ i ]->Render();
}

void cTamerStatus::_OnMouseOutWindow_Skill()
{
	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
		return;
	if( CURSOR_ST.IsIcon() == true )
		return;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
	{
		if( m_IFIcon_BaseSkill[ i ].s_eState == sIFIcon::MOUSE_LCLICK )
		{
			if( m_IFIcon_BaseSkill[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
			{
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_TSKILL, i, 1, &m_IFIcon_BaseSkill[ i ] );
			}
			break;
		}
	}
	
	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
	{		
		if( m_IFIcon_ActiveSkill[ i ].s_eState == sIFIcon::MOUSE_LCLICK )
		{
			if( m_IFIcon_ActiveSkill[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
			{
				int nIndex = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerActiveSkillIndex( i );
				if( nIndex != -1 )
				{
					CURSOR_ST.SetIcon( CURSOR_ICON::CI_TSKILL, nIndex, 1, &m_IFIcon_ActiveSkill[ i ] );
				}
			}
			break;
		}
	}
}

void cTamerStatus::_OnLButtonUp_Skill( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
	for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
	{
		if( ( m_IFIcon_BaseSkill[ i ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon_BaseSkill[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			// 동작
			CURSOR_ST.SetIcon( CURSOR_ICON::CI_TSKILL, i, 1, &m_IFIcon_BaseSkill[ i ] );
		}
		m_IFIcon_BaseSkill[ i ].ReleaseState();
	}

	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
	{
		if( ( m_IFIcon_ActiveSkill[ i ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon_ActiveSkill[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			// 동작
			int nIndex = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerActiveSkillIndex( i );
			if( nIndex != -1 )
			{
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_TSKILL, nIndex, 1, &m_IFIcon_ActiveSkill[ i ] );
			}
		}
		m_IFIcon_ActiveSkill[ i ].ReleaseState();
	}
}

void cTamerStatus::_OnRButtonUp_Skill( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
	for( int i=0; i<IF_TAMER_BASE_SKILL_COUNT; ++i )
	{
		if( ( m_IFIcon_BaseSkill[ i ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon_BaseSkill[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			g_pCharMng->GetTamerUser()->UseSkill( i );
		}
		m_IFIcon_BaseSkill[ i ].ReleaseState();
	}

	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
	{
		if( ( m_IFIcon_ActiveSkill[ i ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon_ActiveSkill[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			int nIndex = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerActiveSkillIndex( i );
			if( nIndex != -1 )
			{
				g_pCharMng->GetTamerUser()->UseSkill( nIndex );
			}
		}
		m_IFIcon_ActiveSkill[ i ].ReleaseState();
	}
}

bool cTamerStatus::_CursorIconLBtnUp_Skill( CURSOR_ICON::eTYPE eIconType, int nIconSlot )
{
	CURSOR_ST.ReleaseIcon();
	return false;
}

void cTamerStatus::_ResetDevice_Skill()
{
	if( m_pSkillBG )
		m_pSkillBG->ResetDevice();

	for( int i=0; i<IF_TAMER_ACTIVE_SKILL_COUNT; ++i )
	{
		if( m_pDeleteButton[i] )
			m_pDeleteButton[i]->ResetDevice();
	}

	for( int i = 0; i < 3; i++ )
	{
		if( m_pSkillText[ i ] )
			m_pSkillText[ i ]->ResetDevice();		
	}
}