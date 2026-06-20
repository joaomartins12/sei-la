#pragma once

#include "../00.Contents/MainFrame_Contents.h"

class cDigimonStatusUI : public cBaseWindow, public CMainFrameContents::ObserverType
{
public:
	cDigimonStatusUI();
	~cDigimonStatusUI();

private:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();

	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetMap(void){ Close( false ); }
	virtual void			ResetDevice();

	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual bool			OnEscapeKey();

private:
	// 위에서부터 순서대로
	enum eAbilityType{ eHP, eDS, eAT, eAS, eCT, eHT, eDE, eBL, eEV, eMAX };
	struct sAbilInfo : public CUSTOMDATA
	{
		sAbilInfo( eAbilityType eType ) : eAbilType( eType ) {}
		~sAbilInfo() {}

		eAbilityType		eAbilType;
	};
	struct sAbility
	{
		sAbility() : pAbilText( NULL ), pAbilListItem( NULL ) {}
		~sAbility() {}

		void				UpdateAbility( std::wstring wsText, bool bIsUpper );

		cString::sTEXT*		pAbilText;
		cListBoxItem*		pAbilListItem;
	};
	cListBox*				m_pAbilList;
	std::map< int, sAbility >	m_mapAbility;

	enum eButtonType{ eLevelUp, eDelete };
	struct sButtonInfo : public CUSTOMDATA
	{
		sButtonInfo( eButtonType eType, int nIndex ) : eBtnType( eType ), nSkillIndex( nIndex ) {}
		~sButtonInfo() {}

		eButtonType		eBtnType;
		int				nSkillIndex;
	};
	struct sSkillInfo : public CUSTOMDATA
	{
		sSkillInfo( int nIconType, int nIndex ) : nSkillIconType( nIconType ), nSkillIndex( nIndex ) {}
		~sSkillInfo() {}

		int					nSkillIconType;
		int					nSkillIndex;
	};
	struct sSkill
	{
		sSkill() : pSkillIcon( NULL ), pSkillMask( NULL ), pSkillSlotImg( NULL ), pSkillLockImg( NULL ), pSkillLvUpBtn( NULL ), 
			pSkillDeleteBtn( NULL ), pSkillLevelTxt( NULL ), pSkillGridItem( NULL ) {}
		~sSkill() {}

		void				UpdateSkill( int nSkillType, int nCurLevel, int nMaxLevel, bool bIsLvUp, bool bIsMask );
		void				ResetSkill();
		void				SetDeleteButton( int nMemorySkill );

		cString::sICON*		pSkillIcon;
		cString::sSPRITE*	pSkillMask;
		cString::sSPRITE*	pSkillSlotImg;
		cString::sSPRITE*	pSkillLockImg;
		cString::sBUTTON*	pSkillLvUpBtn;
		cString::sBUTTON*	pSkillDeleteBtn;
		cString::sTEXT*		pSkillLevelTxt;
		cGridListBoxItem*	pSkillGridItem;
	};
	cGridListBox*			m_pSkillGrid;
	std::map< int, sSkill > m_mapSkill;

	struct sElementUI
	{
		sElementUI() : pElement(NULL), pUpArrow(NULL), pDownArrow(NULL) {}
		~sElementUI() {}

		void				ChangeElement( int nType );
		void				SetSuperiority( bool bIsUp );
		void				Reset();

		cImage*				pElement;
		cSprite*			pUpArrow;
		cSprite*			pDownArrow;
	};
	typedef std::map< int, sElementUI > MAP_ELEMENT;
	typedef MAP_ELEMENT::iterator MAP_ELEMENT_IT;

	struct sEvoBtnInfo : public CUSTOMDATA
	{
		sEvoBtnInfo( int nIndex ) : nEvoIndex( nIndex ) {}
		~sEvoBtnInfo() {}

		int					nEvoIndex;
	};
	struct sEvoInfoUI
	{
		sEvoInfoUI() : pSlotBgImg(NULL), pRideFrameImg(NULL), pLockImg(NULL), pSelMaskImg(NULL), pDigimonImg(NULL), 
			pEvolBtn(NULL), pRideBtn(NULL), bEnable(false) {}
		~sEvoInfoUI() {}

