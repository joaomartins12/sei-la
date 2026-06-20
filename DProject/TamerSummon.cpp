

#include "stdafx.h"
#include "TamerSummon.h"

cTamerSummon*		g_pTamerSummon = NULL;

void cTamerSummon::Destroy()
{
	cBaseWindow::Delete();
}

void cTamerSummon::DeleteResource()
{
	m_pEditName->ReleaseFocus();	
	DeleteScript();	

	SAFE_NIDELETE( m_pEditName );

	g_IME.SetLockConidateList( false );

	if( m_eTargetWindowType != WT_MAKE_TACTICS && m_eType != eTypeOk )
	{
		g_pDataMng->ItemUnlock( m_nValue1 );
	}
}

void cTamerSummon::Create( int nValue )
{
	g_IME.SetLockConidateList( true );

	if( cBaseWindow::Init() == false )
		return;

	InitScript( "CreateName\\Tactics_Win.tga", CsPoint( (g_nScreenWidth-224)/2, (g_nScreenHeight-127)/2 ), CsPoint( 224, 127 ), false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );

	m_pBtnOk = AddButton( CsPoint( 70 ,83 ), CsPoint( 84, 33 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
	//m_pBtnOk->SetEnable( false );

	m_pBtnCancel = AddButton( CsPoint( 203, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	m_eTargetWindowType = cBaseWindow::WT_TAMER_SUMMON;
	m_nTargetWTValue1 = 0;

	switch( m_eTargetWindowType )
	{
	case cBaseWindow::WT_TAMER_SUMMON:
		m_nMaxLengthByte = Language::pLength::name;
		m_nMaxLength = NAME_MAX_LEN;
		AddTitle( UISTRING_TEXT( "TAMERSUMMON_TITLE_WRITE_TAMER" ).c_str(), CsPoint( -3, 4 ), CFont::FS_9 );
		break;	
	default:
		assert_cs( false );
		break;
	}

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	m_pEditName = NiNew cEditBox;
	m_pEditName->Init( m_pRoot, CsPoint( 13, 46 ), CsPoint( 150, 21 ), &ti, false );	
	m_pEditName->SetFontLength( m_nMaxLength );
	m_pEditName->EnableUnderline();	

	if( g_pFocusEdit != NULL )
		g_pFocusEdit->ReleaseFocus();
	m_pEditName->SetFocus();

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cTamerSummon::Update(float const& fDeltaTime)
{
	if( IsShowWindow() == false )
		return;

	if( m_pEditName->IsFocus() == false )
	{
		OnCancel();
		return;
	}

	//m_pBtnOk->SetEnable( _IsEnableName() );
}

bool cTamerSummon::_CheckTamerName( TCHAR* szName )
{
	// 내이름과 중복 검사
	if( _tcsicmp( szName, g_pCharMng->GetTamerUser()->GetName() ) == 0 )
	{
		cPrintMsg::PrintMsg( 30547 );
		return false;
	}

	// 사용해도 되는이름인지 검사	
	if( nsCsFileTable::g_pCuidMng->CheckID( szName ) == false )
	{
		cPrintMsg::PrintMsg( 10029 );
		return false;
	}
	return true;
}

void cTamerSummon::OnOk()
{
	if( m_pBtnOk->IsEnable() == false )
		return;

	TCHAR szName[ Language::pLength::name ];
	g_IME.GetInputComp( szName );

	switch( m_eTargetWindowType )
	{
	case cBaseWindow::WT_TAMER_SUMMON:
		{			
			if( g_pGameIF->IsOpenNpcWindow() || g_pCharMng->GetDigimonUser( 0 )->IsBattle() || g_pDataMng->GetServerSync()->IsChageDigimon() )
			{
				cPrintMsg::PrintMsg( 30024 );
				return;
			}
			// 메세지 박스 떠있는게 있다면 거래 실패
			if( cMessageBox::IsMessageBox() )
			{
				cPrintMsg::PrintMsg( 30024 );
				return;
			}

			std::wstring tamerName(szName);
			switch( Language::CheckTamerName( tamerName ) )
			{
			case Language::STRING_EMPTY:
			case Language::STRING_SIZE_MIN:
			case Language::STRING_SIZE_MAX:		cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_NAME_LEN_ERROR ); return;
			case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_NAME_ERROR_TYPE2 ); return;
			}

			if( _CheckTamerName( szName ) == false )
			{
				g_IME.ReleaseText();
				return;
			}

			g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
			g_pCharMng->GetTamerUser()->DeletePath();
			g_pCharMng->GetDigimonUser( 0 )->DeletePath();

			cSkill::sINFO* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkill( g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkill_Inex() );
			if( pSkill == NULL )
			{
				cPrintMsg::PrintMsg( 30586 );
				OnCancel();
				break;
			}

			CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill->s_pFTSkill->GetInfo()->s_dwID );
			if( net::game )
				net::game->SendTamerSummonSkill( szName, pTamerSkill->Get_TSkill_Info()->s_nIndex, 1 );	// 소환할 테이머 상태 확인
		}		
		break;
	default:
		assert_cs( false );
	}	
}

void cTamerSummon::OnCancel()
{	
	Close();
}

cBaseWindow::eMU_TYPE
cTamerSummon::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	m_eType = eTypeNone;

	if( m_pBtnOk->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		m_eType = eTypeOk;
		OnOk();
		return muReturn;
	}

	if( m_pBtnCancel->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		m_eType = eTypeCancel;
		OnCancel();
		return muReturn;
	}

	return muReturn;
}

void cTamerSummon::Render()
{
	if( IsShowWindow() == false )
		return;

	RenderScript();
	m_pEditName->Render();
}

void cTamerSummon::ResetDevice()
{
	if( IsShowWindow() == false )
		return;

	ResetDeviceScript();
	m_pEditName->ResetDevice();
}

