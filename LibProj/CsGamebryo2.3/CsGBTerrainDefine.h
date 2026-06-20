
#pragma once 

// 오브젝트 고유 ID

#define OBJECT_UNIQ_ID_START	10001
enum eOBJECT_UNIQID
{
};

// 카메라
#define CAMERA_DIST_ANI_ACCEL_CONSTANT			1100.0f
#define	CAMERA_DIST_ANI_ACCEL_CONSTANT_TOOL		2200.0f

// 패쓰엔진
#define CSPATHENGINE_EXPORTER_VERSION			0x00010001			// Path Engine Exporter tool 과 맞춰주자

// 미터 사이즈가 변하면 기존에 제작한 모든 맵은 못쓰게 된다.
#define METER_SIZE						100

#define MAP_RES_VERSION					0x00010013
#define MAPTOOL_RES_VERSION				0x00010000

#define OBJECT_PATH_LEN					160
#define OBJECT_NAME_LEN					32

#define DETAIL_MAP_0_SHADER_NTM			0
#define SHADOW_MAP_SHADER_NTM			1
#define DETAIL_MAP_1_SHADER_NTM			2
#define DETAIL_MAP_2_SHADER_NTM			3
#define DETAIL_MAP_3_SHADER_NTM			4
#define ALPHA_MAP_SHADER_NTM			5
#define EFFECT_MAP_SHADER_NTM			6
#define DETAIL_MAP_4_SHADER_NTM			7

#define DMAP_0_SHADER_CORD				10000
#define DMAP_1_SHADER_CORD				1000
#define DMAP_2_SHADER_CORD				100
#define DMAP_3_SHADER_CORD				10
#define DMAP_4_SHADER_CORD				1

#define ALPHA_MAP_FORMAT				NiPixelFormat::RGBA32
#define ALPHA_MAP_STRIDE				4
#define SHADOW_MAP_FORMAT				NiPixelFormat::RGB24
#define SHADOW_MAP_STRIDE				3
#define EFFECT_MAP_FORMAT				NiPixelFormat::RGBA32		// 24비트는 명암 집어 넣는데 사용 하였다. 오직 마지막 8비트만 남는다.
#define EFFECT_MAP_STRIDE				4


#define GET_SHADER( shader_name )		( NiD3DShaderFactory::GetInstance()->FindLibrary( "NiD3DXEffectShaderLib" )->\
										GetShader( nsCsGBTerrain::g_pRenderer, shader_name, 0 ) )
#define GET_TEXTUREPROPERTY( ninode )			( (NiTexturingProperty*)((NiTriStrips*)ninode->GetAt( 0 ))->GetProperty( NiTexturingProperty::GetType() ) )
#define GET_SHADERMAP( ninode, ntm )			( GET_TEXTUREPROPERTY( ninode )->GetShaderMap( ntm ) )
#define GET_SHADERMAP_PIXELDATA( ninode, ntm )	( ( (NiSourceTexture*)GET_SHADERMAP( ninode, ntm )->GetTexture() )->GetSourcePixelData() )
#define GET_TP_SHADERMAP( texProp, ntm )			( texProp->GetShaderMap( ntm ) )
#define GET_TP_SHADERMAP_PIXELDATA( texProp, ntm )	( ( (NiSourceTexture*)GET_TP_SHADERMAP( texProp, ntm )->GetTexture() )->GetSourcePixelData() )

#define PORTAL_MIN_LIMIT							1000
#define MAPPORTAL_TO_PORTAL( map_portal_ID )		( map_portal_ID%1000 )
#define PORTAL_TO_MAPPORTAL( mapID, portalID )		( portalID < 1000 ? mapID*1000 + portalID : mapID*1000 + MAPPORTAL_TO_PORTAL( portalID ) )


// ==== Typedef
class CsGBTerrainNode;
typedef std::map< DWORD, CsGBTerrainNode* >					MAP_GBTERRAIN_NODE;
typedef std::map< DWORD, CsGBTerrainNode* >::iterator		MAP_GBTERRAIN_NODE_IT;
class CsGBObject;
typedef std::map< DWORD, CsGBObject* >						MAP_GBTERRAIN_OBJECT;
typedef std::map< DWORD, CsGBObject* >::iterator			MAP_GBTERRAIN_OBJECT_IT;
typedef std::list< CsGBObject* >							LIST_GBTERRAIN_OBJECT;
typedef std::list< CsGBObject* >::iterator					LIST_GBTERRAIN_OBJECT_IT;

class CsGBTerrainRoot;
class CsGBObjectResMng;
class CsGBDX9Renderer;
class CsGBSorting;
class CsGBTErrainMng;
class CsSoundMng;

namespace nsCsGBTerrain
{
	extern bool					g_bReloadMap;
	extern DWORD				g_nSvrLibType;
	// Pointer	
	extern CsGBTerrainRoot*		g_pCurRoot;
	extern CsGBDX9Renderer*		g_pRenderer;
	extern CsGBObjectResMng		g_ObjectResMgr;
	extern CsSoundMng*			g_pCsSoundMng;

	//extern NiVisibleArray*		g_pObjectVisibleArray;
	//extern NiVisibleArray*		g_pFirst1ObjVisibleArray;

	// TexFilter
	extern NiTexturingProperty::FilterMode	g_eTexFilter;
	// Shadow
	extern bool								g_bShadowRender;
	// CharOutLine
	extern bool					g_bCharOutLine;
	// Cell
	extern bool					g_bSpeedCellRender;

	// Device Confirm
	struct sDevice
	{
		sDevice(){ g_bPixelShader2 = true; g_bUseMyShader = true; g_nMultiTextureCount = 0; }
		int					g_nMultiTextureCount;
		bool				g_bPixelShader2;
		bool				g_bUseMyShader;
	};
	extern sDevice			g_Device;
	
	// View
	struct sTRView
	{
		sTRView(){ g_fViewDetailDist = 5.0f;	g_fViewVisibleDist = 10000.0f; }
		float				g_fViewDetailDist;
		float				g_fViewVisibleDist;
	};
	extern sTRView			g_TRView;
	
	// Save Load
	struct sSaveLoad
	{
		sSaveLoad(){ g_bLoadPathEngineWarning = true;	g_bSaveBMIncludeSM = true; }
		bool				g_bLoadPathEngineWarning;
		bool				g_bSaveBMIncludeSM;
	};
	extern sSaveLoad		g_SaveLoad;

	// Type
	enum eCREATE_TYPE{ CT_NONE, CT_MAKE, CT_FILELOAD_BYTOOL, CT_FILELOAD, };
	extern eCREATE_TYPE		g_eCreateType;
}








