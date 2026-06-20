
#include "stdafx.h"
#include "NpcFind.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#define IF_NPCFIND_STR_DELTA_Y		12
#define IF_NPCFIND_STR_START_Y		50
#define IF_NPCFIND_MASKSIZE_X		170

#define IF_NPCFINE_ENABLE_DIST		11000.0f


void cNpcFind::Destroy()
{
	cBaseWindow::Delete();
}

void cNpcFind::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pSelectMask );
	SAFE_NIDELETE( m_pMouseOnMask );
	m_StringList.Delete();
}

void cNpcFind::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;
	InitScript( "NpcFind\\Win.tga", CsPoint( DEFAULT_SCREEN_WIDTH -226, 225 ), CsPoint( 224, 305 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	m_pCancelButton = AddButton( CsPoint( 203, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pFindButton = AddButton( CsPoint( 69, 268 ), CsPoint( 76, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "NPCFIND_BTN_SEARCH" ).c_str() );
	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 198, 40 ), CsPoint( 16, 224 ), CsPoint( 16, 15 ), CsRect( CsPoint( 12, 40 ), CsSIZE( 200, 224 ) ), 7 );

	m_pMouseOnMask = NiNew cSprite;
	m_pMouseOnMask->Init( NULL, CsPoint::ZERO, CsPoint( 170, 22 ), "Talk\\Common_Seletbar.tga", false );
	m_pMouseOnMask->SetAlpha( 0.5f );
	m_pSelectMask = NiNew cSprite;
	m_pSelectMask->Init( NULL, CsPoint::ZERO, CsPoint( 170, 22 ), "Talk\\Common_Seletbar.tga", false );

	AddTitle( UISTRING_TEXT( "NPCFIND_TITLE_FIND_NPC" ).c_str() );
	m_pFindButton->SetEnable( false );



	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	cString* pString = NULL;
	
	int nIndex = 0;
	int nRenderIcon;
	bool bRenderOtherInfo = ( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() <= 1 );
	NiPoint2 vTamerPos = g_pCharMng->GetTamerUser()->GetPos2D();

	CNpcMng::NpcCont* pVP = g_pNpcMng->GetNpcVector();
	CNpcMng::NpcCont::iterator it = pVP->begin();	
	for( ; it != pVP->end(); ++it )
	{
		CNpc* pNpc = it->second;
		SAFE_POINTER_CON( pNpc );

		if( ( pNpc->GetFTNpc()->GetInfo()->s_nDisplayPlag & CsNpc::DP_INFOMATION ) == 0 )
			continue;

		if( pNpc->IsEnableUse() == false )
			continue;

		pString = NiNew cString;

		sCHAR_IMAGE* pCharImage = pNpc->GetCharImage();
		if( pCharImage )
		{
			pString->AddSprite( pCharImage->s_pLargeImg, CsPoint( 0, -12 ), CsPoint( 32, 32 ) );
		}
		else
		{
#ifndef _DEBUG
			assert_csm1( false, _T( "Npc이미지 없어여 - %s" ), pNpc->GetName() );
#endif
			ti.SetText( _T( "" ) );
			pString->AddText( &ti )->s_ptSize.x = 32;			
		}


		ti.SetText( pNpc->GetName() );
		pString->AddText( &ti )->s_ptSize.x = 110;

		nRenderIcon = -1;
		if( bRenderOtherInfo )
		{
			switch( pNpc->GetQuestViewType() )
			{
			case CNpc::QV_NONE:				nRenderIcon = -1;			break;
			case CNpc::QV_NPCTALK:			nRenderIcon = 2;			break;
			default:						assert_cs( false );
			}
			if( ( nRenderIcon == -1 )&&( pNpc->GetQuestOwner() ) )
			{
				switch( pNpc->GetQuestOwner()->GetDisplayRev() )
				{
				case cData_QuestOwner::sNpcOwner::REQUITE:
					nRenderIcon = 1;
					break;
				case cData_QuestOwner::sNpcOwner::ENABLE_REV_MAIN:
					nRenderIcon = 10;
					break;
				case cData_QuestOwner::sNpcOwner::ENABLE_REV_SUB:
					nRenderIcon = 0;
					break;
				case cData_QuestOwner::sNpcOwner::ENABLE_REV_REPEAT:
					nRenderIcon = 11;
					break;
				case cData_QuestOwner::sNpcOwner::ENABLE_REV_EVENTREPEAT:
					nRenderIcon = 12;
					break;
				}
			}
		}
		if( nRenderIcon == -1 )
		{
			switch( pNpc->GetFTNpc()->GetInfo()->s_eType )
			{
			case nsCsNpcTable::NT_NONE:										
			case nsCsNpcTable::NT_EVENT_STORE:								
			case nsCsNpcTable::NT_DATS_PORTAL:	
			case nsCsNpcTable::NT_TRADE:
			case nsCsNpcTable::NT_MASTERS_MATCHING:
			case nsCsNpcTable::NT_SPIRIT_EVO:
			case nsCsNpcTable::NT_SPECIAL_EVENT:
				break;
			case nsCsNpcTable::NT_PROPERTY_STORE:							break;
			case nsCsNpcTable::NT_DIGITAMA_TRADE:	nRenderIcon = 3;		break;
			case nsCsNpcTable::NT_PORTAL:			nRenderIcon = 6;		break;
			case nsCsNpcTable::NT_MAKE_TACTICS:		nRenderIcon = 4;		break;
			case nsCsNpcTable::NT_ELEMENT_ITEM:		nRenderIcon = 5;		break;
			case nsCsNpcTable::NT_WAREHOUSE:		nRenderIcon = 7;		break;
			case nsCsNpcTable::NT_TACTICSHOUSE:		nRenderIcon = 13;		break;
			case nsCsNpcTable::NT_GUILD:			nRenderIcon = 14;		break;	
			case nsCsNpcTable::NT_DIGICORE:			nRenderIcon = 3;		break;
			case nsCsNpcTable::NT_CAPSULE_MACHINE:	
			case nsCsNpcTable::NT_GOTCHA_MACHINE:
			case nsCsNpcTable::NT_MYSTERY_MACHINE:			nRenderIcon = 15;		break;
			case nsCsNpcTable::NT_SKILL:					nRenderIcon = 16;		break;
			case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:		nRenderIcon = ICONMINIMAP::ITEM_PRODUCTION_NPC;		break;
			case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:	nRenderIcon = ICONMINIMAP::ITEM_PRODUCTION_NPC;		break;
			default:								assert_cs( false );
			}
		}		
		if( nRenderIcon != -1 )
		{
			pString->AddIcon( CsPoint( 22, 14 ), ICONITEM::MiniMap, nRenderIcon, 0, CsPoint( 0, 0 ) );
		}

		pString->SetValue1( pNpc->GetFTID() );
		pString->SetValue2( nIndex );

		if( ( vTamerPos - pNpc->GetPos2D() ).Length() < IF_NPCFINE_ENABLE_DIST )
		{
			pString->SetValue3( 1 );
		}
		else
		{
			assert_cs( pString->GetElement( 1 )->GetType() == cString::sELEMENT::TEXT );
			( (cString::sTEXT*)pString->GetElement( 1 ) )->s_Text.SetColor( NiColor( 0.5f, 0.5f, 0.5f ) );
#ifndef _DEBUG
			pString->SetValue3( 0 );
#else
			pString->SetValue3( 1 );
#endif
		}

		m_StringList.AddTail( pString );

		++nIndex;
	}

	m_pScrollBar->SetRange( CsPoint( 0, m_StringList.GetSize() + 2 ) );
	m_StringList.SetBuddy( m_pScrollBar );

	m_bRenderSelectMask = m_bRenderMouseOnMask = false;

	bClickDn = false;
}

void cNpcFind::Update(float const& fDeltaTime)
{
	if( g_pCharMng->GetDigimonUser( 0 )->IsBattle() == true )
	{
		Close( false );
		return;
	}
	// Npc와의 거리체크
	std::list< cString* >* pList = m_StringList.GetList();
	std::list< cString* >::iterator it = pList->begin();
	std::list< cString* >::iterator itEnd = pList->end();
	CNpc* pNpc;
	cString* pString;
	NiPoint2 vTamerPos = g_pCharMng->GetTamerUser()->GetPos2D();
	for( ; it!=itEnd; ++it )
	{
		pString = (*it);
		if (g_pNpcMng->IsNpc(pString->GetValue1()) != true)
		{
			Close( false );
			return;
		}
		pNpc = g_pNpcMng->GetNpc( pString->GetValue1() );
		assert_cs( pString->GetElement( 1 )->GetType() == cString::sELEMENT::TEXT );

		if( ( vTamerPos - pNpc->GetPos2D() ).Length() < IF_NPCFINE_ENABLE_DIST )
		{
			( (cString::sTEXT*)pString->GetElement( 1 ) )->s_Text.SetColor( FONT_WHITE );
			pString->SetValue3( 1 );
		}
		else
		{
			( (cString::sTEXT*)pString->GetElement( 1 ) )->s_Text.SetColor( NiColor( 0.5f, 0.5f, 0.5f ) );
#ifndef _DEBUG
			pString->SetValue3( 0 );
#else
			pString->SetValue3( 1 );
#endif
		}
	}

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cNpcFind::Update_ForMouse()
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
	default:
		return muReturn;
	}

	switch( m_pFindButton->Update_ForMouse() )
	{
	case cButton::ACTION_DOWN:
		return muReturn;
	case cButton::ACTION_CLICK:
		{
			cString* pSelString = m_StringList.GetString( m_nSelectMaskIndex );
			cPrintMsg::PrintMsg( 30026, nsCsFileTable::g_pNpcMng->GetNpc( pSelString->GetValue1() )->GetInfo()->s_szName );
			cMessageBox::GetFirstMessageBox()->SetValue1( pSelString->GetValue1() );
		}		
		return muReturn;
	}

	int nActive1;
	CsPoint ptMouseOnMask = CsPoint( 15, IF_NPCFIND_STR_START_Y ) + GetRootClient();
	switch( m_StringList.Update_ForMouse( nActive1, m_nMouseOnMaskIndex, ptMouseOnMask, IF_NPCFIND_STR_DELTA_Y, CsPoint( IF_NPCFIND_MASKSIZE_X+5, 29 ) ) )
	{
	case CURSOR::BUTTON_NONE:
		if( m_StringList.GetString( m_nMouseOnMaskIndex )->GetValue3() == 1 )
			m_bRenderMouseOnMask = true;
		break;

	case CURSOR::LBUTTON_DOWN:
		{
			bClickDn = true;
		}
		break;

	case CURSOR::LBUTTON_UP:
		{
			if(!bClickDn)
			{
				return MUT_NONE;
			}

			bClickDn = false;

			// 가까이에 있을때만
			if( m_StringList.GetString( m_nMouseOnMaskIndex )->GetValue3() == 1 )
			{
				if( m_nSelectMaskIndex != m_nMouseOnMaskIndex )
				{
					m_nSelectMaskIndex = m_nMouseOnMaskIndex;
					m_bRenderSelectMask = true;
					m_pFindButton->SetEnable( true );
				}
				else
				{
					cPrintMsg::PrintMsg( 30026, nsCsFileTable::g_pNpcMng->GetNpc( nActive1 )->GetInfo()->s_szName );				
					cMessageBox::GetFirstMessageBox()->SetValue1( nActive1 );
				}
			}
		}				
		break;
	}

	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cNpcFind::Render()
{
	RenderScript();

	if( m_bRenderMouseOnMask )
	{
		int nPosY = m_StringList.GetStringPosY( IF_NPCFIND_STR_START_Y -4 , m_nMouseOnMaskIndex, IF_NPCFIND_STR_DELTA_Y );
		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( 15, nPosY ) + GetRootClient();
			m_pMouseOnMask->Render( pos );
			m_bRenderMouseOnMask = false;
		}		
	}
	if( m_bRenderSelectMask )
	{
		int nPosY = m_StringList.GetStringPosY( IF_NPCFIND_STR_START_Y -4 , m_nSelectMaskIndex, IF_NPCFIND_STR_DELTA_Y );
		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( 20, nPosY ) + GetRootClient();
			m_pSelectMask->Render( pos );
		}
	}
	m_StringList.Render( CsPoint( 20, IF_NPCFIND_STR_START_Y ) + GetRootClient(), IF_NPCFIND_STR_DELTA_Y );
}

void cNpcFind::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	m_pSelectMask->ResetDevice();
	m_pMouseOnMask->ResetDevice();
	m_StringList.ResetDevice();
}

void cNpcFind::ResetDevice()
{	
	ResetDeviceScript();

	m_pSelectMask->ResetDevice();
	m_pMouseOnMask->ResetDevice();
	m_StringList.ResetDevice();
}
