
#include "stdafx.h"
#include "BaseMng.h"

void CsBaseMng::sPERSON_STORE::Delete()
{
	std::map< int, sOBJECT_INFO* >::iterator it = s_mapObject.begin();
	std::map< int, sOBJECT_INFO* >::iterator itEnd = s_mapObject.end();
	for( ; it!=itEnd; ++it )
	{
		delete it->second;
	}
	s_mapObject.clear();
}
#ifdef GUILD_RENEWAL
void CsBaseMng::sGUILDBASE::Delete()
{
	std::map< int, sLEVELINFO* >::iterator it = s_mapLevelInfo.begin();
	std::map< int, sLEVELINFO* >::iterator itEnd = s_mapLevelInfo.end();
	for( ; it!=itEnd; ++it )
	{
		delete it->second;
	}
	s_mapLevelInfo.clear();

	std::map< int, sGUILDPOINT* >::iterator it2 = s_mapPointInfo.begin();
	std::map< int, sGUILDPOINT* >::iterator itEnd2 = s_mapPointInfo.end();
	for( ; it2!=itEnd2; ++it2 )
	{
		delete it2->second;
	}
	s_mapPointInfo.clear();
}
#endif

void CsBaseMng::Delete()
{
	{
		MAP_IT it = m_mapTamer.begin();
		MAP_IT itEnd = m_mapTamer.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapTamer.clear();

		it = m_mapDigimon.begin();
		itEnd = m_mapDigimon.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapDigimon.clear();
	}


	{
		MAP_MAPINFO_IT it = m_mapMapInfo.begin();
		MAP_MAPINFO_IT itEnd = m_mapMapInfo.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapMapInfo.clear();
	}

	{
		MAP_JUMPBUSTER_IT it = m_mapJumpBuster.begin();
		MAP_JUMPBUSTER_IT itEnd = m_mapJumpBuster.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapJumpBuster.clear();
	}
	
	{
		MAP_GUILD_IT it = m_Guild.begin();
		MAP_GUILD_IT itEnd = m_Guild.end();
		for( ; it!=itEnd; ++it )
		{			
			delete it->second;
		}
		m_Guild.clear();
	}

	{
		std::map< int, sPLAY_PANELTY* >::iterator it = m_mapPlayPanelty.begin();
		std::map< int, sPLAY_PANELTY* >::iterator itEnd = m_mapPlayPanelty.end();
		for( ; it!=itEnd; ++it )
		{
			delete it->second;
		}
		m_mapPlayPanelty.clear();
	}
#ifdef GUILD_RENEWAL
	{
		std::map< DWORD, CsGuild_Skill* >::iterator it = m_mapGuildSkill.begin();
		std::map< DWORD, CsGuild_Skill* >::iterator itEnd = m_mapGuildSkill.end();
		for( ; it!=itEnd; ++it )
		{
			delete it->second;
		}
		m_mapGuildSkill.clear();
	}
	m_sGuildBase.Delete();
#endif
	m_PersonStore.Delete();
}

bool CsBaseMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();
		break;
	case nsCsFileTable::FT_BIN:		
		if( _LoadBin( cPath ) == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack( cPath );
		break;
	default:
		assert_cs( false );
	}

	return true;
}

CsBase* CsBaseMng::GetTamerBase(int nLevel, int nTamerType)
{
	if (nLevel < 1)
		nLevel = 1;

	DWORD dwID = nLevel + nTamerType * BASE_CHARTYPE_CONSTANT;

	MAP_IT it = m_mapTamer.find(dwID);
	if (it != m_mapTamer.end())
		return it->second;

	// Fallback 1: tentar level 1 do mesmo tipo de tamer
	DWORD dwLevelOneID = 1 + nTamerType * BASE_CHARTYPE_CONSTANT;
	it = m_mapTamer.find(dwLevelOneID);
	if (it != m_mapTamer.end())
	{
#ifdef _DEBUG
		TCHAR szMsg[256];
		_stprintf_s(szMsg, 256, _T("CsBaseMng::GetTamerBase fallback same type. level=%d type=%d id=%d fallback=%d\n"), nLevel, nTamerType, dwID, dwLevelOneID);
		OutputDebugString(szMsg);
#endif
		return it->second;
	}

	// Fallback 2: tentar Masaru/primeiro tipo conhecido no mesmo level
	DWORD dwMasaruID = nLevel + BT_MASARU * BASE_CHARTYPE_CONSTANT;
	it = m_mapTamer.find(dwMasaruID);
	if (it != m_mapTamer.end())
	{
#ifdef _DEBUG
		TCHAR szMsg[256];
		_stprintf_s(szMsg, 256, _T("CsBaseMng::GetTamerBase fallback Masaru same level. level=%d type=%d id=%d fallback=%d\n"), nLevel, nTamerType, dwID, dwMasaruID);
		OutputDebugString(szMsg);
#endif
		return it->second;
	}

	// Fallback 3: tentar Masaru level 1
	DWORD dwMasaruLevelOneID = 1 + BT_MASARU * BASE_CHARTYPE_CONSTANT;
	it = m_mapTamer.find(dwMasaruLevelOneID);
	if (it != m_mapTamer.end())
	{
#ifdef _DEBUG
		TCHAR szMsg[256];
		_stprintf_s(szMsg, 256, _T("CsBaseMng::GetTamerBase fallback Masaru level 1. level=%d type=%d id=%d fallback=%d\n"), nLevel, nTamerType, dwID, dwMasaruLevelOneID);
		OutputDebugString(szMsg);
#endif
		return it->second;
	}

	// Último recurso: devolver o primeiro base disponível para não rebentar no CharacterCreate
	if (!m_mapTamer.empty())
	{
#ifdef _DEBUG
		TCHAR szMsg[256];
		_stprintf_s(szMsg, 256, _T("CsBaseMng::GetTamerBase fallback first available. level=%d type=%d id=%d\n"), nLevel, nTamerType, dwID);
		OutputDebugString(szMsg);
#endif
		return m_mapTamer.begin()->second;
	}

#ifdef _DEBUG
	TCHAR szMsg[256];
	_stprintf_s(szMsg, 256, _T("CsBaseMng::GetTamerBase failed. empty tamer base map. level=%d type=%d id=%d\n"), nLevel, nTamerType, dwID);
	OutputDebugString(szMsg);
#endif

	return NULL;
}

CsBase* CsBaseMng::GetDigimonBase( int nLevel, int nDigimonType )
{
// #ifndef _GIVE
// 	if( nLevel > 99 )
// 		nLevel = 99;
// #endif

	if( nLevel < 1 )
		nLevel = 1;

	DWORD dwID = nLevel + nDigimonType*BASE_CHARTYPE_CONSTANT;
	assert_csm2( IsDigimonBase( dwID ) == true, _T( "레벨 = %d, 타입 = %d" ), nLevel, nDigimonType );

	MAP_IT it = m_mapDigimon.find( dwID );
	if( it == m_mapDigimon.end() )
		return NULL;

	return it->second;
}

bool CsBaseMng::IsUseJumpBuster( int nJumpBusterItemID, int nDestMapID )
{
	MAP_JUMPBUSTER_IT it = m_mapJumpBuster.find( nJumpBusterItemID );
	if( it == m_mapJumpBuster.end() )
	{
		assert_cs( m_mapJumpBuster.find( nJumpBusterItemID ) != m_mapJumpBuster.end() );
		return false;
	}
	if( NULL == it->second )
		return false;

	return it->second->IsMap(nDestMapID);	
}

void CsBaseMng::GetJumpBusterItemID( std::list< DWORD >* pList, int nDestMapID )
{
	MAP_JUMPBUSTER_IT it = m_mapJumpBuster.begin();
	MAP_JUMPBUSTER_IT itEnd = m_mapJumpBuster.end();

	for( ; it != itEnd; it++ )
	{
		if( it->second->IsMap( nDestMapID ) )
			pList->push_back( it->first );			 
	}	
}

#ifdef CONSIGNMENT_BALLOON_CASH
TCHAR* CsBaseMng::IsBallonNewType( int nTargetID )
{
	std::map< int, sPERSON_STORE::sOBJECT_INFO* >::iterator it = m_PersonStore.s_mapObject.begin();
	std::map< int, sPERSON_STORE::sOBJECT_INFO* >::iterator itEnd = m_PersonStore.s_mapObject.end();

	for( ; it != itEnd; ++it )
	{
		if( it->second->s_nDigimonID != nTargetID )
			continue;

		if( _tcscmp( it->second->s_szFileName, L"Base") != 0 )
			return it->second->s_szFileName;
	}

	return NULL;
}
#endif

