
#pragma once 

class CsGBLight_Point : public CsGBLight
{
public:
	struct sINFO
	{
		sINFO(){ s_Pos = NiPoint3::ZERO; s_C = s_Q = 0.0f; s_L = 1.0f; s_Range = 2000.0f; }
		NiPoint3		s_Pos;
		float			s_C;
		float			s_L;
		float			s_Q;
		float			s_Range;
		void			CalConstant( float range, float c ){ s_C = c; s_L = CsMax( 0.0f, (10-c)/range ); s_Q = 0.0f; }
		float			CalConstant( float dist ){ float fv = 1.0f/( s_C + s_L*dist + s_Q*dist*dist ); float result = CsMin( 1.0f, fv ); return result < 0.0f ? 1.0f : result; }
	};

public:
	CsGBLight_Point();
	virtual ~CsGBLight_Point(){ Delete(); }

protected:
	NiPointLightPtr		m_pLight;
	sINFO				m_Info;

	int					m_nTRApplyCount;
	int*				m_pTRApplyID;

public:
	virtual NiLight*	GetNiLight(){ return m_pLight; }

	void			Delete();
	void			Create( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Point::sINFO* Info );
	void			Reset( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Point::sINFO* Info );

public:
	sINFO*			GetInfo(){ return &m_Info; }

public:
	bool			IsRange( NiPoint3 pos ){ return ( m_Info.s_Pos - pos ).Length() < m_Info.s_Range; }

	//	state
public:
	void			SetPosition( NiPoint3 pos );
	void			SetPower( float fPower );
	void			SetConstant( float c, float l, float q );
	void			SetRange( float range, float c );
	void			SetDiffuseColor( NiColor color );
	void			SetAmbientColor( NiColor color );
	void			SetSpecularColor( NiColor color );
};