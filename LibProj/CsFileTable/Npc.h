
#pragma once 

class CsNpc
{
public:
	typedef	std::map< DWORD, CsNpc* >					MAP;
	typedef	std::map< DWORD, CsNpc* >::iterator			MAP_IT;

public:
	CsNpc();
	// ====== Base
public:
	void			Delete();
	void			Init();

	// ====== Info
public:
	enum eDISPLAY{
		DP_ZOOM_VIEW		=	0x00000001,
		DP_INFOMATION		=	0x00000002,
	};

public:
	struct sINFO
	{
		sINFO(){ s_nDisplayPlag = DP_ZOOM_VIEW | DP_INFOMATION; }
		DWORD					s_dwNpcID;
		DWORD					s_dwMapID;
		nsCsNpcTable::eNPC_TYPE s_eType;
		nsCsNpcTable::eNPC_MOVE s_eMoveType;
		DWORD					s_nDisplayPlag;
		DWORD					s_dwModelID;
		TCHAR					s_szPenName[ NPC_NAME_LEN ];
		TCHAR					s_szName[ NPC_NAME_LEN ];
		TCHAR					s_szTalk[ NPC_TALK_LEN ];
	};
protected:
	sINFO			m_Info;
public:
	sINFO*			GetInfo(){ return &m_Info; }

	// ====== Type
protected:
	sNPC_TYPE_BASE*	m_pType;
public:
	void			SetType();
	void			SetType( nsCsNpcTable::eNPC_TYPE eType );
	void			SetType( sNPC_TYPE_BASE* pSrcType );
	void			SetMoveType( nsCsNpcTable::eNPC_MOVE eType );

	sNPC_TYPE_BASE*	GetType(){ return m_pType; }

protected:
	void			_DeleteType();

	// ====== Extra
protected:
	sNPC_EXTRA_BASE*	m_pExtraArray[ nsCsNpcTable::NE_MAX_CNT ];

public:
	bool				IsExtra( nsCsNpcTable::eNPC_EXTRA ne );
	void				DeleteExtra( sNPC_EXTRA_BASE* pExtra );
	void				DeleteExtra( nsCsNpcTable::eNPC_EXTRA ne );

	sNPC_EXTRA_BASE*	AddExtra( nsCsNpcTable::eNPC_EXTRA ne );
	sNPC_EXTRA_BASE*	GetExtra( nsCsNpcTable::eNPC_EXTRA ne ){ return m_pExtraArray[ ne ]; }
	sNPC_EXTRA_BASE**	GetExtraArray(){ return m_pExtraArray; }

	// ====== Clone
public:
	CsNpc*			Clone();
	void			CopyData( CsNpc* pSrcNpc );
};

