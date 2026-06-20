#include "stdafx.h"
#include "SkillWindow.h"

void cSkillWin::sSkillSlot::Delete()
{	
	SAFE_DELETE( s_pSkillLVText );
	SAFE_DELETE( s_pPointButton );	
}

void cSkillWin::sSkillSlot::Init( cWindow* pParent, int nIdx )
{
	s_IFIcon_DigimonSkill.Init( CsRect( CsPoint( ( (nIdx % DIGIMON_SKILL_COUNT) * 57 ) + 107, ( ( nIdx / MAX_DIGIMON_COUNT ) * 67 ) + 87 ), CsSIZE( 26, 24 ) ) );
	
	s_pPointButton = NiNew cButton;
	s_pPointButton->Init( pParent, CsPoint( nIdx % DIGIMON_SKILL_COUNT * 57 + 126, nIdx / MAX_DIGIMON_COUNT * 67 + 106 ), CsPoint( 26, 24 ), "Skill\\Skill_Point.tga", false );
	s_pPointButton->SetTexToken( CsPoint( 0, 24 ) );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_9, FONT_WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( L"" );

	s_pSkillLVText = NiNew cText;	
	s_pSkillLVText->Init( NULL, CsPoint::ZERO, &ti, false );
}

void cSkillWin::sSkillSlot::_CheckEnable( int nDigimonIdx, int nSkillIndex )
{
	s_bCurDigimonSkill = false;
	if( nSkillIndex >= DIGIMON_MAX_SKILL_COUNT )
	{
		s_eState = DISABLE;
		return;
	}

	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	CDigimonEvolveInfo* pFTEvolInfo = pUser->GetFTEvol();
	CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( nDigimonIdx );	

	if( pFTEvolObj == NULL )	
	{
		s_eState = DISABLE; 
		return;
	}

	if( pUser->IsEvoPlag( pFTEvolObj->m_nEvoSlot, CDigimonUser::eActive ) != true )
	{
		s_eState = DISABLE;	
		return;
	}
			
	s_bCurDigimonSkill = ( pFTEvolObj->m_nEvoSlot == pUser->GetFTEvolCurObj()->m_nEvoSlot );

	CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID )->GetInfo();
	// 스킬이 존재 하는가
	DWORD nSkillID = pFTDigimon->s_Skill[ nSkillIndex ].s_dwID;
	if( nSkillID == 0 )
	{
		s_eState = DISABLE;
		return;
	}

	CsSkill::sINFO* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillID )->GetInfo();
	// 제한 레벨에 걸리는가
	if( pUser->GetBaseStat()->GetLevel() < pFTSkill->s_nLimitLevel )
	{
		s_eState = NOT_DIGIMON_LV;
		return;
	}

	// 스킬 레벨
	cEvoUnit* pEvoUnit = pUser->GetAttributeEvoUnit( pFTEvolObj->m_nEvoSlot );
	s_nSkillLv = pEvoUnit->m_nSkillLevel[ nSkillIndex ];
	s_pSkillLVText->SetText( s_nSkillLv );
	if( s_nSkillLv == 0 )
	{
		s_eState = NOT_SKILL_LV;
		return;
	}

	s_eState = ENABLE;
}

void cSkillWin::sSkillSlot::Update( int nDigimonIdx, int nSkillIndex )
{
	_CheckEnable( nDigimonIdx, nSkillIndex );

	switch( s_eState )
	{
	case DISABLE:
	case NOT_DIGIMON_LV:
		s_pPointButton->SetEnable( false );
		break;
	case NOT_SKILL_LV:
	case ENABLE:
		s_pPointButton->SetEnable( true );
		break;
	default:
		assert_cs( false );
	}
	
}

