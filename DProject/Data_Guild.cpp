
#include "stdafx.h"
#include "Data_Guild.h"

cData_Guild::cData_Guild()
{
	memset( &m_GuildInfo, 0 , sizeof( sGUILD_INFO ) );
	m_MyInfo = NULL;
	m_bNoticeShow = false;
	m_bOnOff = true;
	m_vpPersonInfo.ClearElement();		
#ifdef GUILD_RENEWAL
	std::map< uint, sGUILDSKILL_INFO* >::iterator itUseSkill = m_mapUsingSkill.begin();
	std::map< uint, sGUILDSKILL_INFO* >::iterator itUseSkillEnd = m_mapUsingSkill.end();
	for( ; itUseSkill != itUseSkillEnd ; itUseSkill++ )
		SAFE_DELETE( itUseSkill->second );

	m_mapUsingSkill.clear();
#endif
}

void cData_Guild::InitNetOff()
{	
}

void cData_Guild::Delete()
{	
	memset( &m_GuildInfo, 0 , sizeof( sGUILD_INFO ) );
	m_MyInfo = NULL;	

	DeleteHistory();

	int nCount = m_vpPersonInfo.Size();
	for( int i=0; i<nCount; ++i )
		SAFE_NIDELETE( m_vpPersonInfo[ i ] );

	m_vpPersonInfo.Destroy();	

	std::list< sNAME* >::iterator it = m_listInviteName.begin();
	std::list< sNAME* >::iterator itEnd = m_listInviteName.end();
	for( ; it!=itEnd; ++it )
		SAFE_NIDELETE( *it );

	m_listInviteName.clear();

	std::list< sGUILD_PERSON_INFO* >::iterator it2 = m_ListPersonInfo.begin();
	std::list< sGUILD_PERSON_INFO* >::iterator it2End = m_ListPersonInfo.end();
	for( ; it2!=it2End; ++it2 )
		SAFE_NIDELETE( *it2 );

	m_ListPersonInfo.clear();	
#ifdef GUILD_RENEWAL
	std::map< uint, sGUILDSKILL_INFO* >::iterator itUseSkill = m_mapUsingSkill.begin();
	std::map< uint, sGUILDSKILL_INFO* >::iterator itUseSkillEnd = m_mapUsingSkill.end();
	for( ; itUseSkill != itUseSkillEnd ; itUseSkill++ )
		SAFE_DELETE(itUseSkill->second);
	m_mapUsingSkill.clear();
#endif
}

void cData_Guild::DeleteHistory()
{
	int nCount = m_vpHistoryInfo.Size();
	for( int i=0; i<nCount; ++i )
		SAFE_NIDELETE( m_vpHistoryInfo[ i ] );

	m_vpHistoryInfo.Destroy();
}

void cData_Guild::SetGuildName( TCHAR const* szGuildName )
{
	assert_cs( szGuildName != NULL );

	// 길드 정보 init
	Delete();

	m_GuildInfo.s_szGuildName = szGuildName;

	if( g_pCharMng->GetTamerUser() )
		g_pCharMng->GetTamerUser()->GetCharOption()->SetGuildName( m_GuildInfo.s_szGuildName.c_str() );	
}

void cData_Guild::Update()
{
	// 보낸지 10초가 지나면 목록에서 삭제
	std::list< sNAME* >::iterator it = m_listInviteName.begin();
	std::list< sNAME* >::iterator itEnd = m_listInviteName.end();
	while( it!=itEnd )
	{
		if( ( (*it)->s_fLifeTime -= g_fDeltaTime ) < 0.0f )
		{
			SAFE_NIDELETE( *it ); 
			it = m_listInviteName.erase( it );
		}
		else
		{
			++it;
		}
	}

	std::list< sGUILD_PERSON_INFO* >::iterator it2 = m_ListPersonInfo.begin();
	std::list< sGUILD_PERSON_INFO* >::iterator it2End = m_ListPersonInfo.end();
	for( ; it2!=it2End; ++it2 )
	{
		if( (*it2)->s_bInit && ( (*it2)->s_bOnline == false ) )
			(*it2)->s_Tick += g_fDeltaTime;

		if( (*it2)->s_Tick > 13.0f )
		{
			(*it2)->s_bInit = false;
			(*it2)->s_Tick = 0;
			SetGuildOnOff( false, (*it2)->s_szName.c_str(), 0, 0, (*it2)->s_nPersonLevel );

			cPrintMsg::PrintMsg( 30125, const_cast<TCHAR*>((*it2)->s_szName.c_str()) );
		}	
	}
}

cData_Guild::eRANK cData_Guild::GetRank( TCHAR const * szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );	
	if( pInfo != NULL )
	{
		return pInfo->s_eRank;
	}
	return GR_NONE;
}

