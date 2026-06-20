
#pragma once 

class CsGBLightMng
{
public:
	struct sINFO
	{
		void Reset(){ s_nLightCount = 0; }
		int		s_nLightCount;
	};
	
public:
	CsGBLightMng();
	~CsGBLightMng(){}

protected:	
	sINFO							m_Info;
	CsGBLight*						m_pDynamicShadowLight;
	std::list<  CsGBLight* >		m_listLight;
	std::list < CsGBLight* >		m_listLightObject;
	std::list < CsGBLight* >		m_listLightChar;

	int								m_nCharDirection;
	int								m_nCharPoint;

	int								m_nTrDirection;
	int								m_nTrPoint;
	char							m_cTrShaderExtra[ 16 ];

public:
	void			Delete();
	void			Init();	

	void				DeleteLight( CsGBLight* pLight );
	CsGBLight_Dir*		AddDirectionLight( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Dir::sINFO* dirInfo, bool bLoad );
	CsGBLight_Point*	AddPointLight( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Point::sINFO* pointInfo, bool bLoad );
	char*				GetShaderTrExt(){ return m_cTrShaderExtra; }	

protected:
	void			_InsertList( CsGBLight* pLight, CsGBLight::sBASE_INFO* baseInfo, bool bLoad );

public:
	void			ApplyObject( NiNode* pNiNode );
	void			ApplyChar( NiNode* pNiNode );
	void			DetachChar( NiNode* pNiNode );

	void			ShaderApply_Char();
	void			MakeTrShaderExtra();
	std::list< CsGBLight* >*	GetLightList(){ return &m_listLight; }
	int				GetLightCount(){ return (int)m_listLight.size(); }
	CsGBLight*		GetLight( int nIndex );
	sINFO*			GetInfo(){ return &m_Info; }
	CsGBLight*		GetDynamicShadowLight(){ return m_pDynamicShadowLight; }

public:
	void			AddObject( CsGBLight* pLight );
	void			RemoveObject( CsGBLight* pLight );
	void			AddChar( CsGBLight* pLight );
	void			RemoveChar( CsGBLight* pLight );

	std::list < CsGBLight* >*	GetCharLightList(){ return &m_listLightChar; }

public:
	void			SaveExtraData( CsNiNodePtr pNiNode );
	void			LoadExtraData( CsNiNodePtr pNiNode );

public:
	bool			IsOnlyCharLight( CsGBLight* pLight );

	//==================================================================================
	//		쉐이더
	//==================================================================================
public:
	void			SetLight( float fLight );
	void			SetLight( float fLightDelta, float fLimit );
	void			ChangeShaderLightRate( float fRate );

public:
	float			m_fLight;	//현재 light값
	
};