bool cSkillWin::sSkillSlot::_CheckSkillUp( int nDigimonIdx, int nSkillIndex )
{
	CDigimonUser*		pUser		=	g_pCharMng->GetDigimonUser( 0 );
	CDigimonEvolveInfo* pFTEvolInfo =	pUser->GetFTEvol();
	CDigimonEvolveObj*	pFTEvolObj	=	pFTEvolInfo->GetEvolveObjByEvoSlot( nDigimonIdx );
	CsDigimon::sINFO*	pFTDigimon	=	nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID )->GetInfo();
	cEvoUnit*			pEvoUnit	=	pUser->GetAttributeEvoUnit( pFTEvolObj->m_nEvoSlot );

	// 선행 스킬 레벨조건이 존재 하는가
	if( pFTDigimon->s_Skill[ nSkillIndex ].s_nReqPrevSkillLevel > 0 )
	{
		assert_cs( nSkillIndex > 0 );		
		if( pEvoUnit->m_nSkillLevel[ nSkillIndex - 1 ] < pFTDigimon->s_Skill[ nSkillIndex ].s_nReqPrevSkillLevel )
		{
			// 선행 스킬 레벨이 해당 레벨에 도달 하지 못하였음
			cPrintMsg::PrintMsg( 30475 );
			return false;
		}		
	}

	CsSkill::sINFO* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTDigimon->s_Skill[ nSkillIndex ].s_dwID )->GetInfo();
	DWORD nNewSkillPoint = pUser->GetNewSkillPoint();

	// 포인트가 작다면 패스
	if( nNewSkillPoint < pFTSkill->s_nLevelupPoint )
	{
		cPrintMsg::PrintMsg( 30475 );
		return false;
	}

	// 최대 레벨이라면 패스
	if( pEvoUnit->m_nSkillLevel[ nSkillIndex ] >= pFTSkill->s_nMaxLevel )
	{
		cPrintMsg::PrintMsg( 30476 );
		return false;
	}
	
	// 스킬 레벨업
	++pEvoUnit->m_nSkillLevel[ nSkillIndex ];
	// 포인트 감소
	pUser->DecNewSkillPoint( pFTSkill->s_nLevelupPoint );

	// 메세지				
	cPrintMsg::PrintMsg( 30005, pFTDigimon->s_szName, pFTSkill->s_szName );

	return true;
}

