
#pragma once 

class CUserServerMove
{
public:
	CUserServerMove() { m_pParent = NULL; }

	//=============================================================================================================
	//	Owner
	//=============================================================================================================
public:
	void			UpdateCamera();
	void			DBClick_Object( cType type, CsC_AvObject* pObject );
	void			SetPos_FromMouse( POINT ptMouse, float fConnectLength, bool bRenderMoveModel, bool bAttack );
#ifdef ZONEMAP_CLICK_MOVE
	void			SetPos_FromZoneMap(CsPoint ptPos, float fConnectLength = 0.f, bool bRenderMoveModel = true, bool bAttack = false);
#endif


	//=============================================================================================================
	//	키보드 이동
	//=============================================================================================================
public:
	enum eKEY{
		KEY_NONE	= 0x00000000,
		KEY_LEFT	= 0x00000001,
		KEY_RIGHT	= 0x00000002,
		KEY_UP		= 0x00000004,
		KEY_DOWN	= 0x00000008,
	};

	enum eKEY_MAPPING{ KMAP_LEFT, KMAP_RIGHT, KMAP_UP, KMAP_DOWN, MAX_KMAP, };

protected:
	NiPoint3			m_vKeyDir;	
	NiPoint3			m_vMappingKeyDir[ MAX_KMAP ];
	DWORD				m_dwKeyCheck;
	CsTimeSeq			m_RetryKeyCheckTimeSeq;
#ifdef KEYBOARD_MOVE
	NiPoint3			m_vMoveUpDir;		// 키보드 이동 방향
	NiPoint3			m_vMoveRightDir;	// 키보드 이동 방향
	bool				m_bCurState;		// 테이머/디지몬 이동 설정 변경 유무 체크
	bool				m_bBackupState;		// 테이머/디지몬 이동 설정 변경 유무 체크
public:
	bool				GetCurState()				{ return m_bCurState; }
	bool				GetBackupState()			{ return m_bBackupState; }
	NiPoint3			GetKeyBoardMoveUpDir()		{ return m_vMoveUpDir; }
	NiPoint3			GetKeyBoardMoveRightDir()	{ return m_vMoveRightDir; }
	void				SetKeyBoardMoveDir();
	void				SetMoveOption( bool bDigimonMove );
	bool				_IsNewPath()				{ return m_bNewPath; }
	void				_SetIsNewPath( bool bNew )	{ m_bNewPath = bNew; }
#endif

public:
	void				ViewReset();

	void				KeyReset();
	void				KeyCheckTimeReset(){ m_RetryKeyCheckTimeSeq.Reset(); }
	
	void				KeyAutoMapping();
	void				RetryKeyCheck();						// 동일 값으로 키체크 다시 할때
	void				SetKeyCheck( DWORD dwKey );	
	DWORD				GetKeyCheck(){ return m_dwKeyCheck; }

protected:
	void				_Update_RetryKeyCheck();

private:
	bool				CheckStateTamerMoved();

	//=============================================================================================================
	//	서버 이동
	//=============================================================================================================
protected:
	CsC_AvObject*		m_pParent;

	bool				m_bNewPath;
	long				m_nOldTotalSize;
	nSync::Pos			m_LastSyncPos;	

public:
	void				Delete();
	void				Init( CsC_AvObject* pParent );	
	void				DeletePath();

	nSync::Pos			GetLastSyncPos(){ return m_LastSyncPos; }
	void				SetLastSyncPos( int x, int y ){ m_LastSyncPos.m_nX = x; m_LastSyncPos.m_nY = y; }

	CsC_AvObject*		GetParent(){ return m_pParent; }

public:
	void				SetServerMoveReady();
#ifdef CRASHDUMP_0424
	void				SetServerMoveReady2( CsC_PathProp* pPathProp );
#endif
	void				SetServerMove( uint nUID, float fCurX, float fCurY, float fRot, iPath* pPath );
	void				SetServerInitPos( NiPoint3 vPos );

private:
	void				__GetNextPos( NiPoint2 vOrg, NiPoint2& vPos, int nIndex );
	void				__SetServerInitPos( NiPoint2 vOrg, NiPoint2 vPos, int nIndex );
	bool				__SendMoveTo( uint nUID, NiPoint2 vOrg, NiPoint2 vPos, float fRot, int nIndex );
	bool				__SendMoveTo( uint nUID, NiPoint2 vOrgStart, NiPoint2 vOrgEnd, NiPoint2 vStart, NiPoint2 vEnd, float fRot );


	//=============================================================================================================
	//	서버 이동 전송
	//=============================================================================================================
protected:
	CsTimeSeq			m_SendMoveTimeSeq;
	struct sSENDMOVE_INFO
	{
		sSENDMOVE_INFO(){ s_nType = 0; }
		int				s_nType;		// 0 이면 이미 전송 완료, 1이면 pGame::MoveTo	2면 pGame::NewPath
		uint			s_nUID;
		nSync::Pos		s_StartPos;
		nSync::Pos		s_TargetPos;
		float			s_fRot;
	};
	sSENDMOVE_INFO		m_SendMoveInfo;

protected:
	void				_SendMove_Server( uint nUID, nSync::Pos pos, float fRot );
	void				_SendMove_Server( uint nUID, nSync::Pos sp, nSync::Pos tp, float fRot );
public:
	void				Update_SendMove_Server();

public:
	enum eLockState
	{
		eKeyboardLock	= 0x01,
		eMouseLock		= 0x10
	};
	short			m_KeyboardMoveLock;
public:
	void			SetMoveLock( short const& lockType );
};

extern CUserServerMove*		g_pServerMoveOwner;