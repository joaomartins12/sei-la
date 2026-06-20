
#pragma once 

class CCharMng : public NiMemObject
{
	struct sTempObj
	{
		sTempObj()
		{
			m_pObj = NULL;
			m_bAutoDel = false;
			m_fLifeTime = 0.0f;
		};

		CsC_AvObject*	m_pObj;
		bool			m_bAutoDel;
		float			m_fLifeTime;
	};
public:
	CCharMng();

#ifndef _GIVE
	public:
		bool m_bRenderChar;
		bool m_bRenderUser;
#endif

public:
#ifdef ADVENTURE_TAMER_CHANGE
	void				DelTamer();
#endif
	void				Delete();
	void				Init();
	void				Reset();	

	void				Update();
	void				Render();
	void				Render_PostChar();

	void				Update_Tutorial();
	void				Render_Tutorial();
	void				Render_PostChar_Tutorial();

	void				ResetShader();

	void				ResetNameTamer();
	void				ResetNameDigimon();
	
public:

	//==============================================================================================================
	// 유저
	//==============================================================================================================
protected:
	void				_UserReleasePointer();
	void				_DeleteContainer();

	//==============================================================================================================
	// 테이머
	//==============================================================================================================
protected:
	//CsVectorLink< CTamer* >	m_vnTamer;
	std::map< DWORD, CTamer* >	m_mapTamer;
	CTamerUser*				m_pTamerUser;

protected:
	void				_UpdateTamer( bool bFigure, bool bFigureEmployment );
	void				_RenderTamer( bool bFigure, bool bFigureEmployment );
	void				_RenderPostChar_Tamer();	
public:
	CTamer*				AddTamer( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, TCHAR const* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwSyncOption );
	CTamerUser*			AddTamerUser( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, TCHAR const* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwSyncOption );
	CTamerUser*			AddTamerUser_Tutorial( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, TCHAR const* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwSyncOption );

	void				ChangeTamerKFM( uint nIDX, uint nFileTableID );

	CTamer*				GetTamer( uint const& nIDX );
	CTamerUser*			GetTamerUser();

	int					GetMyTamerLv() const;

	__int64				GetMyTamerExp() const;

	DWORD				GetMyDigimonFTID( int nSlotNum = 0 ) const;

	bool				IsTamer( uint const & nIDX ) const;
	bool				IsTamerUser( uint const& nIDX ) const;
	bool				IsTamerFromUIDX( uint const& nUIDX );
	bool				IsTamerUserFromUIDX( uint const& nUIDX );
	bool				IsDigimonUserFromUIDX( uint const& nUIDX );
	CTamer*				GetTamerFromUIDX(uint const& nUIDX);
	CDigimon*			GetDigimonFromUIDX(uint const& nUIDX);
	//CsC_AvObject*		GetAvObjectFromUIDX(uint nUIDX);

	void				DeleteTamer( uint const& nIDX );
	void				DeleteGuildName( TCHAR const* szGuildName ); //길드 이름 받아서 주변에 같은 이름으로 된 모든 테이머 길드 가입 정보 변경

	//CsVectorLink< CTamer* >*	GetTamerVector(){ return &m_vnTamer; }
	std::map< DWORD, CTamer* > * GetTamerMap() { return &m_mapTamer; };

	CsC_AvObject*		PickTamer( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength, bool bIncludeSelf );

	//==============================================================================================================
	// 디지몬
	//==============================================================================================================
protected:
	//CsVectorLink< CDigimon* >	m_vnDigimon;
	std::map<DWORD, CDigimon*>	m_mapDigimon;
	
	CDigimonUser*				m_pDigimonUser[ MAX_DIGIMONUSER_COUNT ];

