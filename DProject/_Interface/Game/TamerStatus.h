
#pragma once 

#define IF_TAMER_DIGI_COL				4
#define IF_TAMER_DIGI_ROW				3
#define IF_TAMER_DIGI_MAX_COUNT			12		// IF_TAMER_DIGI_COL * IF_TAMER_DIGI_ROW
#define IF_TAMER_STAT_COUNT				4

#define IF_TAMER_BASE_SKILL_COUNT		8
#define IF_TAMER_ACTIVE_SKILL_COUNT		4
#define IF_TAMER_PASIVE_SKILL_COUNT		4

#define IF_TAMER_CASH_SKILL_COUNT		2

#ifdef CROSSWARS_SYSTEM

	#define IF_CROSSLOADERSLOT_COUNT		16	
	#define IF_CROSSINVEN_X					3
	#define IF_CROSSINVEN_Y					5

	#define IF_CROSSLOADER_WIDTH			255
	#define IF_CROSSLOADER_HEIGHT			424
	#define IF_CROSSINVEN_WIDTH			294
	#define IF_CROSSINVEN_HEIGHT			424

#endif

class cTamerStatus : public cBaseWindow
{
public:
	cTamerStatus();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TAMERSTATUS; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			PlayOpenSound(){ cWindow::PlaySound( cWindow::SD_Wi1 ); }
	virtual void			PlayCloseSound(){ cWindow::PlaySound( cWindow::SD_Wi2 ); }

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnMouseOutWindow();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );
	
	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

#ifdef UI_DIGIMON_DELETE
	virtual bool			OnMacroKey( const MSG& p_kMsg );
#endif
	virtual bool			OnEnterKey() { return ( m_pCheckDeleteDigimon == NULL ) ? false : true; }
	virtual bool			OnEscapeKey(){ Close(); return true; }
	void					OnMoveWindow();
#ifdef CROSSWARS_SYSTEM
protected:
	void					_Update_ForMouse_Main();
#endif
	// 메모리잡힌 컨트롤
protected:
	cRadioButton*	m_pTabMenu;
	

	// 포인터만 있는 컨트롤
protected:	
	cButton*		m_pCancelButton;	
	cButton*		m_pDeleteButton[IF_TAMER_ACTIVE_SKILL_COUNT];	// 캐쉬 스킬 삭제 버튼

public:	
	void			ResetName();
	void			SetTab( int nTab );
	int				GetTab(){ return m_pTabMenu->GetCheckIndex(); }

	//===============================================================================================================
	//
	//	바코드
	//
	//===============================================================================================================

#ifdef USE_BARCODE_REDER

protected:
	cButton*		m_pBtnBarcodeResist;
	cButton*		m_pBtnBarcodeUnResist;
	cButton*		m_pCurBtnBarcode;
	cSprite*		m_pBarcodeBG;

	cString			m_BarcodeStateText;
	uint			m_BarcodeState;
protected:
	void			_DeleteBarcode();
	void			_InitBarcode();
	void			_InitBarcodeState_Text();
	bool			_CalBarcodeState();

	void			_UpdateBarcode();
	void			_RenderBarcode();

	bool			_UpdateBarcode_ForMouse();
	void			_ResetDevice_Barcode();

#endif

	//===============================================================================================================
	//
	//	테이머 장비
	//
	//===============================================================================================================
protected:
	cSprite*		m_pEquipBG;
	sIFIcon			m_IFIcon_Equip[ nLimit::Equip ];
	sIFIcon			m_IFIcon_Equip_Digivice;

	cRenderTex*		m_pRenderTex;
	cText*			m_pStatText[ IF_TAMER_STAT_COUNT ];
	CsRect			m_rcStatRegion[ IF_TAMER_STAT_COUNT ];	
	cText*			m_pNameText;	
	cText*			m_pGuildText;
	cText*			m_pEquipText[ 6 ];
	CsRect			m_FriendshipPoint;	// 친화력

#ifdef WEB_INFOMATION
	cButton*		m_pRank;
#endif