bool cSkillWin::sSkillSlot::Update_ForMouse( CsPoint ptMouseLocalPos, CsPoint ptPos, int nDigimonIdx, int nSkillIdx )
{
	bool bCheck = false;	

	switch( s_pPointButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;

	case cButton::ACTION_CLICK:
		{
			bCheck = true;
			
			if( _CheckSkillUp( nDigimonIdx, nSkillIdx ) )
			{
				// 동기화
				if( net::game )
				{					
					net::game->SendDigimonSkillLevelUp( g_pCharMng->GetDigimonUser( 0 )->GetUniqID(), nDigimonIdx, nSkillIdx );
				}				
			}			
		}	
		// 브레이크 걸지 말고 아래툴팁 고대로 보여주고 리턴하자
	default:
		{
			/*TCHAR sz[ 256 ];
			_stprintf_s( sz, 256, _LAN( "레벨업 하기 위해서 %d 포인트가 필요합니다." ), pFTSkill->s_nLevelupPoint );
			g_pGameIF->GetTooltip()->SetTooltip_Text( m_pSkillUpBtn[ i ]->GetClient(), m_pSkillUpBtn[ i ]->GetSize(), sz );*/
		}		
	}	

	if( s_bCurDigimonSkill == true )
	{
		CURSOR::eTYPE eCursor = g_Cursor.GetButtonState();
		switch( eCursor )
		{	
		case CURSOR::LBUTTON_DOWN:		
			{				
				if( s_IFIcon_DigimonSkill.MouseLBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					s_IFIcon_DigimonSkill.ReleaseState();										
			}
			break;	
		case CURSOR::LBUTTON_PRESS:
			{				
				OnMouseOutWindow_DSkill( ptMouseLocalPos, nSkillIdx );
				if( s_IFIcon_DigimonSkill.s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( s_IFIcon_DigimonSkill.MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						s_IFIcon_DigimonSkill.ReleaseState();
				}								
			}
			break;
		case CURSOR::RBUTTON_DOWN:
			{				
				if( s_IFIcon_DigimonSkill.MouseRBtnClick( ptMouseLocalPos ) == true )
					bCheck = true;
				else
					s_IFIcon_DigimonSkill.ReleaseState();
			}
			break;
		case CURSOR::RBUTTON_PRESS:
			{
				if( s_IFIcon_DigimonSkill.s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( s_IFIcon_DigimonSkill.MouseOn( ptMouseLocalPos ) == true )
						bCheck = true;
					else
						s_IFIcon_DigimonSkill.ReleaseState();
				}
			}
			break;
		default:
			if( s_IFIcon_DigimonSkill.MouseOn( ptMouseLocalPos ) == true )
				bCheck = true;
			else
				s_IFIcon_DigimonSkill.ReleaseState();					
			break;
		}
	}

	if( ( s_IFIcon_DigimonSkill.s_Rect.PtInRect( ptMouseLocalPos ) == TRUE ) && ( s_eState != DISABLE ) )
	{
		CsPoint pos = s_IFIcon_DigimonSkill.GetPos() + ptPos;				
		g_pGameIF->GetTooltip()->SetTooltip( pos, CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::SKILL, nSkillIdx, nDigimonIdx, cBaseWindow::WT_SKILL );					
		bCheck = true;
	}	

	return bCheck;
}

void cSkillWin::sSkillSlot::OnMouseOutWindow_DSkill( CsPoint pos, int nSkillIdx )
{
	if( g_Cursor.GetButtonState() != CURSOR::LBUTTON_PRESS )
		return;
	if( g_Cursor.IsIcon() == true )
		return;	

	if( s_IFIcon_DigimonSkill.s_eState == sIFIcon::MOUSE_LCLICK )
	{
		if( s_IFIcon_DigimonSkill.s_Rect.PtInRect( pos ) == FALSE )
		{
			g_Cursor.SetIcon( CURSOR_ICON::CI_DSKILL, nSkillIdx, 1, &s_IFIcon_DigimonSkill );
			g_Cursor.SetIconSubInfo( 0, 0, 0 );
		}		
	}		
}

void cSkillWin::sSkillSlot::OnLButtonUp_DSkill( CsPoint pos, int nSkillIdx )
{
	if( s_bCurDigimonSkill != true )
		return;

	if( s_eState != ENABLE )
		return;	

	if( ( s_IFIcon_DigimonSkill.s_eState == sIFIcon::MOUSE_LCLICK )&&( s_IFIcon_DigimonSkill.s_Rect.PtInRect( pos ) ) )
	{
		// 동작
		g_Cursor.SetIcon( CURSOR_ICON::CI_DSKILL, nSkillIdx, 1, &s_IFIcon_DigimonSkill );
		g_Cursor.SetIconSubInfo( 0, 0, 0 );
	}
	s_IFIcon_DigimonSkill.ReleaseState();
}

void cSkillWin::sSkillSlot::OnRButtonUp_DSkill( CsPoint pos, int nSkillIdx )
{
	if( s_bCurDigimonSkill != true )
		return;

	if( s_eState != ENABLE )
		return;	
}

void cSkillWin::sSkillSlot::ResetDevice()
{
	if( s_eState == DISABLE )
		return;

	s_pPointButton->ResetDevice();
	s_pSkillLVText->ResetDevice();	
}

void cSkillWin::sSkillSlot::Render( CsPoint ptPos, int nDigimonIdx, int nSkillIdx, cSprite* pIconBG, cSprite* pIconBlock )
{
	if( s_eState == DISABLE )
		return;

	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );

	CDigimonEvolveInfo* pFTEvolInfo = pUser->GetFTEvol();	
	CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( nDigimonIdx );
	CsSkill* pInfo = pUser->GetSkillMng()->GetFTSkill( pFTEvolObj->m_dwID, nSkillIdx );
	if( pInfo == NULL )
		return;

	CsPoint posIcon = s_IFIcon_DigimonSkill.GetPos() + ptPos;

	pIconBG->Render( posIcon - CsPoint( 4, 4 ) );
	g_pIconMng->RenderSkill( pInfo, posIcon );

	switch( s_eState )
	{
	case NOT_DIGIMON_LV:
		{
			pIconBlock->Render( posIcon );
		}
		break;
	case NOT_SKILL_LV:
		{
			pIconBlock->Render( posIcon );
			s_pPointButton->Render();
			s_pSkillLVText->Render( posIcon + CsPoint( 33, 25 ), DT_CENTER );
		}
		break;
	case ENABLE:
		{
			if( ( s_bCurDigimonSkill == true )&& pUser->IsLoad() )
			{
				cSkill::sINFO* pSkill = pUser->GetSkillMng()->GetSkill( nSkillIdx );

				// 쿨타임 랜더링
				if( pSkill->s_CoolTime.IsCoolTime() )
					g_pIconMng->RenderTime( posIcon, 16, &pSkill->s_CoolTime );

				// 마스크 있다면 마스크 랜더링
				if( s_IFIcon_DigimonSkill.s_eState != sIFIcon::NONE )
				{			
					s_IFIcon_DigimonSkill.RenderMask( posIcon, CsPoint( 32, 32 ) );
				}
			}
			s_pPointButton->Render();
			s_pSkillLVText->Render( posIcon + CsPoint( 33, 25 ), DT_CENTER );
		}
		break;
	default:
		assert_cs( false );
	}
}

void cSkillWin::_Delete_DSkill()
{
	SAFE_DELETE( m_pDigimonSkillBg );
	SAFE_DELETE( m_pCurDigimonSkillMask );
	SAFE_DELETE( m_pScroll );
	SAFE_DELETE( m_pIconBG );
	SAFE_DELETE( m_pIconBlock );
	SAFE_DELETE( m_pTextNewSkillPoint );

	for( int i = 0; i < DIGIMON_TEXT_COUNT; i++ )
		SAFE_DELETE( m_pTextDigimonSkill[ i ] );

	for( int i=0; i<nLimit::EvoUnit; ++i )
	{	
		SAFE_DELETE( m_pEvolImeage[ i ] );
	}				

	for( int i = 0; i < MAX_DIGIMON_COUNT * DIGIMON_SKILL_COUNT; i++ )
	{
		m_sSkillSlot[ i ].Delete();
	}
}

void cSkillWin::_Create_DSkill()
{
	cText::sTEXTINFO ti;
	ti.Init();
	ti.s_eTextAlign = DT_CENTER;

	ti.SetText( _LAN( "디지몬 스킬" ) );
	m_pTextDigimonSkill[ 0 ] = NiNew cText;
	m_pTextDigimonSkill[ 0 ]->Init( m_pRoot, CsPoint( 54, 55 ), &ti, false );

	ti.SetText( _LAN( "클래스 스킬" ) );
	m_pTextDigimonSkill[ 1 ] = NiNew cText;
	m_pTextDigimonSkill[ 1 ]->Init( m_pRoot, CsPoint( 54, 436 ), &ti, false );

	m_pDigimonSkillBg = NiNew cSprite;
	m_pDigimonSkillBg->Init( m_pRoot, CsPoint::ZERO, CsPoint( 431, 604 ), "Skill\\DSkill_Win.tga", false );

	m_pCurDigimonSkillMask = NiNew cSprite;
	m_pCurDigimonSkillMask->Init( m_pRoot, CsPoint::ZERO, CsPoint( 365, 58 ), "Skill\\Mask.tga", false );

	m_pScroll = NiNew cScrollBar;
	m_pScroll->Init( cScrollBar::TYPE_1, m_pRoot, CsPoint( 403, 48 ), CsPoint( 16, 365 ), CsPoint( 16, 15 ), 
		CsRect( CsPoint( 13, 48 ), CsPoint( 395, 413 ) ), MAX_DIGIMON_COUNT, false );

	m_pIconBG = NiNew cSprite;
	m_pIconBG->Init( NULL, CsPoint::ZERO, CsPoint( 40, 40 ), "Skill\\IconBG.tga", false );

	m_pIconBlock = NiNew cSprite;
	m_pIconBlock->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), NiColorA( 0.1f, 0.1f, 0.1f, 0.7f ), false );	

	for( int i = 0; i < DIGIMON_SKILL_COUNT * MAX_DIGIMON_COUNT; i++ )
	{	
		m_sSkillSlot[ i ].Init( m_pRoot, i );				
	}

	TCHAR szPoint[ 32 ];
	_stprintf_s( szPoint, 32, _T( "Point : %.2d" ), g_pCharMng->GetDigimonUser( 0 )->GetNewSkillPoint() );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( szPoint );
	m_pTextNewSkillPoint = NiNew cText;
	m_pTextNewSkillPoint->Init( m_pRoot, CsPoint( 310, 54 ), &ti, false );

	SetDigimonImeage();
}