	std::queue< CDigimon* >		m_queueChangeModel;

protected:
	void				_UpdateDigimon( bool bFigure );
	void				_RenderDigimon( bool bFigure );
	void				_RenderPostChar_Digimon();	
public:
	CDigimon*			AddDigimon( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, nSync::Pos sPos, float fRot, TCHAR const* szName );
	CDigimonUser*		AddDigimonUser( int nUserIndex, uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, std::list<nsDPART::sDPART_INFO> const* pParts, nSync::Pos sPos, float fRot, TCHAR const* szName, bool bInitPos );
	CDigimonUser*		AddDigimonUser_Tutorial( int nUserIndex, uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, std::list<nsDPART::sDPART_INFO> const* pParts, nSync::Pos sPos, float fRot, TCHAR const* szName, bool bInitPos );

	void				ChangeDigimonKFM( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts);
	void				ChangeDigimonModel( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, LPTSTR szName, float fNextScale, int nNextLevel);
	//void				ChangeDigimonModel( CDigimon* pDigimon ){ m_queueChangeModel.push( pDigimon ); }
#ifdef CROSSWARS_SYSTEM
	void				ChangeCrossDigimonKFM( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, bool bUser = true  );
	void				ChangeCrossDigimonModel( uint nIDX, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, LPTSTR szName, float fNextScale, int nNextLevel, bool bUser = true );
#endif
	CDigimon*			GetDigimon( uint const& nIDX );
	CDigimonUser*		GetDigimonUser( int const& nIndex );

	bool				IsDigimonUser( uint nIDX );

	void				DeleteDigimon( uint nIDX );		// 디지몬유저가 날라 오면 패스, 없는거 날라와도 패스
	void				DeleteDigimonUser( uint nIDX );

	//CsVectorLink< CDigimon* >*	GetDigimonVector(){ return &m_vnDigimon; }
	std::map<DWORD, CDigimon*>*	GetDigimonMap() {return &m_mapDigimon; }

	CsC_AvObject*		PickDigimon( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength, bool bIncludeSelf );

	//==============================================================================================================
	// 몬스터
	//==============================================================================================================
protected:
	//CsVectorLink< CMonster* >	m_vnMonster;	
	std::map<DWORD,CMonster*>	m_mapMonster;
	std::queue< int >			m_queueMonsterTempID;

protected:
	void				_UpdateMonster();
	void				_RenderMonster();
	void				_RenderPostChar_Monster();
public:
	CMonster*			AddMonster( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, CMonster::eMONSTER_STATE MonsterState );
	CMonster*			AddMonster_Tutorial( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, CMonster::eMONSTER_STATE MonsterState );
	//CMonster*			GetMonster( uint nIDX ){ if( m_vnMonster.IsElement( nIDX ) == false ) return NULL; return m_vnMonster[ nIDX ]; }
	CMonster*			GetMonster( uint const& nIDX );
	CMonster*			FindNearMonster( int iMonsterID); 
	void				DeleteMonster( uint nIDX, CsC_AvObject::eDELETE_STATE ds );	

	//CsVectorLink< CMonster* >*	GetMonsterVector(){ return &m_vnMonster; }
	std::map< DWORD, CMonster* >*	GetMonsterVector(){ return &m_mapMonster; }

	CsC_AvObject*		PickMonster( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength );

	int					GetMonsterTempID();
	void				PushMonsterTempID( int nIDX );

	int					GetMyDigimonLv() const;

	// 몬스터찾기
public:
	void				NextSelectMonster();
protected:
	CsC_AvObject*		_FindObject_FromDist( CsC_AvObject* pFinder, CsC_AvObject* pSelect, int nFindRTTI, int nExceptValue = 0 );