#ifdef GUILD_RENEWAL
bool CsBaseMng::_IsGuildBaseSkill( int nSkillCode )
{
	std::list< CsGuild_Skill::sINFO* >::iterator it = m_mapGuildSkill[0]->GetList()->begin();
	std::list< CsGuild_Skill::sINFO* >::iterator itEnd = m_mapGuildSkill[0]->GetList()->end();

	for( ; it != itEnd ; it++ )
	{
		if( nSkillCode == (*it)->s_nSkillCode )
			return true;
	}
	return false;
}
#endif

CsBaseMng::sGuild* CsBaseMng::GetGuildInfo( DWORD dwLevel )
{ 
	MAP_GUILD::iterator it = m_Guild.find( dwLevel );
	if( it == m_Guild.end() )
		return NULL;

	return it->second;
}

void CsBaseMng::_LoadExcel()
{
	_LoadExcel_TamerDigimon();
	_LoadExcel_MapInfo();
	_LoadExcel_Party();	
	_LoadExcel_Guild();	
	_LoadExcel_JumpBuster();
	_LoadExcel_Limit();	
	_LoadExcel_PersonStore();
	_LoadExcel_PlayPanelty();
#ifdef GUILD_RENEWAL
	_LoadExcel_GuildSkill(); 
	_LoadExcel_GuildBase();
#endif
	_LoadExcel_EvolutionBase();

	_LoadExcel_DSkillMaxLevel();
	_LoadExcel_DskillOpenExpansion();
}

