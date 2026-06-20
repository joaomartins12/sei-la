

#pragma once 


class CsMapListMng;
class CsMapStartMng;
class CsMapPortalMng;
class CsMapMonsterMng;
class CsMapNpcMng;
class CsMapRegionMng;
class CsMapResurrectionMng;
class CsMapCharLightMng;
class CsWeatherMng;
class CsMapTableObjMng;
class CsMapObjectMng;
namespace nsCsMapTable
{
	enum eMode { eMode_None = 0, eMode_Tool, eMode_Client };

	extern bool						g_bUseMapList;
	extern CsMapListMng*			g_pMapListMng;

	extern bool						g_bUseMapStart;
	extern CsMapStartMng*			g_pMapStartMng;

	extern bool						g_bUseMapPortal;
	extern CsMapPortalMng*			g_pMapPortalMng;

	extern eMode					g_eModeMapMonster;
	extern CsMapMonsterMng*			g_pMapMonsterMng;

	extern bool						g_bUseMapNpc;
	extern CsMapNpcMng*				g_pMapNpcMng;

	extern bool						g_bUseMapRegion;
	extern CsMapRegionMng*			g_pMapRegionMng;

	extern bool						g_bUseMapResurrection;
	extern CsMapResurrectionMng*	g_pMapResurrectionMng;

	extern bool						g_bUseMapAreaBuff;
	extern CsMapResurrectionMng*	g_pMapAreaBuff;

	extern bool						g_bUseMapCharLight;
	extern CsMapCharLightMng*		g_pMapCharLightMng;

	extern bool						g_bUseWeather;
	extern CsWeatherMng*			g_pWeatherMng;

	extern bool						g_bUseTableObj;
	extern CsMapTableObjMng*		g_pMapTableObjMng;

	extern bool						g_bUseActObject;
	extern CsMapObjectMng*			g_pActObjectMng;
}