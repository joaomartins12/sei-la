

#include "stdafx.h"
#include "CreateName.h"

cCreateName*		g_pCreateName = NULL;

cCreateName* cCreateName::NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, CsPoint pos )
{
	if( g_pGameIF->IsActiveWindow( wt, nWTValue1 ) == true )
	{
		if( g_pGameIF->_GetPointer( wt, nWTValue1 )->IsEnableWindow() == false )
			return NULL;
	}

	DeleteInstance();

	assert_cs( g_pCreateName == NULL );
	g_pCreateName = NiNew cCreateName;
	g_pCreateName->Init( wt, nWTValue1, pos );	

#ifdef IME_WINDOW_POSITION
	g_bIMEPosition = true;
#endif

	return g_pCreateName;
}

bool cCreateName::UpdateInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pCreateName )
	{
		if( ( g_pCreateName->GetTargetWindowType() == wt )&&( g_pCreateName->GetTargetWindowWTValue() == nWTValue1 ) )
		{
			if( g_pCreateName )
			{
				g_pCreateName->Update(g_fDeltaTime);
				if( g_pCreateName )
				{
					if( g_pCreateName->IsShowWindow() == false )
					{		
						DeleteInstance();
						return false;
					}		
				}
				return true;
			}
		}		
	}
	return false;
}

cBaseWindow::eMU_TYPE
cCreateName::Update_ForMouseInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	cBaseWindow::eMU_TYPE muReturn = MUT_OUT_WINDOW;

	if( g_pCreateName )
	{
		if( ( g_pCreateName->GetTargetWindowType() == wt )&&( g_pCreateName->GetTargetWindowWTValue() == nWTValue1 ) )
		{
			muReturn = g_pCreateName->Update_ForMouse();
		}

		if( g_pCreateName )
		{
			if( g_pCreateName->IsShowWindow() == false )
			{		
				DeleteInstance();
			}
		}
		else
		{
			return MUT_OUT_WINDOW;
		}
	}
	return muReturn;
}

void cCreateName::RenderInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pCreateName )
	{
		if( ( g_pCreateName->GetTargetWindowType() == wt )&&( g_pCreateName->GetTargetWindowWTValue() == nWTValue1 ) )
		{
			g_pEngine->ScreenSpace();
			g_pCreateName->Render();
		}		
	}
}

bool cCreateName::DeleteInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pCreateName )
	{
		if( ( g_pCreateName->GetTargetWindowType() == wt )&&( g_pCreateName->GetTargetWindowWTValue() == nWTValue1 ) )
		{
			SAFE_NIDELETE( g_pCreateName );
			return true;
		}		
	}
	return false;
}

bool cCreateName::DeleteInstance()
{
	if( g_pCreateName )
	{
		SAFE_NIDELETE( g_pCreateName );
		return true;
	}
	return false;
}

void cCreateName::Destroy()
{
	cBaseWindow::Delete();
}

void cCreateName::DeleteResource()
{
	m_pEditName->ReleaseFocus();	
	DeleteScript();	

	SAFE_NIDELETE( m_pEditName );

	g_IME.SetLockConidateList( false );

#ifdef IME_WINDOW_POSITION
	g_bIMEPosition = false;
#endif

	if( m_eTargetWindowType != WT_MAKE_TACTICS && m_eType != eTypeOk )
	{
		g_pDataMng->ItemUnlock( m_nValue1 );
	}
}