void CsBaseMng::_LoadExcel_TamerDigimon()
{
#define IL_COL_LEVEL					0
#define IL_COL_EXP						IL_COL_LEVEL + 1 // 1

#define TAMER_COL_COUNT					6
#define IL_COL_MASARU_START				IL_COL_EXP + 2 // 3
#define IL_COL_TOHMA_START				IL_COL_MASARU_START + TAMER_COL_COUNT
#define IL_COL_YOSHINO_START			IL_COL_TOHMA_START + TAMER_COL_COUNT
#define IL_COL_HANJIHO_START			IL_COL_YOSHINO_START + TAMER_COL_COUNT

#define IL_COL_TAICHI_START				IL_COL_HANJIHO_START + TAMER_COL_COUNT
#define IL_COL_MIMI_START				IL_COL_TAICHI_START + TAMER_COL_COUNT
#define IL_COL_MATT_START				IL_COL_MIMI_START + TAMER_COL_COUNT
#define IL_COL_TAKERU_START				IL_COL_MATT_START + TAMER_COL_COUNT

#define IL_COL_HIKARI_START				IL_COL_TAKERU_START + TAMER_COL_COUNT
#define IL_COL_SORA_START				IL_COL_HIKARI_START + TAMER_COL_COUNT

#define IL_COL_JANG_START				IL_COL_SORA_START + TAMER_COL_COUNT
#define IL_COL_JUNG_START				IL_COL_JANG_START + TAMER_COL_COUNT

#define DIGIMON_COL_COUNT				7
#define IL_COL_SHORT_START				IL_COL_EXP + 2
#define IL_COL_LONG_START				IL_COL_SHORT_START + DIGIMON_COL_COUNT
#define IL_COL_DEX_START				IL_COL_LONG_START + DIGIMON_COL_COUNT
#define IL_COL_DEF_START				IL_COL_DEX_START + DIGIMON_COL_COUNT
	


	char cBuffer[ CEXCLE_CELL_LEN ];

	// Tamer
	{		
#define BASE_TAMER_SHEET_NAME			"Tamer"

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), BASE_TAMER_SHEET_NAME );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );	
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		CsBase::sINFO info;
		while( excel.MoveNext() == true )
		{
			// Level
			info.s_nLevel = atoi( excel.GetData( IL_COL_LEVEL, cBuffer ) );
			if( info.s_nLevel == 0 )
				continue;

			// Exp
			info.s_dwExp = _atoi64( excel.GetData( IL_COL_EXP, cBuffer ) );

			// 마사루
			info.s_nHP = atoi( excel.GetData( IL_COL_MASARU_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_MASARU_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_MASARU_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_MASARU_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_MASARU_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_MASARU_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_MASARU;

			CsBase* pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			// 토우마
			info.s_nHP = atoi( excel.GetData( IL_COL_TOHMA_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_TOHMA_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_TOHMA_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_TOHMA_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_TOHMA_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_TOHMA_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_TOHMA;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			// 요시노
			info.s_nHP = atoi( excel.GetData( IL_COL_YOSHINO_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_YOSHINO_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_YOSHINO_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_YOSHINO_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_YOSHINO_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_YOSHINO_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_YOSHINO;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			// 한지호
			info.s_nHP = atoi( excel.GetData( IL_COL_HANJIHO_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_HANJIHO_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_HANJIHO_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_HANJIHO_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_HANJIHO_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_HANJIHO_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_HANJIHO;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			// 신테일
			info.s_nHP = atoi( excel.GetData( IL_COL_TAICHI_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_TAICHI_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_TAICHI_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_TAICHI_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_TAICHI_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_TAICHI_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_TAICHI;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			//이미나
			info.s_nHP = atoi( excel.GetData( IL_COL_MIMI_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_MIMI_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_MIMI_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_MIMI_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_MIMI_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_MIMI_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_MIMI;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			//메튜
			info.s_nHP = atoi( excel.GetData( IL_COL_MATT_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_MATT_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_MATT_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_MATT_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_MATT_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_MATT_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_MATT;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			//리키
			info.s_nHP = atoi( excel.GetData( IL_COL_TAKERU_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_TAKERU_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_TAKERU_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_TAKERU_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_TAKERU_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_TAKERU_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_TAKERU;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			// 신나리
			info.s_nHP = atoi( excel.GetData( IL_COL_HIKARI_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_HIKARI_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_HIKARI_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_HIKARI_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_HIKARI_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_HIKARI_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_HIKARI;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			// 한소라
			info.s_nHP = atoi( excel.GetData( IL_COL_SORA_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_SORA_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_SORA_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_SORA_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_SORA_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_SORA_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_SORA;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;

			// 장한솔
			info.s_nHP = atoi( excel.GetData( IL_COL_JANG_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_JANG_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_JANG_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_JANG_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_JANG_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_JANG_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_JANG;

			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;
// 
// 			// 정석
			info.s_nHP = atoi( excel.GetData( IL_COL_JUNG_START, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_JUNG_START + 1, cBuffer ) );
			info.s_nMoveSpeed = atoi( excel.GetData( IL_COL_JUNG_START + 2, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_JUNG_START + 3, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_JUNG_START + 4, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_JUNG_START + 5, cBuffer ) );
			info.s_nEvasion = 100;
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_JUNG;
			pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapTamer[ info.s_dwID ] = pBase;
		}
		excel.Close();
	}

	// Digimon
	{		
#define BASE_DIGIMON_SHEET_NAME			"digimon"

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), BASE_DIGIMON_SHEET_NAME );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );	
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		CsBase::sINFO info;
		while( excel.MoveNext() == true )
		{
			// Level
			info.s_nLevel = atoi( excel.GetData( IL_COL_LEVEL, cBuffer ) );
			if( info.s_nLevel == 0 )
				continue;

			// Exp
			info.s_dwExp = _atoi64( excel.GetData( IL_COL_EXP, cBuffer ) );

			// 근거리형
			info.s_nHP = atoi( excel.GetData( IL_COL_SHORT_START + 0, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_SHORT_START + 1, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_SHORT_START + 2, cBuffer ) );
			info.s_nEvasion = atoi( excel.GetData( IL_COL_SHORT_START + 3, cBuffer ) );
			info.s_nCritical = atoi( excel.GetData( IL_COL_SHORT_START + 4, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_SHORT_START + 5, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_SHORT_START + 6, cBuffer ) );
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_SHORT_TYPE;

			CsBase* pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapDigimon[ info.s_dwID ] = pBase;

			// 원거리형형
			info.s_nHP = atoi( excel.GetData( IL_COL_LONG_START + 0, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_LONG_START + 1, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_LONG_START + 2, cBuffer ) );
			info.s_nEvasion = atoi( excel.GetData( IL_COL_LONG_START + 3, cBuffer ) );
			info.s_nCritical = atoi( excel.GetData( IL_COL_LONG_START + 4, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_LONG_START + 5, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_LONG_START + 6, cBuffer ) );
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_LONG_TYPE;

			pBase = pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapDigimon[ info.s_dwID ] = pBase;

			// 민첩
			info.s_nHP = atoi( excel.GetData( IL_COL_DEX_START + 0, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_DEX_START + 1, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_DEX_START + 2, cBuffer ) );
			info.s_nEvasion = atoi( excel.GetData( IL_COL_DEX_START + 3, cBuffer ) );
			info.s_nCritical = atoi( excel.GetData( IL_COL_DEX_START + 4, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_DEX_START + 5, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_DEX_START + 6, cBuffer ) );
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_DEX_TYPE;

			pBase = pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapDigimon[ info.s_dwID ] = pBase;

			// 방어
			info.s_nHP = atoi( excel.GetData( IL_COL_DEF_START + 0, cBuffer ) );
			info.s_nDS = atoi( excel.GetData( IL_COL_DEF_START + 1, cBuffer ) );
			info.s_nDefence = atoi( excel.GetData( IL_COL_DEF_START + 2, cBuffer ) );
			info.s_nEvasion = atoi( excel.GetData( IL_COL_DEF_START + 3, cBuffer ) );
			info.s_nCritical = atoi( excel.GetData( IL_COL_DEF_START + 4, cBuffer ) );
			info.s_nAttack = atoi( excel.GetData( IL_COL_DEF_START + 5, cBuffer ) );
			info.s_nHitRate = atoi( excel.GetData( IL_COL_DEF_START + 6, cBuffer ) );
			info.s_dwID = info.s_nLevel + BASE_CHARTYPE_CONSTANT*BT_DEF_TYPE;

			pBase = pBase = csnew CsBase;
			pBase->Init( &info );

			m_mapDigimon[ info.s_dwID ] = pBase;
		}
		excel.Close();
	}
}

void CsBaseMng::_LoadExcel_MapInfo()
{
#define IL_COL_MAPID					3
#define IL_COL_SHOUT_SEC				2
#define IL_COL_ENABLE_CHECK_MACRO		9
#define IL_COL_CHECK_MACRO_TIME			10
#define BASE_MAPINFO_SHEET_NAME			"MapInfo"

	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), BASE_MAPINFO_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsBaseMapInfo::sINFO info;
	while( excel.MoveNext() == true )
	{
		// MapID
		info.s_nMapID = atoi( excel.GetData( IL_COL_MAPID, cBuffer ) );
		if( info.s_nMapID == 0 )
			continue;

		// Shout Second
		info.s_nShoutSec = atoi( excel.GetData( IL_COL_SHOUT_SEC, cBuffer ) )*1000;

		// 매크로 체크 사용 여부
		info.s_bEnableCheckMacro = ( atoi( excel.GetData( IL_COL_ENABLE_CHECK_MACRO, cBuffer ) ) == 1 );
		
		CsBaseMapInfo* pBase = csnew CsBaseMapInfo;
		pBase->Init( &info );

		m_mapMapInfo[ info.s_nMapID ] = pBase;
	}
	excel.Close();
}

void CsBaseMng::_LoadExcel_Party()
{
#define BASE_PARTY_SHEET_NAME		"Party"

	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), BASE_PARTY_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	excel.MoveNext();

	m_Party.s_fPartyDist = atoi( excel.GetData( 9, cBuffer ) )*100.0f;

	excel.Close();
}

void CsBaseMng::_LoadExcel_JumpBuster()
{
#define IL_COL_ITEMID					0
#define IL_COL_DESTMAPID				1
#define BASE_JUMPBUSTER_SHEET_NAME		"Jumpbooster"

	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Item(), BASE_JUMPBUSTER_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD dwItemID, dwMapID;
	while( excel.MoveNext() == true )
	{
		// ItemID
		dwItemID = atoi( excel.GetData( IL_COL_ITEMID, cBuffer ) );
		// MapID
		dwMapID = atoi( excel.GetData( IL_COL_DESTMAPID, cBuffer ) );

		// 첫 정보
		if( m_mapJumpBuster.find( dwItemID ) == m_mapJumpBuster.end() )
		{
			CsBaseJumpBuster* pJumpBuster = csnew CsBaseJumpBuster;
			m_mapJumpBuster[ dwItemID ] = pJumpBuster;
		}
		
		m_mapJumpBuster[ dwItemID ]->Insert( dwMapID );
	}
	excel.Close();
}

void CsBaseMng::_LoadExcel_Guild()
{
#define BASE_GUILD_SHEET_NAME		"Guild"

	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), BASE_GUILD_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );		
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );		
		
	while( excel.MoveNext() == true )
	{
		// Level
		int nUse = atoi( excel.GetData( 0, cBuffer ) );
		if( nUse == 0 )
			continue;

		sGuild* pBase = csnew sGuild;

		pBase->s_nLevel = atoi( excel.GetData( 1, cBuffer ) );
		pBase->s_nFame = atoi( excel.GetData( 2, cBuffer ) );

		pBase->s_nItemNo1 = atoi( excel.GetData( 3, cBuffer ) );
		pBase->s_nItemCount1 = atoi( excel.GetData( 5, cBuffer ) );
		pBase->s_nItemNo2 = atoi( excel.GetData( 6, cBuffer ) );
		pBase->s_nItemCount2 = atoi( excel.GetData( 8, cBuffer ) );

		pBase->s_nMaxGuildPerson = atoi( excel.GetData( 9, cBuffer ) );
		pBase->s_nIncMember	= atoi( excel.GetData( 10, cBuffer ) );
		pBase->s_nMaxGuild2Master = atoi( excel.GetData( 11, cBuffer ) );
		
		pBase->s_nMasterLevel = atoi( excel.GetData( 12, cBuffer ) );
		pBase->s_nNeedPerson = atoi( excel.GetData( 13, cBuffer ) );

		m_Guild[ pBase->s_nLevel ] = pBase;
	}
	excel.Close();
}

void CsBaseMng::_LoadExcel_Limit()
{
#define SHEET_NAME					"DM_Limit"

	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Limit(), SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );		
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	// 디지몬 보관소
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	m_Limit.s_nMaxTacticsHouse = atoi( excel.GetData( 4, cBuffer ) );
	assert_cs( ( m_Limit.s_nMaxTacticsHouse % 10 ) == 0 );	

	// 창고
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	m_Limit.s_nMaxWareHouse = atoi( excel.GetData( 4, cBuffer ) );
	assert_cs( ( m_Limit.s_nMaxWareHouse % 7 ) == 0 );

	// 조합창고 보관슬롯 수
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	m_Limit.s_nUnionStore = atoi( excel.GetData( 4, cBuffer ) );

#ifdef NEW_SHARESTASH
	// 국내용 공유 창고	엑셀 DM_Limit 수정 2012/8/16 vf00
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	m_Limit.s_nMaxShareStash = atoi( excel.GetData( 4, cBuffer ) );
	assert_cs( ( m_Limit.s_nMaxShareStash % 7 ) == 0 );

#endif
#ifdef CROSSWARS_SYSTEM
	// 크로스 디지몬 슬롯 수
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	m_Limit.s_nMaxCrossTactics = atoi( excel.GetData( 4, cBuffer ) );
	assert_cs( ( m_Limit.s_nMaxCrossTactics % 16 ) == 0 );


	// 크로스 디지몬 슬롯 수
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	m_Limit.s_nMaxCrossInven = atoi( excel.GetData( 4, cBuffer ) );
	assert_cs( ( m_Limit.s_nMaxCrossInven % 15 ) == 0 );
#endif

	bSuccess = excel.MoveNext();
	if( bSuccess )
		excel.Read_Value( 4, m_Limit.s_nConsume_XG );

	bSuccess = excel.MoveNext();
	if( bSuccess )
		excel.Read_Value( 4, m_Limit.s_nCharge_XG );	

	excel.Close();
}

void CsBaseMng::_LoadExcel_PersonStore()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	{
		bool bSuccess;
		cExcelReader excel;
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(),"개인상점" );
			assert_cs( bSuccess == true );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "Individual_Shops" /*"개인상점"*/ );
			assert_cs( bSuccess == true );
			break;
		default:
			assert_cs( false );
		}
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );

		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );
		excel.GetData( 0, cBuffer );
		m_PersonStore.s_BaseInfo.s_fPerson_Charge = (float)atof( cBuffer )/100.0f;

		excel.GetData( 1, cBuffer );
		m_PersonStore.s_BaseInfo.s_fEmployment_Charge = (float)atof( cBuffer )/100.0f;
		m_PersonStore.s_BaseInfo.s_fStoreDist = (float)atoi( excel.GetData( 2, cBuffer ) );

#ifdef CONSIGNMENT_CREATE_RESTRICT
		m_PersonStore.s_BaseInfo.s_fNpcDist = (float)atoi( excel.GetData( 6, cBuffer ) );
#endif

		excel.Close();
	}

	{
		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "Employment" );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );

		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		while( excel.MoveNext() == true )
		{
			int nItemID = atoi( excel.GetData( 0, cBuffer ) );
			sPERSON_STORE::sOBJECT_INFO* pObject = csnew sPERSON_STORE::sOBJECT_INFO;
			pObject->s_nItemID = nItemID;
			pObject->s_nDigimonID = atoi( excel.GetData( 1, cBuffer ) );
			pObject->s_fScale = atoi( excel.GetData( 2, cBuffer ) )/10000.0f;
			pObject->s_nSlotCount = atoi( excel.GetData( 5, cBuffer ) );
#ifdef CONSIGNMENT_BALLOON_CASH
			wchar_t wBuffer[ CEXCLE_CELL_LEN ];
			// 말풍선 파일 이름
			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::KOREA_ORIGINAL:
			case nsCsFileTable::KOREA_TRANSLATION:
			case nsCsFileTable::ENGLISH:
			case nsCsFileTable::ENGLISH_A:
				M2W( pObject->s_szFileName, excel.GetData( 6, cBuffer ), MAX_FILENAME );
				break;
			case nsCsFileTable::THAILAND:
			case nsCsFileTable::TAIWAN:
			case nsCsFileTable::HONGKONG:
				wcscpy_s( pObject->s_szFileName, MAX_FILENAME, excel.GetData( 6, wBuffer ) );
				break;
			default:
				assert_cs( false );
			}
#endif

			assert_cs( m_PersonStore.s_mapObject.find( pObject->s_nItemID ) == m_PersonStore.s_mapObject.end() );
			m_PersonStore.s_mapObject[ pObject->s_nItemID ] = pObject;
		}
		excel.Close();
	}
}


void CsBaseMng::_LoadExcel_PlayPanelty()
{
	char cBuffer[ CEXCLE_CELL_LEN ];
	bool bSuccess;
	cExcelReader excel;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
	case nsCsFileTable::ENGLISH:
	case nsCsFileTable::ENGLISH_A:
		bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "피로도페널티" );
		assert_cs( bSuccess == true );
		break;
	case nsCsFileTable::THAILAND:
	case nsCsFileTable::TAIWAN:
	case nsCsFileTable::HONGKONG:
		bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "Fatigability_Penalty" /*"피로도페널티"*/ );
		assert_cs( bSuccess == true );
		break;
	default:
		assert_cs( false );
	}
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	while( excel.MoveNext() == true )
	{
		int nPLevel = atoi( excel.GetData( 4, cBuffer ) );
		if( nPLevel == 0 )
			continue;

		sPLAY_PANELTY* pObject = csnew sPLAY_PANELTY;
		pObject->s_nPaneltyLevel = nPLevel;
		pObject->s_nExp = atoi( excel.GetData( 5, cBuffer ) );
		pObject->s_nDrop = atoi( excel.GetData( 6, cBuffer ) );
		assert_cs( m_mapPlayPanelty.find( nPLevel ) == m_mapPlayPanelty.end() );
		m_mapPlayPanelty[ nPLevel ] = pObject;
	}

	excel.Close();	
}