cData_Guild::sGUILD_PERSON_INFO* cData_Guild::GetInfo( int nDataIdx )
{
	std::list< sGUILD_PERSON_INFO* >::iterator it = m_ListPersonInfo.begin();
	std::list< sGUILD_PERSON_INFO* >::iterator itEnd = m_ListPersonInfo.end();

	for( ; it!=itEnd; ++it )
	{
		if( --nDataIdx < 0 )
		{
			return *it;
		}
	}	
	assert_cs( false );
	return NULL;
}

void cData_Guild::SetGuildInfoData()
{ 
	m_GuildInfo.s_nCurPersonCount = 0;
	m_GuildInfo.s_n2AndMasterNum = 0;
	m_GuildInfo.s_nMaxPersonCount = nsCsFileTable::g_pBaseMng->GetGuildInfo( m_GuildInfo.s_nGuildLevel )->s_nMaxGuildPerson + m_GuildInfo.s_nIncMember;
	m_GuildInfo.s_nPersonCount = m_ListPersonInfo.size();

	std::list< sGUILD_PERSON_INFO* >::iterator it = m_ListPersonInfo.begin();
	std::list< sGUILD_PERSON_INFO* >::iterator itEnd = m_ListPersonInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_bOnline == true )
			m_GuildInfo.s_nCurPersonCount++;

		if( (*it)->s_eRank == GR_2NDMASTER )
			m_GuildInfo.s_n2AndMasterNum++;
	}	

	if( g_pGameIF && g_pGameIF->GetCommunity() )
	{
		g_pGameIF->GetCommunity()->MakeGuildInfo();
	}
}

void cData_Guild::SetGuildPersonInfo( sGUILD_PERSON_INFO* pPerson )
{ 
	if( pPerson->s_bOnline )
		pPerson->s_bInit = true;

	if( pPerson->s_szName.compare( g_pCharMng->GetTamerUser()->GetName() ) == 0 )//길드원 정보가 나일때
		m_MyInfo = pPerson;		

	if( pPerson->s_eRank == GR_MASTER )
		m_GuildInfo.s_szMasterName = pPerson->s_szName;

	m_ListPersonInfo.push_back( pPerson );
}

void cData_Guild::SetGuildOnOff( bool bValue, TCHAR const * szTamerName, DWORD nMapID, int nChannel, int nLevel )
{
	if( m_MyInfo == NULL )
		return;

	assert_cs( m_MyInfo->s_eRank != GR_NONE );

	if( bValue )
		m_GuildInfo.s_nCurPersonCount++;
	else
		m_GuildInfo.s_nCurPersonCount--;

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szTamerName, nIdx );
	assert_cs( pInfo != NULL );
	pInfo->s_bOnline = bValue;
	pInfo->s_dwMapID = nMapID;
	pInfo->s_nChannel = nChannel;	
	pInfo->s_nPersonLevel = nLevel;
#ifdef GUILD_RENEWAL
	if( pInfo->s_bOnline )
		pInfo->s_nLastPlayTime = INT_MAX;	//온라인
	else
		pInfo->s_nLastPlayTime = _TIME_TS;	//로그아웃 시간
#endif

	if( ( pInfo->s_bInit == false ) && ( bValue == true ) )
	{
		cPrintMsg::PrintMsg( 30124, const_cast<TCHAR*>(pInfo->s_szName.c_str()) );
		pInfo->s_bInit = true;
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
			g_pGameIF->GetCommunity()->_AddGuildPerson( pInfo, nIdx );	
#else
			g_pGameIF->GetCommunity()->AddGuildPerson( pInfo, nIdx );		
#endif
	}
}

void cData_Guild::SetTempOff( TCHAR const * szTamerName )
{
	if( m_MyInfo == NULL )
		return;
	assert_cs( m_MyInfo->s_eRank != GR_NONE );

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szTamerName, nIdx );	
	pInfo->s_bOnline = false;
#ifdef GUILD_RENEWAL
	pInfo->s_nLastPlayTime = _TIME_TS;	// 접속시간 저장
#endif
}

void cData_Guild::SetLeavePerson( TCHAR const * szTamerName, int nType )
{
	if( m_MyInfo == NULL || m_MyInfo->s_eRank == GR_NONE )
		return;

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szTamerName, nIdx );
	if( pInfo == NULL )
		return;
	
	switch( nType )
	{
	case 0: //본인이 탈퇴한경우
		cPrintMsg::PrintMsg( 30132, const_cast<TCHAR*>(szTamerName) );
		break;
	case 1: //강퇴당한 경우
		cPrintMsg::PrintMsg( 30131, const_cast<TCHAR*>(szTamerName) );
		break;
	default:
		assert_cs( false );
		break;
	}						

	SetGuildInfoData();
	std::list< sGUILD_PERSON_INFO* >::iterator it = m_ListPersonInfo.begin();
	std::list< sGUILD_PERSON_INFO* >::iterator itEnd = m_ListPersonInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_szName.compare( szTamerName ) == 0 )
		{
			SAFE_NIDELETE( *it );  
			m_ListPersonInfo.erase( it );			
			break;
		}
	}	

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{	
		if( nIdx == g_pGameIF->GetCommunity()->GetCurSelect() )
			g_pGameIF->GetPopup()->ClosePopup( cPopUpWindow::GUILD_MENU );			

		g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_GUILD );
	}
}

