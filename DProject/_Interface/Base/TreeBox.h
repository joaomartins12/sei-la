
#pragma once


class cTreeBoxItem : public NiMemObject
{
public:
		typedef std::list<cTreeBoxItem*>		LBItemList;

		cTreeBoxItem();
		cTreeBoxItem(UINT item_ID, bool disabled = false );
		virtual ~cTreeBoxItem(void);

		void			ResetDevice();

		void			SetItem( cString* pSetItem );
		cString*		GetItem();
		void			AddChildItem( cTreeBoxItem* pAddItem );
		void			DeleteAllChildItem();

		void			Delete();
		UINT			getID(void) const;
		bool			isSelected(void) const;
		bool			isDisabled(void) const;
		void			setID(UINT nItemID);

		void			SetAlpha( float fAlpha );
		bool			getIsOpen(void);
		void			setIsOpen(bool bOpen);
		void			toggleIsOpen(void);
		size_t			getItemCount(void) const;
		LBItemList&		getItemList(void);

		int				getItemHeight() const;
		void			setSelected(bool setting);

		cTreeBoxItem*	getTreeItemFromIndex(size_t itemIndex);
		virtual void	Render(CsPoint csPos);

		void			SetUserData(CUSTOMDATA* pData);
		CUSTOMDATA*		GetUserData() const;

		void			SetOpenButton(cButton* pOpenBtn, CsPoint csDeltaPos);
		void			SetCloseButton(cButton* pOpenBtn, CsPoint csDeltaPos);

		void			OnMouseOver();
		void			OnMouseLeave();
		void			OnMouseBtnDown();
		void			OnMouseBtnUp();
		void			onMousePressed();
		void			SetControlOnMode(bool bValue);

		std::wstring	GetItemText();
		void			GetItemList( std::list<cTreeBoxItem*>& BoxItemList );

	protected:
		cString*		m_pItem;
		cButton*		m_pOpenBtn;
		cButton*		m_pCloseBtn;

		CUSTOMDATA*		m_pUserDate;
		int				m_itemID;
		bool			m_bSelected;
		bool			m_bDisabled;
		bool			m_bIsOpen;
		int				m_nItemHeight;
		LBItemList		m_listItems;

		CsPoint			m_pOpenDataPos;
		CsPoint			m_pCloseDataPos;
};

class cTreeBox : public cWindow
{
	friend class cTreeBoxItem;
	typedef std::list<cTreeBoxItem*>		LBItemList;

public:
	enum TREE_EVENT
	{
		TreeEvent_Item_MouseDown,
		TreeEvent_Item_MouseUp,
		TreeEvent_Selection_Changed,
		TreeEvent_ListContents_Changed,
		TreeEvent_SortMode_Changed,
		TreeEvent_ItemToggle_Changed,
		TreeEvent_Node_State_Changed,
		TreeEvent_MouseOverItem_Change, // [6/8/2016 hyun] 마우스 오버된 아이템이 변경시
		TreeEvent_MouseOverItem_Release,	 // [6/8/2016 hyun] 마우스 오버 해제
	};

	cTreeBox();
	virtual ~cTreeBox();

public:
	void					Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize );
	void					Init( cWindow* pParent, CsPoint pos, CsPoint size, char* cBGImagePath, bool bApplyWindowSize );

	virtual void			Delete();

	virtual void			SetAlpha( float fAlpha );
	virtual void			Update(float const& fDeltaTime) {}
	virtual int				Update_ForMouse( CsPoint ptParent );	
	virtual void			Render();

	virtual void			ResetDevice();
	virtual eTYPE			GetType(){ return T_TreeBox; }

	virtual void			OnMouseLeave();
	virtual	void			SetEnable( bool bValue );

	bool					AddItem( cTreeBoxItem* pAddItem );
	void					SetBackGorundImg( char* cImagePath );
	void					SetScrollBar( cScrollBar* pScrollbar );

	int						GetItemIdx( cTreeBoxItem const * pAddItem ) const;
	size_t					GetItemCount() const;
	cTreeBoxItem const *	GetItem( int const& nIdx ) const;
	void					SetSelectMode(int MainID, int SubID);

	void					RemoveAllItem();
	void					RemoveItem( const int nIdx );

	bool					darwItemList( LBItemList& itemList, CsPoint& itemPos, CsRect const& rcRender, int const& nStartIdx, int & nCount );
	bool					isAutoDeleted(void) const;
	void					setAutoDeleted(bool setting);

	void					setChildItemGab(CsPoint gab);
	void					setItemGab(CsPoint gab);
	void					setMargin(CsPoint gab);
	void					setItemHeight(int itemHeight);
	void					UpdateRects();

	void					GetItemList( std::list<cTreeBoxItem*>& BoxItemList );
	void					SetOnlyOpenMode(bool bMode);

private:
	bool					selectionChanged( cTreeBoxItem * pSelItem );
	bool					onMouseButtonDown(CsPoint ptParent);
	bool					onMouseButtonUp(CsPoint ptParent);
	bool					onMouseMove(CsPoint ptParent);
	bool					onMousePressed(CsPoint ptParent);

	cTreeBoxItem*			getItemAtPoint(CsPoint const& pt) const;
	cTreeBoxItem*			getItemFromListAtPoint(const LBItemList& itemList, float* bottomY, CsPoint const& pt, int const& nStartIdx, int & nCount) const;

	void					configureScrollbars();
	float					getTotalItemHeight(void) const;
	void					getTotalItemsInListHeight(const LBItemList& itemList, float* heightSum) const;
	void					getShowItemSize(const LBItemList& itemList, UINT* nCount);
	void					closeAllItem();

	bool					BtnOnEvent(CsPoint pt);

private:
	bool					m_bEnalbe;
	cScrollBar*				m_pScrollBar;
	cSprite*				m_pBackGroundImg;

	cTreeBoxItem*			m_MouseOverItem;
	cTreeBoxItem*			m_MouseDownItem;
	cTreeBoxItem*			m_SelectedItem;

	LBItemList				m_listItems;

	CsPoint					m_CsChildItemGab;
	CsPoint					m_CsItemGab;
	CsPoint					m_CsMargin;

	int						m_nItemHeight;
	bool					m_bAutoDeleted;
	bool					m_bOnlyOpenMode;
};


