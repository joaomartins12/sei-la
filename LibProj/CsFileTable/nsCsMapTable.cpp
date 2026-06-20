
#include "stdafx.h"
#include "nsCsMapTable.h"


bool					nsCsMapTable::g_bUseMapList = true;
CsMapListMng*			nsCsMapTable::g_pMapListMng = NULL;

bool					nsCsMapTable::g_bUseMapStart = true;
CsMapStartMng*			nsCsMapTable::g_pMapStartMng = NULL;

bool					nsCsMapTable::g_bUseMapPortal = true;
CsMapPortalMng*			nsCsMapTable::g_pMapPortalMng = NULL;

nsCsMapTable::eMode		nsCsMapTable::g_eModeMapMonster = nsCsMapTable::eMode_Tool;
CsMapMonsterMng*		nsCsMapTable::g_pMapMonsterMng = NULL;

bool					nsCsMapTable::g_bUseMapNpc = true;
CsMapNpcMng*			nsCsMapTable::g_pMapNpcMng = NULL;

bool					nsCsMapTable::g_bUseMapRegion = true;
CsMapRegionMng*			nsCsMapTable::g_pMapRegionMng = NULL;

bool					nsCsMapTable::g_bUseMapResurrection = true;
CsMapResurrectionMng*	nsCsMapTable::g_pMapResurrectionMng = NULL;

bool					nsCsMapTable::g_bUseMapCharLight = true;
CsMapCharLightMng*		nsCsMapTable::g_pMapCharLightMng = NULL;

bool					nsCsMapTable::g_bUseWeather = true;
CsWeatherMng*			nsCsMapTable::g_pWeatherMng = NULL;

bool					nsCsMapTable::g_bUseTableObj = true;
CsMapTableObjMng*		nsCsMapTable::g_pMapTableObjMng = NULL;

bool					nsCsMapTable::g_bUseActObject = true;
CsMapObjectMng*			nsCsMapTable::g_pActObjectMng = NULL;