void cCreateName::Init( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, CsPoint pos )
{
	g_IME.SetLockConidateList( true );

	if( cBaseWindow::Init() == false )
		return;

	CsPoint SepPos = pos + CsPoint( 0, -127 );
	if( pos != CsPoint( -1, -1 ) )
	{
		if( SepPos.y < 0 )
		{
			SepPos = pos + CsPoint( 0, 33 );
		}
		if( SepPos.x + 174 > g_nScreenWidth )
		{
			SepPos.x -= 174;
		}
	}
	else
	{
		SepPos.x = (g_nScreenWidth-174)/2;
		SepPos.y = (g_nScreenHeight-127)/2;
	}
	
	InitScript( "CreateName\\Tactics_Win.tga", SepPos, CsPoint( 174, 127 ), false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );

	m_pBtnOk = AddButton( CsPoint( 45 ,83 ), CsPoint( 84, 33 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
	m_pBtnOk->SetEnable( true );
	m_pBtnCancel = AddButton( CsPoint( 153, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	CFont::eFACE_SIZE eStrFontSize = CFont::FS_9;
	CsPoint ptStrDeltaY( -3, 4 );

	m_eTargetWindowType = wt;
	m_nTargetWTValue1 = nWTValue1;

	m_bValue2 = false;	// true이면 우클릭을 이용한 알부화, false이면 통상적인 알부화

	switch( m_eTargetWindowType )
	{
#ifdef CROSSWARS_SYSTEM
	case cBaseWindow::WT_MAKE_CROSSTACTICS:
#endif
	case cBaseWindow::WT_MAKE_TACTICS:
		m_nMaxLengthByte = Language::pLength::name;
		m_nMaxLength = NAME_MAX_LEN;
		AddTitle( UISTRING_TEXT( "CREATENAME_TITLE_TYPE_YOUR_NAME" ).c_str(), ptStrDeltaY, eStrFontSize );
		break;
	case cBaseWindow::WT_CHANGE_T_NAME:
		m_nMaxLengthByte = Language::pLength::name;
		m_nMaxLength = NAME_MAX_LEN;
		AddTitle( UISTRING_TEXT( "CREATENAME_TITLE_TYPE_YOUR_TAMER_NAME" ).c_str(), ptStrDeltaY, eStrFontSize );
		break;	
	case cBaseWindow::WT_CHANGE_D_NAME:
		m_nMaxLengthByte = Language::pLength::name;
		m_nMaxLength = NAME_MAX_LEN;
		AddTitle( UISTRING_TEXT( "CREATENAME_TITLE_TYPE_YOUR_DIGIMON_NAME" ).c_str(), ptStrDeltaY, eStrFontSize );
		break;
	case cBaseWindow::WT_CREATE_GUILD_NAME:
		m_nMaxLengthByte = Language::pLength::GuildName - 3;
		m_nMaxLength = NAME_GUILD_MAX_LEN;
		AddTitle( UISTRING_TEXT( "CREATENAME_TITLE_TYPE_YOUR_GUILD_NAME" ).c_str(), ptStrDeltaY, eStrFontSize ); 		
		break;
	case cBaseWindow::WT_INVITE_GUILD_NAME:
		m_nMaxLengthByte = Language::pLength::name;
		m_nMaxLength = NAME_MAX_LEN;
		AddTitle( UISTRING_TEXT( "CREATENAME_TITLE_TYPE_YOUR_NAME" ).c_str(), ptStrDeltaY, eStrFontSize ); 		
		break;
	case cBaseWindow::WT_FRIEND_MEMO:
		m_nMaxLengthByte = 64;
		m_nMaxLength = NAME_FRIEND_COMMENT_LEN;
		AddTitle( UISTRING_TEXT( "COMMON_TXT_MEMO" ).c_str() ); 		
		break;
	case cBaseWindow::WT_SHUTOUT_MEMO:
		m_nMaxLengthByte = 64;
		m_nMaxLength = NAME_BLOCK_COMMENT_LEN;
		AddTitle( UISTRING_TEXT( "COMMON_TXT_MEMO" ).c_str() ); 		
		break;
	case cBaseWindow::WT_GUILDRANK_NAME:
		m_nMaxLengthByte = Language::pLength::GuildClassName;
		m_nMaxLength = NAME_MAX_LEN;
		AddTitle( UISTRING_TEXT( "CREATENAME_TITLE_TITLE_CHANGE" ).c_str(), ptStrDeltaY - CsPoint( 3, 0 ), eStrFontSize ); 		
		break;
	case cBaseWindow::WT_GUILDRANK_EXPLAIN:
		m_nMaxLengthByte = 64;
		m_nMaxLength = NAME_GUILDRANK_COMMENT_LEN;
		AddTitle( UISTRING_TEXT( "CREATENAME_TITLE_DUTY_CHANGE" ).c_str(), ptStrDeltaY - CsPoint( 3, 0 ), eStrFontSize ); 		
		break;
	case cBaseWindow::WT_RECOMMEND_NAME:
		m_nMaxLengthByte =  Language::pLength::name;
		m_nMaxLength = NAME_MAX_LEN;
		AddTitle( UISTRING_TEXT( "CREATENAME_TITLE_RECOMMEND_FRIEND" ).c_str(), ptStrDeltaY - CsPoint( 3, 0 ), eStrFontSize );
		break;

	case cBaseWindow::WT_SPIRIT_EVOLUTION:
		m_nMaxLengthByte = Language::pLength::name;
		m_nMaxLength = NAME_MAX_LEN;
		AddTitle( UISTRING_TEXT( "CREATENAME_TITLE_TYPE_YOUR_DIGIMON_NAME" ).c_str(), ptStrDeltaY, eStrFontSize );
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

void cCreateName::Update(float const& fDeltaTime)
{
	if( IsShowWindow() == false )
		return;

	if( m_pEditName->IsFocus() == false )
	{
		OnCancel();
		return;
	}

// #ifdef NOT_ENTER_CREATENAME	// 문자 조합이 완료 된 상태여야 한다.
// 	m_pBtnOk->SetEnable( _IsEnableName()&&( g_IME.GetCombination() == false ) );
// #else
// 	m_pBtnOk->SetEnable( _IsEnableName() );
// #endif
}

bool cCreateName::_CheckTamerName( TCHAR* szName )
{
	// 내이름과 중복 검사
	if( _tcsicmp( szName, g_pCharMng->GetTamerUser()->GetName() ) == 0 )
	{
		cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_TAMER_NAME_EXIST );
		return false;
	}

	// 사용해도 되는이름인지 검사	
	if( nsCsFileTable::g_pCuidMng->CheckID( szName ) == false )
	{
		cPrintMsg::PrintMsg( 10029 );
		return false;
	}
#ifdef VERSION_USA //이름길이 체크 
	if( _tcslen( szName ) < NAME_MIN_LEN )
		return false;
#endif

	switch( Language::CheckTamerName( std::wstring(szName) ) )
	{
	case Language::STRING_EMPTY:
	case Language::STRING_SIZE_MIN:
	case Language::STRING_SIZE_MAX:	cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_NAME_LEN_ERROR ); return false;
	case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10029 ); return false;
	case Language::SUCCEEDED:
		break;
	default:
		return false;
	}

	return true;
}

bool cCreateName::_CheckDigimonName( TCHAR* szName, bool bCheckCurDigimon )
{
	if( bCheckCurDigimon == true )
	{
		if( _tcsicmp( g_pCharMng->GetDigimonUser( 0 )->GetName(), szName ) == 0 )
		{
			cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_DIGIMON_NAME_EXIST );
			return false;
		}
	}

	// 등록된 이름중에 같은 이름이 있는지 검사
	for( int i=0; i<nLimit::Memory; ++i )
	{
		cData_PostLoad::sDATA* pData = g_pDataMng->GetTactics()->GetTactics( i );
		if( pData->s_Type.m_nType == 0 )
			continue;

		// 이미 사용중인 이름이라면
		if( _tcsicmp( pData->s_szName, szName ) == 0 )
		{
			cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_DIGIMON_NAME_EXIST );
			return false;
		}
	}	

	// 사용해도 되는이름인지 검사	
	if( nsCsFileTable::g_pCuidMng->CheckID( szName ) == false )
	{
		cPrintMsg::PrintMsg( 10029 );
		return false;
	}


	switch( Language::CheckDigimonName( std::wstring(szName) ) )
	{
	case Language::STRING_EMPTY:
	case Language::STRING_SIZE_MIN:
	case Language::STRING_SIZE_MAX:	cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_NAME_LEN_ERROR ); return false;
	case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10029 ); return false;
	case Language::SUCCEEDED:
		break;
	default:
		return false;
	}

	return true;
}

