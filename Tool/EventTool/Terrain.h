#pragma once
#include "ShaderHelper.h"

class CTerrain : public NiMemObject
{
public:
	CTerrain(void);
	~CTerrain(void);

public:
	void			Init();
	void			Delete();

	void			Update();
	void			Render();	

public:
	DWORD			m_dwMapId; /**< 현재 로드한 맵번호*/	

	//=====================================================================================	
	//	쉐이더	
	//=====================================================================================
protected:
	ShaderHelper			m_ShaderHelper;

	//=====================================================================================	
	//	디폴트 라이트 - 지형이 없을 경우에 쓰는 라이트	
	//=====================================================================================
protected:
	NiDirectionalLightPtr	m_pDefaultLight;
protected:
	void				_CreateDefaultLight();
	void				_DeleteDefaultLight();


};

extern CTerrain g_Terrain;
extern CsGBTerrainMng*	g_pTerrainMng;