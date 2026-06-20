#include "StdAfx.h"
#include "Terrain.h"

CTerrain g_Terrain;
CsGBTerrainMng*		g_pTerrainMng = NULL;

CTerrain::CTerrain(void)
{
}

CTerrain::~CTerrain(void)
{
}

void CTerrain::Delete()
{
	// 지형 매니져 제거
	if( g_pTerrainMng != NULL )
	{
		g_pTerrainMng->Delete();
		SAFE_NIDELETE( g_pTerrainMng );
	}

	_DeleteDefaultLight();
}

void CTerrain::Init()
{
	NiImageConverter::SetImageConverter( NiNew NiDevImageConverter );
	NiTexture::SetMipmapByDefault(true);
	NiSourceTexture::SetUseMipmapping(true);	
	NiMaterial::SetDefaultWorkingDirectory(".\\Data\\Shaders\\Generated");	
	NiSourceTexture::SetDestroyAppDataFlag( false );

	_CreateDefaultLight();

	char cShader[ MAX_PATH ] = ".\\Data\\Shaders";
	char* apcShaderDirectories[1];
	apcShaderDirectories[0] = cShader;
	m_ShaderHelper.SetupShaderSystem( NULL, 0, apcShaderDirectories, 1 );

#ifdef _DEBUG
	nsCsGBTerrain::g_eTexFilter = NiTexturingProperty::FILTER_NEAREST;
#endif

	// 지형 매니져
	assert_cs( g_pTerrainMng == NULL );
	g_pTerrainMng = NiNew CsGBTerrainMng;
	g_pTerrainMng->Init( g_pRenderer,
		8,
		true,
		true,
		15000.0f, 50000.0f );

	CsMapList::sINFO* pListInfo = nsCsMapTable::g_pMapListMng->GetList( 1 )->GetInfo();	

	g_pTerrainMng->CreateRoot_ByTool( pListInfo->s_dwMapID );
	nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFog( false );	

	m_dwMapId = 1;
}

void CTerrain::Update()
{	
	g_Camera.Update_ByTool( g_fDeltaTime );
	g_pTerrainMng->Update_ByTool( g_fDeltaTime );	
	
}

void CTerrain::Render()
{
	g_pTerrainMng->BeginRender();
	g_pTerrainMng->RenderTerrain_ByTool( false );
	g_pTerrainMng->RenderObject_ByTool( g_fDeltaTime, true );
	g_pTerrainMng->EndRender();

}

//=====================================================================================
//	디폴트 라이트
//=====================================================================================
void CTerrain::_CreateDefaultLight()
{
	assert_cs( m_pDefaultLight == NULL );
	m_pDefaultLight = NiNew NiDirectionalLight;
	m_pDefaultLight->SetDiffuseColor( NiColor(0.3f, 0.3f, 0.3f) );
	m_pDefaultLight->SetAmbientColor( NiColor(0.9f, 0.9f, 0.9f) );	

	NiMatrix3 mat;
	mat.MakeYRotation( -NI_HALF_PI*0.5f );
	m_pDefaultLight->SetRotate( mat );
	m_pDefaultLight->Update( 0.0f );

	// 쓰레드 리소스 매니져에 연결
	g_pThread->GetResMng()->SetDefaultLight( m_pDefaultLight );
}

void CTerrain::_DeleteDefaultLight()
{
	m_pDefaultLight = 0;
}