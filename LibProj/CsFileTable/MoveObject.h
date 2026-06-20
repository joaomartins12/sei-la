

#pragma once 

#define TO_MO_ID( kind, id )		( kind + id )
#define MOTO_KIND( MOID )			(CsMoveObject::eKIND)( (int)( MOID/CsMoveObject::MO_NPC )*CsMoveObject::MO_NPC )
#define MOTO_ID( MOID )				( MOID%CsMoveObject::MO_NPC )

#pragma pack(push)
#pragma pack(1)

class CsMoveObject
{
public:
	enum eKIND{ MO_NPC = 100000, MO_MONSTER = 200000, };
public:
	typedef	std::map< DWORD, CsMoveObject* >				MAP;
	typedef	std::map< DWORD, CsMoveObject* >::iterator		MAP_IT;


public:
	enum eACT_TYPE{ AT_START, AT_WAIT, AT_MOVE, AT_GOTO_FIRST_LOOP, };
	struct sACT_BASE
	{
		typedef	std::list< sACT_BASE* >				LIST;
		typedef	std::list< sACT_BASE* >::iterator	LIST_IT;

		eACT_TYPE			GetType(){ return s_eActType; }
		virtual sACT_BASE*	Clone( sACT_BASE* pSrc ) = 0;
		virtual	void		Save( FILE* fp ) = 0;
		virtual	void		Load( FILE* fp ) = 0;
		virtual void		LoadFilePack( int nHandle ) = 0;

		virtual void		Delete(){}
		virtual float		GetTime( float fLength ){ return 0; }

		virtual void		SetPosX( int x ){}
		virtual void		SetPosY( int y ){}
		virtual void		SetTime( int nSec ){}
		virtual void		SetAni( DWORD dwAni ){}

		virtual int			GetPosX(){ return 0; }
		virtual int			GetPosY(){ return 0; }
		virtual int			GetTime(){ return 0; }
		virtual DWORD		GetAni(){ return 0; }

	protected:
		eACT_TYPE			s_eActType;
	};
	struct sACT_START : public sACT_BASE
	{
		sACT_START(){ s_eActType = AT_START; }
		virtual sACT_BASE*	Clone( sACT_BASE* pSrc );
		virtual	void		Save( FILE* fp );
		virtual	void		Load( FILE* fp );
		virtual void		LoadFilePack( int nHandle );

		virtual void		SetPosX( int x ){ s_PosX = x; }
		virtual void		SetPosY( int y ){ s_PosY = y; }
		virtual int			GetPosX(){ return s_PosX; }
		virtual int			GetPosY(){ return s_PosY; }

		int		s_PosX;
		int		s_PosY;
	};
	struct sACT_WAIT : public sACT_BASE
	{
		sACT_WAIT(){ s_eActType = AT_WAIT; }
		virtual sACT_BASE*	Clone( sACT_BASE* pSrc );
		virtual	void		Save( FILE* fp );
		virtual	void		Load( FILE* fp );
		virtual void		LoadFilePack( int nHandle );
		virtual float		GetTime( float fLength ){ return (float)s_nTimeSec; }

		virtual void		SetTime( int nSec ){ s_nTimeSec = nSec; }
		virtual void		SetAni( DWORD dwAni ){ s_dwAni = dwAni; }
		virtual int			GetTime(){ return s_nTimeSec; }
		virtual DWORD		GetAni(){ return s_dwAni; }

		DWORD	s_dwAni;
		int		s_nTimeSec;
	};
	struct sACT_MOVE : public sACT_BASE
	{
		sACT_MOVE(){ s_eActType = AT_MOVE; }
		virtual sACT_BASE*	Clone( sACT_BASE* pSrc );
		virtual	void		Save( FILE* fp );
		virtual	void		Load( FILE* fp );
		virtual void		LoadFilePack( int nHandle );
		virtual float		GetTime( float fLength ){ return fLength/(float)s_nVelocity; }

		virtual void		SetPosX( int x ){ s_PosX = x; }
		virtual void		SetPosY( int y ){ s_PosY = y; }
		virtual void		SetTime( int nSec ){ s_nVelocity = nSec; }
		virtual void		SetAni( DWORD dwAni ){ s_dwAni = dwAni; }

		virtual int			GetPosX(){ return s_PosX; }
		virtual int			GetPosY(){ return s_PosY; }
		virtual int			GetTime(){ return s_nVelocity; }
		virtual DWORD		GetAni(){ return s_dwAni; }

		int		s_PosX;
		int		s_PosY;

		DWORD	s_dwAni;
		int		s_nVelocity;
	};
	struct sACT_GOTO_FIRST_LOOP : public sACT_BASE
	{
		sACT_GOTO_FIRST_LOOP(){ s_eActType = AT_GOTO_FIRST_LOOP; }
		virtual sACT_BASE*	Clone( sACT_BASE* pSrc );
		virtual	void		Save( FILE* fp );
		virtual	void		Load( FILE* fp );
		virtual void		LoadFilePack( int nHandle );
		virtual float		GetTime( float fLength ){ return fLength/(float)s_nVelocity; }

		virtual void		SetTime( int nSec ){ s_nVelocity = nSec; }
		virtual void		SetAni( DWORD dwAni ){ s_dwAni = dwAni; }
		virtual int			GetTime(){ return s_nVelocity; }
		virtual DWORD		GetAni(){ return s_dwAni; }

		DWORD	s_dwAni;
		int		s_nVelocity;
	};

public:
	CsMoveObject();

public:
	void			Delete();
	void			Init();

	void			Save( FILE* fp );
	void			Load( FILE* fp );
	void			LoadFilePack( int nHandle );

	bool			GetPos( float fAccumTime, float& x, float& y, float& dx, float& dy, DWORD& nAni, int& nMoveSpeed );

public:
	bool						IsAct( eACT_TYPE at );
	void						AddAct( sACT_BASE* pSrc );
	std::list< sACT_BASE* >*	GetActList(){ return &m_listAct; }
	void						GetFirstMovePoint( int& x, int& y );
	void						GetLastMovePoint( int& x, int& y );

	void						Setkind( eKIND kind ){ m_eKind = kind; }
	eKIND						GetKind(){ return m_eKind; }
	void						SetFTID( int nFTID ){ m_nFileTableID = nFTID; }
	int							GetFTID(){ return m_nFileTableID; }
	int							GetMoveObjectID(){ return TO_MO_ID( m_eKind, m_nFileTableID ); }

	void						SetTotalTime( float fTime ){ m_fTotalTime = fTime; }
	float						GetTotalTime(){ return m_fTotalTime; }
	void						SetMapID( DWORD dwMapID ){ m_dwMapID = dwMapID; }
	DWORD						GetMapID() { return m_dwMapID; }
	void						ResetActMove();

protected:
	eKIND						m_eKind;
	int							m_nFileTableID;
	std::list< sACT_BASE* >		m_listAct;
	float						m_fTotalTime;
	DWORD						m_dwMapID;
};

#pragma pack(pop)