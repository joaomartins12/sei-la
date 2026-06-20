#pragma once

#define WEATHER_TEXTURE_DEPTH	7
class CsGBWeather : public NiMemObject
{
public:
	CsGBWeather();
	virtual ~CsGBWeather();

	enum eWeather { eType_Snow = 0, eType_Rain, eType_End };

public:
	static	void	GlobalShotInit();
	static	void	GlobalShotDown();

public:
	void		Delete();
	
	void		Render();
	void		Update( float fDeltaTime, D3DXMATRIX* matView );

	void		SetType( eWeather eType ); 
	void		SetPerformance( int nPerformance );  
	void		SetLevel( int nLevel );
	void		Off();
	void		SetPos( NiPoint3 vPos )				{ m_vPos = vPos; }						
	void		LevelIncrease()						{ if( m_nLevel < WEATHER_TEXTURE_DEPTH - 1 ) m_nLevel++; }
	void		LevelDecrease()						{ if( m_nLevel > 0 ) m_nLevel--; }									

protected:
	void		_Destroy();
	void		_Init();
	void		_SetTexture( bool bFileLoad );
	void		_UpdateLevel();

	void		_DeleteRain();	
	void		_RendRain();
	void		_MakeRain( float fDeltaTime );
	float		_GetRandomFloat( float low, float high );

protected:
	eWeather				m_eType;
	CsTimeSeq				m_LevelTime;

	int			m_nLevel;	//희망레벨
	int			m_nCurLevel;	//현재레벨
	int			m_nPerformance;	//옵션 성능
	int			m_nData[ 3 ];

	NiPoint3	m_vPos;
	float	 	m_fVelocity;
	float		m_fAccumTime;

	bool		m_bRender;
	bool		m_bOption;

protected:
	struct sRainINFO : public NiMemObject
	{
	public:
		sRainINFO(){ s_AccumTime = 0.0f; s_vPos = NiPoint3::ZERO; }	

		NiPoint3		s_vPos;
		float			s_AccumTime;
	};
	CsNodeObj					m_NodeRainParticle;
	std::list < sRainINFO* >	m_RainInfoList;
	float						m_fRainEndTime;

	CsNodeObj					m_NodeFar;
	NiPoint3					m_vFarPos;

	CsNodeObj					m_NodeFarFar;
	NiPoint3					m_vFarFarPos;
		
	CsNodeObj					m_NodeNear;
	NiPoint3					m_vNearPos;

	CsNodeObj					m_NodeSnowParticle;		//캐릭터 둘러싼 눈 파티클
	NiPoint3					m_vSnowParticlePos;

	CsNodeObj					m_NodeSnowParticle2;	//카메라 가까운 눈 파티클
	NiPoint3					m_vSnowParticlePos2;

	NiSourceTexturePtr			m_pTexture[ WEATHER_TEXTURE_DEPTH + 1 ];
	NiSourceTexturePtr			m_pTexture2[ WEATHER_TEXTURE_DEPTH + 1 ];
	
};

extern CsGBWeather*		g_pWeather;
