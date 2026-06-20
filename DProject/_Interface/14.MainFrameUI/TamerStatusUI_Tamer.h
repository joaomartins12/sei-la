#pragma once

#include "../14.MainFrameUI/TamerStatusUI_SubViewer.h"

class cTamerStatusUI_Tamer : public cTamerStatusUI_SubViewer
{
public:
	cTamerStatusUI_Tamer();
	~cTamerStatusUI_Tamer();

	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);	

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			Update_ForMouse();
	virtual void			RenderScript();

	virtual void			ResetDevice();

	virtual void			ReleaseSelect();
	virtual void			UpdateViewer();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

private:
	struct sEquipInfo : public CUSTOMDATA
	{
		sEquipInfo( int nPType, uint nType ) : nPartsType( nPType ), nItemType( nType ) {}
		~sEquipInfo() {}

		int					nPartsType;
		uint				nItemType;
	};
	struct sEquipList
	{
		sEquipList() : pItemIcon( NULL ), pGridItem( NULL ) {}
		~sEquipList() {}

		void				ChangeItem( uint nItemType, CsCoolTimeSeq* pCoolTime );

		cString::sICON*		pItemIcon;
		cGridListBoxItem*	pGridItem;
	};

	std::map< int, sEquipList >		m_mapEquip;
	cGridListBox*			m_pLEquipList;
	cGridListBox*			m_pREquipList;
/////////////////////////////////////////////////////////////////////////////////
	enum eAbilityType{ eAT, eDE, eHP, eDS };
	struct sAbilInfo : public CUSTOMDATA
	{
		sAbilInfo( eAbilityType eType, int nPoint ) : eAbilType( eType ), nAbilPoint( nPoint ) {}
		~sAbilInfo() {}

		eAbilityType		eAbilType;
		int					nAbilPoint;
	};
	struct sAbility
	{
		sAbility() : pAbilText( NULL ), pAbilListItem( NULL ) {}
		~sAbility() {}

		void				SetText( int nAbilPoint );

		cString::sTEXT*		pAbilText;
		cListBoxItem*		pAbilListItem;
	};

	std::map< int, sAbility >		m_mapTamerAbil;
	std::map< int, sAbility >		m_mapApplyAbil;
	cListBox*				m_pTamerAbilList;
	cListBox*				m_pApplyAbilList;
/////////////////////////////////////////////////////////////////////////////////
#ifdef WEB_INFOMATION
	cButton*				m_pWebInfoButton;
#endif

	cText*					m_pLevelText;
	cText*					m_pNameText;
	cText*					m_pGuildText;
	cText*					m_pTamerAbilText;
	cText*					m_pApplyAbilText;
	cText*					m_pFriendShipText;

	CsRect					m_rcFriendShip;

	cRenderTex*				m_pTamerRenderTex;


private:
	BOOL					_SetEquipTooltip( cGridListBoxItem const* pOverItem );
	BOOL					_SetStatTooltip( cListBoxItem const* pOverItem );

	void					_MakeEquipGrid();
	void					_AddEquipGridItem( cGridListBox* pGrid, int nGridIndex, int nEquipType );

	void					_MakeAbilList();
	void					_AddTamerAbilListItem( cListBox* pList, eAbilityType eAbilType, int nAbilPoint );
	void					_AddApplyAbilListItem( cListBox* pList, eAbilityType eAbilType, int nAbilPoint );
	void					_UpdateAbilList();
	void					_ChangeAbilPoint( eAbilityType eAbilType, int nPoint, int nFSPoint );
	int						_ChangeToSystemAbilType( eAbilityType eAbilType );

	void					_UpdateTamerInfo();

#ifdef WEB_INFOMATION
	void					_OnClickWebInfo( void* pSender, void* pData );
#endif
	void					_OnLClickDownEquipItem( void* pSender, void* pData );
	void					_OnRClickEquipItem( void* pSender, void* pData );

	bool					_OnClickEquipItem( cGridListBoxItem const* pOverItem );
};