void cSkillWin::_Update_DSkill()
{	
	for( int i = 0; i < MAX_DIGIMON_COUNT; ++i )
	{
		for( int j = 0; j < DIGIMON_SKILL_COUNT; ++j )
		{
			m_sSkillSlot[ i % MAX_DIGIMON_COUNT * DIGIMON_SKILL_COUNT + j ].Update( i + m_pScroll->GetCurPosIndex() + 1, j );
		}
	}

	// 포인트 정보
	TCHAR szPoint[ 32 ];
	_stprintf_s( szPoint, 32, _T( "Point : %.2d" ), g_pCharMng->GetDigimonUser( 0 )->GetNewSkillPoint() );
	cText::sTEXTINFO ti;
	ti.Init();
	ti.SetText( szPoint );
	m_pTextNewSkillPoint->SetText( szPoint );
}

bool cSkillWin::_Update_ForMouse_DSkill()
{	
	CsPoint ptMouseLocalPos = MousePosToWindowPos( g_Cursor.GetPos() );

	bool bCheck = false;
	for( int i = 0; i < MAX_DIGIMON_COUNT * DIGIMON_SKILL_COUNT; ++i )
	{
		bCheck = m_sSkillSlot[ i ].Update_ForMouse( ptMouseLocalPos, GetRootClient(),
				( i / MAX_DIGIMON_COUNT ) + m_pScroll->GetCurPosIndex() + 1, i % DIGIMON_SKILL_COUNT );					
	}

	if( m_pScroll )
		m_pScroll->Update_ForMouse();

	return bCheck;
}