#ifdef GUILD_RENEWAL

void CsBaseMng::_LoadExcel_GuildBase()
{
	#define IL_COL_LIMIT_LEVEL			0
	#define IL_COL_CHANGE_RATE			IL_COL_LIMIT_LEVEL + 1	// 1
	#define IL_COL_GUILD_LEVEL			IL_COL_CHANGE_RATE + 1	// 2
	#define IL_COL_REQ_EXP				IL_COL_GUILD_LEVEL + 1	// 3
	#define IL_COL_POINT_TYPE			IL_COL_REQ_EXP + 1		// 4
	#define IL_COL_ADD_POINT			IL_COL_POINT_TYPE + 1	// 5

	char cBuffer[ CEXCLE_CELL_LEN ];
#define BASE_GUILDBASE_SHEET_NAME			"Guild_Base"

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), BASE_GUILDBASE_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	while( excel.MoveNext() == true )
	{
		// 길드 생성 테이머 제한 레벨
		int nLv = atoi( excel.GetData( IL_COL_LIMIT_LEVEL, cBuffer ) );
		if( atoi( excel.GetData( IL_COL_LIMIT_LEVEL, cBuffer ) ) != 0 )
			m_sGuildBase.s_nLimitLv = atoi( excel.GetData( IL_COL_LIMIT_LEVEL, cBuffer ) );

		// GSP 전환 비율
		if( atoi( excel.GetData( IL_COL_CHANGE_RATE, cBuffer ) ) != 0 )
			m_sGuildBase.s_nChangeRate = atoi( excel.GetData( IL_COL_CHANGE_RATE, cBuffer ) );

		// 길드 레벨 별 필요 명성치
		if( atoi( excel.GetData( IL_COL_GUILD_LEVEL, cBuffer ) ) != 0 )
		{
			sGUILDBASE::sLEVELINFO* pLevelInfo = csnew sGUILDBASE::sLEVELINFO;
			pLevelInfo->s_nLevel = atoi( excel.GetData( IL_COL_GUILD_LEVEL, cBuffer ) );
			pLevelInfo->s_nReqExp = atoi( excel.GetData( IL_COL_REQ_EXP, cBuffer ) );
			m_sGuildBase.s_mapLevelInfo[pLevelInfo->s_nLevel] = pLevelInfo;
		}
		// 공헌도 획득 조건에 따른 길드포인트 획득량
		if( atoi( excel.GetData( IL_COL_POINT_TYPE, cBuffer ) ) != 0 )
		{
			sGUILDBASE::sGUILDPOINT* pPointInfo = csnew sGUILDBASE::sGUILDPOINT;
			pPointInfo->s_nType = atoi( excel.GetData( IL_COL_POINT_TYPE, cBuffer ) );
			pPointInfo->s_nPoint = atoi( excel.GetData( IL_COL_ADD_POINT, cBuffer ) );
			m_sGuildBase.s_mapPointInfo[pPointInfo->s_nType] = pPointInfo;
		}
	}
	excel.Close();
}