void cData_Guild::SetJoinPerson( sGUILD_PERSON_INFO* pPerson )
{
	if( m_MyInfo == NULL || m_MyInfo->s_eRank == GR_NONE )
		return;

	SetGuildPersonInfo( pPerson );
	SetGuildInfoData();

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
			g_pGameIF->GetCommunity()->_AddGuildPerson( pPerson, m_ListPersonInfo.size() - 1 );
#else
			g_pGameIF->GetCommunity()->AddGuildPerson( pPerson, m_ListPersonInfo.size() - 1 );
#endif
	}
}

void cData_Guild::SetLevelUp( TCHAR const* szTamerName, int nLevel )
{
	if( m_MyInfo == NULL || m_MyInfo->s_eRank == GR_NONE )
		return;

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szTamerName, nIdx );	
	if( pInfo == NULL )
		return;

	pInfo->s_nPersonLevel = nLevel;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
			g_pGameIF->GetCommunity()->_AddGuildPerson( pInfo, nIdx );
#else
			g_pGameIF->GetCommunity()->AddGuildPerson( pInfo, nIdx );
#endif
	}
}


void cData_Guild::SetClassRefresh( TCHAR const * szTamerName, cData_Guild::eRANK nClass )
{
	if( m_MyInfo == NULL || m_MyInfo->s_eRank == GR_NONE )
		return;

	switch( nClass )
	{
	case GR_MASTER:		_ChangeGuildMaster( szTamerName );	break;
	case GR_2NDMASTER:	_Change2ndMaster( szTamerName );	break;
	case GR_NORMAL:		_ChangeNormal( szTamerName );		break;
	case GR_DAT:		_ChangeDatMember( szTamerName );	break;
	case GR_NEW:		_ChangeNewbi( szTamerName );		break;
	default:
		assert_cs( false );
		break;
	}
}

void cData_Guild::ReName( TCHAR const* szFrom, TCHAR const * szTo )
{
	if( m_MyInfo == NULL || m_MyInfo->s_eRank == GR_NONE )
		return;

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szFrom, nIdx );	
	if( pInfo == NULL )
		return;
	pInfo->s_szName = szTo;

	if( pInfo->s_eRank == GR_MASTER )
	{
		m_GuildInfo.s_szMasterName = szTo;
		SetGuildInfoData();	
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		if( nIdx == g_pGameIF->GetCommunity()->GetCurSelect() )
			g_pGameIF->GetPopup()->ClosePopup( cPopUpWindow::GUILD_MENU );			

		if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
			g_pGameIF->GetCommunity()->_AddGuildPerson( pInfo, nIdx );	
#else
			g_pGameIF->GetCommunity()->AddGuildPerson( pInfo, nIdx );	
#endif
	}
}

void cData_Guild::SendGuildInvite( TCHAR const * szTamerName )
{
	if(szTamerName == NULL)
	{
		cPrintMsg::PrintMsg( 30137 );	// 접속중이지 않거나, 존재하지 않는 상대입니다.
		return;
	}
	// 현재 상황에서는 초대가 되지 않는다. ( Npc이용, 전투중, 거래중, 진화중, 파트너몬 교체중 )
	else if( g_pGameIF->IsOpenNpcWindow() || g_pCharMng->GetDigimonUser( 0 )->IsBattle() || g_pDataMng->GetServerSync()->IsChageDigimon() 
		|| cMessageBox::IsMessageBox() || g_pResist->IsTutorial()  )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}

	else if( m_MyInfo->s_eRank != GR_MASTER && m_MyInfo->s_eRank != GR_2NDMASTER )
	{ 
		cPrintMsg::PrintMsg( 30158, GetRankName( GR_2NDMASTER ) );
		return;
	}
 
	// 나라면 패스
	else if( _tcscmp( g_pCharMng->GetTamerUser()->GetName(), szTamerName ) == 0 )
	{
		cPrintMsg::PrintMsg( 11101 );
		return;
	}	

	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_INVATE, 0, const_cast<TCHAR*>(szTamerName) );	
}

