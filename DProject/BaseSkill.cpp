#include "stdafx.h"
#include "SkillWindow.h"

#define SKILL_COL	7

void cSkillWin::_Delete_BSkill()
{
	SAFE_DELETE( m_pBaseSkillBg );

	for( int i = 0; i < BASE_TEXT_COUNT; i++ )
		SAFE_DELETE( m_pTextBaseSkill[ i ] );
}

void cSkillWin::_Create_BSkill()
{
	m_pBaseSkillBg = NiNew cSprite;
	m_pBaseSkillBg->Init( m_pRoot, CsPoint::ZERO, CsPoint( 431, 604 ), "Skill\\Action_Win.tga", false );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;

	ti.SetText( _LAN( "기본스킬" ) );
	m_pTextBaseSkill[ 0 ] = NiNew cText;
	m_pTextBaseSkill[ 0 ]->Init( m_pRoot, CsPoint( 72, 43 ), &ti, false );

	ti.SetText( _LAN( "동작스킬" ) );
	m_pTextBaseSkill[ 1 ] = NiNew cText;
	m_pTextBaseSkill[ 1 ]->Init( m_pRoot, CsPoint( 72, 215 ), &ti, false );	

	CsSIZE size( 32, 32 );
	for( int i = 0; i < BASE_SKILL_COUNT; i++ )
	{
		m_IFIcon_BaseSkill[ i ].Init( CsRect( CsPoint( 40 + ( (i % SKILL_COL) * 50 ) , 83 + ( ( i / SKILL_COL ) * 49 ) ), size ) );
	}

	for( int i = 0; i < EMOTION_SKILL_COUNT; i++ )
	{
		m_IFIcon_EmotionSkill[ i ].Init( CsRect( CsPoint( 40 + ( (i % SKILL_COL) * 50 ) , 251 + ( ( i / SKILL_COL ) * 59 ) ), size ) );
	}	
}

void cSkillWin::_Update_BSkill()
{


}

