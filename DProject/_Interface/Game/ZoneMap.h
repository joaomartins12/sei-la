
#pragma once 

class cZoneMap : public cBaseWindow, public CMapContents::ObserverType
{
public:
	cZoneMap();
	~cZoneMap();

public:
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_ZONEMAP; }
	virtual void				Destroy();
	virtual	bool				Close( bool bSound =true );
	virtual void				DeleteResource();
	virtual void				ResetMap();
	virtual void				Create( int nValue = 0 );

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();

	virtual void				Render();
	virtual void				ResetDevice();	
	virtual bool				OnEscapeKey(){ Close(); return true; }		

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

protected:
	void				_Update_ForMouseMapInfo();
	void				_Update_ForMouseParty();

protected:
	void				_DeleteNpcInfo();

	void				_DeleteZone();
	void				_ResetDevice_Zone();
	void				_LoadZone( int nMapID = -1 );
	void				_Update_ForMouse_Zone();
	void				_Render_Zone();
	void				_Render_Party();
	void				_RenderNpc();
	void				_RenderRegion();

	void				_DeleteArea();
	void				_ResetDevice_Area();
	void				_LoadArea( int nType );
	void				_Update_ForMouse_Area();
	void				_Render_Area();

	void				_DeleteWorld();
	void				_ResetDevice_World();
	void				_LoadWorld( int nType );
	void				_Update_ForMouse_World();
	void				_Render_World();

	void				_DeleteMainMap();
	void				_ResetDevice_MainMap();
	void				_LoadMainMap();
	void				_Update_ForMouse_MainMap();
	void				_Render_MainMap();

	void				_DeleteHelpMap();
	void				_LoadHelpMap();	
	void				_Render_HelperMap();
	void				_Update_Helper();
	void				_ResetDevice_HelpMap();

	void				_Render_EmploymentMap();

	void				_DeleteMapInfo();
	void				_Render_NpcInfo();

	void				_SetNpcInfo( int nMapID = -1 );
	//void				_SetNpcInfo_Tutorial( int nMapID = -1 );

	void				_Render_MonInfo();
	void				_SetMonInfo( int nMapID = -1 );

	CsPoint				_CalZoneMapPos( NiPoint2 vPos, CsPoint ptSize );


	// 메모리잡힌 컨트롤
public:
	enum eRadio_Type			{ 
		eType_Nor = 0,
		eType_Npc,
		eType_Mon,
		eType_Pos,
		eType_Employment,
		eType_End };

protected:
	cSprite*					m_pMyZoneMask;
	cButton*					m_pBtnBack;
	cButton*					m_pTranslate;

	cSprite*					m_pZoneMap;
	cSprite*					m_pAreaMap;
	cSprite*					m_pWorldMap;
	cSprite*					m_pMainMap;
	cSprite*					m_pArrow;	
	cSprite*					m_pExtendBtn;
	cSprite*					m_pCircle;
	
	eRadio_Type					m_nRadioIndex;
	cRadioButton*				m_pRadioBtn;	

	cScrollBar*					m_pScroll;

	bool						m_bDungeon;			// 던전인 경우는 지역명 표시 안함.
	cSprite*					m_pRegionIcon;		// 지역명 표시 아이콘
	cText*						m_RegionName;		// 지역명
	cText*						m_RegionShowText;	// 지역명 표시/숨기기 텍스트
	cCheckBox*					m_CheckRegionShow;	// 지역명 표시/숨기기 체크박스

public:
	void						ResetRadioIdx() { m_nRadioIndex = eType_Nor; }
	void						SetCheckRadioIndex( eRadio_Type type ){ m_pRadioBtn->SetCheckIndex( type ); }
	eRadio_Type					GetCheckRadioIndex(){ return (eRadio_Type)m_pRadioBtn->GetCheckIndex(); }
	

	// 헬퍼에 사용되는 변수