bool cData_Guild::IsSendInvate( TCHAR const * szTamerName )
{
	// 이미 보낸 목록에 존재 하는지 찾는다.
	std::list< sNAME* >::iterator it = m_listInviteName.begin();
	std::list< sNAME* >::iterator itEnd = m_listInviteName.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_szName.compare( szTamerName ) == 0 )
		{
			cPrintMsg::PrintMsg( 30062 );	
			return true;
		}
	}

	// 보낸 목록에 저장
	sNAME* pName = NiNew sNAME;
	pName->s_szName = szTamerName;
	pName->s_fLifeTime = 10.0f;
	m_listInviteName.push_back( pName );
	return false;
}

void cData_Guild::RecvInvite( TCHAR const * szName )
{
	// 나라면 패스
	if( _tcscmp( g_pCharMng->GetTamerUser()->GetName(), szName ) == 0 )
	{		
		return;
	}

	if( m_MyInfo )
	{
		if( m_MyInfo->s_eRank == GR_MASTER || m_MyInfo->s_eRank == GR_2NDMASTER )
		{
			assert_cs( net::game );

			// 이미 보낸 목록에 존재 하는지 찾는다.
			std::list< sNAME* >::iterator it = m_listInviteName.begin();
			std::list< sNAME* >::iterator itEnd = m_listInviteName.end();
			for( ; it!=itEnd; ++it )
			{
				if( (*it)->s_szName.compare( szName ) == 0 )
				{
					SAFE_NIDELETE( *it );  
					m_listInviteName.erase( it );
					break;
				}
			}
		}
	}	
}

cData_Guild::sGUILD_PERSON_INFO* cData_Guild::_GetPersonInfo( TCHAR const* szName, int& nIdx )
{
	std::list< sGUILD_PERSON_INFO* >::iterator it = m_ListPersonInfo.begin();
	std::list< sGUILD_PERSON_INFO* >::iterator itEnd = m_ListPersonInfo.end();
	
	for( ; it!=itEnd; ++it, ++nIdx )
	{
		if( (*it)->s_szName.compare( szName ) == 0 )
			return *it;
	}	
	return NULL;
}

void cData_Guild::_ChangeGuildMaster( TCHAR const * szName )
{
	std::list< sGUILD_PERSON_INFO* >::iterator it = m_ListPersonInfo.begin();
	std::list< sGUILD_PERSON_INFO* >::iterator itEnd = m_ListPersonInfo.end();
	int nMasterIdx = 0;
	for( ; it!=itEnd; ++it, ++nMasterIdx )
	{
		if( (*it)->s_eRank == GR_MASTER )
		{
			(*it)->s_eRank = GR_NORMAL;					

			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
			{	
				// 기존 길마가 본인이었다면
				if((*it)->s_szName.compare( g_pCharMng->GetTamerUser()->GetName() ) == 0 )
				{
					g_pGameIF->GetCommunity()->SetNoticeBtn( false );
					if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILDADMIN )
						g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_GUILDADMIN );
				}	

				if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
					g_pGameIF->GetCommunity()->_AddGuildPerson( (*it), nMasterIdx );
#else
					g_pGameIF->GetCommunity()->AddGuildPerson( (*it), nMasterIdx );
#endif
			}
			break;
		}
	}	

	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	pInfo->s_eRank = GR_MASTER;
	m_GuildInfo.s_szMasterName = szName;

	if( g_pGameIF && g_pGameIF->GetCommunity() )
	{
		// 정보창 갱신
		g_pGameIF->GetCommunity()->MakeGuildInfo();

		// 새로운 길마가 본인이라면
		if( _tcscmp( g_pCharMng->GetTamerUser()->GetName(), szName ) == 0 )
		{
			g_pGameIF->GetCommunity()->SetNoticeBtn( true );
			if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILDADMIN )
				g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_GUILDADMIN );
		}	

		if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
			g_pGameIF->GetCommunity()->_AddGuildPerson( pInfo, nIdx );
#else
			g_pGameIF->GetCommunity()->AddGuildPerson( pInfo, nIdx );
#endif
	}
}

void cData_Guild::_Change2ndMaster( TCHAR const * szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	pInfo->s_eRank = GR_2NDMASTER;

	m_GuildInfo.s_n2AndMasterNum++;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
			g_pGameIF->GetCommunity()->_AddGuildPerson( pInfo, nIdx );
#else
			g_pGameIF->GetCommunity()->AddGuildPerson( pInfo, nIdx );
#endif
	}
}

void cData_Guild::_ChangeDatMember( TCHAR const* szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	assert_cs( pInfo != NULL );
	if( pInfo->s_eRank == GR_2NDMASTER )
		m_GuildInfo.s_n2AndMasterNum--;

	pInfo->s_eRank = GR_DAT;			

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
			g_pGameIF->GetCommunity()->_AddGuildPerson( pInfo, nIdx );
#else
			g_pGameIF->GetCommunity()->AddGuildPerson( pInfo, nIdx );
#endif
	}
}

