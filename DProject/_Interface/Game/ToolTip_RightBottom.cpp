
#include "stdafx.h"
#include "ToolTip_RightBottom.h"

cToolTip_Right::cToolTip_Right()
{
	m_eType = NONE;
	m_nMainID = m_nSubID_1 = m_nSubID_2 = m_nSubID_3 = -1;
	m_pData = NULL;

	m_bRenderTooltip = false;
}

void cToolTip_Right::Delete()
{
	cTooltip::Delete();
	SAFE_NIDELETE( m_pHPBar );
}

void cToolTip_Right::Init()
{
	cTooltip::Init();
	m_pHPBar = NULL;
	m_pTarget = NULL;
}

void cToolTip_Right::Update( float const& fDeltaTime )
{
	if( !m_bRenderTooltip )
		return;

	cTooltip::Update(fDeltaTime);

	// hp
	if( m_pHPBar && m_pTarget )
	{
		if( m_pTarget->GetBaseStat() == NULL )
			return;
		m_pHPBar->SetRange( CsPoint( 0, m_pTarget->GetBaseStat()->GetMaxHP() ) );
		m_pHPBar->SetBarPos( m_pTarget->GetBaseStat()->GetHP() );
	}
}

// void cToolTip_Right::_Update()
// {
// 	cTooltip::_Update();
// 
// 	// hp
// 	if( m_pHPBar && m_pTarget )
// 	{
// 		if( m_pTarget->GetBaseStat() == NULL )
// 			return;
// 		m_pHPBar->SetRange( CsPoint( 0, m_pTarget->GetBaseStat()->GetMaxHP() ) );
// 		m_pHPBar->SetBarPos( m_pTarget->GetBaseStat()->GetHP() );
// 	}
// }

void cToolTip_Right::Render()
{
	if( m_pHPBar && m_bRenderTooltip )
		m_pHPBar->Render();

	cTooltip::Render(m_ptRenderPos, true);	//2017-01-19-nova
}

void cToolTip_Right::Render( CsPoint ptPos )
{
	if( m_pHPBar && m_bRenderTooltip )
		m_pHPBar->Render();

	cTooltip::Render( ptPos );	
}

void cToolTip_Right::ResetDevice()
{
	cTooltip::ResetDevice();

	if( m_pHPBar )
		m_pHPBar->ResetDevice();
}