void CsBaseMng::_LoadExcel_GuildSkill()
{
#define IL_COL_SECTION				0
#define IL_COL_GROUP				IL_COL_SECTION + 1			// 1
#define IL_COL_STEP					IL_COL_GROUP + 1			// 2
#define IL_COL_CONSUME_GSP			IL_COL_STEP + 1				// 3
#define IL_COL_REQ_LEVEL			IL_COL_CONSUME_GSP + 1		// 4
#define IL_COL_SKILLCODE			IL_COL_REQ_LEVEL + 1		// 5
#define IL_COL_NAME					IL_COL_SKILLCODE + 1		// 6
#define IL_COL_COMMENT				IL_COL_NAME + 1				// 7

	char cBuffer[ CEXCLE_CELL_LEN ];

#define BASE_GUILDSKILL_SHEET_NAME			"Guild_Skill"

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), BASE_GUILDSKILL_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsGuild_Skill* pGuildSkill = csnew CsGuild_Skill;
	int nGroupNum = 0;

	while( excel.MoveNext() == true )
	{
		// 같은 그룹은 리스트로 모아서 저장
		if( nGroupNum != atoi( excel.GetData( IL_COL_GROUP, cBuffer ) ) )
		{
			if( pGuildSkill != NULL )
				m_mapGuildSkill[ nGroupNum ] = pGuildSkill;
			nGroupNum = atoi( excel.GetData( IL_COL_GROUP, cBuffer ) );
			pGuildSkill = csnew CsGuild_Skill;
		}			
		CsGuild_Skill::sINFO* pInfo = csnew CsGuild_Skill::sINFO;

		// 스킬 타입
		pInfo->s_nSkillType = atoi( excel.GetData( IL_COL_SECTION, cBuffer ) );
		// 스킬 그룹 ID
		pInfo->s_nGroup = atoi( excel.GetData( IL_COL_GROUP, cBuffer ) );
		// 스킬 단계
		pInfo->s_nSkillLevel = atoi( excel.GetData( IL_COL_STEP, cBuffer ) );
		// 소모 GSP
		pInfo->s_nConsGsp = atoi( excel.GetData( IL_COL_CONSUME_GSP, cBuffer ) );
		// 길드 요구 레벨
		pInfo->s_nReqLevel = atoi( excel.GetData( IL_COL_REQ_LEVEL, cBuffer ) );
		// 스킬 코드
		pInfo->s_nSkillCode = atoi( excel.GetData( IL_COL_SKILLCODE, cBuffer ) );

		TCHAR szBuffer[ 2048 ];
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			// 스킬 이름
			M2W( szBuffer, excel.GetData( IL_COL_NAME, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= MAX_FILENAME -1 )
				CsMessageBox( MB_OK, L"DM_Base의 Guild_Skill시트의 SkillName 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.",  (int)_tcslen( szBuffer ), MAX_FILENAME );
			else
				_tcscpy_s( pInfo->s_szSkillName, MAX_FILENAME, szBuffer );
			// 스킬 설명
			M2W( szBuffer, excel.GetData( IL_COL_COMMENT, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= MAX_FILENAME -1 )
				CsMessageBox( MB_OK, L"DM_Base의 Guild_Skill시트의 SkillComment 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.",  (int)_tcslen( szBuffer ), MAX_FILENAME );
			else
				_tcscpy_s( pInfo->s_szSkillComment, MAX_FILENAME, szBuffer );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			{
				wchar_t wBuffer[ CEXCLE_CELL_LEN ];

				// 스킬 이름
				wcscpy_s( szBuffer, 2048, excel.GetData( IL_COL_NAME, wBuffer ) );
				if( (int)_tcslen( szBuffer ) >= MAX_FILENAME -1 )
					CsMessageBox( MB_OK, L"DM_Base의 Guild_Skill시트의 SkillName 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.",  (int)_tcslen( szBuffer ), MAX_FILENAME );
				wcscpy_s( pInfo->s_szSkillName, MAX_FILENAME, szBuffer );

				// 스킬 설명
				wcscpy_s( szBuffer, 2048, excel.GetData( IL_COL_COMMENT, wBuffer ) );
				if( (int)_tcslen( szBuffer ) >= MAX_FILENAME -1 )
					CsMessageBox( MB_OK, L"DM_Base의 Guild_Skill시트의 SkillComment 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.",  (int)_tcslen( szBuffer ), MAX_FILENAME );
				wcscpy_s( pInfo->s_szSkillComment, MAX_FILENAME, szBuffer );				
			}
			break;
		default:
			assert_cs( false );
		}
		pGuildSkill->InsertList( pInfo );
	}
	m_mapGuildSkill[ nGroupNum ] = pGuildSkill;

	excel.Close();
}
#endif

void CsBaseMng::_LoadExcel_EvolutionBase()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "Evolution_apply" );
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		return;
	}

	excel.FirstOfSkip(2);

	while( excel.MoveNext() )
	{
		int nEvolutionType = 0;
		excel.Read_Value( 0, nEvolutionType );
		if( 0 == nEvolutionType )
			break;

		sBaseEvolutionApply addItem;
		excel.Read_Value( 1, addItem.m_EvolutionTypeName );
		excel.Read_Value( 2, addItem.m_nApplyValue );

		m_mapBaseEvolutionInfos.insert( std::make_pair( nEvolutionType, addItem ) );
	}

}

void CsBaseMng::_LoadExcel_DSkillMaxLevel()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "D_StartSkillLv" );
	if( !bSuccess )
		return;

	excel.FirstOfSkip(2);

	while( excel.MoveNext() )
	{
		int nEvolutionType = 0;
		excel.Read_Value( 0, nEvolutionType );
		if( 0 == nEvolutionType )
			break;

		sEvoMaxkSkillLv addItem;
		excel.Read_Value( 2, addItem.s_SkillExpStartLv );
		int nCol = 3;
		while( 1 )
		{
			int nMaxLv = 0;
			excel.Read_Value( nCol, nMaxLv );
			if( 0 == nMaxLv )
				break;

			addItem.s_SkillMaxLvs.push_back( nMaxLv );
			nCol++;
		}

		auto rib = m_mapDigimonEvoMaxLevel.insert( std::make_pair( nEvolutionType, addItem ) );
		if( !rib.second )
			CsMessageBox( MB_OK, L"DM_Base의 GD_StartSkillLv시트의 중복 진화 타입이 존재 합니다. - %d", nEvolutionType);
	}
}

void CsBaseMng::_LoadExcel_DskillOpenExpansion()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Base(), "D_SkillLvOpen" );
	if( !bSuccess )
		return;

	excel.FirstOfSkip(2);

	while( excel.MoveNext() )
	{
		int nSkillOpenItemSubType = 0;
		excel.Read_Value( 0, nSkillOpenItemSubType );
		if( 0 == nSkillOpenItemSubType )
			break;

		sExpansionCondition addItem;
		excel.Read_Value( 1, addItem.nExpansionRank );

		int nCol = 2;
		while( 1 )
		{
			int nDEvolType = 0;	// 디지몬 진화 타입
			excel.Read_Value( nCol, nDEvolType );
			if( 0 == nDEvolType )
				break;

			addItem.digimonEvoTypes.push_back( nDEvolType );
			nCol += 2;
		}

		auto rib = m_mapDskillOpenExpansionData.insert( std::make_pair( nSkillOpenItemSubType, addItem ) );
		if( !rib.second )
			CsMessageBox( MB_OK, L"DM_Base의 D_SkillLvOpen시트의 중복된 아이템 서브 타입이 존재 합니다. - %d", nSkillOpenItemSubType);
	}
}

#define BASE_TABLE_BIN			"DMBase.bin"
void CsBaseMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, BASE_TABLE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapTamer.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	{
		MAP_IT it = m_mapTamer.begin();
		MAP_IT itEnd = m_mapTamer.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsBase::sINFO ), 1, fp );
		}

		nCount = (int)m_mapDigimon.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		it = m_mapDigimon.begin();
		itEnd = m_mapDigimon.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsBase::sINFO ), 1, fp );
		}
	}

	{
		nCount = (int)m_mapMapInfo.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		MAP_MAPINFO_IT it = m_mapMapInfo.begin();
		MAP_MAPINFO_IT itEnd = m_mapMapInfo.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsBaseMapInfo::sINFO ), 1, fp );
		}
	}

	{
		nCount = (int)m_mapJumpBuster.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		MAP_JUMPBUSTER_IT it = m_mapJumpBuster.begin();
		MAP_JUMPBUSTER_IT itEnd = m_mapJumpBuster.end();
		for( ; it!=itEnd; ++it )
		{
			std::map< DWORD, DWORD >* pMap = it->second->GetMap();
			std::map< DWORD, DWORD >::iterator itEle = pMap->begin();
			std::map< DWORD, DWORD >::iterator itEleEnd = pMap->end();
			nCount = (int)pMap->size();
			DWORD nItemID = it->first;
			fwrite( &nItemID, sizeof( DWORD ), 1, fp );
			fwrite( &nCount, sizeof( int ), 1, fp );			
			for( ; itEle!=itEleEnd; ++itEle )
			{
				fwrite( &itEle->second, sizeof( DWORD ), 1, fp );
			}
		}
	}

	fwrite( &m_Party, sizeof( sPARTY ), 1, fp );

	{
		nCount = (int)m_Guild.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		MAP_GUILD_IT it = m_Guild.begin();
		MAP_GUILD_IT itEnd = m_Guild.end();
		for( ; it!=itEnd; ++it )
		{		
			fwrite( it->second, sizeof( sGuild ), 1, fp );			
		}
	}

	fwrite( &m_Limit, sizeof( sLIMIT ), 1, fp );

	{
		fwrite( &m_PersonStore.s_BaseInfo, sizeof( sPERSON_STORE::sBASE_INFO ), 1, fp );

		std::map< int, sPERSON_STORE::sOBJECT_INFO* >::iterator it = m_PersonStore.s_mapObject.begin();
		std::map< int, sPERSON_STORE::sOBJECT_INFO* >::iterator itEnd = m_PersonStore.s_mapObject.end();
		int nCnt = (int)m_PersonStore.s_mapObject.size();
		fwrite( &nCnt, sizeof( int ), 1, fp );
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second, sizeof( sPERSON_STORE::sOBJECT_INFO ), 1, fp );
		}
	}

	{
		int nCnt = (int)m_mapPlayPanelty.size();
		fwrite( &nCnt, sizeof( int ), 1, fp );

		std::map< int, sPLAY_PANELTY* >::iterator it = m_mapPlayPanelty.begin();
		std::map< int, sPLAY_PANELTY* >::iterator itEnd = m_mapPlayPanelty.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second, sizeof( sPLAY_PANELTY ), 1, fp );
		}
	}
