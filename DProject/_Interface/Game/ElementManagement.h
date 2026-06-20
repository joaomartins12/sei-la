#pragma once
#include "EquipSystemSubViewer.h"

class cElementManagement : public cEquipSystemSubViewer
{
public:
	cElementManagement();
	~cElementManagement();

public:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			Update_ForMouse();
	virtual void			RenderScript();
	virtual void			ResetDevice();

public:
	virtual void			OnLButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );
	virtual void			UpdateViewer();

private:
	enum eRegistType{ eRegistNone, eRegistElem, eRegistEquip };
	enum eButtonType{ eBtnAnalysis, eBtnSuccess, eBtnCancel };

	struct sButtonInfo : public CUSTOMDATA
	{
		sButtonInfo() : eBtnState( eBtnAnalysis ) {}
		~sButtonInfo() {}

		eButtonType eBtnState;
	};
	struct sRegistInfo : public CUSTOMDATA
	{
		sRegistInfo( eRegistType eRegist, int nIndex ) : eType( eRegist ), nInvenIndex( nIndex ) {}
		~sRegistInfo() {}

		eRegistType			eType;
		int					nInvenIndex;
	};
	struct sElemInfo : public CUSTOMDATA
	{
		sElemInfo( uint nType, u1 nRate ) : nItemType( nType ), nItemRate( nRate ) {}
		~sElemInfo() {}

		void SetItem( uint nType, u1 nRate )
		{
			nItemType = nType;
			nItemRate = nRate;
		}

		uint				nItemType;
		u1					nItemRate;
	};

	struct sRegistItem
	{
		sRegistItem() : pItemIcon( NULL ), pItemName( NULL ), pItemControl( NULL ) {}
		~sRegistItem() {}

		void		ChangeRegistItem( eRegistType eType, uint nItemType, int nInvenIndex, std::wstring wsItemName );
		void		ChangeRate( u1 nRate );
		void		Reset();

		cString::sICON*		pItemIcon;
		cString::sTEXT*		pItemName;
		cListBoxItem*		pItemControl;
	};

	struct sSocketItem
	{
		sSocketItem() : pItemIcon( NULL ), pAttachedImg( NULL ), 
			pSelectedImg( NULL ), pItemControl( NULL ) {}
		~sSocketItem() {}

		void		ChangeSocketItem( uint nItemType, u1 nRate );
		void		SetAttached( bool bVisible );
		void		SetSelected( bool bVisible );
		void		Reset();

		cString::sICON*		pItemIcon;
		cString::sSPRITE*	pAttachedImg;
		cString::sSPRITE*	pSelectedImg;
		cGridListBoxItem*	pItemControl;
	};

	struct sEquipControl 
	{
		sEquipControl() : pGridBox( NULL ), pAttachCost( NULL ), pDetachCost( NULL ), 
			pAttachBtn( NULL ), pDetachBtn( NULL ), pElemEquip( NULL ) {}
		~sEquipControl() {}

		bool		Update_ForMouse();
		CsPoint		GetRootSize() const;
		bool		GetEnable() const;
		void		SetEnable( bool bEnable );
		void		SetAttachUI( bool bVisible, bool bEnable, u8 nMoney = 0 );
		void		SetDetachUI( bool bVisible, bool bEnable, u8 nMoney = 0 );
		void		SetSocketTooltip( cGridListBoxItem const* pOverItem );
		void		SetExplainBtnTooltip();
		void		ResetUI();
		void		AddChildControl( cWindow* pControl );

		std::map< int, sSocketItem >	mapSocket;
		cGridListBox*		pGridBox;
		cMoneyIF*			pAttachCost;
		cMoneyIF*			pDetachCost;
		cButton*			pAttachBtn;
		cButton*			pDetachBtn;
		cScriptUI*			pElemEquip;
	};

	struct sCheckControl
	{
		sCheckControl() : pProgressBar( NULL ), pAnalysisCost( NULL ), pAnalysisBtn( NULL ), pElemCheck( NULL ) {}
		~sCheckControl() {}

		bool		Update_ForMouse();
		bool		GetEnable() const;
		void		SetEnable( bool bEnable );
		void		SetAnalysisUI( bool bVisible, bool bEnable, u8 nMoney = 0 );
		void		SetProgressBar( bool bPlay );
		void		SetAnalysisButton( eButtonType eType );
		void		ResetUI();
		void		AddChildControl( cWindow* pControl );

		cUI_TimerProgressBar*	pProgressBar;
		cMoneyIF*			pAnalysisCost;
		cButton*			pAnalysisBtn;
		cScriptUI*			pElemCheck;
	};

	cListBox*				m_pListBox;

	sRegistItem				m_sRegistItem;
	sEquipControl			m_sEquipCon;
	sCheckControl			m_sCheckCon;

	cButton*				m_pHelpButton;

private:
	void			_MakeEmptyRegistUI();
	void			_MakeEmptyEquipUI();
	void			_MakeEmptyCheckUI();

	void			_MakeEmptySocketGrid( int const& nSlotCount );
	void			_AddElementSocket( int const& nSlotIndex, int const& nItemType, u1 const& nRate, bool const& bAttach );

	void			_SetRegistEquip( uint const& nInvenIndex, uint const& nItemType, int const& nSocketCount, std::wstring const& wsItemName );
	void			_SetRegistSocket( int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable );
	void			_SetRegistElement(uint const& nInvenIndex, uint const& nItemType, std::wstring const& wsItemName, u8 const& nMoney, bool const& bEnable);
	void			_SetRegistItemTooltip( cListBoxItem const* pOverItem );

	void			_SuccessAnalysis( u1 const& nRate );

	void			_SuccessAttach();
	void			_SuccessDetach();
	void			_SelectSocket( int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable );
	void			_ReleaseSocket( int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable, bool const& bReleaseAll );

private:
	void			_OnClickAnalysisButton( void* pSender, void* pData );
	void			_OnClickAttachButton( void* pSender, void* pData );
	void			_OnClickDetachButton( void* pSender, void* pData );
	void			_OnClickElementSocket( void* pSender, void* pData );
	void			_OnRClickElementSocket( void* pSender, void* pData );
	void			_OnRClickRegistItem( void* pSender, void* pData );
	void			_OnClickHelpButton( void* pSender, void* pData );
	void			_ProcessSuccess( void* pSender, void* pData );
};