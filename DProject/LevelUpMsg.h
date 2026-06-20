
#pragma once

class cGlobal : public NiMemObject
{
public:
	cGlobal() {};
	virtual ~cGlobal() {};

public:
	enum eState{ ALPHA_INCREASE, LIVE, ALPHA_DECREASE, DIE,	};

	virtual eState	Update(float const& fDeltaTime) = 0;
	virtual bool	Render() = 0;

};

class cLevelUpMsg : public cGlobal
{
public:
	cLevelUpMsg();
	virtual ~cLevelUpMsg();

public:
	struct sINFO : public NiMemObject
	{
		sINFO(){ s_pText = NULL; s_pText2 = NULL; s_pText3 = NULL; s_pText4 = NULL; s_pText5 = NULL; s_pText6 = NULL; }
		~sINFO(){ Delete(); }		

		void				Init( TCHAR const* szText, TCHAR const* szText2 = NULL, TCHAR const* szText3 = NULL, 
								TCHAR const* szText4 = NULL, TCHAR const* szText5 = NULL, TCHAR const* szText6 = NULL );
		void				Delete();
		cGlobal::eState		Update(float const& fDeltaTime);
		void				Render();

		cText*			s_pText;
		cText*			s_pText2;
		cText*			s_pText3;
		cText*			s_pText4;
		cText*			s_pText5;
		cText*			s_pText6;

		float			s_fAlpha;
		float			s_fTime;

		cGlobal::eState			s_eState;
	};

protected:
	std::list< sINFO* >	m_listInfo;

	bool					m_bMsgDie;
	float					m_fAccumTime;		// m_pBuffAni Ani Time
	float					m_fAlpha;			// m_pBuffAni Alpha
	cGlobal::eState			m_eState;			// m_pBuffAni State
	cSpriteAni*				m_pMsgAni;			// 메세지 배경 이미지

public:
	void					Delete();

	cGlobal::eState			Update(float const& fDeltaTime);
	bool					Render();

	void					SetMessage( TCHAR const* szText, TCHAR const* szText2, TCHAR const* szText3 );
	void					BackGroundAniUpdate(float const& fDeltaTime);
};

