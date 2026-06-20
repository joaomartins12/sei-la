
#pragma once 


class CsGBLight : public NiMemObject
{
public:
	enum eTYPE
	{
		DIRECTION, POINT,
	};

	struct sBASE_INFO
	{
		sBASE_INFO(){
			s_bEnableLight = true; s_bObjectShadow = false; s_bTRShadow = true; s_bApplyObject = true; s_bApplyChar = false;
			s_bApplyDynamicShadow = false;
			s_Diffuse = NiColor(0.3f, 0.3f, 0.3f);		s_Ambient = NiColor(0.9f, 0.9f, 0.9f);		s_Specular = NiColor::WHITE;
			s_fPower = 1.0f;
		}

		CsGBLight::eTYPE	s_eType;		
		bool				s_bEnableLight;
		bool				s_bObjectShadow;
		bool				s_bTRShadow;
		bool				s_bApplyObject;
		bool				s_bApplyChar;
		bool				s_bApplyDynamicShadow;
		NiColor				s_Diffuse;
		NiColor				s_Ambient;		
		float				s_fPower;
		NiColor				s_Specular;
		char				s_Dummy[ 16 ];
	};

public:
	CsGBLight();
	virtual ~CsGBLight(){}

	void				Create( CsGBLight::sBASE_INFO* baseInfo );


protected:
	sBASE_INFO			m_BaseInfo;

public:
	sBASE_INFO*			GetBaseInfo(){ return &m_BaseInfo; }
	DWORD				GetType(){ return m_BaseInfo.s_eType; }
	bool				IsEnable(){ return m_BaseInfo.s_bEnableLight; }
	

public:	
	virtual NiLight*	GetNiLight() = 0;
};