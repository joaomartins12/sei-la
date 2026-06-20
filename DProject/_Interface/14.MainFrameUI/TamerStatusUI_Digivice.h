#pragma once

#include "../14.MainFrameUI/TamerStatusUI_SubViewer.h"

class cTamerStatusUI_Digivice : public cTamerStatusUI_SubViewer
{
public:
	cTamerStatusUI_Digivice();
	~cTamerStatusUI_Digivice();

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
	struct sChipsetInfo : public CUSTOMDATA
	{
		sChipsetInfo( uint nType, int nRate, int nIndex ) : nItemType( nType ), nItemRate( nRate ), nChipsetIndex( nIndex ) {}
		~sChipsetInfo() {}

		uint				nItemType;
		int					nItemRate;
		int					nChipsetIndex;
	};
	struct sChipset
	{
		sChipset() {}
		~sChipset() {}

		void					ChangeItem( uint nItemType, uint nItemCount = 1 );
		void					SetChipsetBg( bool bIsVisible );

		cString::sSPRITE*		pChipsetBg;
		cString::sICON*			pChipsetIcon;
		cGridListBoxItem*		pGridItem;
	};
	std::map< int, sChipset >	m_mapChipset;
	cGridListBox*			m_pChipset;
	cText*					m_pChipsetText;

	struct sTacticsInfo : public CUSTOMDATA
	{
		sTacticsInfo( int nIndex ) : nTacticsIndex( nIndex ) {}
		~sTacticsInfo() {}

		int				nTacticsIndex;
	};
	struct sTactics
	{
		sTactics() : pSlotImg( NULL ), pDigimonImg( NULL ), pCanopyImg( NULL ), pLevelText( NULL ) {}
		~sTactics() {}

		void					UpdateDigimon( int nTacticsIndex, int nTacticsLevel, bool bIsOpen, std::string strPath, int nHatchLv );

		cString::sSPRITE*		pSlotImg;
		cString::sSPRITE*		pDigimonImg;
		cString::sSPRITE*		pCanopyImg;
		cString::sTEXT*			pLevelText;
		cListBoxItem*			pListItem;
	};
	std::map< int, sTactics >	m_mapTactics;
	cListBox*				m_pLTactics;
	cListBox*				m_pRTactics;

private:
	int						_GetSlotType( int nIndex );

	BOOL					_SetTacticsTooltip( cListBoxItem const* pOverItem );

	void					_MakeChipsetGrid();
	void					_AddChipsetGridItem( int nIndex, uint nItemType, int nItemRate, int nCount = 1 );

	void					_MakeTacticsList();
	void					_AddTacticsListItem( cListBox* pListBox, int nIndex, CsPoint ptPos, bool bIsLeft );

	void					_OnMouseOverChipset( void* pSender, void* pData );
	void					_OnLClickChipset( void*pSender, void* pData );
	void					_OnRClickChipset( void* pSender, void* pData );
	bool					_OnClickChipset( cGridListBoxItem const* pOverItem );

	void					_OnClickTactics( void* pSender, void* pData );
};