bool cSkillWin::_Update_ForMouse_BSkill()
{
	bool bCheck = false;

	CURSOR::eTYPE eCursor = g_Cursor.GetButtonState();
	CsPoint ptMouseLocalPos = MousePosToWindowPos( g_Cursor.GetPos() );

	switch( eCursor )
	{	
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int i=0; i<BASE_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_BaseSkill[ i ].MouseLBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					m_IFIcon_BaseSkill[ i ].ReleaseState();
			}
			for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_EmotionSkill[ i ].MouseLBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					m_IFIcon_EmotionSkill[ i ].ReleaseState();
			}			
		}
		break;	
	case CURSOR::LBUTTON_PRESS:
		{
			_OnMouseOutWindow_BaseSkill();
			for( int i=0; i<BASE_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_BaseSkill[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( m_IFIcon_BaseSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						m_IFIcon_BaseSkill[ i ].ReleaseState();
				}
			}
			for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_EmotionSkill[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( m_IFIcon_EmotionSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						m_IFIcon_EmotionSkill[ i ].ReleaseState();
				}
			}			
		}
		break;
	case CURSOR::RBUTTON_DOWN:
		{
			for( int i=0; i<BASE_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_BaseSkill[ i ].MouseRBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					m_IFIcon_BaseSkill[ i ].ReleaseState();
			}
			for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_EmotionSkill[ i ].MouseRBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					m_IFIcon_EmotionSkill[ i ].ReleaseState();
			}			
		}
		break;
	case CURSOR::RBUTTON_PRESS:
		{
			for( int i=0; i<BASE_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_BaseSkill[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( m_IFIcon_BaseSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						m_IFIcon_BaseSkill[ i ].ReleaseState();
				}
			}
			for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
			{
				if( m_IFIcon_EmotionSkill[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( m_IFIcon_EmotionSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						m_IFIcon_EmotionSkill[ i ].ReleaseState();
				}
			}			
		}
		break;
	default:
		for( int i=0; i<BASE_SKILL_COUNT; ++i )
		{
			if( m_IFIcon_BaseSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
				bCheck = true;
			else
				m_IFIcon_BaseSkill[ i ].ReleaseState();
		}
		for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
		{
			if( m_IFIcon_EmotionSkill[ i ].MouseOn( ptMouseLocalPos ) == true )
				bCheck = true;
			else
				m_IFIcon_EmotionSkill[ i ].ReleaseState();
		}		
		break;
	}

	// 툴팁용
	{
		for( int i=0; i<BASE_SKILL_COUNT; ++i )
		{
			if( m_IFIcon_BaseSkill[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				if( g_pCharMng->GetTamerUser()->GetSkillMng()->IsSkill( i ) )
				{
					CsPoint pos = m_IFIcon_BaseSkill[ i ].GetPos() + GetRootClient();				
					g_pGameIF->GetTooltip()->SetTooltip( pos, CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::SKILL, i, 0, cBaseWindow::WT_SKILL );
				}
				break;
			}
		}
		for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
		{
			if( m_IFIcon_EmotionSkill[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				int nIndex = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerEmotionSkillIndex( i );
				if( nIndex != -1 )
				{
					CsPoint pos = m_IFIcon_EmotionSkill[ i ].GetPos() + GetRootClient();
					g_pGameIF->GetTooltip()->SetTooltip( pos, CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::SKILL, nIndex, 0, cBaseWindow::WT_SKILL );
				}				
				break;
			}
		}		
	}	

	return bCheck;
}

void cSkillWin::_Render_BSkill()
{
	m_pBaseSkillBg->Render();	

	cSkill* pSkillMng = g_pCharMng->GetTamerUser()->GetSkillMng();	

	cSkill::sINFO* pSkill;
	CsPoint pos;

	for( int i=0; i<BASE_SKILL_COUNT; ++i )
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
			g_pIconMng->RenderTime( pos, 16, &pSkill->s_CoolTime );

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_BaseSkill[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_BaseSkill[ i ].RenderMask( pos, CsPoint( 32, 32 ) );
		}
	}


	for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
	{		
		pSkill = pSkillMng->GetTamerEmotionSkill( i );		

		if( pSkill == NULL )
			break;

		// 위치
		pos = m_IFIcon_EmotionSkill[ i ].GetPos() + GetRootClient();

		// 아이템 랜더링
		g_pIconMng->RenderSkill( pSkill, pos );

		// 쿨타임 랜더링
		if( pSkill->s_CoolTime.IsCoolTime() )
			g_pIconMng->RenderTime( pos, 16, &pSkill->s_CoolTime );

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_EmotionSkill[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_EmotionSkill[ i ].RenderMask( pos, CsPoint( 32, 32 ) );
		}
	}

	for( int i = 0; i < BASE_TEXT_COUNT; i++ )
		m_pTextBaseSkill[ i ]->Render();
}

void cSkillWin::_ResetDevice_BSkill()
{
	if( m_pBaseSkillBg )
		m_pBaseSkillBg->ResetDevice();

	for( int i = 0; i < BASE_TEXT_COUNT; i++ )
		m_pTextBaseSkill[ i ]->ResetDevice();	
}

void cSkillWin::_OnMouseOutWindow_BaseSkill()
{
	if( g_Cursor.GetButtonState() != CURSOR::LBUTTON_PRESS )
		return;
	if( g_Cursor.IsIcon() == true )
		return;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( g_Cursor.GetPos() );

	for( int i=0; i<BASE_SKILL_COUNT; ++i )
	{
		if( m_IFIcon_BaseSkill[ i ].s_eState == sIFIcon::MOUSE_LCLICK )
		{
			if( m_IFIcon_BaseSkill[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
			{
				g_Cursor.SetIcon( CURSOR_ICON::CI_TSKILL, i, 1, &m_IFIcon_BaseSkill[ i ] );
			}
			break;
		}
	}

	for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
	{		
		if( m_IFIcon_EmotionSkill[ i ].s_eState == sIFIcon::MOUSE_LCLICK )
		{
			if( m_IFIcon_EmotionSkill[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
			{
				int nIndex = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerEmotionSkillIndex( i );
				if( nIndex != -1 )
				{
					g_Cursor.SetIcon( CURSOR_ICON::CI_TSKILL, nIndex, 1, &m_IFIcon_EmotionSkill[ i ] );
				}
			}
			break;
		}
	}
}

void cSkillWin::_OnLButtonUp_BSkill( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
	for( int i=0; i<BASE_SKILL_COUNT; ++i )
	{
		if( ( m_IFIcon_BaseSkill[ i ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon_BaseSkill[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			// 동작
			g_Cursor.SetIcon( CURSOR_ICON::CI_TSKILL, i, 1, &m_IFIcon_BaseSkill[ i ] );
		}
		m_IFIcon_BaseSkill[ i ].ReleaseState();
	}

	for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
	{
		if( ( m_IFIcon_EmotionSkill[ i ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon_EmotionSkill[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			// 동작
			int nIndex = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerEmotionSkillIndex( i );
			if( nIndex != -1 )
			{
				g_Cursor.SetIcon( CURSOR_ICON::CI_TSKILL, nIndex, 1, &m_IFIcon_EmotionSkill[ i ] );
			}
		}
		m_IFIcon_EmotionSkill[ i ].ReleaseState();
	}	
}

void cSkillWin::_OnRButtonUp_BSkill( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
	for( int i=0; i<BASE_SKILL_COUNT; ++i )
	{
		if( ( m_IFIcon_BaseSkill[ i ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon_BaseSkill[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			g_pCharMng->GetTamerUser()->UseSkill( i );
		}
		m_IFIcon_BaseSkill[ i ].ReleaseState();
	}

	for( int i=0; i<EMOTION_SKILL_COUNT; ++i )
	{
		if( ( m_IFIcon_EmotionSkill[ i ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon_EmotionSkill[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			int nIndex = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerEmotionSkillIndex( i );
			if( nIndex != -1 )
			{
				g_pCharMng->GetTamerUser()->UseSkill( nIndex );
			}
		}
		m_IFIcon_EmotionSkill[ i ].ReleaseState();
	}
}

bool cSkillWin::_CursorIconLBtnUp_BSkill( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	g_Cursor.ReleaseIcon();
	return true;
}