protected:
	void			_DeleteEquip();
	void			_CreateEquip();
	void			_InitEquip();
	void			_InitText_Equip();

	void			_Update_Equip();
	bool			_UpdateIcon_ForMouse_Equip();
	void			_RenderEquip();

	//	TamerStatus_Equip.cpp
	void			_UpdateEquipParts(CsPoint pos, sIFIcon::eSTATE	check, BOOL MouseOn);
	void			_UpdateEquipDigivice(CsPoint pos, sIFIcon::eSTATE	check, BOOL MouseOn);

	//	TamerStatus_Digivice.cpp
	void			_UpdateDigiviceMemory(CsPoint pos, sIFIcon::eSTATE	check, BOOL MouseOn);
	void			_UpdateDigiviceChipset(CsPoint pos, sIFIcon::eSTATE	check, BOOL MouseOn);

	void			_OnMouseOutWindow_Equip();
	void			_OnLButtonUp_Equip( CsPoint pos );
	bool			_CursorIconLBtnUp_Equip( CURSOR_ICON::eTYPE eIconType, int nIconSlot );

	void			_ResetDevice_Equip();

	void			_OnRButtonUp_Equip( CsPoint pos );
	void			_OnMouseRButtonOutWindow_Equip();

	//===============================================================================================================
	//
	//	디지바이스
	//
	//===============================================================================================================
protected:
	cRenderTex*		m_pRenderDigiviceTex;
	cText*			m_pDigiviceText[ 2 ];

	cSprite*		m_pDigiviceBG;
	cSprite*		m_pMemoryIconBG;
	cSprite*		m_pTSkillIconBG;
	cSprite*		m_pChipsetIconBG;
	cSprite*		m_pMemoryIcon_Lock;

	sIFIcon			m_IFIcon_vice_Memory[ nLimit::Memory ];	
	DWORD			m_nTacticsDigimonID[ nLimit::Memory ];
	sCHAR_IMAGE*	m_pTacticsImage[ nLimit::Memory ];

	sIFIcon			m_IFIcon_vice_Chip[ nLimit::Chipset ];
	sIFIcon			m_IFIcon_vice_TSkill[ nLimit::SkillSlot ];
	
	// 디지몬 제거
	cSprite*		m_pCheckDeleteDigimon;
	cEditBox*		m_pEditResistNumber;
	cButton*		m_pDeleteDigimonCloseBtn;
	cButton*		m_pDeleteDigimonOkBtn;
	int				m_nDigimonIdx;
	cStringList		m_pDelString;

public:
	void			InitDeleteDigimon( int nIdx );
	void			DeleteDigimonDel();

protected:
	void			_DeleteDigivice();	
	void			_CreateDigivice();
	void			_InitDigivice();
	void			_InitText_Digivice();	

	bool			_UpdateDigimonDel();
	void			_RenderDeleteDigimon();

	bool			_UpdateIcon_ForMouse_Digivice();
	void			_RenderDigivice();	

	void			_OnMouseOutWindow_Digivice();
	void			_OnLButtonUp_Digivice( CsPoint pos );
	void			_OnRButtonUp_Digivice( CsPoint pos );

	bool			_CursorIconLBtnUp_Digivice( CURSOR_ICON::eTYPE eIconType, int nIconSlot );

	void			_ResetDevice_Digivice();
	

	//===============================================================================================================
	//
	//	테이머 스킬
	//
	//===============================================================================================================
protected:
	cSprite*		m_pSkillBG;
	sIFIcon			m_IFIcon_BaseSkill[ IF_TAMER_BASE_SKILL_COUNT ];
	sIFIcon			m_IFIcon_ActiveSkill[ IF_TAMER_ACTIVE_SKILL_COUNT ];
	sIFIcon			m_IFIcon_PasiveSkill[ IF_TAMER_PASIVE_SKILL_COUNT ];
	cText*			m_pSkillText[ 3 ];

protected:
	void			_DeleteSkill();
	void			_CreateSkill();
	void			_InitSkill();
	void			_InitText_Skill();

	bool			_UpdateIcon_ForMouse_Skill();
	void			_RenderSkill();

	void			_OnMouseOutWindow_Skill();
	void			_OnLButtonUp_Skill( CsPoint pos );
	void			_OnRButtonUp_Skill( CsPoint pos );
	bool			_CursorIconLBtnUp_Skill( CURSOR_ICON::eTYPE eIconType, int nIconSlot );

	void			_ResetDevice_Skill();
#ifdef CROSSWARS_SYSTEM
	//===============================================================================================================
	//
	//	크로스 로더
	//
	//===============================================================================================================


