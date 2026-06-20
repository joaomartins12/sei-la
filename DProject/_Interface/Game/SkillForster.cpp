#include "stdafx.h"
#include "SkillForster.h"

void cSkillForster::Destroy()
{	
	cBaseWindow::Delete();	
}

void cSkillForster::DeleteResource()
{	
	m_bSelect = false;

	DeleteScript();
	SAFE_NIDELETE( m_pMouseOnMask );
	SAFE_NIDELETE( m_pExpandImage );
	SAFE_NIDELETE( m_pMoney );

	m_ListSkill.Delete();
	m_ListSkillDisc.Delete();
}

void cSkillForster::Create(int nValue /* = 0  */)
{	
	if( IsLive() == false )
	{	
		m_bExpandActive = false;
		m_bExpandPassive = false;
		SetRootClient( CsPoint( 150, 165 ) );			
	}	
	
	m_pTarget = NULL;

	cBaseWindow::Init();
	InitScript( "SkillForest\\Ta_Skill_Win.tga", m_ptRootClient, CsPoint( 302, 481 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );		
	AddTitle( _LAN( "테이머 스킬" ) );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
	ti.SetText( _LAN( "습득할 스킬을 선택하십시오." ) );
	AddText( &ti, CsPoint( 20, 42 ) );

	ti.s_eFontSize = CFont::FS_10;	
	m_pCancelButton = AddButton( CsPoint( 280, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	m_pMouseOnMask = NiNew cSprite;
	m_pMouseOnMask->Init( NULL, CsPoint::ZERO, CsPoint( 240, 19 ), "Talk\\Common_Seletbar.tga", false );
	m_pMouseOnMask->SetAlpha( 0.5f );

	m_pExpandImage = NiNew cImage;
	m_pExpandImage->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12 ), "QuestList\\Expand.tga", false );
	m_pExpandImage->SetTexToken( CsPoint( 0, 12 ) );

	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 279, 36 ), CsPoint( 16, 275 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 10, 37 ), CsSIZE( 280, 403 ) ), 11 );

	m_pLearn = AddButton( CsPoint( 144, 443 ), CsPoint( 76, 27 ), cButton::IMAGE_NOR_1, _LAN( "배우기" ) );	
	m_pLearn->SetEnable( false );
	m_pClosed = AddButton( CsPoint( 222, 443 ), CsPoint( 76, 27 ), cButton::IMAGE_NOR_1, _LAN( "닫기" ) );	

	m_pMoney = NiNew cMoneyIF;
	m_pMoney->Init( cMoneyIF::DISP_FULL, g_pDataMng->GetInven()->GetMoney(), false );

	_SetActiveSkillList(); 
	_SetPassiveSkillList(); 
	m_pScrollBar->SetRange( CsPoint( 0, m_ListSkill.GetSize() ) );
	m_ListSkill.SetBuddy( m_pScrollBar );
}

cBaseWindow::eMU_TYPE
cSkillForster::Update_ForMouse()
{	
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;	
	}

	switch( m_pClosed->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;	
	}

	switch( m_pLearn->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:		
		return muReturn;	
	}

	cString* pString;		
	CsPoint  ptSelMarkPos = CsPoint( 20, 75 ) + GetRootClient();	
	int nCursor = m_ListSkill.Update_ForMouse( &pString, m_nMouseOnMaskIndex, m_nSelectSkill, ptSelMarkPos, 5, CsPoint( 250, 23 ) ); 
	if( m_nMouseOnMaskIndex != cString::INVALIDE_STRING_VALUE ) 
	{
		switch( nCursor )
		{
		case CURSOR::BUTTON_OUTWINDOW:
			m_bSelect = false;		
			break;
		case CURSOR::BUTTON_NONE:
		case CURSOR::LBUTTON_DOWN:
			m_bSelect = true;		
			break;
		case CURSOR::LBUTTON_UP:
			if( m_bSelect == true )
			{	
				if( m_nMouseOnMaskIndex == 0 )
				{
					if( nCursor == CURSOR::LBUTTON_UP ) // 액티브스킬 버튼
					{
						m_bExpandActive = ( m_bExpandActive == true ) ? false : true;
						_ResetSkillList();
						m_ListSkillDisc.Delete();
						m_pLearn->SetEnable( false );
					}
				}
				else if( m_nMouseOnMaskIndex == m_nMaxActiveSkill + 1 )
				{
					if( nCursor == CURSOR::LBUTTON_UP ) // 패시브스킬 버튼
					{
						m_bExpandPassive = ( m_bExpandPassive == true ) ? false : true;
						_ResetSkillList();
						m_ListSkillDisc.Delete();
						m_pLearn->SetEnable( false );
					}
				}
				else
				{
					_SkillDisc( m_nSelectSkill );					
				}
			}				
			return muReturn;
		}		
	}

	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;	

	_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cSkillForster::Update(float const& fDeltaTime)
{	
	g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_SKILL_FORSTER );

	// 테이머와의 거리 체크
	if( m_pTarget )
	{
		assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
		if( ( (CNpc*)m_pTarget )->CheckTamerDist() == false )
		{		
			Close();
			return;
		}
	}

	if( m_pMoney )
		m_pMoney->SetMoney( g_pDataMng->GetInven()->GetMoney() );

	_UpdateMoveWindow();	
}