#ifdef GUILD_RENEWAL
	{	// Guild_Base
		fwrite( &m_sGuildBase.s_nLimitLv, sizeof( short ), 1, fp );
		fwrite( &m_sGuildBase.s_nChangeRate, sizeof( int ), 1, fp );

		std::map< int, sGUILDBASE::sLEVELINFO* >::iterator it = m_sGuildBase.s_mapLevelInfo.begin();
		std::map< int, sGUILDBASE::sLEVELINFO* >::iterator itEnd = m_sGuildBase.s_mapLevelInfo.end();
		int nMapSize = (int)m_sGuildBase.s_mapLevelInfo.size();
		fwrite( &nMapSize, sizeof( int ), 1, fp );
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second, sizeof( sGUILDBASE::sLEVELINFO ), 1, fp );
		}

		std::map< int, sGUILDBASE::sGUILDPOINT* >::iterator it2 = m_sGuildBase.s_mapPointInfo.begin();
		std::map< int, sGUILDBASE::sGUILDPOINT* >::iterator itEnd2 = m_sGuildBase.s_mapPointInfo.end();
		nMapSize = (int)m_sGuildBase.s_mapPointInfo.size();
		fwrite( &nMapSize, sizeof( int ), 1, fp );
		for( ; it2!=itEnd2; ++it2 )
		{
			fwrite( it2->second, sizeof( sGUILDBASE::sGUILDPOINT ), 1, fp );
		}
	}

	{	// Guild_Skill
		int nCnt = (int)m_mapGuildSkill.size();

		fwrite( &nCnt, sizeof( int ), 1, fp );

		MAP_GUILDSKILL_IT it = m_mapGuildSkill.begin();
		MAP_GUILDSKILL_IT itEnd = m_mapGuildSkill.end();
		
		for( ; it!=itEnd; ++it )
		{
			nCount = (int)(it->second->GetList()->size());
			fwrite( &nCount, sizeof( int ), 1, fp );

			std::list< CsGuild_Skill::sINFO* >::iterator it2 = it->second->GetList()->begin();
			std::list< CsGuild_Skill::sINFO* >::iterator itEnd2 = it->second->GetList()->end();
			for( ; it2!=itEnd2; ++it2 )
			{
				fwrite( (*it2), sizeof( CsGuild_Skill::sINFO ), 1, fp );
			}
		}
	}
#endif

	{
		int nCount = (int)m_mapBaseEvolutionInfos.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );
		MAP_EVOLUTION_BASE_INFOS_CIT it = m_mapBaseEvolutionInfos.begin();
		for( ; it != m_mapBaseEvolutionInfos.end(); ++it )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(it->first) );
			CmUtil::CmBinSave::Save_Value( fp, it->second.m_EvolutionTypeName );
			CmUtil::CmBinSave::Save_Value( fp, &it->second.m_nApplyValue );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	{
		int nCount = (int)m_mapDigimonEvoMaxLevel.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );
		MAP_DIGIMON_EVOMAXLV_CIT it = m_mapDigimonEvoMaxLevel.begin();
		for( ; it != m_mapDigimonEvoMaxLevel.end(); ++it )
		{
			CmUtil::CmBinSave::Save_Value( fp, &it->first );
			CmUtil::CmBinSave::Save_Value( fp, &it->second.s_SkillExpStartLv );
			
			int nSubCount = (int)it->second.s_SkillMaxLvs.size();
			CmUtil::CmBinSave::Save_Value( fp, &nSubCount );
			LIST_RANK_MAXSKILL_CIT subIT = it->second.s_SkillMaxLvs.begin();
			for( ; subIT != it->second.s_SkillMaxLvs.end(); ++subIT )
				CmUtil::CmBinSave::Save_Value( fp, &(*subIT) );
		}
	}

	{
		int nCount = (int)m_mapDskillOpenExpansionData.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );
		MAP_DSKILL_OPEN_EXPANSION_DATA_CIT it = m_mapDskillOpenExpansionData.begin();
		for( ; it != m_mapDskillOpenExpansionData.end(); ++it )
		{
			CmUtil::CmBinSave::Save_Value( fp, &it->first );

			CmUtil::CmBinSave::Save_Value( fp, &it->second.nExpansionRank );
			
			int nSubCount = (int)it->second.digimonEvoTypes.size();
			CmUtil::CmBinSave::Save_Value( fp, &nSubCount );

			std::list<int>::const_iterator subIT = it->second.digimonEvoTypes.begin();
			for( ; subIT != it->second.digimonEvoTypes.end(); ++subIT )
				CmUtil::CmBinSave::Save_Value( fp, &(*subIT) );
		}
	}

	//////////////////////////////////////////////////////////////////////////


	fclose( fp );
}

bool CsBaseMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, BASE_TABLE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;

	{
		CsBase::sINFO info;
		fread( &nCount, sizeof( int ), 1, fp );
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsBase::sINFO ), 1, fp );

			CsBase* pObject = csnew CsBase;
			pObject->Init( &info );
			// Pack data sometimes contains duplicate tamer-base rows; keep first, skip dups.
			if( m_mapTamer.find( info.s_dwID ) != m_mapTamer.end() )
			{
				delete pObject;
				continue;
			}
			m_mapTamer[ info.s_dwID ] = pObject;
		}

		fread( &nCount, sizeof( int ), 1, fp );
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsBase::sINFO ), 1, fp );

			CsBase* pObject = csnew CsBase;
			pObject->Init( &info );
			assert_cs( m_mapDigimon.find( info.s_dwID ) == m_mapDigimon.end() );
			m_mapDigimon[ info.s_dwID ] = pObject;
		}
	}	

	{
		CsBaseMapInfo::sINFO info;
		fread( &nCount, sizeof( int ), 1, fp );
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsBaseMapInfo::sINFO ), 1, fp );

			CsBaseMapInfo* pObject = csnew CsBaseMapInfo;
			pObject->Init( &info );
			assert_cs( m_mapMapInfo.find( info.s_nMapID ) == m_mapMapInfo.end() );
			m_mapMapInfo[ info.s_nMapID ] = pObject;
		}
	}

	{
		int nDestMapCount;
		DWORD nItemID, nDestMapID;
		fread( &nCount, sizeof( int ), 1, fp );
		for( int i=0; i<nCount; ++i )
		{
			fread( &nItemID, sizeof( DWORD ), 1, fp );
			fread( &nDestMapCount, sizeof( int ), 1, fp );

			assert_cs( m_mapJumpBuster.find( nItemID ) == m_mapJumpBuster.end() );
			CsBaseJumpBuster* pJumpBuster = csnew CsBaseJumpBuster;
			m_mapJumpBuster[ nItemID ] = pJumpBuster;

			for( int dm=0; dm<nDestMapCount; ++dm )
			{
				fread( &nDestMapID, sizeof( DWORD ), 1, fp );
				m_mapJumpBuster[ nItemID ]->Insert( nDestMapID );
			}
		}
	}

	fread( &m_Party, sizeof( sPARTY ), 1, fp );

	{
		fread( &nCount, sizeof( int ), 1, fp );		
		for( int i=0; i<nCount; ++i )
		{		
			sGuild* pGuildInfo = csnew sGuild;
			fread( pGuildInfo, sizeof( sGuild ), 1, fp );	
			m_Guild[ pGuildInfo->s_nLevel ] = pGuildInfo;
		}
	}

	fread( &m_Limit, sizeof( sLIMIT ), 1, fp );

	{
		fread( &m_PersonStore.s_BaseInfo, sizeof( sPERSON_STORE::sBASE_INFO ), 1, fp );

		int nCnt = 0;
		fread( &nCnt, sizeof( int ), 1, fp );
		for( int i=0; i<nCnt; ++i )
		{
			sPERSON_STORE::sOBJECT_INFO* pObject = csnew sPERSON_STORE::sOBJECT_INFO;			
			fread( pObject, sizeof( sPERSON_STORE::sOBJECT_INFO ), 1, fp );

			assert_cs( m_PersonStore.s_mapObject.find( pObject->s_nItemID ) == m_PersonStore.s_mapObject.end() );
			m_PersonStore.s_mapObject[ pObject->s_nItemID ] = pObject;
		}
	}

	{
		int nCnt = 0;
		fread( &nCnt, sizeof( int ), 1, fp );

		for( int i=0; i<nCnt; ++i )
		{
			sPLAY_PANELTY* pObject = csnew sPLAY_PANELTY;
			fread( pObject, sizeof( sPLAY_PANELTY ), 1, fp );

			assert_cs( m_mapPlayPanelty.find( pObject->s_nPaneltyLevel ) == m_mapPlayPanelty.end() );
			m_mapPlayPanelty[ pObject->s_nPaneltyLevel ] = pObject;
		}
	}
