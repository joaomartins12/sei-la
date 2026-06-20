
#pragma once 

class cAchieveInfo : public NiMemObject
{
public:
	static void		GlobalInit();
	static void		GlobalShotDown();

public:
	cAchieveInfo();
	~cAchieveInfo();

public:
	enum eState{
		ALPHA_INCREASE, LIVE, ALPHA_DECREASE, DIE,
	};

	struct sINFO : public NiMemObject
	{
		sINFO(){ s_pText = NULL; s_pTextLayer = NULL; }
		~sINFO(){ Delete(); }		

		void		Init( TCHAR const* szText );
		void		Delete();
		eState		Update(float const& fDeltaTime);
		void		Render();

		cText*			s_pText;
		cSprite*		s_pTextLayer;

		float			s_fAlpha;
		float			s_fTime;

		eState			s_eState;
	};

protected:
	std::list< sINFO* >	m_listInfo;

public:
	void			Delete();

	void			SetAchieveInfo( TCHAR const* szText );

	void			Update(float const& fDeltaTime);
	bool			Render();
};

extern cAchieveInfo*		g_pAchieveInfo;