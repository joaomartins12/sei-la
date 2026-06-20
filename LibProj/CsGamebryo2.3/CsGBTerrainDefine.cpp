
#include "stdafx.h"
#include "CsGBTerrainDefine.h"

bool							nsCsGBTerrain::g_bReloadMap = false;
DWORD							nsCsGBTerrain::g_nSvrLibType = 1;

// Pointer
CsGBTerrainRoot*				nsCsGBTerrain::g_pCurRoot = NULL;
CsGBDX9Renderer*				nsCsGBTerrain::g_pRenderer = NULL;
CsGBObjectResMng				nsCsGBTerrain::g_ObjectResMgr;
CsSoundMng*						nsCsGBTerrain::g_pCsSoundMng = NULL;

// TexFilter
NiTexturingProperty::FilterMode	nsCsGBTerrain::g_eTexFilter = NiTexturingProperty::FILTER_TRILERP;
bool							nsCsGBTerrain::g_bShadowRender = true;
bool							nsCsGBTerrain::g_bCharOutLine = true;
bool							nsCsGBTerrain::g_bSpeedCellRender = true;

// Device Confirm
nsCsGBTerrain::sDevice			nsCsGBTerrain::g_Device;

nsCsGBTerrain::sTRView			nsCsGBTerrain::g_TRView;

nsCsGBTerrain::sSaveLoad		nsCsGBTerrain::g_SaveLoad;

// Type
nsCsGBTerrain::eCREATE_TYPE		nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_NONE;