
#pragma once 

class CsGBLight_Dir : public CsGBLight
{
public:
	struct sINFO
	{
		sINFO(){ s_Rotation.MakeZRotation( -NI_HALF_PI ); s_Pos = NiPoint3( 0, 0, 8000.0f ); s_fScale = 6.0f; }
		NiMatrix3	s_Rotation;

		// 툴상에서 시각 적으로 보기 위한 것들
		NiPoint3	s_Pos;
		float		s_fScale;
	};

public:
	CsGBLight_Dir();
	virtual ~CsGBLight_Dir(){ Delete(); }

protected:
	NiDirectionalLightPtr	m_pLight;
	sINFO					m_Info;

public:
	virtual NiLight*	GetNiLight(){ return m_pLight; }

	void			Delete();
	void			Create( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Dir::sINFO* Info );
	void			Reset( CsGBLight::sBASE_INFO* baseInfo, CsGBLight_Dir::sINFO* Info );

public:
	sINFO*			GetInfo(){ return &m_Info; }

	//	state
public:
	void			SetRotation( NiMatrix3 rot );
	void			SetPower( float fPower );
	void			SetDiffuseColor( NiColor color );
	void			SetAmbientColor( NiColor color );
	void			SetSpecularColor( NiColor color );
};