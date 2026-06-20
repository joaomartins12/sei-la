
#include "stdafx.h"

#ifdef GUILD_MARK_DOWN

#include "Data_GuildMark.h"
#include <UrlMon.h>
#pragma comment(lib, "urlmon.lib")

#define  REFRESHTIME			60		//단위는 분단위
#define  MARK_DIRECTORY			_T( "Data\\NP\\Guildmark\\" )

cData_GuildMark::cData_GuildMark()
{
}

void cData_GuildMark::Delete()
{
	m_ListGuild.clear();	
}

void cData_GuildMark::Init()
{	
	if( nsCSFILE::IsSubDirectory( MARK_DIRECTORY ) == false )
	{
		nsCSFILE::CreateDirectory( MARK_DIRECTORY );		
	}
}

void cData_GuildMark::ResetCheck()
{
	if( NULL == g_pResist || NULL == net::game )
		return;

	uint nTime = ( net::game->GetTimeTS() - g_pResist->m_Global.s_nRefreshTime ) / 60;
	if( nTime > REFRESHTIME )
	{
		nsCSFILE::DeleteDirectory( MARK_DIRECTORY );
	}

	g_pResist->m_Global.s_nRefreshTime = net::game->GetTimeTS();
}

void cData_GuildMark::Update()
{
	if( m_ListGuild.empty() )
		return;

	std::list< CCharOption* >::iterator it = m_ListGuild.begin();
	std::list< CCharOption* >::iterator itEnd = m_ListGuild.end();

	char szDown[ MAX_PATH ];		
	char szFile[ MAX_PATH ];
	while( it!=itEnd )
	{
		int nUID = (*it)->GetGuildUID();		
		switch(net::cluster)
		{	
		/*
		case 100: sprintf_s(szDown, MAX_PATH, "http://www.digimonmasters.com/files/guild/logo/01/icon/%d.bmp", nUID);		break;
		case 200 : sprintf_s( szDown, MAX_PATH, "http://www.digimonmasters.com/files/guild/logo/02/icon/%d.bmp", nUID );		break;
		case 300 : sprintf_s( szDown, MAX_PATH, "http://www.digimonmasters.com/files/guild/logo/03/icon/%d.bmp", nUID );		break;	
		case 400 : sprintf_s( szDown, MAX_PATH, "http://www.digimonmasters.com/files/guild/logo/04/icon/%d.bmp", nUID );		break;	
		*/
		default:
			{
				it = m_ListGuild.erase( it );
			}
			continue;
		}

		//::DeleteUrlCacheEntryA( szDown );

		bool bSetMark = true;
		sprintf_s( szFile, MAX_PATH, "Data\\NP\\Guildmark\\%d_%d.bmp", nUID, net::cluster );
		HRESULT hr = ::URLDownloadToFileA( 0, szDown, szFile, 0, 0 );
		if( SUCCEEDED(hr) && ( _access_s( szFile, 0 ) == 0 ) )
		{
			if( !DmCS::FileFn::IsBmpIntegrityCheck( szFile ) )// 길드마크가 정보가 잘못榮?
			{
				::DeleteFileA( szFile );
				bSetMark = false;
			}
		}

		(*it)->SetGuildMark( bSetMark, bSetMark ? szFile : NULL );

		it = m_ListGuild.erase( it );
	}
}

void cData_GuildMark::MarkDownload( CCharOption* pOption )
{
	if( g_pCharMng->GetTamerUser()->GetCharOption()->GetGuildUID() == pOption->GetGuildUID() )
	{
		m_ListGuild.push_back( pOption );
	}	
	else
	{
		char szFile[ MAX_PATH ];
		sprintf_s( szFile, MAX_PATH, "Data\\NP\\Guildmark\\%d_%d.bmp", pOption->GetGuildUID(), net::cluster );

		if( !CsFPS::CsFPSystem::IsExist( 0, szFile ) )
		{
			m_ListGuild.push_back( pOption );
		}	
		else
		{
			if( !DmCS::FileFn::IsBmpIntegrityCheck( szFile ) )
			{
				::DeleteFileA( szFile );
				m_ListGuild.push_back( pOption );
			}
			else if( pOption->SetGuildMark( true, szFile ) == false )
			{
				m_ListGuild.push_back( pOption );
			}
		}
	}
}

#endif