		bool				UpdateMouse( int nIndex );
		void				Reset();
		void				SetSlot( std::string strPath, int nEvoSlot, int nState, bool bVisible, bool bEnable, bool bLock, bool bEvoBtn, bool bXAnti );
		void				SetPos( CsPoint ptPos, bool bXAnti );
		CsPoint				GetPos() const;

		cSprite*			pSlotBgImg;
		cSprite*			pRideFrameImg;
		cSprite*			pLockImg;
		cSprite*			pSelMaskImg;
		cImage*				pDigimonImg;
		cButton*			pEvolBtn;
		cButton*			pRideBtn;
		bool				bEnable;
	};
	typedef std::map< int, sEvoInfoUI > MAP_EVOINFO;
	typedef MAP_EVOINFO::iterator MAP_EVOINFO_IT;
	MAP_EVOINFO				m_mapEvoInfo;

	cStringList				m_BaseInfo;

	cSprite*				m_pBackGround;
	cSprite*				m_pLineX;
	cSprite*				m_pLineY;

	cImage*					m_pMyNormal;
	cSprite*				m_pNormalFull;
	cUI_ProgressBar*		m_pNormalExp;
	MAP_ELEMENT				m_mapCompNormal;

	cImage*					m_pMyNature;
	cSprite*				m_pNatureFull;
	cUI_ProgressBar*		m_pNatureExp;
	MAP_ELEMENT				m_mapCompNature;

	cUI_ProgressBar*		m_pDigimonExpBar;
	cUI_ProgressBar*		m_pSkillExpBar;
	cText*					m_pSkillExp;
	cText*					m_pSkillPoint;

	cText*					m_pDigimonLevel;
	cText*					m_pDigimonName;
	cText*					m_pEnchant;
	cSprite*				m_pEnchantMax;
	cImage*					m_pDigimonRank;

	cButton*				m_pTalkButton;
	cButton*				m_pHelpButton;
	cButton*				m_pCloseButton;

	cRenderTex*				m_pRenderDigimon;
	cRenderTex*				m_pRenderXDigimon;
	bool					m_bPendingUpdateStat;
	bool					m_bPendingUpdateSkill;
	bool					m_bPendingChangeDigimon;

private:
	void					_MakeAbilList();
	void					_AddAbilList( int nIndex );
	void					_UpdateAbility();
	int						_ChangeToSystemAbilType( eAbilityType eType );
	std::wstring			_AbilToStr( eAbilityType eType );

	void					_MakeSkillGrid();
	void					_AddSkillGrid( int nIndex );
	void					_UpdateSkill();
	void					_ResetSKill();

	void					_MakeBaseInfo();
	void					_UpdateBaseInfo();

	void					_MakeAttributeInfo();
	void					_AddNormalElement( CsPoint& ptStart );
	void					_AddNatureElement( CsPoint& ptStart );
	void					_UpdateElement();

	void					_MakeEvoInfo();
	void					_AddEvoInfo();
	void					_UpdateEvoInfo();
	bool					_UpdateMouseEvoInfo();
	void					_ResetEvoInfo();
	bool					_RequestEvolve( int nIndex );

	bool					_SetBaseInfoTooltip();
	bool					_SetEvoInfoTooltip();
	bool					_SetStatToolTip( cListBoxItem const* pOverItem );
	bool					_SetSkillTooltip( cGridListBoxItem const* pOverItem );

	void					_OnLClickDownSkill( void* pSender, void* pData );
	void					_OnClickSkillButton( void* pSender, void* pData );
	void					_OnClickTalkButton( void* pSender, void* pData );
	void					_OnClickHelpButton( void* pSender, void* pData );
	void					_OnClickCloseButton( void* pSender, void* pData );
	void					_OnClickEvoButton( void* pSender, void* pData );
	void					_OnClickRideButton( void* pSender, void* pData );
	void					_OnClickAttributeNormal( void* pSender, void* pData );
	void					_OnClickAttributeNature( void* pSender, void* pData );
	void					_OnClickDigimonImage( void* pSender, void* pData );

	void					_RenderLine();
};
