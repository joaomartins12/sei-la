
#pragma once 

class cRegionInfo : public NiMemObject
{
public:
	static void		GlobalInit();
	static void		GlobalShotDown();

public:
	cRegionInfo();
	~cRegionInfo();

public:
	enum eState{
		ALPHA_INCREASE, LIVE, ALPHA_DECREASE,
	};
	enum eTYPE{ 
		MAIN_REGION, SUB_RESION,
	};

	struct sINFO : public NiMemObject
	{
		sINFO(){ s_pText = NULL; s_pTextLayer = NULL; }
		~sINFO(){ Delete(); }		

		void		Init( TCHAR* szText, cRegionInfo::eTYPE type );
		void		Delete();
		bool		Update(float const& fDeltaTime);
		void		Render();

		cText*			s_pText;
		cSprite*		s_pTextLayer;

		float			s_fAlpha;
		float			s_fTime;

		eTYPE			s_eType;
		eState			s_eState;
	};

protected:
	TCHAR			m_szText[ MAX_PATH ];
	sINFO*			m_pCurInfo;
	sINFO*			m_pOldInfo;
	

public:
	void			Delete();

	void			SetRegionInfo( TCHAR const* szText, cRegionInfo::eTYPE type );

	void			Update(float const& fDeltaTime);
	void			Render();
};

extern cRegionInfo*		g_pRegionInfo; 