protected:



	struct sINFO
	{
		cSprite*			s_pMainImg;
		cSprite*			s_pNSotImg;

		cSprite*			s_pEnableImg;
		cSpriteAni*			m_pSpriteAniArray;		// 크로스 선택 에니메이션
	
		sCHAR_IMAGE*		s_pCharImage;
		sIFIcon				s_IFIcon;
		DWORD				s_nModelID;
		DWORD				s_nDigimonID;
		int					s_nEnable;
		int					s_nRank;
		int					s_nIdx;

		void				Delete();
		void				Init( CsRect rc )
		{ 
			s_IFIcon.Init( rc ); 
			
			s_nIdx = -1;
			m_pSpriteAniArray = NULL;
			s_pNSotImg = s_pMainImg = s_pEnableImg = NULL; 
			s_pCharImage = NULL;
			s_nRank = s_nEnable = s_nDigimonID = s_nModelID = 0; 
		
		}
		void				CheckModelID( int nModelID );
	};

	typedef	std::vector<sINFO**>						vppINFO;
	typedef	std::vector<sINFO**>::iterator				vppINFO_IT;


	struct sortfind {
		bool operator() ( sINFO** i, sINFO** j ) 
		{ 
			return ( (*i)->s_nRank < (*j)->s_nRank );
		}

		bool operator() ( sINFO** i ) 
		{ 
			return (2 == (*i)->s_nRank);
		}
	} alg;
	

	sINFO*			m_pCrossInfoArray;
	sINFO**			m_pSelectInfo;

	sIFIcon*			m_IFIcon;

	vppINFO				m_VecSelectInfo;
	vppINFO				m_VecCombineInfo;
protected:

	cSprite*		m_pCrossBG;
	cSprite*		m_pCrossHouseBG;


	cButton*		m_pCrossBtn;
	cButton*		m_pHouseOpenBtn;
	cButton*		m_pHouseCloseBtn;
	cButton*		m_pCurHouseBtn;

	bool			m_bViewHouse;
	bool			m_bSummoning;			// 현재 소환되있는가?

	


	cSprite*		m_pCrownRequareLine;		// 크라운 요구치 라인
	//cSprite*		m_pCrownGaugeBG;		//  게이지 bg
	cSprite*		m_pCrownGaugeBar;		// 게이지 bar
	cSprite*		m_pCrownGaugeStencil;		// 게이지 스텐실
	
	cSprite*		m_pCrossEnable;			// 크로스 버튼 활성화 이미지
	cSpriteAni*			m_pCrossEnableAni;		// 크로스 활성화 에니메이션

	CsRect			m_rcCrownGaugeRegion;//m_rcCrownGaugeRegion;	// 게이지 영역

	cRadioButton*		m_pPageRadioBtn;
	int					m_nPageIndex;

	cSprite*			m_pBlock;


	CDigimon*			m_pRenderDigimon;
	cRenderTex*			m_pCrossRenderTex;

	cText*				m_pDigimonName;
	cText*				m_pDigimonHeight;
	cString				m_DigimonElement;
	int					m_nDigimonLevel;

protected:

	void			_DeleteCross();
	void			_CreateCross();
	void			_InitCross( /*bool IsCrossLoader*/ );

	void			_Update_Cross();
	bool			_UpdateIcon_ForMouse_CrossLoader();
	eMU_TYPE		_UpdateIcon_ForMouse_CrossInven();
	void			_RenderCross();
	void			_RenderCrossInven();

	void			_OnMouseOutWindow_Cross();
	void			_OnLButtonUp_Cross( CsPoint pos );
	void			_OnRButtonUp_Cross( CsPoint pos );
	bool			_CursorIconLBtnUp_Cross( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	void			_ResetDevice_Cross();

protected:	
	
	void			SelectDigimon(void);
	bool			_GetCrossEnable();
	void			_SetSelected( int i);
	void			_SetRelease( int i);
	bool			_CheckCodeCrown();
	CsPoint			_CalCrownRate(DWORD dwCurCR, DWORD dwMaxCR, CsPoint Size);

		bool	_RButtonUp_UI( int nInvenIndex );		
		void	_RButtonUp_Npc( int nInvenIndex );
public:	
	void			StatusDigimon(void);
	void			ResetCrossLoader( void );
	CsPoint			_GetCrossLoaderSize(){ return CsPoint( IF_CROSSLOADER_WIDTH, IF_CROSSLOADER_HEIGHT ); }
	CsPoint			_GetCrossInvenSize(){ return CsPoint( IF_CROSSINVEN_WIDTH, IF_CROSSINVEN_HEIGHT ); }
	CsPoint			_GetCrossInvenPos(){ return CsPoint( _GetCrossLoaderSize().x, 0 ) + GetRootClient(); }

	void			_OpenHouseWin(){ m_bViewHouse = true;  m_pCurHouseBtn = m_pHouseCloseBtn; }
	void			_CloseHouseWin(){  m_bViewHouse = false; m_pCurHouseBtn = m_pHouseOpenBtn; }
#endif
};