bool cCreateName::_CheckGuildName( TCHAR* szName )
{
	// 현재 길드 이름인지 체크
	if( _tcsicmp( szName, g_pCharMng->GetTamerUser()->GetCharOption()->GetGuildName() ) == 0 )
	{
		cPrintMsg::PrintMsg( 30121 );
		return false;
	}

	// 사용해도 되는이름인지 검사	
	if( nsCsFileTable::g_pCuidMng->CheckID( szName ) == false )
	{
		cPrintMsg::PrintMsg( 10029 );
		return false;
	}

	switch( Language::CheckGuildName( std::wstring(szName) ) )
	{
	case Language::STRING_EMPTY:
	case Language::STRING_SIZE_MIN:
	case Language::STRING_SIZE_MAX:	cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_NAME_LEN_ERROR ); return false;
	case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10029 ); return false;
	case Language::SUCCEEDED:
		break;
	default:
		return false;
	}

	return true;
}

bool cCreateName::_CheckInviteName( TCHAR* szName )
{
	// 본인 이름인지 체크
	if( _tcsicmp( szName, g_pCharMng->GetTamerUser()->GetName() ) == 0 )
	{
		//20170614 길드 초대할때 자기 자신을 초대하면 체크 후 아무런 메시지가 없어서 추가.
		cPrintMsg::PrintMsg( 11101 );
		return false;
	}

	// 사용해도 되는이름인지 검사	
	if( nsCsFileTable::g_pCuidMng->CheckID( szName ) == false )
	{
		cPrintMsg::PrintMsg( 10029 );
		return false;
	}

// 	if( Language::SUCCEEDED != Language::CheckTamerName( std::wstring(szName) ) )
// 	{
// 		cPrintMsg::PrintMsg( 11101 );
// 		return false;
// 	}

	return true;
}

