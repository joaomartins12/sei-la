

#include "stdafx.h"
#include "Popup.h"
#include "common_vs2019/pParty.h"

void cPopUpWindow::_Make_PartyMenu()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "PARTY_MENU_POPUP_TITLE" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( PARTY_MENU_NONE );
	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	_AddLine();

	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_WHITE;
	switch( m_nSubID1 )
	{
	case PARTY_NR_MY:
		{
			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "PARTY_LEAVE" ).c_str() );
			pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 5 );
			pString->TailAddSizeX( 3 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_LEAVE );
			m_StringList.AddTail( pString );

			_AddLine();

			ti.s_eFontSize = POPUP_MENU_SIZE;
			ti.s_Color = POPUP_MENU_COLOR;			

			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_TITLE" ).c_str() );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_NONE );
			pString->AddSizeY( 5 );
			m_StringList.AddTail( pString );

			sLootingRule lootingRule;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_PARTY_LOOTING_INFO, &lootingRule );

 			_AddLine();

			ti.s_eFontSize = CFont::FS_10;
			ti.s_Color = FONT_WHITE;

			pString = NiNew cString;
			
			int nRuleIconIdx = 14;
			switch( lootingRule.m_nCropType )
			{
			case nCrop::Base:	
				{
					ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_BASE" ).c_str() );	
					nRuleIconIdx = 14;
				}
				break;
			case nCrop::Order:	
				{
					ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ORDER" ).c_str() );	
					nRuleIconIdx = 16;
				}
				break;	
			case nCrop::Free:
				{					
					if( lootingRule.m_nRareRate == 0 )
					{
						ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_FREE" ).c_str() );		
						nRuleIconIdx = 17;
					}
					else
					{
						ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_RANDOM" ).c_str() );		
						nRuleIconIdx = 15;
					}
				}
				break;
			default:
				assert_cs( false );
				break;
			}			

			pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, nRuleIconIdx );
			pString->TailAddSizeX( 3 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_NONE );
			m_StringList.AddTail( pString );			

			pString = NiNew cString;
			switch( lootingRule.m_nDispRareGrade )
			{					
			case nCrop::RareRate::Lv1:
				ti.s_Color = g_pDataMng->GetItemColor( 2 );
				ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_ALL" ).c_str() );								
				break;
			case nCrop::RareRate::Lv2:
				ti.s_Color = g_pDataMng->GetItemColor( 3 );
				ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_MAGIC" ).c_str() );								
				break;
			case nCrop::RareRate::Lv3:
				ti.s_Color = g_pDataMng->GetItemColor( 4 );
				ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_REARE" ).c_str() );								
				break;
			case nCrop::RareRate::Lv4:
				ti.s_Color = g_pDataMng->GetItemColor( 5 );
				ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_EPIC" ).c_str() );								
				break;
			default:
				assert_cs( false );
				break;
			}

			pString->HeadAddSizeX( 13 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_NONE );
			m_StringList.AddTail( pString );
		}
		break;
	case PARTY_NR_OTHER:
		{
			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "PARTY_MEMBER_WHISPER" ).c_str() );
			pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 0 );
			pString->TailAddSizeX( 3 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_WHISPER );
			m_StringList.AddTail( pString );
		}
		break;
	case PARTY_HS_MY:
		{
			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "PARTY_LEAVE" ).c_str() );
			pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 5 );
			pString->TailAddSizeX( 3 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_LEAVE );
			m_StringList.AddTail( pString );

			_AddLine();

			ti.s_eFontSize = POPUP_MENU_SIZE;
			ti.s_Color = POPUP_MENU_COLOR;

			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_TITLE" ).c_str() );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_NONE );
			pString->AddSizeY( 5 );
			m_StringList.AddTail( pString );

			sLootingRule lootingRule;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_PARTY_LOOTING_INFO, &lootingRule );

			_AddLine();

			ti.s_eFontSize = CFont::FS_10;
			ti.s_Color = FONT_WHITE;

			pString = NiNew cString;

			int nRuleIconIdx = 14;
			switch( lootingRule.m_nCropType )
			{
			case nCrop::Base:	
				{
					ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_BASE" ).c_str() );	
					nRuleIconIdx = 14;
				}
				break;
			case nCrop::Order:	
				{
					ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ORDER" ).c_str() );	
					nRuleIconIdx = 16;
				}
				break;	
			case nCrop::Free:
				{					
					if( lootingRule.m_nRareRate == 0 )
					{
						ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_FREE" ).c_str() );		
						nRuleIconIdx = 17;
					}
					else
					{
						ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_RANDOM" ).c_str() );		
						nRuleIconIdx = 15;
					}
				}
				break;
			default:
				assert_cs( false );
				break;
			}			

			pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, nRuleIconIdx );
			pString->TailAddSizeX( 3 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_ITEM_LOOT );
			m_StringList.AddTail( pString );			

			pString = NiNew cString;
			switch( lootingRule.m_nDispRareGrade )
			{						
			case nCrop::RareRate::Lv1:				
				ti.s_Color = g_pDataMng->GetItemColor( 2 );
				ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_ALL" ).c_str() );								
				break;
			case nCrop::RareRate::Lv2:
				ti.s_Color = g_pDataMng->GetItemColor( 3 );
				ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_MAGIC" ).c_str() );								
				break;
			case nCrop::RareRate::Lv3:
				ti.s_Color = g_pDataMng->GetItemColor( 4 );
				ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_REARE" ).c_str() );								
				break;
			case nCrop::RareRate::Lv4:
				ti.s_Color = g_pDataMng->GetItemColor( 5 );
				ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_EPIC" ).c_str() );								
				break;
			default:
				assert_cs( false );
				break;
			}

			pString->HeadAddSizeX( 13 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_ITEM_RATE );
			m_StringList.AddTail( pString );
			
		}
		break;
	case PARTY_HS_OTHER:
		{
			pString = NiNew cString;
			ti.SetText(UISTRING_TEXT( "PARTY_MEMBER_BENISH" ).c_str() );
			pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 5 );
			pString->TailAddSizeX( 3 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_KICK );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "PARTY_MASTER_APPOINT" ).c_str() );
			pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 6 );
			pString->TailAddSizeX( 3 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_SETHOST );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			ti.SetText( UISTRING_TEXT( "PARTY_MEMBER_WHISPER" ).c_str() );
			pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 0 );
			pString->TailAddSizeX( 3 );
			pString->AddText( &ti );
			pString->SetValue1( PARTY_MENU_WHISPER );
			m_StringList.AddTail( pString );
		}
		break;
	}
}

