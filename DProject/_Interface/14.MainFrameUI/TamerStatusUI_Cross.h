#pragma once
#ifdef CROSSWARS_SYSTEM

#include "../14.MainFrameUI/TamerStatusUI_SubViewer.h"

class cTamerStatusUI_Cross : public cTamerStatusUI_SubViewer
{
public:
	cTamerStatusUI_Cross();
	~cTamerStatusUI_Cross();

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
	// 크로스 워즈 칩셋 정보
	struct sCrossInvenInfo : public CUSTOMDATA
	{
		sCrossInvenInfo( uint nItemType, int nCount, int nIndex ) 
			: nCrossItemType(nItemType), nCrossItemCount(nCount), nCrossIndex(nIndex) {}
		~sCrossInvenInfo() {}

		uint			nCrossItemType;
		int				nCrossItemCount;
		int				nCrossIndex;
	};
	struct sCrossInven
	{
		sCrossInven() : pCrossIcon(NULL), pGridItem(NULL) {}
		~sCrossInven() {}

		void				ChangeCrossItem( uint nItemType, int nItemCount );

		cString::sSPRITE*	pSlotImg;
		cString::sICON*		pCrossIcon;
		cGridListBoxItem*	pGridItem;
	};
	std::map< int, sCrossInven > m_mapCrossInven;
	cGridListBox*			m_pCrossInven;

	struct sCrossTacticsInfo : public CUSTOMDATA
	{
		sCrossTacticsInfo( DWORD dwBaseID, int nIndex ) : dwTacticsID( dwBaseID ), nTacticsIndex( nIndex ){}
		~sCrossTacticsInfo(){}

		DWORD				dwTacticsID;
		int					nTacticsIndex;
	};
	struct sCrossTactics
	{
		sCrossTactics()
			: pTacticsImg(NULL), pSelectImg(NULL), pConnectImg(NULL), pEnableImg(NULL), pGridItem(NULL) {}
		~sCrossTactics(){}

		void				UpdateCrossTactics( std::string strPath, DWORD dwBaseID, bool bConnect, bool bEnable );

		cString::sSPRITE*	pTacticsImg;
		cString::sSPRITE*	pSelectImg;
		cString::sSPRITE*	pConnectImg;
		cString::sSPRITE*	pEnableImg;
		cGridListBoxItem*	pGridItem;
	};
	std::map< int, sCrossTactics >	m_mapCrossTactics;
	cGridListBox*			m_pCrossTactics;

	CDigimon*				m_pRenderDigimon;
	cRenderTex*				m_pDigimonRenderTex;
	
	cButton*				m_pDigiCrossBtn;

	cText*					m_pLevelText;
	cText*					m_pNameText;
	cText*					m_pChipsetText;

	cUI_ProgressBar*		m_pCrossProgress;

private:
	void					_MakeCrossInven();
	void					_AddCrossItem( int nIndex );

	void					_MakeCrossTactics();
	void					_AddCrossTactics( int nIndex );

	BOOL					_SetCrossInvenToolTip( cGridListBoxItem const* pOverItem );
	void					_UseCrossInvenItem( int nIndex );

	void					_ChangeCInvenSize( int nCount );
	void					_ChangeCrossTactics( int nIndex );

	void					_OnClickDigiCross( void* pSender, void* pData );
	void					_OnClickTactics( void* pSender, void* pData );
	void					_OnLCLickCrossInven( void* pSender, void* pData );
	void					_OnRClickCrossInven( void* pSender, void* pData );
	bool					_OnClickCrossInven( cGridListBoxItem const* pOverItem );
};

#endif // CROSSWARS_SYSTEM