void cCreateName::OnOk()
{
	if( m_pBtnOk->IsEnable() == false )
		return;
	
	TCHAR szName[ MAX_FILENAME ];
	g_IME.GetInputComp( szName );

#ifdef TAIWAN_NAME_LENGTH	// 중문 캐릭터 이름은 8자를 넘을 수 없다.
//#if ( defined VERSION_TW || defined VERSION_HK )
	// 디지몬 부화, 테이머 및 디지몬 이름 교체 시
	if( m_eTargetWindowType == cBaseWindow::WT_MAKE_TACTICS || m_eTargetWindowType == cBaseWindow::WT_CHANGE_T_NAME || m_eTargetWindowType == cBaseWindow::WT_CHANGE_D_NAME )
	{
		if( _tcslen( szName ) < NAME_MIN_LEN || _tcslen( szName ) > NAME_MAX_LEN )
		{
			cPrintMsg::PrintMsg( 10021 );
			return;
		}

//		if( !g_pCharacterCreate->Taiwan_StringCheck() )
//			return;
	}
#endif

	switch( m_eTargetWindowType )
	{
#ifdef CROSSWARS_SYSTEM
	case cBaseWindow::WT_MAKE_CROSSTACTICS:
		if( _CheckDigimonName( szName, true ) == false )
		{
		//	g_IME.ReleaseText();
			return;
		}

		//g_pGameIF->GetMakeTactics()->RequiteTactics( szName );

		//동기화
		if( net::game )
		{			
			net::game->SendCrossWarsHatch( m_nValue1, szName);
		}
		g_pDataMng->GetInvenCross()->GetData( TO_ID(m_nValue1) )->DecreaseCount( 1 );
		g_pDataMng->ItemUnlock( m_nValue1 );
	

		Close();
		break;
#endif
	case cBaseWindow::WT_MAKE_TACTICS:
		{
			if( _CheckDigimonName( szName, true ) == false )
			{
				//g_IME.ReleaseText();
				return;
			}

			assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) );
			assert_cs( g_pGameIF->GetMakeTactics()->IsShowWindow() );
			g_pGameIF->GetMakeTactics()->RequiteTactics( szName );

			Close();		

			if( g_pGameIF->GetMakeTactics()->getBackDiskBtnStatus() )
			{
				g_pDataMng->GetInven()->ItemCrop( g_pGameIF->GetMakeTactics()->_GetBackupDiskType(), 1 );
				g_pGameIF->GetMakeTactics()->_ResistBackupDiskDisableControl();
			}
		}
		break;
	case cBaseWindow::WT_CHANGE_T_NAME:
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

			if( _CheckTamerName( szName ) == false )
			{
				//g_IME.ReleaseText();
				return;
			}

			g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
			g_pCharMng->GetTamerUser()->DeletePath();
			g_pCharMng->GetDigimonUser( 0 )->DeletePath();

			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CASH_CHANGE_TNAME, 0, &m_nValue1 );

			if( net::game )
			{
				net::game->SendChangeTamerName( m_nValue1, szName );
				cPrintMsg::PrintMsg( 14010 );
			}
		}		
		break;
	case cBaseWindow::WT_CHANGE_D_NAME:
		{
			if( _CheckDigimonName( szName, true ) == false )
			{
				//g_IME.ReleaseText();
				return;
			}

			if( m_bValue2 == true )	// 우클릭을 이용한 알 부화
			{
				if( net::game )
					net::game->SendRClickDigimonHatch( g_pDataMng->GetInven()->GetData( m_nValue1 )->GetType(), m_nValue1, szName );

				//g_pDataMng->GetInven()->GetData( m_nValue1 )->DecreaseCount( 1 );

				//g_pTacticsAni->Start( g_pDataMng->GetInven()->GetData( m_nValue1 )->GetType(), g_pCharMng->GetTamerUser() );

				Close();
				break;
			}
			else	// 통상적인 알 부화
			{
				#ifdef CROSSWARS_SYSTEM
					CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
					if(pUser != NULL && nsCsFileTable::g_pDigimonMng != NULL)
					{
						if( nsCsFileTable::g_pDigimonMng->IsCrossCombine(pUser->GetBaseDigimonFTID()))
						{
							cPrintMsg::PrintMsg( 30455 );
							g_pDataMng->ItemUnlock( m_nValue1 );
							Close();
							return;
						}
						else
						{
							if( net::game )
								net::game->SendChangeDigimonName( m_nValue1, szName );
						}
					}
				#else
					if( net::game )
						net::game->SendChangeDigimonName( m_nValue1, szName );
				#endif
			}

			g_pDataMng->GetInven()->GetData( m_nValue1 )->DecreaseCount( 1 );
			g_pDataMng->ItemUnlock( m_nValue1 );

			cData_QuickSlot::CheckItemCount_AllUser();

			CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );

			// 포스트로드 변경
			_tcscpy_s( g_pDataMng->GetPostLoad()->GetDigimonData()->s_szName, Language::pLength::name+1, szName  );

			// 케릭터 이름
			pUser->GetCharOption()->SetName( szName );

			// 디지몬 타겟창
			g_pGameIF->GetMainDigimonWindow()->SetTargetObject( pUser );
			// 디지몬 스테이터스창
			GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL );

			pUser->SetAnimation( ANI::IDLE_CHAR );
			pUser->GetProp_Effect()->AddEffect( "system\\Skillup.nif", 1.0f, 0 );
			cWindow::PlaySound( cWindow::SD_Qu1 );

			Close();
		}
		break;
	case cBaseWindow::WT_CREATE_GUILD_NAME:
		{
			if( g_pCharMng->GetDigimonUser( 0 )->IsBattle() || g_pDataMng->GetServerSync()->IsChageDigimon() )
			{
				cPrintMsg::PrintMsg( 30024 );
				return;
			}
			// 메세지 박스 떠있는게 있다면 실패
			if( cMessageBox::IsMessageBox() )
			{
				cPrintMsg::PrintMsg( 30024 );
				return;
			}
#ifdef TAIWAN_NAME_LENGTH
			if( _tcslen(szName) > 7 )	// 중문 길드 이름은 7자를 넘을 수 없다.
			{
				cPrintMsg::PrintMsg(30202);
				//g_IME.ReleaseText();
				return;
			}
#endif

			if( _CheckGuildName( szName ) == false )
			{
				cPrintMsg::PrintMsg( 10024 );
			//	g_IME.ReleaseText();
				return;			
			}

			g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
			g_pCharMng->GetTamerUser()->DeletePath();
			g_pCharMng->GetDigimonUser( 0 )->DeletePath();

			cPrintMsg::PrintMsg( 30154, szName );	
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();						
			pMBox->SetCharValue1( szName );
			Close();
		}
		break;
	case cBaseWindow::WT_INVITE_GUILD_NAME:
		{
			if( _CheckInviteName( szName ) == false )
			{
				//g_IME.ReleaseText();
				return;			
			}			

			//길드원 초대	
			if( net::game )
			{
				std::wstring wsInviteName(szName);
				GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GUILD_INVITE, &wsInviteName );
			}
			
			Close();
		}
		break;
	case cBaseWindow::WT_FRIEND_MEMO:
		{	
			std::pair<bool, std::wstring> memoInfo = std::pair<bool, std::wstring>(true, wstring(szName));
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::SEND_FRIEND_SHUTOUT_MEMO, &memoInfo);
			Close();
		}
		break;
	case cBaseWindow::WT_SHUTOUT_MEMO:
		{
			std::pair<bool, std::wstring> memoInfo = std::pair<bool, std::wstring>(false, wstring(szName));
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::SEND_FRIEND_SHUTOUT_MEMO, &memoInfo);
			Close();
		}
		break;
	case cBaseWindow::WT_GUILDRANK_NAME:
		{
			GS2C_GUILD_CHANGE_CLASSNAME_SEND changeClassName;
			changeClassName.nClass = m_nValue1;
			changeClassName.wsName = szName;
			changeClassName.baseWindowType = m_eTargetWindowType;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GUILD_CHANGECLASSNAME, &changeClassName );
			Close();
		}
		break;
	case cBaseWindow::WT_GUILDRANK_EXPLAIN:
		{
			GS2C_GUILD_CHANGE_CLASSNAME_SEND changeClassName;
			changeClassName.nClass = m_nValue1;
			changeClassName.wsName = szName;
			changeClassName.baseWindowType = m_eTargetWindowType;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GUILD_CHANGECLASSNAME, &changeClassName );
			Close();
		}
		break;

	case cBaseWindow::WT_RECOMMEND_NAME:
		{
			std::wstring strTamerName( szName );
			if( strTamerName.empty() )
				return;

			if( strTamerName.compare( g_pCharMng->GetTamerUser()->GetName()) == 0 )
			{
				cPrintMsg::PrintMsg( 30499, const_cast< TCHAR* >(strTamerName.data()) );
				return;
			}

			if( net::game )
				net::game->SendAddFriendly( strTamerName.c_str() );
			Close();
		}
		break;

	case WT_SPIRIT_EVOLUTION:
		{
			if( !_CheckDigimonName( szName, true ) )
				return;

			std::pair<int,std::wstring> data;
			data.first = m_nValue1;
			data.second = szName;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_ITEM_TO_DIGIMON, &data );	

			Close();
		}
		break;
	default:
		assert_cs( false );
	}	
}