void cSkillWin::_Render_DSkill()
{
	m_pDigimonSkillBg->Render();

	for( int i = 0; i < DIGIMON_TEXT_COUNT; i++ )
		m_pTextDigimonSkill[ i ]->Render();	

	m_pScroll->Render();
	m_pTextNewSkillPoint->Render();
	 
	CDigimonEvolveInfo* pFTEvolInfo = g_pCharMng->GetDigimonUser( 0 )->GetFTEvol();	
	for( int i = 0; i < MAX_DIGIMON_COUNT; i++ )
	{
		CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( i + m_pScroll->GetCurPosIndex() + 1 );
		if( pFTEvolObj == NULL )
			continue;

		if( g_pCharMng->GetDigimonUser( 0 )->IsEvoPlag( pFTEvolObj->m_nEvoSlot, CDigimonUser::eActive ) != true )
			continue;

		// 현재 내 디지몬일경우		
		if( pFTEvolObj->m_nEvoSlot == g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot )
		{
			m_pCurDigimonSkillMask->Render( CsPoint( 21, ( i * 67 ) + 74 ) );
		}

		// 디지몬 얼굴
		m_pEvolImeage[ pFTEvolObj->m_nEvoSlot ]->Render( GetRootClient() + CsPoint( 35, 87 + (i * 67) ) );

		for( int j = 0; j < DIGIMON_SKILL_COUNT; j++ )
		{				
			m_sSkillSlot[ i % MAX_DIGIMON_COUNT * DIGIMON_SKILL_COUNT + j ].Render( GetRootClient(), i + m_pScroll->GetCurPosIndex() + 1, j, m_pIconBG, m_pIconBlock );			
		}
	}		
}