void cData_Guild::_ChangeNormal( TCHAR const * szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	assert_cs( pInfo != NULL );
	if( pInfo->s_eRank == GR_2NDMASTER )
		m_GuildInfo.s_n2AndMasterNum--;

	pInfo->s_eRank = GR_NORMAL;			

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
			g_pGameIF->GetCommunity()->_AddGuildPerson( pInfo, nIdx );
#else
			g_pGameIF->GetCommunity()->AddGuildPerson( pInfo, nIdx );
#endif
	}
}

void cData_Guild::_ChangeNewbi( TCHAR const* szName )
{
	int nIdx = 0;
	sGUILD_PERSON_INFO* pInfo = _GetPersonInfo( szName, nIdx );
	assert_cs( pInfo != NULL );
	if( pInfo->s_eRank == GR_2NDMASTER )
		m_GuildInfo.s_n2AndMasterNum--;

	pInfo->s_eRank = GR_NEW;			

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_GUILD )
#ifdef GUILD_RENEWAL
			g_pGameIF->GetCommunity()->_AddGuildPerson( pInfo, nIdx );
#else
			g_pGameIF->GetCommunity()->AddGuildPerson( pInfo, nIdx );
#endif
	}
}

#ifdef GUILD_RENEWAL

void cData_Guild::_PersonSort( int nButton, bool bSort )
{
	// 길드원 정보
	std::list< sGUILD_PERSON_INFO* >::iterator it = m_ListPersonInfo.begin();
	std::list< sGUILD_PERSON_INFO* >::iterator itEnd = m_ListPersonInfo.end();
	std::list< sGUILD_PERSON_INFO* > listSort;	// 정렬하기위해 임시로 담아둘 곳
	for( ; it!=itEnd; ++it )
	{
		listSort.push_back( *it );
	}
	m_ListPersonInfo.clear();

	itEnd = listSort.end();
	while( listSort.size() != 0 )
	{
		std::list< sGUILD_PERSON_INFO* >::iterator itInput = listSort.begin();
		it = itInput;		
		for( ; it!=itEnd; ++it )
		{
			switch( nButton )
			{
				// Level
			case 0:
				if( ( (*it)->s_nPersonLevel < (*itInput)->s_nPersonLevel ) == bSort )
					itInput = it;
				break;
				// 이름
			case 1:
				if( ( (*it)->s_szName.compare( (*itInput)->s_szName ) < 0 ) == bSort )
					itInput = it;
				break;
				// 직급
			case 2:	
				if( ( (*it)->s_eRank < (*itInput)->s_eRank ) == bSort )
					itInput = it;
				break;
				// 계급
			case 3:
				if( ( (*it)->s_nUserRank < (*itInput)->s_nUserRank ) == bSort )
					itInput = it;	
				break;
				// 접속상태
			case 4:
				if( ( (*it)->s_nLastPlayTime < (*itInput)->s_nLastPlayTime ) == bSort )
					itInput = it;	
				break;			
			default:
				assert_cs( false );
			}
		}

		m_ListPersonInfo.push_back( *itInput );
		listSort.erase( itInput );
	}
}
#else
void cData_Guild::PersonSort( int nType, int nButton, bool bSort )
{
	std::list< sGUILD_PERSON_INFO* >::iterator it = m_ListPersonInfo.begin();
	std::list< sGUILD_PERSON_INFO* >::iterator itEnd = m_ListPersonInfo.end();

	std::list< sGUILD_PERSON_INFO* > listSort;
	for( ; it!=itEnd; ++it )
	{
		listSort.push_back( *it );
	}
	m_ListPersonInfo.clear();

	itEnd = listSort.end();
	while( listSort.size() != 0 )
	{
		std::list< sGUILD_PERSON_INFO* >::iterator itInput = listSort.begin();
		it = itInput;		
		for( ; it!=itEnd; ++it )
		{
			switch( nButton )
			{
				// Level
			case 0:
				if( ( (*it)->s_nPersonLevel < (*itInput)->s_nPersonLevel ) == bSort )
					itInput = it;
				break;

				// 이름
			case 1:
				if( ( (*it)->s_szName.compare( (*itInput)->s_szName ) < 0 ) == bSort )
					itInput = it;
				break;
										
				// 랭크
			case 2:	
				{
					switch( nType )
					{
					case cCommunity::INFO:
						// 위치
						if( ( (*it)->s_dwMapID < (*itInput)->s_dwMapID ) == bSort )
							itInput = it;
						break;
					case cCommunity::ACTIVE:
						// 기여등급
						if( ( (*it)->s_nContribution < (*itInput)->s_nContribution ) == bSort )
							itInput = it;
						break;
					case cCommunity::RANK:
						// 직급
						if( ( (*it)->s_eRank < (*itInput)->s_eRank ) == bSort )
							itInput = it;
						break;
					default:
						assert_cs( false );
						break;
					}
				}				
				break;
				// 기여 등급
			case 3:
				if( ( (*it)->s_nContribution < (*itInput)->s_nContribution ) == bSort )
					itInput = it;	
				break;							
			default:
				assert_cs( false );
			}
		}

		m_ListPersonInfo.push_back( *itInput );
		listSort.erase( itInput );
	}
}
#endif	// GUILD_RENEWAL

