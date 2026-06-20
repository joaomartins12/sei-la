
#pragma once 

#define MAX_SYSTEM_MSG			5
#define SYSTEM_TEXT_LEN			222

class cSystemMsg : public NiMemObject
{
public:
	struct sMSG_INFO : public NiMemObject
	{
		void			Delete();
		void			Init();
		bool			Update();
		int				Render( int ny, bool bFirst, int nType = 0 );
		void			SetString( TCHAR* szText, bool bExist );

		float			s_fLifeTime;
		TCHAR			s_szText[ SYSTEM_TEXT_LEN ];
		NiColor			s_Color;
		cStringList*	s_pString;
	};

	enum eMSG_TYPE{ 
		MSG_SYSTEM			= 0,
		MSG_MONSTER_SKILL	= 1,
		MSG_TYPE_MAX,
	};

protected:
	eMSG_TYPE			m_eMsgType;

protected:
	int								m_nLastIndex;
	TCHAR							m_szLastMsg[ SYSTEM_TEXT_LEN ];

	sMSG_INFO	m_MsgInfo[ MAX_SYSTEM_MSG ];
	std::stack< sMSG_INFO* >		m_stackMsgInfo;
	std::list< sMSG_INFO* >			m_listMsgInfo;
	cSprite*						m_pBGSprite;

public:
	static void GlobalInit();
	static void GlobalShotDown();

public:
	void		Delete();
	void		Init( int nType = 0 );

	void		ResetMap();

	void		Update();
	void		Render();

	void		AddSystemMessage( TCHAR* szMsg );
	int			GetSysyemMessageSize() { return m_listMsgInfo.size(); }
	//void		AddSystemMsg( TCHAR* szMsg ) { AddSystemMessage(szMsg); }

protected:
	void		_Parcing( TCHAR* szMsg, void* pValue1, void* pValue2, void* pValue3 );
	bool		_ParcingToString( TCHAR* szString, TCHAR* szParcing, void* pValue1, void* pValue2, void* pValue3 );
};

extern cSystemMsg*		g_pSystemMsg;
extern cSystemMsg*		g_pSkillMsg;
