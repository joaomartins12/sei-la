
#pragma once

class cListBoxItem : public NiMemObject
{
public:
	cListBoxItem();
	cListBoxItem(UINT item_ID, bool disabled = false );
	virtual ~cListBoxItem(void);

	void			ResetDevice();
	void			Delete();

	void			SetItem( cString* pSetItem );
	cString*		GetItem()	const;

	UINT			getID(void) const;
	bool			isDisabled(void) const;
	void			setID(UINT nItemID);

	void			SetAlpha( float fAlpha );

	int				getItemHeight() const;
	void			setSelected(bool setting);
	bool			isSelected(void) const;
	void			SetItemHeight( int const& nItemHeigh );

	cListBoxItem*	getTreeItemFromIndex(size_t itemIndex);
	virtual void	Render(CsPoint csPos);

	void			Update( float const& fDeltaTime );

	void			OnMouseOver();
	void			OnMouseLeave();
	void			OnMouseBtnDown();
	void			OnMouseBtnUp();
	void			onMousePressed();

	void			OnMouseOver(CsPoint csPt);
	void			OnMouseLeave(CsPoint csPt);
	void			OnMouseBtnDown(CsPoint csPt);
	void			OnMouseBtnUp(CsPoint csPt);
	void			onMousePressed(CsPoint csPt);

	void			SetUserData( CUSTOMDATA* pUserData );
	CUSTOMDATA*		GetUserData() const;
	CsPoint			GetWorldPos() const;
	void			SetVisible(bool bValue);
	bool			GetVisible() const;
	void			SetScrollVisible(bool bValue);	//2017-06-05-nova
	bool			GetScrollVisible() const;		//2017-06-05-nova

protected:
	cString	*		m_pItem;
	CUSTOMDATA*		m_pUserData;
	int				m_itemID;
	bool			m_bSelected;
	bool			m_bDisabled;
	int				m_nItemHeight;
	bool			m_bVisible;
	bool			bScrollVisible;		//2017-06-05-nova	Scroll에서 화면에 그려지는 여부 체크 if( pos.y + (*it)->getItemHeight() > csTotalSize.y)
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class cListBox : public cWindow
{
public:
	cListBox();
	virtual ~cListBox();

	enum LIST_EVENT
	{
		LIST_ADDITEM  = 0,
		LIST_INSERTITEM,
		LIST_REMOVEITEM,
		LIST_SELECT_CHANGE_ITEM,
		LIST_SELECTED_ITEM,	// 눌렀던 아이템 눌러도 호출되는 이벤트
		LIST_SELECTED_ITEM_RIGHT_DOWN,	//선택 아이템을 우클릭
		LIST_SELECTED_ITEM_RIGHT_UP,
		LIST_ITEM_RIGHT_UP,				//무조건 우클릭업 이벤트 들어온다
		LIST_ITEM_MOUSE_OVER,
	};

public:
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize );
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, char* cBGImagePath, bool bApplyWindowSize );

	virtual void	Delete();

	virtual void	SetAlpha( float fAlpha );
	virtual int		Update_ForMouse( CsPoint ptParent );	
	virtual void	Render();
	
	virtual void	ResetDevice();
	virtual eTYPE	GetType(){ return T_ListBox; }

	virtual void	OnMouseLeave();
	virtual void    Update(float const& fDeltaTime);
	virtual	void	SetEnable( bool bValue );

	void			SetBackGorundImg( char* cImagePath, float fAlpha = 1.0f );
	void			SetMouseOverImg( char* cImagePath, float fAlpha = 1.0f );
	void			SetSelectedImg( char* cImagePath, float fAlpha = 1.0f );
	void			SetMouseOverImg(cImage* pImage, bool bSizeFix = false);
	void			SetSelectedImg(cImage* pImage, bool bSizeFix= false);

	bool			AddItem( cListBoxItem* pAddItem );
	bool			AddItemMultiHeight( cListBoxItem* pAddItem, int minusHeight );	//2017-06-05-nova
	bool			InsertItem( int nPos, cListBoxItem* pAddItem );
	void			RemoveItem( cListBoxItem* pAddItem );
	void			RemoveItem( int nPos );
	void			RemoveAllItem();

	void			SetMargin(CsPoint margin);
	CsPoint			GetMargin() const;
	void			SetItemGab(CsPoint gab);

	int				GetItemIndex(cListBoxItem const* pFindItem) const;
	void			SetScrollBar( cScrollBar* pScrollBar );

	cListBoxItem const*	GetItemFormPos( int nPos ) const;
	cListBoxItem const*	GetMouseOverItem() const; 
	cListBoxItem const*	GetMouseDownItem() const;


	size_t			GetItemCount() const;
	cListBoxItem	const*	GetSelectedItem() const;
	int				GetSelectedItemIdx() const;
	//void			SetSelectMode(int MainID, int SubID);
	void			SetItemHeight( int nItemHeight );
	

	void			ChangeSelectFront();
	void			ChangeSelectNext();
	void			ShowSelectItemScrollBar();

	void			SetSelectedItemFromIdx( int const& nSelIdx, bool bSendEvent = true );
	void			SetSelectedItemFromItemPtr( cListBoxItem * pFindItem, bool bSendEvent = true );
	void			SetBackOverAndSelectedImgRender(bool bValue);

	void			SetScrollBarFirstPos();
	void			SetScrollBarEndPos();

	void			UnSelectionItem();
	void			UnSelectionItem(int const& nIdx);
	void			UnSelectionItem(cListBoxItem const* pFindItem);
	void			SetVisibleItemAll( bool bValue );
	void			SetVisibleItem( bool bValue, int const& nIdx, bool bUpdateScrollbar = true );
	void			SetVisibleItems( bool bValue, std::list<int> const& nIdx );
	void			SetVisibleItems( std::map<int, bool> const& nIdx );
	void			SetHideItemRender( bool bValue );
	void			SetManaulSelectChange( bool bValue );
	bool			IsManaulSelectChange() const;

	CsPoint			GetItemtoMousePos( CsPoint localPos, cListBoxItem * pFindItem );

	void			SetScrollBarRenderCount(int cnt);	//2017-06-05-nova
	void			changeAllItemHeight(int const& nNewHeight);
	void			configureScrollbars();

	void			SetAutoItemSelectStateChange(bool bValue);

