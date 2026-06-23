
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

void cData_MapRegion::InRegion(int nRegionIndex)
{
	if (nRegionIndex >= nLimit::Region)
		return;

	if (!nsCsGBTerrain::g_pCurRoot)
		return;

	if (!nsCsGBTerrain::g_pCurRoot->GetInfo())
		return;

	if (!nsCsMapTable::g_pMapListMng)
		return;

	const int nMapIDX = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	CsMapList* pMapList = nsCsMapTable::g_pMapListMng->GetList(nMapIDX);
	if (!pMapList)
		return;

	CsMapList::sINFO* pMapInfo = pMapList->GetInfo();
	if (!pMapInfo)
		return;

	DWORD nMapRegionID = pMapInfo->s_nMapRegionID;

	// Se o RegionID vier inválido do server/tabela, não deixa corromper array.
	if (nMapRegionID >= nLimit::Map)
		return;

	// 원래 오픈되어 있지 않은 맵이라면
	if (m_MapRegionBackup.IsOpened(nMapRegionID) == false)
	{
		GS2C_RECV_CHECKTYPE recv;

		recv.nType = AchieveContents::CA_MAP_1;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

		recv.nType = AchieveContents::CA_MAP_2;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

		recv.nType = AchieveContents::CA_MAP_3;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

		recv.nType = AchieveContents::CA_MAP_4;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

		recv.nType = AchieveContents::CA_MAP_5;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

		recv.nType = AchieveContents::CA_MAP_6;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

		recv.nType = AchieveContents::CA_MAP_ALL;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);
	}

	switch (nRegionIndex)
	{
	case CsGBTerrainRoot::CR_INVALIDE:
		break;

	case CsGBTerrainRoot::CR_WORLD:
	{
		// Em debug antigo isto podia rebentar se o server ainda não abriu a região.
		// Mantemos seguro para localhost/emulador.
		if (!IsNetOff() && m_MapRegion.IsOpened(nMapRegionID) == false)
			return;
	}
	break;

	default:
	{
		int nSafeMapRegionID = nsCsMapTable::g_pMapListMng->MapIDToMapRegionID(nMapIDX);

		if (nSafeMapRegionID < 0 || nSafeMapRegionID >= nLimit::Map)
			return;

		if (m_MapRegion.SetOpenRegion(nSafeMapRegionID, nRegionIndex) == true)
		{
			if (net::game)
			{
				net::game->SendOpenRegion(nRegionIndex);

				if (g_pDataMng && g_pDataMng->GetQuest())
				{
					g_pDataMng->GetQuest()->RevQuestCheck();
					g_pDataMng->GetQuest()->CalProcess();
				}
			}
		}
	}
	break;
	}
}

bool cData_MapRegion::IsOpenedWorld(int nWorldID)
{
	if (!nsCsFileTable::g_pWorldMapMng)
		return false;

	CsWorldMap* pWorld = nsCsFileTable::g_pWorldMapMng->GetWorld(nWorldID);
	if (!pWorld)
		return false;

	std::list< CsAreaMap* >* pList = pWorld->GetAreaList();
	if (!pList)
		return false;

	std::list< CsAreaMap* >::iterator it = pList->begin();
	std::list< CsAreaMap* >::iterator itEnd = pList->end();

	for (; it != itEnd; ++it)
	{
		CsAreaMap* pArea = (*it);
		if (!pArea)
			continue;

		CsAreaMap::sINFO* pFTArea = pArea->GetInfo();
		if (!pFTArea)
			continue;

		if (IsOpenedMap(pFTArea->s_nMapID) == true)
			return true;
	}

	return false;
}

bool cData_MapRegion::IsOpenedMap(int nMapID)
{
	if (!nsCsMapTable::g_pMapListMng)
		return false;

	CsMapList* pList = nsCsMapTable::g_pMapListMng->GetList(nMapID);
	if (!pList)
		return false;

	CsMapList::sINFO* pInfo = pList->GetInfo();
	if (!pInfo)
		return false;

	USHORT ID = pInfo->s_nMapRegionID;

	if (ID >= nLimit::Map)
		return false;

	return m_MapRegion.IsOpened(ID);
}