void cSkillForster::Render()
{
	RenderScript();	

	if( m_pMoney != NULL )
		m_pMoney->Render( CsPoint( -5, 448 ) + GetRootClient() );

	if( m_bSelect == true )
	{
		int nPosY = m_ListSkill.GetStringPosY( 74, m_nMouseOnMaskIndex, 5 );
		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( 20, nPosY ) + GetRootClient();
			m_pMouseOnMask->Render( pos );	
		}	
	}

	m_ListSkill.Render( GetRootClient() + CsPoint( 20, 75 ), 5 );	
	m_ListSkillDisc.Render( GetRootClient() + CsPoint( 21, 334 ), 0 );
}

void cSkillForster::ResetDevice()
{	
	ResetDeviceScript();	

	if( m_pMoney )
		m_pMoney->ResetDevice();

	if( m_pMouseOnMask )
		m_pMouseOnMask->ResetDevice();	

	m_ListSkill.ResetDevice();
	m_ListSkillDisc.ResetDevice();
}

void cSkillForster::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
	
}

void cSkillForster::_ResetSkillList()
{
	m_ListSkill.Delete();
	_SetActiveSkillList();
	_SetPassiveSkillList();
	m_pScrollBar->SetRange( CsPoint( 0, m_ListSkill.GetSize() ) );
}

void cSkillForster::_SetActiveSkillList()
{
	CsSkill::MAP_IT it = nsCsFileTable::g_pSkillMng->GetSkillMap()->begin();
	CsSkill::MAP_IT itEnd = nsCsFileTable::g_pSkillMng->GetSkillMap()->end();

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.SetText( _LAN( "액티브 스킬" ) );
	m_nMaxActiveSkill = 0;

	cString* pString = NULL;
	pString = NiNew cString;
	pString->AddImage( m_pExpandImage, ( m_bExpandActive == true ) ? 1 : 0, CsPoint( 0, 0 ) );	
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( m_nMaxActiveSkill );

	m_ListSkill.AddTail( pString );
	if( m_bExpandActive == true )
	{
		//액티브스킬 넣기
		for( ; it != itEnd; it++ )
		{
			if( it->second->GetInfo()->s_nLimitLevel == 0 || it->second->IsTamerSkill() == false )
				continue;

			if( it->second->GetInfo()->s_dwID > 8100000 )
			{
				switch( g_pCharMng->GetTamerUser()->GetModelID() )
				{
				case CODE_MASARU:
					if( it->second->IsTamerUnicSkill( 8100000 ) == false )
						continue;
						break;
				case CODE_TOHMA:	
					if( it->second->IsTamerUnicSkill( 8200000 ) == false )
						continue;
						break;
				case CODE_YOSHINO:
					if( it->second->IsTamerUnicSkill( 8300000 ) == false )
						continue;
						break;
				case CODE_IKUTO:
					if( it->second->IsTamerUnicSkill( 8400000 ) == false )
						continue;
						break;
				}
			}

			if( it->second->IsActive() == true )
			{
				m_nMaxActiveSkill++;
				_AddSkill( it->second, m_nMaxActiveSkill );					
			}
		}
	}
}

void cSkillForster::_DeleteActiveSkill()
{
	for( int i = 0; i < m_nMaxActiveSkill; i++ )
	{
		m_ListSkill.RemoveIndex( i );
	}
}

void cSkillForster::_SetPassiveSkillList()
{
	CsSkill::MAP_IT it = nsCsFileTable::g_pSkillMng->GetSkillMap()->begin();
	CsSkill::MAP_IT itEnd = nsCsFileTable::g_pSkillMng->GetSkillMap()->end();

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.SetText( _LAN( "패시브 스킬" ) );
	m_nMaxPassiveSkill = m_nMaxActiveSkill + 1;

	cString* pString = NULL;
	pString = NiNew cString;
	pString->AddImage( m_pExpandImage, ( m_bExpandPassive == true ) ? 1 : 0, CsPoint( 0, 0 ) );	
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( m_nMaxActiveSkill + 1 );	

	m_ListSkill.AddTail( pString );			
	if( m_bExpandPassive == true )
	{
		//패시브스킬 넣기
		for( ; it != itEnd; it++ )
		{
			if( it->second->GetInfo()->s_nLimitLevel == 0 || it->second->IsTamerSkill() == false )
				continue;

			if( it->second->GetInfo()->s_dwID > 8100000 )
			{
				switch( g_pCharMng->GetTamerUser()->GetModelID() )
				{
				case CODE_MASARU:
					if( it->second->IsTamerUnicSkill( 8100000 ) == false )
						continue;
						break;
				case CODE_TOHMA:	
					if( it->second->IsTamerUnicSkill( 8200000 ) == false )
						continue;
						break;
				case CODE_YOSHINO:
					if( it->second->IsTamerUnicSkill( 8300000 ) == false )
						continue;
						break;
				case CODE_IKUTO:
					if( it->second->IsTamerUnicSkill( 8400000 ) == false )
						continue;
						break;
				}
			}
			
			if( it->second->IsPasive() == true )
			{
				m_nMaxPassiveSkill++;
				_AddSkill( it->second, m_nMaxPassiveSkill );					
			}			
		}
	}
}