private:
	float			getItemAtPoint(CsPoint const& pt, cListBoxItem const * pFindItem );
	cListBoxItem*	getItemFromListAtPoint(CsPoint & localpt, int const& nStartIdx);

private:
	void			UpdateRects();
	
	float			getTotalItemHeight(void) const;

	bool			onMouseButtonDown(CsPoint ptParent);
	bool			onMouseButtonUp(CsPoint ptParent);
	bool			onMouseMove(CsPoint ptParent);
	bool			onMousePressed(CsPoint ptParent);

	bool			changeSelectItem( cListBoxItem* pSelItem );
	bool			selectionChanged( cListBoxItem* pSelItem, bool bSendEvent = true );
	

	cListBoxItem*	getItemAtPoint(CsPoint const& pt) const;
	cListBoxItem*	getItemFromListAtPoint(float* bottomY, CsPoint const& pt, int const& nStartIdx) const;
	//20170215
	bool			onMouseRButtonDown(CsPoint ptParent);
	bool			onMouseRButtonUp(CsPoint ptParent);

private:
	bool					m_bEnable;
	cScrollBar*				m_pScrollBar;
	cSprite*				m_pBackGroundImg;
	cSprite*				m_pSelectedImg;
	cSprite*				m_pMoverImg;

	std::list<cListBoxItem*>m_listItems;

	cListBoxItem*			m_pSelectedItem;
	cListBoxItem*			m_pOverItem;
	cListBoxItem*			m_pDownItem;

	int						m_nItemHeight;
	CsPoint					m_CsMargin;
	CsPoint					m_CsItemGab;
	bool					m_bBackOverAndSelectedImgRender;
	bool					m_bItemHideRender;
	bool					m_bManaulSelectChange;
	bool					m_bAutoItemSelectStateChange;
};