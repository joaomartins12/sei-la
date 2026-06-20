
#pragma once

class cData_FriendList
{
public:
	enum
	{
		FRIENDLIST_MAX_FRIENDCOUNT = 50,
		FRIENDLIST_MAX_SHUTOUTCOUNT = 50,
	};

public:
	void		Delete();
	void		LogOut() { Delete(); m_bSendCheck = false; } 
	void		Init();	

public:
	enum eONLINE_STATE{ OFFLINE, ONLINE, NO_EXIST, };
	struct sFRIEND_INFO : public NiMemObject
	{
		sFRIEND_INFO() { memset( s_szName, 0, sizeof( s_szName ) ); memset( s_szMemo, 0, sizeof( s_szMemo ) ); }

		void			SetName( const std::wstring& NewName );

		eONLINE_STATE	s_eOnlineState;		
		TCHAR			s_szName[ Language::pLength::name ];
		TCHAR			s_szMemo[ Language::pLength::str ];
	};

	struct sSHUTOUT_INFO : public NiMemObject
	{
		sSHUTOUT_INFO() { memset( s_szName, 0, sizeof( s_szName ) ); memset( s_szMemo, 0, sizeof( s_szMemo ) ); }

		void			SetName( const std::wstring& NewName );

		TCHAR		s_szName[ Language::pLength::name ];
		TCHAR		s_szMemo[ Language::pLength::str ];
	};

protected:
	CsVectorPB< sFRIEND_INFO* >		m_vpFriend;  // 模备 格废
	CsVectorPB< sSHUTOUT_INFO* >	m_vpShutout; // 瞒窜 格废
	bool							m_bSendCheck;

public:
	CsVectorPB< sFRIEND_INFO* >*	GetFriendListVector(){ return &m_vpFriend; }
	CsVectorPB< sSHUTOUT_INFO* >*	GetShutoutListVector(){ return &m_vpShutout; }	

public:
	void			SendFriendInfo();
	void			InitFirend( TCHAR const* szName, TCHAR const* szMemo, int nOnline );

	void			AddFriend( TCHAR const* szName, int nOnline );		
	void			AddFriendMemo( TCHAR const* szName, TCHAR const* szMemo );
	void			FriendOnOff( TCHAR const* szName, int nOnline );	
	
	bool			CheckFriend( TCHAR const* szName );	
	sFRIEND_INFO*	GetFriendInfo( int nIndex );
	sFRIEND_INFO*	GetFriendInfo( const TCHAR* szName );
	void			DelFriend( int nIndex );

public:
	void			InitShutout( TCHAR const* szName, TCHAR const* szMemo, bool bOnline );
	void			AddShutout( TCHAR const* szName, bool bOnline );	
	void			AddShutoutMemo( TCHAR const* szName, TCHAR const* szMemo );

	bool			CheckShutout( TCHAR const* szName );
	bool			IsShutout( TCHAR const* szName );
	sSHUTOUT_INFO*	GetShutInfo( int nIndex );
	sSHUTOUT_INFO*	GetShutInfo( const TCHAR* szName );
	void			DelShutout( int nIndex );	

#ifdef GUILD_RENEWAL
public:
	bool			m_bOnOffCheck;	//立加 模备父 焊扁 眉农
	void			SetOnOff( bool bCheck ) { m_bOnOffCheck = bCheck; }
	bool			GetOnOff() { return m_bOnOffCheck; }
#endif

	void			ChangeTamerName( const std::wstring& BefoleName, const std::wstring& NewName );
};
