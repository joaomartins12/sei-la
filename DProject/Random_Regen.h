

#pragma once

class cRandomRegen : public cGlobal
{
public:
	cRandomRegen();
	~cRandomRegen();

public:
	struct sINFO : public NiMemObject
	{
		sINFO(){ s_pText = NULL; s_pText2 = NULL; s_pMonName = NULL;
		}
		~sINFO(){ Delete(); }		

		void				Init( TCHAR const* szFirst, TCHAR const* szMonName, TCHAR const* szSecond );
		void				Delete();
		cGlobal::eState		Update(float const& fDeltaTime);
		void				Render();

		cText*			s_pText;
		cText*			s_pText2;
		cText*			s_pMonName;

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

	void			SetRandomRegenStart( TCHAR const* szMonsterName );
	void			SetRandomRegenEnd( TCHAR const* szMonsterName );
	void			RandomRegendMsgBGUpdate();

};