#ifdef GUILD_RENEWAL
	{	// Guild_Base
		fread( &m_sGuildBase.s_nLimitLv, sizeof( short ), 1, fp );
		fread( &m_sGuildBase.s_nChangeRate, sizeof( int ), 1, fp );

		int nMapSize = 0;
		fread( &nMapSize, sizeof( int ), 1, fp );
		for( int i=0; i<nMapSize; ++i )
		{
			sGUILDBASE::sLEVELINFO* pLevel = csnew sGUILDBASE::sLEVELINFO;			
			fread( pLevel, sizeof( sGUILDBASE::sLEVELINFO ), 1, fp );

			m_sGuildBase.s_mapLevelInfo[ pLevel->s_nLevel ] = pLevel;
		}

		fread( &nMapSize, sizeof( int ), 1, fp );
		for( int i=0; i<nMapSize; ++i )
		{
			sGUILDBASE::sGUILDPOINT* pPoint = csnew sGUILDBASE::sGUILDPOINT;			
			fread( pPoint, sizeof( sGUILDBASE::sGUILDPOINT ), 1, fp );

			m_sGuildBase.s_mapPointInfo[ pPoint->s_nType ] = pPoint;
		}
	}

	{	// Guild_Skill
		int nCount;
		int	nListSize;
		fread( &nCount, sizeof( int ), 1, fp );

		for( int i=0; i<nCount; ++i )
		{
			CsGuild_Skill* pGiuldSkill = csnew CsGuild_Skill;		

			int nGroupIdx = 0;
			fread( &nListSize, sizeof( int ), 1, fp );
			for( int j = 0; j < nListSize; j++ )
			{
				CsGuild_Skill::sINFO* pInfo = csnew CsGuild_Skill::sINFO;		
				fread( pInfo, sizeof( CsGuild_Skill::sINFO ), 1, fp );			
				nGroupIdx = pInfo->s_nGroup;
				pGiuldSkill->InsertList( pInfo );
			}

			m_mapGuildSkill[ nGroupIdx ] = pGiuldSkill;
		}
	}
#endif

	{
		int nCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			int nType = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nType );
			if( 0 == nType )
			{
				assert_cs( false );
				break;
			}
		
			sBaseEvolutionApply readItem;
			CmUtil::CmBinRead::Read_Value( fp, readItem.m_EvolutionTypeName );
			CmUtil::CmBinRead::Read_Value( fp, &readItem.m_nApplyValue );
			m_mapBaseEvolutionInfos.insert( std::make_pair( nType, readItem ) );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	{
		int nCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			int nEvoType = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nEvoType );
			if( 0 == nEvoType )
				break;

			sEvoMaxkSkillLv addItem;
			CmUtil::CmBinRead::Read_Value( fp, &addItem.s_SkillExpStartLv );

			int nSubCount = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nSubCount );
			for( int n = 0; n < nSubCount; ++n )
			{
				int nMaxLv = 0;
				CmUtil::CmBinRead::Read_Value( fp, &nMaxLv );	
				if( 0 == nMaxLv )
					break;

				addItem.s_SkillMaxLvs.push_back( nMaxLv );
			}

			m_mapDigimonEvoMaxLevel.insert( std::make_pair( nEvoType, addItem ) );
		}
	}

	{
		int nCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			int nOpenItemSubType = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nOpenItemSubType );
			if( 0 == nOpenItemSubType )
				break;

			sExpansionCondition addItem;
			CmUtil::CmBinRead::Read_Value( fp, &addItem.nExpansionRank );
			int nSubCount = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nSubCount );
			for( int n = 0; n < nSubCount; ++n )
			{
				int nEvoType = 0;
				CmUtil::CmBinRead::Read_Value( fp, &nEvoType );
				if( 0 == nEvoType )
					break;

				addItem.digimonEvoTypes.push_back( nEvoType );
			}

			m_mapDskillOpenExpansionData.insert( std::make_pair( nOpenItemSubType, addItem ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	fclose( fp );
	return true;
}

void CsBaseMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, BASE_TABLE_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	
	{
		CsBase::sINFO info;
		_read( nHandle, &nCount, sizeof( int ) );
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsBase::sINFO ) );

			CsBase* pObject = csnew CsBase;
			pObject->Init( &info );
			// Pack data sometimes contains duplicate tamer-base rows; keep first, skip dups.
			if( m_mapTamer.find( info.s_dwID ) != m_mapTamer.end() )
			{
				delete pObject;
				continue;
			}
			m_mapTamer[ info.s_dwID ] = pObject;
		}

		_read( nHandle, &nCount, sizeof( int ) );
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsBase::sINFO ) );

			CsBase* pObject = csnew CsBase;
			pObject->Init( &info );
			assert_cs( m_mapDigimon.find( info.s_dwID ) == m_mapDigimon.end() );
			m_mapDigimon[ info.s_dwID ] = pObject;
		}
	}

	{
		CsBaseMapInfo::sINFO info;
		_read( nHandle, &nCount, sizeof( int ) );
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsBaseMapInfo::sINFO ) );

			CsBaseMapInfo* pObject = csnew CsBaseMapInfo;
			pObject->Init( &info );
			assert_cs( m_mapMapInfo.find( info.s_nMapID ) == m_mapMapInfo.end() );
			m_mapMapInfo[ info.s_nMapID ] = pObject;
		}
	}

	{
		int nDestMapCount;
		DWORD nItemID, nDestMapID;
		_read( nHandle, &nCount, sizeof( int ) );
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &nItemID, sizeof( DWORD ) );
			_read( nHandle, &nDestMapCount, sizeof( int ) );

			assert_cs( m_mapJumpBuster.find( nItemID ) == m_mapJumpBuster.end() );
			CsBaseJumpBuster* pJumpBuster = csnew CsBaseJumpBuster;
			m_mapJumpBuster[ nItemID ] = pJumpBuster;

			for( int dm=0; dm<nDestMapCount; ++dm )
			{
				_read( nHandle, &nDestMapID, sizeof( DWORD ) );
				m_mapJumpBuster[ nItemID ]->Insert( nDestMapID );
			}
		}
	}

	_read( nHandle, &m_Party, sizeof( sPARTY ) );

	{
		_read( nHandle, &nCount, sizeof( int ) );
		for( int i=0; i<nCount; ++i )
		{		
			sGuild* pGuildInfo = csnew sGuild;
			_read( nHandle, pGuildInfo, sizeof( sGuild ) );	
			m_Guild[ pGuildInfo->s_nLevel ] = pGuildInfo;
		}
	}

	_read( nHandle, &m_Limit, sizeof( sLIMIT ) );

	{
		_read( nHandle, &m_PersonStore.s_BaseInfo, sizeof( sPERSON_STORE::sBASE_INFO ) );

		int nCnt = 0;
		_read( nHandle, &nCnt, sizeof( int ) );
		for( int i=0; i<nCnt; ++i )
		{
			sPERSON_STORE::sOBJECT_INFO* pObject = csnew sPERSON_STORE::sOBJECT_INFO;			
			_read( nHandle, pObject, sizeof( sPERSON_STORE::sOBJECT_INFO ) );

			assert_cs( m_PersonStore.s_mapObject.find( pObject->s_nItemID ) == m_PersonStore.s_mapObject.end() );
			m_PersonStore.s_mapObject[ pObject->s_nItemID ] = pObject;
		}
	}

	{
		int nCnt = 0;
		_read( nHandle, &nCnt, sizeof( int ) );

		for( int i=0; i<nCnt; ++i )
		{
			sPLAY_PANELTY* pObject = csnew sPLAY_PANELTY;
			_read( nHandle, pObject, sizeof( sPLAY_PANELTY ) );

			assert_cs( m_mapPlayPanelty.find( pObject->s_nPaneltyLevel ) == m_mapPlayPanelty.end() );
			m_mapPlayPanelty[ pObject->s_nPaneltyLevel ] = pObject;
		}
	}