protected:
	CsPoint							m_HelperRenderPos;	
	float							m_fStep;	
	float							m_fRot;

	cSprite*						m_pRadar;
	cSprite*						m_pRadarBack;

	CsTimeSeq						m_Time;
	cButton*						m_pHelperMove;

	// 포인터만 있는 컨트롤
protected:
	cButton*					m_pCancelButton;
	sCHAR_IMAGE*				m_pImage;

#ifdef ZONEMAP_CLICK_MOVE
	cSprite*					m_pArrivePoint;	// 도착 예정 지역
	void						_ClickMapMove();	// 존맵 길찾기
public:
	void						RefreshArrivePoint(CsPoint ptMouse);	// 존맵이 열린 상태에서 길찾기할 때 도착 지점 갱신
protected:
	std::list<CsPoint>			m_PathList;		// Cell 좌표 -> 맵 좌표 -> 윈도우 좌표
	void						_UpdatePath();
	void						_PathLineRender();
#endif

protected:
	CsPoint						_GetMoveArrivePos();	// 이동하려는 맵의 XY좌표를 반환
	void						_GMTeleport();			// GM 텔레포트 _ 기존 연산 수정

protected:
	//맵의 몬스터 NPC 정보 저장
	struct sMap_INFO : public NiMemObject
	{
	public:
		sMap_INFO(){ s_pImage = NULL; s_pBtn = NULL; s_pName = NULL; s_pTitle = NULL; s_pEtc = NULL; s_pCircle = NULL; }
		enum eType { Type_Npc = 0, Type_Monster };

		void					Delete();
		void					Init( eType eType, cSprite* pSprite, DWORD dwFileID = -1, DWORD dwMapID = -1 );
		cBaseWindow::eMU_TYPE	Update_ForMouse();
		void					Render( CsPoint ptPos );
		void					ResetDevice();
		cButton*				GetButtonControl() { return s_pBtn; }
	public:
		eType						s_eType;
		sCHAR_IMAGE*				s_pImage;
		DWORD						s_dwFileID;
		DWORD						s_dwMapID;

	protected:
		cButton*					s_pBtn;
		cString*					s_pName;
		cString*					s_pTitle;
		cString*					s_pEtc;
		TCHAR						s_szName[ NPC_NAME_LEN ];

		cSprite*					s_pCircle;	//포인터만 존재
	};
	std::list< sMap_INFO* >		m_listMapInfo;

protected:
	std::list< cMiniMap::sNPC_INFO* >		m_listNpcInfo;
	// 계산하는데 필요한 값
	bool						m_bHeightHigh;
	CsPoint						m_ptArrowPos;
	float						m_Meter_To_Pixel;
	float						m_fHalfDeltaSize;
	float						m_fMaxSize;
	float						m_fAspectiveHeight;


	enum eSTATE{ CUR_ZONE, OTHER_ZONE, AREAMAP, WORLDMAP, MAINMAP };
	eSTATE						m_eState;
	int							m_nLoadZoneMapID;
	int							m_nLoadAreaMapID;
	int							m_nLoadWorldMapID;

	struct sBTN_INFO : public NiMemObject
	{		
		virtual ~sBTN_INFO(){ SAFE_NIDELETE( s_pButton ); }
		cButton*	s_pButton;
		int			s_nID;
	};

	std::list< sBTN_INFO* >		m_listAreaBtn;
	std::list< sBTN_INFO* >		m_listWorldBtn;
	std::list< sBTN_INFO* >		m_listMainMapBtn;

protected:
	void						_CalValue( DWORD dwMapID );
	void						_UpperMapLevel();

protected:
	void						_Update_TamerPos();
	CsPoint						_GetCenterZonePos();
	bool						IsOpenMap();
	bool						IsMonster(int nMapID = -1);

public:
	cWindow* 					GetChildControl( std::string const& sControlName );
	cWindow*					GetMapObjectControl( int const& nType, DWORD const& dwTargetIDX );
};