	//==============================================================================================================
	// 위탁 상점
	//==============================================================================================================
protected:
	//CsVectorLink< CEmployment* >	m_vnEmployment;
	std::map<DWORD,CEmployment*>	m_mapEmployment;

protected:
	void				_UpdateEmployment( BOOL bFigure );
	void				_RenderEmployment( BOOL bFigure );
	void				_RenderPostChar_Employment();
public:
	//bool				IsEmployment( uint nIDX ){ return m_vnEmployment.IsElement( nIDX ); }
	bool				IsEmployment( uint const& nIDX ) const;
	CEmployment*		AddEmployment( uint nIDX, uint nItemID, NiPoint3 vPos, float fRot );
	//CEmployment*		GetEmployment( uint nIDX ){ if( m_vnEmployment.IsElement( nIDX ) == false ) return NULL; return m_vnEmployment[ nIDX ]; }
	CEmployment*		GetEmployment( uint const& nIDX );
	void				DeleteEmployment( uint nIDX );

	//CsVectorLink< CEmployment* >*	GetEmploymentVector(){ return &m_vnEmployment; }
	std::map<DWORD, CEmployment*> *	GetEmploymentMap() { return &m_mapEmployment; }

	CsC_AvObject*		PickEmployment( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength );

	//==============================================================================================================
	// 펫
	//==============================================================================================================
protected:
	//CsVectorLink< CPat* >	m_vnPat;
	std::map<DWORD,CPat*>	m_mapPat;

protected:
	void				_UpdatePat( BOOL bFigure );
	void				_RenderPat( BOOL bFigure );
	void				_RenderPostChar_Pat();	
public:
	// 외부에서 바로 호출하지 말고 CTamer 쪽에서 호출하자
	CPat*				AddPat( CsC_AvObject* pParent, uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot );
	CPat*				AddPat( CsC_AvObject* pParent, uint nIDX, uint nFileTableID, sCREATEINFO* pProp, NiPoint3 vPos, float fRot );
	void				DeletePat( uint nIDX );

	//CsVectorLink< CPat* >*	GetPatVector(){ return &m_vnPat; }
	std::map<DWORD,CPat*> *	GetPatMap() { return &m_mapPat; }

	//==============================================================================================================
	// 임시 케릭터
	//==============================================================================================================
protected:
	//CsVectorLink< CsC_AvObject* >	m_vnTempChar;
	std::map<DWORD,CsC_AvObject*>	m_mapTempChar;

protected:	
	void				_UpdateTempChar();
	void				_RenderTempChar();
public:
	void				DeleteTempChar( CsC_AvObject* pDeleteChar );
	CTamer*				AddTempTamer( sCREATEINFO* pProp, uint nFileTableID, NiPoint3 vPos, float fRot, TCHAR* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwServerOption );
	CDigimon*			AddTempDigimon( sCREATEINFO* pProp, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, NiPoint3 vPos, float fRot, TCHAR* szName );
	CDigimon*			AddTempDigimon( uint nIDX, sCREATEINFO* pProp, uint nFileTableID, std::list<nsDPART::sDPART_INFO> const* pParts, NiPoint3 vPos, float fRot, TCHAR* szName );
	CsC_AvObject*		GetTempChar( uint nIDX );

	bool				AttachEffectFromTamerUIDX(uint nTamerUID, char* cEffectPath, float fScale, DWORD dwPlag, bool bPreLoad = false);



	// 부여 ID
protected:
	std::stack< uint >	m_stackTempCharID;
public:	
	uint				PopTempCharID();
	void				PushTempCharID( uint nUID );

	//==============================================================================================================
	// 디버그 케릭터
	//==============================================================================================================

#ifdef DEBUG_SYNC_CHAR

protected:
	//CsVectorLink< CsC_AvObject* >	m_vnDebugChar;
	std::map<DWORD, CsC_AvObject*>	m_mapDebugChar;

protected:	
	void				_UpdateDebugChar();
	void				_RenderDebugChar();
public:
	void				DeleteDebugChar( CsC_AvObject* pDeleteChar );
	CTamer*				AddDebugTamer( uint nIDX, sCREATEINFO* pProp, uint nFileTableID, NiPoint3 vPos, float fRot, TCHAR* szName, CsC_PartObject::sCHANGE_PART_INFO* pPartCordArray, DWORD dwServerOption );
	CDigimon*			AddDebugDigimon( uint nIDX, sCREATEINFO* pProp, uint nFileTableID, NiPoint3 vPos, float fRot, TCHAR* szName );
	CMonster*			AddDebugMonster( uint nIDX, sCREATEINFO* pProp, uint nFileTableID, NiPoint3 vPos, float fRot, TCHAR* szName );