void cToolTip_Right::SetRightTooltip( CsC_AvObject* pTarget )
{	
	m_ptPatchSize = CsPoint::ZERO;		

	// 전부 이전과 동일 하다면 생성하지말고 리턴
	if( ( m_eType == TEXT )&&( m_pTarget == pTarget ) )
	{
		m_bRenderTooltip = true;
		return;
	}

	SAFE_NIDELETE( m_pHPBar );
	m_pTarget = pTarget;
	m_DelayTime.Start();
	m_vpEditElement.ClearElement();	

	m_StringList.Delete();
	m_StringList_Name.Delete();
	m_eType = TEXT;	

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );		

	switch( m_pTarget->GetLeafRTTI() )
	{
	case RTTI_NPC:
		{	
			CNpc* pTarget = (CNpc*)m_pTarget;

			_tcscpy_s( m_szText, MAX_PATH, pTarget->GetName() );
			ti.s_Color = pTarget->GetNameColor();
			g_pStringAnalysis->Cut( &m_StringList, 500, m_szText, &ti );			

			ti.s_Color = FONT_WHITE;
			_tcscpy_s( m_szText, MAX_PATH, pTarget->GetCharOption()->GetPenName() );
			g_pStringAnalysis->Cut( &m_StringList, 500, m_szText, &ti );			

			std::wstring wsLevel;
			DmCS::StringFn::Format( wsLevel, L"%s ??", UISTRING_TEXT( "COMMON_TXT_LEVEL" ).c_str() );
			g_pStringAnalysis->Cut( &m_StringList, 500, wsLevel.c_str(), &ti );
		}
		break;	
	case RTTI_ITEM:
		{			
			CItem* pTarget = (CItem*)m_pTarget;

			CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem(	m_pTarget->GetFTID() );
			SAFE_POINTER_BEK( pItem );
			SAFE_POINTER_BEK( pItem->GetInfo() );
			ti.s_Color = g_pDataMng->GetItemColor( pItem->GetInfo() );
			_tcscpy_s( m_szText, MAX_PATH, pItem->GetInfo()->s_szName );
			g_pStringAnalysis->Cut( &m_StringList, 500, m_szText, &ti );				

			if( m_pAdaptParty && !m_pAdaptParty->IsJoinParty() )
			{
				if( g_pCharMng->GetTamerUser()->GetUniqID() == pTarget->GetOwnerUID() )
				{
					if( pItem->GetInfo()->s_nType_L == 92 )
					{
						ti.s_Color = NiColor( 0.0f, 1.0f, 0.0f );
						_stprintf_s( m_szText, MAX_PATH, _T( "%s(%s)" ), UISTRING_TEXT( "TOOLTIP_CAPTURABLE" ).c_str(), UISTRING_TEXT( "TOOLTIP_SCANNABLE" ).c_str() );
					}
					else
					{
						ti.s_Color = NiColor( 0.0f, 1.0f, 0.0f );
						_tcscpy_s( m_szText, MAX_PATH, UISTRING_TEXT( "TOOLTIP_CAPTURABLE" ).c_str() );
					}				
				}
				else
				{
					ti.s_Color = NiColor( 1.0f, 0.0f, 0.0f );
					_tcscpy_s( m_szText, MAX_PATH, UISTRING_TEXT( "TOOLTIP_NON_CAPTURABLE" ).c_str() );
				}

				g_pStringAnalysis->Cut( &m_StringList, 500, m_szText, &ti );	
			}
		}
		break;
	case RTTI_TAMER:
		{	
			CTamer* pTarget = (CTamer*)m_pTarget;

			_tcscpy_s( m_szText, MAX_PATH, pTarget->GetName() );
			ti.s_Color = pTarget->GetNameColor();
			g_pStringAnalysis->Cut( &m_StringList, 500, m_szText, &ti );			

			ti.s_Color = FONT_WHITE;
			if( _tcslen( pTarget->GetCharOption()->GetGuildName() ) > 0 )
			{
				_stprintf_s( m_szText, MAX_PATH, _T( "<%s>" ), pTarget->GetCharOption()->GetGuildName() );
				g_pStringAnalysis->Cut( &m_StringList, 200, m_szText, &ti );			
			}			

			std::wstring wsLevel = UISTRING_TEXT( "TOOLTIP_LEVEL_PLAYER" );
			DmCS::StringFn::Replace( wsLevel, L"#Num#", pTarget->GetBaseStat()->GetLevel() );
			g_pStringAnalysis->Cut( &m_StringList, 500, wsLevel.c_str(), &ti );			
		}
		break;
	case RTTI_DIGIMON:
		{	
			CDigimon* pTarget = (CDigimon*)m_pTarget;
			SAFE_POINTER_BEK( pTarget );

			_tcscpy_s( m_szText, MAX_PATH, pTarget->GetName() );
			ti.s_Color = pTarget->GetNameColor();
			g_pStringAnalysis->Cut( &m_StringList, 500, m_szText, &ti );

			CsDigimon* pDigimonFT = pTarget->GetFTDigimon();
			SAFE_POINTER_BEK( pDigimonFT );

			CsDigimon::sINFO* pDigimonInfo = pDigimonFT->GetInfo();
			SAFE_POINTER_BEK( pDigimonInfo );

			ti.s_Color = FONT_WHITE;

			std::wstring evName;
			if( nsCsFileTable::g_pBaseMng )
				evName = nsCsFileTable::g_pBaseMng->GetEvoTypeName( (int)pDigimonInfo->s_eEvolutionType );

			std::wstring msg;
			DmCS::StringFn::Format( msg, L"%s(%s)", pDigimonInfo->s_szName, evName.c_str() );			
			g_pStringAnalysis->Cut( &m_StringList, 500, msg.c_str(), &ti );			

			std::wstring wsLevel;
			DmCS::StringFn::Format( wsLevel, L"%s %d", UISTRING_TEXT( "COMMON_TXT_LEVEL" ).c_str(), pTarget->GetBaseStat()->GetLevel() );
			g_pStringAnalysis->Cut( &m_StringList, 500, wsLevel.c_str(), &ti );			
		}
		break;
	case RTTI_TUTORIAL_MONSTER:
	case RTTI_MONSTER:
		{				
			CMonster* pTarget = (CMonster*)m_pTarget;

			//이름
			_tcscpy_s( m_szText, MAX_PATH, pTarget->GetName() );			
			ti.s_Color = pTarget->GetNameColor();
			g_pStringAnalysis->Cut( &m_StringList, 500, m_szText, &ti );			

			ti.s_Color = FONT_WHITE;			

			_tcscpy_s( m_szText, MAX_PATH, pTarget->GetDigimonFT()->GetInfo()->s_szForm );							
			g_pStringAnalysis->Cut( &m_StringList, 500, m_szText, &ti );

			std::wstring wsLevel;

			switch( pTarget->GetMonsterFT()->GetInfo()->s_nClass )
			{
			case 3: case 4:
				DmCS::StringFn::Format( wsLevel, L"%s ??", UISTRING_TEXT( "COMMON_TXT_LEVEL" ).c_str() );
				break;
			default:
				DmCS::StringFn::Format( wsLevel, L"%s %d", UISTRING_TEXT( "COMMON_TXT_LEVEL" ).c_str(), pTarget->GetBaseStat()->GetLevel() );
				break;
			}			
			g_pStringAnalysis->Cut( &m_StringList, 500, wsLevel.c_str(), &ti );				
		}
		break;
	case RTTI_EMPLOYMENT:
		m_pTarget = NULL;
		return;

	default:
		// 마우스 오버 툴팁만 체크
		assert_csm( false, L"잘못된 타겟 정보" );
		m_pTarget = NULL;
		return;
	}	

	// 마무리 설정
	m_nMaxSizeX = m_StringList.CalMaxSize( 0 ).x;	
	m_ptMaxSize = m_StringList.CalMaxSize( 0 ) + CsPoint( 21, 27 );		
	m_pPopupWindow->ReSize( m_ptMaxSize );

	m_ptRenderPos = CsPoint( g_nScreenWidth - m_ptMaxSize.x - 20, g_nScreenHeight - m_ptMaxSize.y - 47 );

	// hp	
// 	if( m_pTarget->GetLeafRTTI() == RTTI_MONSTER )
// 	{	
// 		m_pHPBar = NiNew cProgressBar;
// 		m_pHPBar->Init( NULL, cProgressBar::BAR_1IMAGE, m_ptRenderPos + CsPoint( 1, m_StringList.CalMaxSize( 0 ).y + 26 ), CsPoint( m_nMaxSizeX + 19, 10 ), 
// 			CsPoint( 2, 2 ), "TargetWindow\\HpBar1.tga", false );		
// 		m_pHPBar->SetRenderPosText( cProgressBar::TEXT_NONE );
// 	}
// 	else if( m_pTarget->GetLeafRTTI() != RTTI_ITEM )
// 	{
// 		m_pHPBar = NiNew cProgressBar;
// 		m_pHPBar->Init( NULL, cProgressBar::BAR_1IMAGE, m_ptRenderPos + CsPoint( 1, m_StringList.CalMaxSize( 0 ).y + 26 ), CsPoint( m_nMaxSizeX + 19, 10 ), 
// 			CsPoint( 2, 2 ), "TargetWindow\\DsBar1.tga", false );		
// 	}

	m_bRenderTooltip = true;
}