void cData_Guild::SetNotice( TCHAR const* szNotice, bool bChat /* = true  */)	// 길드 공지.
{
	if( _tcslen( szNotice ) == 0 )
		GetGuildInfo()->s_szNotice = _LAN( "길드 공지를 작성해 주세요." );
	else
	{
		GetGuildInfo()->s_szNotice = szNotice;

		TCHAR szTemp[ MAX_PATH ];		
		_stprintf_s( szTemp, MAX_PATH, _T( "%s %s" ), _LAN( "길드 공지 :" ), szNotice );		
		if( bChat == false )
		{
			if( m_bNoticeShow == false )
			{
				m_bNoticeShow = true;
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::GUILD_NOTICE;
				CProtocol.m_wStr = szTemp;
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
			}
		}
		else
		{
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::GUILD_NOTICE;
			CProtocol.m_wStr = szTemp;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
	{
		g_pGameIF->GetCommunity()->SetNotice( szNotice );
	}
}

void cData_Guild::SetGuildLevel( int nLevel )
{
	m_GuildInfo.s_nGuildLevel = nLevel;

	if( g_pGameIF && g_pGameIF->GetCommunity() )
	{
		g_pGameIF->GetCommunity()->MakeGuildInfo();
	}
}

void cData_Guild::SetHistory( sHISTORY_INFO* sInfo )
{
	if( m_vpHistoryInfo.Size() == 0 )
	{
		m_vpHistoryInfo.PushBack( sInfo ); 
	}
	else
	{
		bool bInsert = false;
		for( int idx = 0; idx < m_vpHistoryInfo.Size(); idx++ )
		{
			if( sInfo->s_nTime < m_vpHistoryInfo[ idx ]->s_nTime )
			{
				m_vpHistoryInfo.PushInsert( idx - 1, sInfo );
				bInsert = true;
				break;
			}
		}	

		if( bInsert == false )
			m_vpHistoryInfo.PushBack( sInfo );
	}
}

#ifdef GUILD_RENEWAL
bool cData_Guild::sGUILDSKILL_INFO::_IsCoolTime()
{
	return _TIME_TS < s_nEndTime;	// ( 현재시간 < 쿨타임 종료 시간 ) true : 쿨타임 중 , false : 쿨타임 아님 
}

cData_Guild::sGUILDSKILL_INFO* cData_Guild::GetUsingSkill( u4 nSkillCode )
{
	// 스킬 코드로 적용중인 스킬 얻기( 쿨타임/적용기간, 스킬 존재 유무 확인용 )

	std::map< uint, sGUILDSKILL_INFO* >::const_iterator it = m_mapUsingSkill.find( nSkillCode );
	if( it == m_mapUsingSkill.end() )
		return NULL;

	return it->second;
}

void cData_Guild::SetUsingSkill( bool bUsing , u4 nSkillCode, u4 nEndTime /*= 0*/, u4 nUseTime /*= 0*/ )
{
	if( bUsing )	// 사용중으로 들어온 스킬
	{
		sGUILDSKILL_INFO* sInfo = csnew sGUILDSKILL_INFO;
		sInfo->s_nSkillCode = nSkillCode;
		if( nUseTime != 0 )
		{
			int nCoolDown = (int)nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode )->GetInfo()->s_fCooldownTime;
			int nRemainTime = (int)nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode )->GetInfo()->s_fDamageTime;
			
			if( nCoolDown != 0 )
				nEndTime = nUseTime + nCoolDown;
			else
				nEndTime = nUseTime + nRemainTime;
			sInfo->s_nEndTime = nEndTime;
		}
		else
			sInfo->s_nEndTime = nEndTime;
		m_mapUsingSkill[nSkillCode] = sInfo;
	}
	else		// 사용중인 스킬 지우기
	{
		// 스킬코드로 같은놈 찾아서 지우기
		m_mapUsingSkill[nSkillCode] = NULL;
	}
}