#ifdef GUILD_RENEWAL
	{	// Guild_Base
		_read( nHandle, &m_sGuildBase.s_nLimitLv, sizeof( short ) );
		_read( nHandle, &m_sGuildBase.s_nChangeRate, sizeof( int ) );

		int nMapSize = 0;
		_read( nHandle, &nMapSize, sizeof( int ) );
		for( int i=0; i<nMapSize; ++i )
		{
			sGUILDBASE::sLEVELINFO* pLevel = csnew sGUILDBASE::sLEVELINFO;			
			_read( nHandle, pLevel, sizeof( sGUILDBASE::sLEVELINFO ) );

			m_sGuildBase.s_mapLevelInfo[ pLevel->s_nLevel ] = pLevel;
		}

		_read( nHandle, &nMapSize, sizeof( int ) );
		for( int i=0; i<nMapSize; ++i )
		{
			sGUILDBASE::sGUILDPOINT* pPoint = csnew sGUILDBASE::sGUILDPOINT;			
			_read( nHandle, pPoint, sizeof( sGUILDBASE::sGUILDPOINT ) );

			m_sGuildBase.s_mapPointInfo[ pPoint->s_nType ] = pPoint;
		}
	}

	{	// Guild_Skill
		int nCount;
		int	nListSize;
		int nGroupIdx;
		_read( nHandle, &nCount, sizeof( int ) );

		for( int i=0; i<nCount; ++i )
		{
			CsGuild_Skill* pGuildSkill = csnew CsGuild_Skill;		

			_read( nHandle, &nListSize, sizeof( int ) );
			for( int j = 0; j < nListSize; j++ )
			{
				CsGuild_Skill::sINFO* pInfo = csnew CsGuild_Skill::sINFO;		
				_read( nHandle, pInfo, sizeof( CsGuild_Skill::sINFO ) );			
				nGroupIdx = pInfo->s_nGroup;
				pGuildSkill->InsertList( pInfo );
			}
			m_mapGuildSkill[ nGroupIdx ] = pGuildSkill;
		}
	}
#endif


	{
		int nCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			int nType = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nType );
			if( 0 == nType )
			{
				assert_cs( false );
				break;
			}

			sBaseEvolutionApply readItem;
			CmUtil::CmPackRead::Read_Value( nHandle, readItem.m_EvolutionTypeName );
			CmUtil::CmPackRead::Read_Value( nHandle, &readItem.m_nApplyValue );
			m_mapBaseEvolutionInfos.insert( std::make_pair( nType, readItem ) );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	{
		int nCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			int nEvoType = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nEvoType );
			if( 0 == nEvoType )
				break;

			sEvoMaxkSkillLv addItem;
			CmUtil::CmPackRead::Read_Value( nHandle, &addItem.s_SkillExpStartLv );
			int nSubCount = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nSubCount );
			for( int n = 0; n < nSubCount; ++n )
			{
				int nMaxLv = 0;
				CmUtil::CmPackRead::Read_Value( nHandle, &nMaxLv );	
				if( 0 == nMaxLv )
					break;

				addItem.s_SkillMaxLvs.push_back( nMaxLv );
			}

			m_mapDigimonEvoMaxLevel.insert( std::make_pair( nEvoType, addItem ) );
		}
	}

	{
		int nCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			int nOpenItemSubType = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nOpenItemSubType );
			if( 0 == nOpenItemSubType )
				break;

			sExpansionCondition addItem;
			CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nExpansionRank );
			int nSubCount = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nSubCount );
			for( int n = 0; n < nSubCount; ++n )
			{
				int nEvoType = 0;
				CmUtil::CmPackRead::Read_Value( nHandle, &nEvoType );
				if( 0 == nEvoType )
					break;

				addItem.digimonEvoTypes.push_back( nEvoType );
			}

			m_mapDskillOpenExpansionData.insert( std::make_pair( nOpenItemSubType, addItem ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}


int CsBaseMng::GetEvoApplyValue(int const& nEvoType) const
{
	MAP_EVOLUTION_BASE_INFOS_CIT it = m_mapBaseEvolutionInfos.find( nEvoType );
	if( it == m_mapBaseEvolutionInfos.end() )
		return 0;

	return it->second.m_nApplyValue;
}

std::wstring CsBaseMng::GetEvoTypeName( int const& nEvoType ) const
{
	MAP_EVOLUTION_BASE_INFOS_CIT it = m_mapBaseEvolutionInfos.find( nEvoType );
	if( it == m_mapBaseEvolutionInfos.end() )
		return L"Unknown";

	return it->second.m_EvolutionTypeName;
}

CsBaseMng::MAP_EVOLUTION_BASE_INFOS const& CsBaseMng::GetBaseEvoInfos() const
{
	return m_mapBaseEvolutionInfos;
}

//////////////////////////////////////////////////////////////////////////
// 사용하려는 아이템이 진화 타입과 같은지 체크
bool CsBaseMng::IsEnableSkillMaxLvUpFromEvoType( int const& nEvoType, int const& nUseItemSubType ) const
{
	MAP_DSKILL_OPEN_EXPANSION_DATA_CIT it = m_mapDskillOpenExpansionData.find( nUseItemSubType );
	if( it == m_mapDskillOpenExpansionData.end() )
		return false;

	return it->second.IsEnableEvoType( nEvoType );
}

int CsBaseMng::GetOpenSkillRankFromItemSubType( int const& nUseItemSubType ) const
{
	MAP_DSKILL_OPEN_EXPANSION_DATA_CIT it = m_mapDskillOpenExpansionData.find( nUseItemSubType );
	if( it == m_mapDskillOpenExpansionData.end() )
		return 0;

	return it->second.nExpansionRank;
}

int	CsBaseMng::GetBeforOpenSkillRankeFromItemSubType( int const& nUseItemSubType ) const
{
	return GetOpenSkillRankFromItemSubType(nUseItemSubType) - 1;
}

int CsBaseMng::GetSkillRankFromSkillMaxLv( int const& nEvoType, int const& nSkillMaxLv ) const
{
	MAP_DIGIMON_EVOMAXLV_CIT it = m_mapDigimonEvoMaxLevel.find( nEvoType );
	if( it == m_mapDigimonEvoMaxLevel.end() )
		return 0;

	LIST_RANK_MAXSKILL_CIT subIT = it->second.s_SkillMaxLvs.begin();
	for( int n = 0; subIT != it->second.s_SkillMaxLvs.end(); ++subIT, ++n )
	{
		if( nSkillMaxLv <= *subIT )
			return n;
	}

	return 0;
}

int CsBaseMng::GetSkillMaxLvFromItemSubType( int const& nEvoType, int const& nUseItemSubType ) const
{
	MAP_DIGIMON_EVOMAXLV_CIT it = m_mapDigimonEvoMaxLevel.find( nEvoType );
	if( it == m_mapDigimonEvoMaxLevel.end() )
		return 0;

	int nSkillRanke = GetOpenSkillRankFromItemSubType( nUseItemSubType );
	LIST_RANK_MAXSKILL_CIT subIT = it->second.s_SkillMaxLvs.begin();
	for( int n = 0; subIT != it->second.s_SkillMaxLvs.end(); ++subIT, ++n )
	{
		if( n != nSkillRanke )
			continue;
		return *subIT;
	}

	return 0;
}

__int64 CsBaseMng::GetDigimonSkillMaxExp( int const& nEvoType, int const& nSkillExpLv )
{
	MAP_DIGIMON_EVOMAXLV_CIT it = m_mapDigimonEvoMaxLevel.find( nEvoType );
	if( it == m_mapDigimonEvoMaxLevel.end() )
		return 0;

	int nSkillTableLv = it->second.s_SkillExpStartLv + nSkillExpLv;
	CsBase* pBaseTable = GetDigimonBase( nSkillTableLv, CsBaseMng::BT_DIGIMON_EXP );
	SAFE_POINTER_RETVAL( pBaseTable, 0 );
	CsBase::sINFO* pInfo = pBaseTable->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, 0 );
	return pInfo->s_dwExp;
}