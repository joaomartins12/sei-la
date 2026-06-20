
#pragma once

class cFileBless : public cGlobal
{
public:
	cFileBless();
	~cFileBless();

public:
	struct sINFO : public NiMemObject
	{
		sINFO(){ s_pText = NULL; s_pText2 = NULL; s_pText3 = NULL; s_pText4 = NULL;	s_bRaidDie = false;
		}
		~sINFO(){ Delete(); }		

		void				Init( TCHAR* szText );
		void				Delete();
		cGlobal::eState		Update(float const& fDeltaTime);
		void				Render();

		cText*			s_pText;
		cText*			s_pText2;
		cText*			s_pText3;
		cText*			s_pText4;

		bool			s_bRaidDie;

		float			s_fAlpha;
		float			s_fTime;

		cGlobal::eState			s_eState;

		void			Init( TCHAR const* szTamerName, TCHAR const* szDigimonName, TCHAR const* szMonsterName, TCHAR const* szItemBuff );
	};


protected:
	std::list< sINFO* >	m_listInfo;

	bool					m_bRaidDie;
	float					m_fAccumTime;		// m_pBuffAni Ani Time
	float					m_fAlpha;			// m_pBuffAni Alpha
	cGlobal::eState			m_eState;			// m_pBuffAni State
	cSpriteAni*				m_pBuffAni;			// 메세지 배경 이미지

public:
	void					Delete();
	virtual cGlobal::eState	Update(float const& fDeltaTime);

	bool					Render();

	void					SetRaidInfo( TCHAR const* szMonsterName, TCHAR const* szTamerName, TCHAR const* szDigimonName, TCHAR const* szItemBuff );
	void					RaidMsgBGUpdate();

};
