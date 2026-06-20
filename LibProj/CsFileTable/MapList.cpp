
#include "stdafx.h"
#include "MapList.h"

CsMapList::sINFO::sINFO():s_dwResurrectionMapID(0),s_nFatigue_Type(0),s_nHeight(0),s_nWidth(0)
,s_nFatigue_DeBuff(0),s_nFatigue_StartTime(0),s_nFatigue_AddTime(0),s_nFatigue_AddPoint(0)
,s_nCamera_Max_Level(0),s_bXgConsumeType(false),s_bBattleTagUse(false),s_nMapRegionID(0)
,s_dwMapID(0)
{
}

CsMapList::sINFO::~sINFO()
{

}

void CsMapList::sINFO::Init()
{
	s_cMapName.clear();
	s_cMapPath.clear();
	s_szMapDiscript.clear();
	s_szMapDiscript_Eng.clear();
	s_cBGSound.clear();

	s_dwMapID = 0;
	s_nWidth = 0;
	s_nHeight = 0;
	s_nMapRegionID = 0;
	s_dwResurrectionMapID = 0;
	s_nFatigue_Type = 0;
	s_nFatigue_DeBuff = 0;
	s_nFatigue_StartTime = 0;
	s_nFatigue_AddTime = 0;
	s_nFatigue_AddPoint = 0;
	s_nCamera_Max_Level = 0;
	s_bXgConsumeType = false;
	s_bBattleTagUse = false;
}
//////////////////////////////////////////////////////////////////////////

CsMapList::CsMapList()
{
}

CsMapList::~CsMapList()
{

}

void CsMapList::Delete()
{

}

void CsMapList::Init( sINFO* pSrcInfo )
{
	m_Info.s_cMapName = pSrcInfo->s_cMapName;
	m_Info.s_cMapPath = pSrcInfo->s_cMapPath;
	m_Info.s_nHeight = pSrcInfo->s_nHeight;
	m_Info.s_nWidth = pSrcInfo->s_nWidth;
	m_Info.s_dwMapID = pSrcInfo->s_dwMapID;
	m_Info.s_szMapDiscript = pSrcInfo->s_szMapDiscript;
	m_Info.s_szMapDiscript_Eng = pSrcInfo->s_szMapDiscript_Eng;
	m_Info.s_cBGSound = pSrcInfo->s_cBGSound;
	m_Info.s_nMapRegionID = pSrcInfo->s_nMapRegionID;
	m_Info.s_dwResurrectionMapID = pSrcInfo->s_dwResurrectionMapID;
	m_Info.s_nFatigue_Type = pSrcInfo->s_nFatigue_Type;
	m_Info.s_nFatigue_DeBuff = pSrcInfo->s_nFatigue_DeBuff;
	m_Info.s_nFatigue_StartTime = pSrcInfo->s_nFatigue_StartTime;
	m_Info.s_nFatigue_AddTime = pSrcInfo->s_nFatigue_AddTime;
	m_Info.s_nFatigue_AddPoint = pSrcInfo->s_nFatigue_AddPoint;
	m_Info.s_nCamera_Max_Level = pSrcInfo->s_nCamera_Max_Level;
	m_Info.s_bXgConsumeType = pSrcInfo->s_bXgConsumeType;
	m_Info.s_bBattleTagUse = pSrcInfo->s_bBattleTagUse;
	//memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