void cSkillWin::_ResetDevice_DSkill()
{
	if( m_pDigimonSkillBg )
		m_pDigimonSkillBg->ResetDevice();
	
	if( m_pCurDigimonSkillMask )
		m_pCurDigimonSkillMask->ResetDevice();

	if( m_pScroll )
		m_pScroll->ResetDevice();

	if( m_pIconBG )
		m_pIconBG->ResetDevice();

	if( m_pIconBlock )
		m_pIconBlock->ResetDevice();

	if( m_pTextNewSkillPoint )
		m_pTextNewSkillPoint->ResetDevice();

	for( int i=0; i<nLimit::EvoUnit; ++i )
	{
		if( m_pEvolImeage[ i ] )
			m_pEvolImeage[ i ]->ResetDevice();
	}

	for( int i = 0; i < DIGIMON_TEXT_COUNT; i++ )
		m_pTextDigimonSkill[ i ]->ResetDevice();	

	for( int i = 0; i < MAX_DIGIMON_COUNT * DIGIMON_SKILL_COUNT; ++i )
		m_sSkillSlot[ i ].ResetDevice();
}

void cSkillWin::_OnLButtonUp_DSkill( CsPoint pos )
{	
	CsPoint ptLocal = MousePosToWindowPos( pos );
	for( int i=0; i<DIGIMON_SKILL_COUNT * MAX_DIGIMON_COUNT; ++i )
	{
		m_sSkillSlot[ i ].OnLButtonUp_DSkill( ptLocal, i % DIGIMON_SKILL_COUNT );	
	}
}

void cSkillWin::_OnRButtonUp_DSkill( CsPoint pos )
{
	
}

bool cSkillWin::_CursorIconLBtnUp_DSkill( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	g_Cursor.ReleaseIcon();
	return true;
}

void cSkillWin::SetDigimonImeage()
{
	for( int i=0; i<nLimit::EvoUnit; ++i )
	{	
		SAFE_DELETE( m_pEvolImeage[ i ] );
	}	

	m_nOpenCount = 0;
	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID() );
	for( int i=0; i<nLimit::EvoUnit; ++i )
	{	
		CDigimonEvolveObj* pInfo = pFTEvolInfo->GetEvolveObjByEvoSlot( i );	
		if( pInfo == NULL )
			continue;		
		
		if( g_pCharMng->GetDigimonUser( 0 )->IsEvoPlag( pInfo->m_nEvoSlot, CDigimonUser::eActive ) == true )
		{
			m_nOpenCount++;	
		}

		char cImgFile[ MAX_PATH ];
		CsDigimon::sINFO* pFTInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->m_dwID )->GetInfo();
		strcpy_s( cImgFile, MAX_PATH, g_pModelDataMng->GetData( pFTInfo->s_dwModelID )->GetInfo()->s_cKfmPath );		
		size_t len = strlen( cImgFile );
		cImgFile[ len - 4 ] = NULL;
		strcat_s( cImgFile, MAX_PATH, "_evo.tga" );

		// 진화 이미지 로드
		m_pEvolImeage[ i ] = NiNew cImage;
		m_pEvolImeage[ i ]->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), cImgFile, false, false );
		m_pEvolImeage[ i ]->SetTexToken( CsPoint( 32, 0 ) );		
	}	

	m_pScroll->SetRange( CsPoint( 0, m_nOpenCount ) );
	
	// 스킬 매니져
	m_pTargetSkillMng = g_pCharMng->GetDigimonUser( 0 )->GetSkillMng();

	// 창 업데이트
	_Update_DSkill();
	_ResetDevice_DSkill();
}