bool cPopUpWindow::_Update_ForMouse_PartyMenu()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			switch( nActive1 )
			{
			case PARTY_MENU_LEAVE:
				{
					GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_LEAVE, NULL );
					// 자신거 안들어옴
					ClosePopup();
// 					if( net::game->SendPartyLeave() == true )
// 						g_pDataMng->GetParty()->DelParty( g_pDataMng->GetParty()->GetMySlotIndex(), cData_Party::LT_LEAVE );
				}
				break;
			case PARTY_MENU_KICK:
				{
					// 자신거 들어옴

					GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_MEMBER_BANISH, &m_nSubID2 );
					ClosePopup();

// 					cData_Party::sINFO* pPartyInfo = g_pDataMng->GetParty()->GetParty( m_nSubID2 );
// 					net::game->SendPartyKick( pPartyInfo->s_TamerStat.s_szName );
				}
				break;
			case PARTY_MENU_WHISPER:
				{
					std::pair<int,std::wstring> data;
					data.first = m_nSubID2;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_PARTY_MEMBER_NAME, &data );
					
					if( !data.second.empty() )
					{
						ST_CHAT_PROTOCOL	CProtocol;
						CProtocol.m_Type = NS_CHAT::WHISPER_REPLY;
						CProtocol.m_wStr = data.second;
						GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
					}

					ClosePopup();


				}
				break;
			case PARTY_MENU_SETHOST:
				{
					// 자신거 들어옴

					GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_CHANGE_MASTER, &m_nSubID2 );
					ClosePopup();

// 					cData_Party::sINFO* pPartyInfo = g_pDataMng->GetParty()->GetParty( m_nSubID2 );
// 					net::game->SendPartyChangeMaster( pPartyInfo->s_nSlotIndex );
				}
				break;
			case PARTY_MENU_NONE:
				break;
			case PARTY_MENU_ITEM_LOOT:
				{
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_PARTY_LOOTING_WINDOW );
					ClosePopup();			
// 					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIVISION ) == false )					
// 					{
// 						g_pGameIF->GetDynamicIF( cBaseWindow::WT_DIVISION );
// 					}					
				}
				break;			
			case PARTY_MENU_ITEM_RATE:
				{
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_PARTY_ITEMRATE_WINDOW );
					ClosePopup();					

// 					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIVISION ) == false )					
// 					{
// 						g_pGameIF->GetDynamicIF( cBaseWindow::WT_DIVISION, 0, 1 );
// 					}					
				}
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	}

	switch( nActive1 )
	{
	case PARTY_MENU_LEAVE:
	case PARTY_MENU_WHISPER:
	case PARTY_MENU_KICK:
	case PARTY_MENU_SETHOST:
	case PARTY_MENU_ITEM_LOOT:	
	case PARTY_MENU_ITEM_RATE:
		m_bRenderSelMask = true;
		break;
	}

	return true;
}