void cData_Guild::SetBaseSkill( int nLevel )
{
	// 레벨에 따라 Base길드스킬 -> 적용 스킬 리스트에 넣기
	std::list< CsGuild_Skill::sINFO* >::iterator it = nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList()->begin();
	std::list< CsGuild_Skill::sINFO* >::iterator itEnd = nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList()->end();
	// Base스킬 중 적용 중인놈이 있으면 없애주기(중첩 관리)
	std::map< uint, sGUILDSKILL_INFO* >::iterator itUse = m_mapUsingSkill.begin();
	std::map< uint, sGUILDSKILL_INFO* >::iterator itUseEnd = m_mapUsingSkill.end();
	for( ; itUse != itUseEnd ; itUse++ )
	{
		if( itUse->second == NULL )
			continue;
		for( ; it != itEnd ; it++ )
		{
			if( (*it)->s_nSkillCode == itUse->second->s_nSkillCode )
			{
				if( itUse->second->s_nEndTime != 0 )
					m_mapUsingSkill[ itUse->second->s_nSkillCode ]->s_nCount = 0;
				else
					m_mapUsingSkill[ itUse->second->s_nSkillCode ] = NULL;
				break;
			}
		}
		it = nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList()->begin();
	}

	it = nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList()->begin();
	for( ; it != itEnd ; it++ )
	{
		if( (*it)->s_nReqLevel > nLevel )
			continue;

		sGUILDSKILL_INFO* sInfo = csnew sGUILDSKILL_INFO;
		sInfo->s_nSkillCode = (*it)->s_nSkillCode;
		sInfo->s_nEndTime = 0;
		if( m_mapUsingSkill[sInfo->s_nSkillCode] != NULL )	// 이미 같은 스킬코드를 가지는 스킬이 존재 한다면
		{
			// 중첩 수 셋팅
			sInfo->s_nCount = m_mapUsingSkill[sInfo->s_nSkillCode]->s_nCount + 1;
			if( m_mapUsingSkill[sInfo->s_nSkillCode]->s_nEndTime != 0 )
				sInfo->s_nEndTime = m_mapUsingSkill[sInfo->s_nSkillCode]->s_nEndTime;	// 쿨타임 저장
		}
		m_mapUsingSkill[sInfo->s_nSkillCode] = sInfo;
	}
	CheckPasiveSkill();
}
NiColor cData_Guild::CheckPasiveSkill()
{
	// Base스킬 중 패시브 스킬 능력치 적용
	int nAddLv = 0;

	int nColorLv = 0;	// 색 변경 우선순위. 색 변경 스킬 효과 추가 시 우선순위 체크
	NiColor pColor = DEFAULT_NAMECOLOR;

	std::map< uint, sGUILDSKILL_INFO* >::iterator it = m_mapUsingSkill.begin();
	std::map< uint, sGUILDSKILL_INFO* >::iterator itEnd = m_mapUsingSkill.end();

	for( ; it != itEnd ; it++ )
	{
		if( it->second == NULL )
			continue;
		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( it->second->s_nSkillCode );
		for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
		{
			if( pSkill->GetInfo()->s_Apply[i].s_nID == 0 )
				break;
			switch( pSkill->GetInfo()->s_Apply[ i ].s_nA )
			{
			case GUILD_MEMBERUP:	// 길드 인원 확장
				nAddLv += ( pSkill->GetInfo()->s_Apply[ i ].s_nB * it->second->s_nCount );	// 중첩 수 계산
				break;
			case GUILD_NAMECOLOR:	// 색 변경
				if( nColorLv < 1 )
				{
					float fR = (float)( pSkill->GetInfo()->s_Apply[i].s_nB/100 );
					float fG = (float)( (pSkill->GetInfo()->s_Apply[i].s_nB/10) % 10 );
					float fB = (float)( pSkill->GetInfo()->s_Apply[i].s_nB%10 );
					pColor = NiColor( fR / 10.0f, fG / 10.0f, fB / 10.0f );
					nColorLv = 1;
				}
				break;
			}
		}
	}

	// GUILD_MEMBERUP(52) - 길드 인원 확장된 만큼 추가
	m_GuildInfo.s_nIncMember = nAddLv;

	// GUILD_NAMECOLOR(54) - 이름색 지정
	g_pCharMng->GetTamerUser()->GetCharOption()->SetNameColor( pColor );

	return pColor;
}