	CsC_AvObject*		GetDebugChar( uint nIDX );

	void				CheckDebugChar();

#endif

	//==============================================================================================================
	// 제거 케릭터
	//==============================================================================================================
protected:
	std::list< CsC_AvObject* >	m_listDeleteChar;

protected:
	void				_UpdateDeleteChar();
	void				_RenderDeleteChar();
	void				_RenderPostChar_DeleteChar();

public:
	std::list< CsC_AvObject* >*	GetListDeleteChar(){ return &m_listDeleteChar; }

	//==============================================================================================================
	// 몬스터 스킬 이펙트용 임시 오브젝트
	//==============================================================================================================

protected:
	//CsVectorLink< sTempObj >	m_vnTempObject;		// 바닥 이펙트 표시용 더미 오브젝트 벡터
	std::map<DWORD, sTempObj>	m_mapTempObject;

protected:	
	void				_UpdateTempObject();
	void				_RenderTempObject();

public:
	void				DeleteTempObject( CsC_AvObject* pDeleteChar );
	void				DeleteTempObject( uint nMonsterUID );
	CsC_AvObject*		AddTempObject( uint nMonsterUID, NiPoint3 vPos, bool bAutoDel = false, float fLifeTime = 0.0f );
//	CsC_AvObject*		AddTempObject( uint nMonsterUID, NiPoint3 vPos );
	CsC_AvObject*		GetTempObject( uint nMonsterUID );

protected:
	std::stack< uint >	m_stackTempObjectID;			// 오브젝트 ID 
public:
	uint				PopTempObjectID();
	void				PushTempObjectID( uint nUID );

	//==============================================================================================================
	// 싱크
	//==============================================================================================================
#ifdef SYNC_DATA_LIST

public:
	struct sSYNC_DATA : public NiMemObject
	{
		enum eType{ NONE, ADD_EMPLOYMENT, DEL_EMPLOYMENT };
		struct sEMPLOYMENT
		{
			nSync::Pos	s_Pos;
			cType		s_Type;
			bool		s_bNew;
			float		s_nFloat;
			uint		s_nItemType;
			TCHAR		s_szShopName[Language::pLength::ShopName+1];
		};

		
		eType		s_Type;
		int			s_NextFrame;
		sEMPLOYMENT	s_EmploymentData;		
	};
protected:	
	std::stack< sSYNC_DATA* >	m_SyncMempool;
	std::list< sSYNC_DATA* >	m_listSync_1;			// 우선순위 높은거 - 케릭터, 몬스터
	std::list< sSYNC_DATA* >	m_listSync_2;			// 우선순위 낮은거 - 위탁상점

protected:
	void			_DeleteSync();
	void			_InitSync();
	void			_UpdateSync();	
	void			_Sync_AddEmployment( sSYNC_DATA::sEMPLOYMENT* pData );

public:	
	sSYNC_DATA*		NewSync();
	void			DelSync( sSYNC_DATA* pData );

	void			InsertSync_1( sSYNC_DATA* pData );
	void			InsertSync_2( sSYNC_DATA* pData, int nNextFrame );

#endif

public:
	void			SetMonsterSE_Highlight_TableID( DWORD const& dwTableID, bool bShow );
	bool			IsSEHighLightMonster_TableID( DWORD const& dwTableID );

private:
	void			_AddMonsterSE_HighList_TableID(DWORD const& dwTableID);
	void			_RemoveMonsterSE_HighList_TableID(DWORD const& dwTableID);
	void			_RemoveAllMonsterSE_HighList();

private:
	std::list<DWORD>	m_listMonsterSE_HighLight;
};

extern CCharMng*		g_pCharMng;