void cSkillForster::_DeletePassiveSkill()
{
	for( int i = 0; i < m_nMaxPassiveSkill; i++ )
	{
		m_ListSkill.RemoveIndex( i );
	}
}

void cSkillForster::_AddSkill( CsSkill* pSkill, int nIdx )
{
	TCHAR sz[ 32 ];	 
	cString* pString = NiNew cString;
	pString->TailAddSizeX( 18 );

	CsPoint ptPos;	
	int nBase;	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	ti.s_Color = FONT_RED;
	if( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() >= pSkill->GetInfo()->s_nLimitLevel )
	{
// #ifndef DIGIMON_CASH_SKILL
// 		// 해당 cpp는 현재 사용하지 않는 cpp임. 이전 신규 스킬 시스템 관련 코드로 보임.
// 		// s_nLearnMoney 해당 변수의 data를 읽어오는 Excel Table의 용도 변경으로 사용 안함.
// 		if( g_pDataMng->GetInven()->GetMoney().GetInt() >= pSkill->GetInfo()->s_nLearnMoney )
// 			ti.s_Color = FONT_GREEN;
// #endif
	}

	ti.SetText( pSkill->GetInfo()->s_szName );
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
	
	pString->AddText( &ti );
	pString->TailAddSizeX( 178 - ptPos.x );	

	_stprintf_s( sz, 32, _LAN( "(레벨%d)" ), pSkill->GetInfo()->s_nMaxLevel );
	ti.SetText( sz );
	pString->AddText( &ti );	
	pString->SetValue1( nIdx );
	pString->SetValue2( pSkill->GetInfo()->s_dwID );

	m_ListSkill.AddTail( pString );
}

void cSkillForster::_SkillDisc( DWORD dwID )
{
	m_ListSkillDisc.Delete();  

	TCHAR sz[ 128 ];
	cString* pString = NULL;
	CsSkill::sINFO* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( dwID )->GetInfo();	

	m_pLearn->SetEnable( false );
	if( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() >= pSkill->s_nLimitLevel )
	{
// #ifndef DIGIMON_CASH_SKILL
// 		// 해당 cpp는 현재 사용하지 않는 cpp임. 이전 신규 스킬 시스템 관련 코드로 보임.
// 		// s_nLearnMoney 해당 변수의 data를 읽어오는 Excel Table의 용도 변경으로 사용 안함.
// 		if( g_pDataMng->GetInven()->GetMoney().GetInt() >= pSkill->s_nLearnMoney )
// 			m_pLearn->SetEnable( true );
// #endif
	}
	
	pString = NiNew cString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.s_Color = FONT_YELLOW;
	ti.SetText( pSkill->s_szName );
	pString->AddIcon( CsPoint( 32, 32 ), ICONITEM::SKILL3, pSkill->s_nIcon );
	pString->TailAddSizeX( 10 );
	pString->AddText( &ti );	
	m_ListSkillDisc.AddTail( pString );

	ti.s_Color = FONT_WHITE;	
	pString = NiNew cString;	
	_stprintf_s( sz, 128, _LAN( "요구사항 : %d 레벨, %s (%d레벨)" ), pSkill->s_nLimitLevel, pSkill->s_szName, 1 );
	ti.SetText( sz );	
	pString->HeadAddSizeX( 40 );
	pString->AddText( &ti, CsPoint( 0, -15 ) );	
	m_ListSkillDisc.AddTail( pString );

	pString = NiNew cString;	
	ti.SetText( _LAN( "비용 : " ) );
	pString->AddText( &ti );
	// 해당 cpp는 현재 사용하지 않는 cpp임. 이전 신규 스킬 시스템 관련 코드로 보임.
	pString->AddMoneyIF( cMoneyIF::DISP_SIMPLE_1, pSkill->s_nReq_Item, FONT_WHITE, TOOLTIP_FONT_1_SIZE, DT_LEFT );
	m_ListSkillDisc.AddTail( pString );

	ti.SetText( pSkill->s_szComment );
	g_pStringAnalysis->Cut( &m_ListSkillDisc, 270, pSkill->s_szComment, &ti );
}