int cData_Guild::CheckActSkill( int nSkillCode )// 리턴값 - ( 0 : 같은그룹 아님, 1: 같은 그룹의 상위 존재, 스킬코드: 같은 그룹이지만 하위 스킬 존재 )
{
	// 사용하려는 스킬보다 상위 스킬이 적용되어 있는가
	std::map< uint, sGUILDSKILL_INFO* >::iterator it = m_mapUsingSkill.begin();
	std::map< uint, sGUILDSKILL_INFO* >::iterator itEnd = m_mapUsingSkill.end();
	// 지금 사용하려는 스킬
	CsSkill* pCurSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode );
	if( pCurSkill == NULL )
		return false;

	int nCurSkillCode = 0;
	int nVal = 0;

	CsSkill* pSkill;	// 적용 중인 스킬
	for( ; it != itEnd ; it++ )
	{
		if( it->second == NULL )
			continue;
		// 적용 중인 스킬
		pSkill = nsCsFileTable::g_pSkillMng->GetSkill( it->second->s_nSkillCode );
		
		if( pSkill == NULL || pCurSkill == NULL )
			continue;

		for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
		{
			if( pSkill->GetInfo()->s_Apply[i].s_nID == 0 )
				break;
			// 효과가 같으면서
			if( pCurSkill->GetInfo()->s_Apply[i].s_nA == pSkill->GetInfo()->s_Apply[i].s_nA )
			{
				if( nVal < pSkill->GetInfo()->s_Apply[i].s_nB )
				{
					nCurSkillCode = pSkill->GetInfo()->s_dwID;
					nVal = pSkill->GetInfo()->s_Apply[i].s_nB;
				}
				// 효과값이 크거나 같은 스킬이 적용 중이면
				if( pCurSkill->GetInfo()->s_Apply[i].s_nB <= pSkill->GetInfo()->s_Apply[i].s_nB )
				{
					return 1;	// 같은 그룹의 상위 스킬 존재
				}
			}
		}
	}
	if( nCurSkillCode > 0 )
		return nCurSkillCode;	// 같은 그룹이지만 하위 스킬 존재
	else
		return 0;	// 같은그룹 아님
}

bool cData_Guild::IsActSkillExist()
{
	std::map< uint, sGUILDSKILL_INFO* >::iterator it = m_mapUsingSkill.begin();
	std::map< uint, sGUILDSKILL_INFO* >::iterator itEnd = m_mapUsingSkill.end();

	int nVal = 0;
	for(; it != itEnd; it++)
	{
		if(it->second == NULL)
			continue;
		if( nsCsFileTable::g_pBaseMng->_IsGuildBaseSkill( it->second->s_nSkillCode ) == false )
			return true;	//Base 스킬이 아닌 스킬이면 Active 스킬이지
	}
	return false; // Active 스킬이 없음
}

int cData_Guild::GetUsingSkillValue( int nApply )
{
	std::map< uint, sGUILDSKILL_INFO* >::iterator it = m_mapUsingSkill.begin();
	std::map< uint, sGUILDSKILL_INFO* >::iterator itEnd = m_mapUsingSkill.end();
	
	CsSkill* pSkill;	// 적용 중인 스킬
	int nVal = 0;
	for( ; it != itEnd ; it++ )
	{
		if( it->second == NULL )
			continue;
		// 적용 중인 스킬
		pSkill = nsCsFileTable::g_pSkillMng->GetSkill( it->second->s_nSkillCode );

		if( pSkill == NULL )
			continue;

		for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
		{
			if( pSkill->GetInfo()->s_Apply[i].s_nID == 0 )
				break;
			// 효과가 같으면서
			if( pSkill->GetInfo()->s_Apply[i].s_nA == nApply )
			{
				nVal += ( pSkill->GetInfo()->s_Apply[i].s_nB * it->second->s_nCount );	// 중첩 수 계산
			}
		}
	}
	return nVal;
}

void cData_Guild::ClearUsingSkill()
{
	m_mapUsingSkill.clear();
}

void cData_Guild::SetGuildMemo( TCHAR const * szName, TCHAR const * szMemo )
{
	MAP_GUILD_IT it = m_ListPersonInfo.begin();
	MAP_GUILD_IT itEnd = m_ListPersonInfo.end();

	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_szName.compare( szName ) == 0 )
		{
			(*it)->s_szGuildMemo = szMemo;
			return;
		}
	}
}

int cData_Guild::GetOnlineCount()
{
	MAP_GUILD_IT it = m_ListPersonInfo.begin();
	MAP_GUILD_IT itEnd = m_ListPersonInfo.end();
	int nCount = 0;
	for( ; it != itEnd; it++ )
	{
		if( (*it)->s_bOnline )
			nCount++;
	}
	return nCount;
}
#endif

void cData_Guild::SetRankName( eRANK eRank, TCHAR const * szName )
{ 
	_tcscpy_s( &m_szGuildRankName[ eRank ][ 0 ], Language::pLength::GuildClassName + 1, szName ); 
}

void cData_Guild::SetRankDesc( eRANK eRank, TCHAR const * szDesc )
{ 
	_tcscpy_s( &m_szGuildRankDesc[ eRank ][ 0 ], MAX_PATH , szDesc ); 
}
//길드 폐쇠전 
bool cData_Guild::PreCheckSendGuildDestory()
{
	if(m_ListPersonInfo.size() > 1)
	{
		cPrintMsg::PrintMsg( 30159 );
		return false;
	}
	cPrintMsg::PrintMsg( 30152 );
	return true;
}