
#include "stdafx.h"
#include "Data_MapRegion.h"

cData_MapRegion::cData_MapRegion()
{
	for( int i=0; i<nLimit::Map; ++i )
	{
		m_MapRegion.m_nOpenRegion[ i ] = 0;
		m_MapRegionBackup.m_nOpenRegion[ i ] = 0;
	}
}

void cData_MapRegion::Delete()
{
}

void cData_MapRegion::Init()
{
	m_bFirstLoad = true;
}

void cData_MapRegion::LogOut()
{
	m_bFirstLoad = true;
}

void cData_MapRegion::InitNetOff()
{
	for( int i=0; i<nLimit::Map; ++i )
		m_MapRegion.m_nOpenRegion[ i ] = 0;
}

void cData_MapRegion::InRegion( int nRegionIndex )
{
	DWORD nMapRegionID = nsCsMapTable::g_pMapListMng->GetList( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )->GetInfo()->s_nMapRegionID;
	// 원래 오픈되어 있지 않은 맵이라면
	if( m_MapRegionBackup.IsOpened( nMapRegionID ) == false )
	{
		//==========================================================================================================
		// 업적
		//==========================================================================================================
		GS2C_RECV_CHECKTYPE recv;
		recv.nType = AchieveContents::CA_MAP_1;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );

		recv.nType = AchieveContents::CA_MAP_2;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );

		recv.nType = AchieveContents::CA_MAP_3;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );

		recv.nType = AchieveContents::CA_MAP_4;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );

		recv.nType = AchieveContents::CA_MAP_5;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );

		recv.nType = AchieveContents::CA_MAP_6;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );

		recv.nType = AchieveContents::CA_MAP_ALL;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
	}

	if( nRegionIndex >= nLimit::Region )
		return;

	switch( nRegionIndex )
	{
	case CsGBTerrainRoot::CR_INVALIDE:
		break;
	case CsGBTerrainRoot::CR_WORLD:
		{
			assert_cs( IsNetOff() ||( m_MapRegion.IsOpened( nMapRegionID ) == true ) );
		}
		break;
	default:
		{
			int nMapIDX = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
			int nMapRegionID = nsCsMapTable::g_pMapListMng->MapIDToMapRegionID( nMapIDX );
			if( m_MapRegion.SetOpenRegion( nMapRegionID, nRegionIndex ) == true )
			{
				if( net::game )
				{
					net::game->SendOpenRegion( nRegionIndex );
					g_pDataMng->GetQuest()->RevQuestCheck();
					g_pDataMng->GetQuest()->CalProcess();
				}
			}
		}		
	}
}

bool cData_MapRegion::IsOpenedWorld( int nWorldID )
{
	std::list< CsAreaMap* >* pList = nsCsFileTable::g_pWorldMapMng->GetWorld( nWorldID )->GetAreaList();
	std::list< CsAreaMap* >::iterator it = pList->begin();
	std::list< CsAreaMap* >::iterator itEnd = pList->end();
	CsAreaMap::sINFO* pFTArea = NULL;
	for( ; it!=itEnd; ++it )
	{
		pFTArea = (*it)->GetInfo();
		if( IsOpenedMap( pFTArea->s_nMapID ) == true )
			return true;
	}
	return false;	
}

bool cData_MapRegion::IsOpenedMap( int nMapID )
{
	CsMapList*	pList =nsCsMapTable::g_pMapListMng->GetList( nMapID );

	SAFE_POINTER_RETVAL(pList, false);

	CsMapList::sINFO*	pInfo = pList->GetInfo();
	SAFE_POINTER_RETVAL(pInfo, false);

	USHORT	ID = pInfo->s_nMapRegionID;
	return m_MapRegion.IsOpened( ID );
}