void cCreateName::OnCancel()
{	
	Close();
}

cBaseWindow::eMU_TYPE
cCreateName::Update_ForMouse()
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

void cCreateName::Render()
{
	if( IsShowWindow() == false )
		return;

	RenderScript();

#ifdef IME_WINDOW_POSITION
	if( m_pEditName != NULL )
	{
		CsPoint TempPos = m_pEditName->GetDefaultPos();
		CsPoint TempStringPos = m_pEditName->GetText()->GetStringSize();
		g_nIMEWidth = GetRootClient().x + TempPos.x + TempStringPos.x;
		g_nIMEHeight = GetRootClient().y + TempPos.y + TempStringPos.y;
	}
#endif

	m_pEditName->Render();
}

void cCreateName::ResetDevice()
{
	if( IsShowWindow() == false )
		return;

	ResetDeviceScript();
	m_pEditName->ResetDevice();
}

bool cCreateName::OnMacroKey(const MSG& p_kMsg)
{
	switch(p_kMsg.message)
	{
	case WM_KEYUP:
		{
			switch(p_kMsg.wParam)
			{
			case VK_RETURN:
				{
					// 이름 변경은 아직 보류
					if( m_eTargetWindowType == cBaseWindow::WT_CHANGE_T_NAME ||
						m_eTargetWindowType == cBaseWindow::WT_CHANGE_D_NAME ||
						m_eTargetWindowType == cBaseWindow::WT_INVITE_GUILD_NAME ||	// 길드 초대 이름 기입
						m_eTargetWindowType == cBaseWindow::WT_GUILDRANK_NAME )		// 길드 계급 이름 변경
						return false;
#ifndef NOT_ENTER_CREATENAME
					OnOk();
					return true;
#endif
				}
				break;			
			}
		}
		break;
	}

	return false;
}

// bool cCreateName::_IsEnableName()
// {
// 	TCHAR szName[ MAX_FILENAME ];
// 	g_IME.GetInputComp( szName );
// 
// #if ( defined VERSION_HK )	//#if ( defined VERSION_TW || defined VERSION_HK )
// 	while( chkcodeTaiwan( szName, m_nMaxLengthByte ) == false )
// #else
// 	while( chkcode( szName, m_nMaxLengthByte ) == false )
// #endif
// 	{
// 		if( m_eTargetWindowType != cBaseWindow::WT_FRIEND_MEMO && m_eTargetWindowType != cBaseWindow::WT_SHUTOUT_MEMO )
// 		{
// 			cPrintMsg::PrintMsg( 10026 );
// 		}		
// 		int len = _tcslen( szName );
// 		szName[ len - 1 ] = NULL;
// 		m_pEditName->SetText( szName );
// 	}
// 
// 	if( _tcslen( szName ) < NAME_MIN_LEN )
// 		return false;
// 
// 	return cFontCheck::FontCheck( szName );
// }
