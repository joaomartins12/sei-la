
#pragma once

class cGridListBoxItem : public NiMemObject
{
public:
	cGridListBoxItem();
	cGridListBoxItem(UINT item_ID, CsPoint ptSize, bool disabled = false );
	virtual ~cGridListBoxItem(void);

	void			ResetDevice();
	void			Delete();
	void			Update( float const& fDeltaTime );

	void			SetItem( cString* pSetItem );
	cString*		GetItem()	const;

	UINT			getID(void) const;
	void			setID(UINT nItemID);

	void			SetAlpha( float fAlpha );

	void			setSelected(bool setting);
	bool			isSelected(void) const;

	CsPoint			getItemSize() const;
	void			setItemSize( CsPoint const& nItemHeigh );

	virtual void	Render(CsPoint csPos);

	void			OnMouseOver();
	void			OnMouseLeave();
	void			OnMouseBtnDown();
	void			OnMouseBtnUp();
	void			onMousePressed();

	void			SetUserData( CUSTOMDATA* pUserData );
	CUSTOMDATA*		GetUserData() const;
	CsPoint			GetWorldPos() const;

	void			SetVisible( bool bVisible ) const;
	bool			GetVisible() const;

	void			SetEnable( bool bEnable ) const;
	bool			GetEnable() const;
	//visible 소팅을 위한 오퍼레이트 오버로딩.
	//bool			VisibleComp(const cGridListBoxItem& first, const cGridListBoxItem& second);
	

protected:
	cString	*		m_pItem;
	CUSTOMDATA*		m_pUserData;
	int				m_itemID;
	bool			m_bSelected;
	CsPoint			m_ptItemSize;		//grid item size
	mutable bool	m_bVisible;
	mutable bool	m_bEnable;
	CsPoint			m_ptLastRenderPos;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class cGridListBox : public cWindow
{
public:
	cGridListBox();
	virtual ~cGridListBox();

	enum GRID_EVENT
	{
		GRID_ADDITEM  = 0,
		GRID_INSERTITEM,
		GRID_REMOVEITEM,
		GRID_SELECT_CHANGE_ITEM,
		GRID_SELECTED_ITEM,
		GRID_CLICKED_ITEM, // 눌렀던 아이템 눌러도 호출되는 이벤트
		GRID_SELECTED_ITEM_RIGHT,	//선택 아이템을 우클릭
		GRID_MOUSE_DOWN_ON_ITEM, //아이템 위에서 마우스 다운
		GRID_MOUSE_UP_ON_GRIDLIST, //그리드 리스트 위에서 MOUSE_UP이 일어날때 
		GRID_CHANGED_MOUSEON_ITEM,//아이템이 이동할때
		GRID_BUTTON_UP,				//아이템 내부의 cString이 버튼을 가지고 있을 때 ButtonUp 이벤트 발생
		GRID_SPRITE_UP,				//아이템 내부의 cString이 스프라이트를 가지고 있을 때 At Mouse On Sprite in ITem 
		GRID_MOUSE_LEAVE,			// 마우스가 그리드에서 나갈때
	};
	enum RenderType
	{
		//가로로 다음 아이템이 그려진다.
		LowRightDown,			//키패드 3번 방향으로 확장
		LowRightUp,				//키패드 9번 방향으로 확장
		LowLeftDown,			//키패드 1번 방향으로 확장
		LowLeftUp,				//키패드 7번 방향으로 확장

		//세로로 다음 아이템이 그려진다.
		ColumnDownRight,		//키패드 3번 방향으로 확장
		ColumnDownLeft,			//키패드 1번 방향으로 확장
		ColumnUpRight,			//키패드 9번 방향으로 확장
		ColumnUpLeft,			//키패드 7번 방향으로 확장
	};
	enum StartPointType
	{
		RightTop,				//오른쪽 상단 LowLeftDown, ColumnDownLeft
		RightBottom,			//오른쪽 하단 LowLeftUp, ColumnUpLeft
		LeftTop,				//왼쪽 상단 LowRightDown, ColumnDownRight
		LeftBottom,				//왼쪽 하단 LowRightUp, ColumnUpRight
	};

public:
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, CsPoint gapSize, CsPoint itemSize, RenderType enRenderType, StartPointType enStartPointType, bool bApplyWindowSize, int nItemColumnSize );
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, CsPoint gapSize, CsPoint itemSize, RenderType enRenderType, StartPointType enStartPointType, char* cBGImagePath, bool bApplyWindowSize, int nItemColumnSize );

	virtual void	Delete();

	virtual void	SetAlpha( float fAlpha );
	virtual int		Update_ForMouse( CsPoint ptParent );	
	virtual void    Update( float const& fDeltaTime );
	virtual void	Render();
	
	virtual void	ResetDevice();
	virtual eTYPE	GetType(){ return T_ListBox; }
	virtual	void	SetEnable( bool bValue );

	void			SetBackGorundImg( char* cImagePath );
	void			SetMouseOverImg( char* cImagePath );
	void			SetSelectedImg( char* cImagePath );
	void			SetMouseOverImg(cImage* pImage);
	void			SetSelectedImg(cImage* pImage);
	void			SetBackGorundImg( char* cImagePath, CsPoint const& pos, CsPoint const& size );
	void			SetMouseOverImg( char* cImagePath , CsPoint const& pos, CsPoint const& size);
	void			SetSelectedImg( char* cImagePath, CsPoint const& pos, CsPoint const& size );

	bool			AddItem( cGridListBoxItem* pAddItem );
	bool			InsertItem( int nPos, cGridListBoxItem* pAddItem );
	void			RemoveItem( cGridListBoxItem* pAddItem );
	void			RemoveItem( int nPos );
	void			RemoveAllItem();

	void			SetItemGab(CsPoint gab);

	void			SetScrollBar( cScrollBar* pScrollBar );
	void			SetScrollBarFirstPos();
	void			SetScrollBarEndPos();
	void			SetScrollBarPos( int const& nVisibleIndex );

	cGridListBoxItem const*	GetItemFormGridItemID( int const& nGridID ) const;
	cGridListBoxItem const*	GetItemFormPos( int nPos ) const;
	cGridListBoxItem const*	GetMouseOverItem() const; 
	int				GetItemIndex(cGridListBoxItem const* pFindItem);

	size_t			GetItemCount() const;
	cGridListBoxItem	const*	GetSelectedItem() const;
	int				GetSelectedItemIdx();
	void			SetSelectMode(int MainID, int SubID);
	void			SetItemSize( CsPoint ptItemSize );

	void			onMouseLeave();

	void			ChangeSelectFront();
	void			ChangeSelectNext();

	void			SetSelectedItemFromIdx( int const& nSelIdx, bool bSendEvent = true );
	void			SetBackOverAndSelectedImgRender(bool bValue);
	void			SetUsePressedMoveEvent(bool TOF);

	void			SetRenderType( RenderType renderType );
	RenderType		GetRenderType();
	void			SetStartPointType( StartPointType startPointType );
	StartPointType	GetStartPointType();

	void			AutoSetExtendSize();
	void			VisibleSort();
	void			SortforSealMaster();
	void			SortforSealMasterCount();

	cGridListBoxItem*	getItemAtPoint(CsPoint const& pt) const;
	cGridListBoxItem*	getItemFromListAtPoint(float* bottomY, CsPoint const& pt, int const& nStartIdx) const;
	void			RecountItemID(int nPos);
	void			configureScrollbars();
	std::list<cGridListBoxItem*>* GetListItem();

	void			SetMargin(CsPoint ptMargin);
	CsPoint			GetMargin();
	void			ChangeShowLinePos( int const& nLineNumber );

	int				GetShowLineStartPos() const;
	int				GetItemExtendSize() const;
	void			ResetItemExtendSize();

	void			releaseSelection(void);

	void			SetVisibleItemCount( int const& nCount );
	void			SetVisibleControl( bool bVsible, int const& nIdx );
	void			SetVisibleControl( bool bVsible, cGridListBoxItem const * pControl );

	void			SetAutoSelection( bool bValue );

private:
	void			UpdateRects();
	float			getTotalItemHeight(void) const;

	bool			onMouseButtonDown(CsPoint ptParent);
	bool			onMouseButtonUp(CsPoint ptParent);
	bool			onMouseMove(CsPoint ptParent);
	bool			onMousePressed(CsPoint ptParent);

	bool			selectionChanged( cGridListBoxItem* pSelItem, bool bSendEvent = true );
	void			changeAllItemHeight(int const& nNewHeight);
	void			changeAllItemWidth(int const& nNewWidth);

	//20170215
	bool			onMouseRButtonDown(CsPoint ptParent);
	bool			onMouseRButtonUp(CsPoint ptParent);

	
	void			RenderFromType(int const& nRenderStartIdx);
	void			SetScrollBarRenderCount();
	bool			BtnOnEvent(CsPoint pt);
	int				GetVisibleItemCount();

private:
	bool					m_bEnable;
	cScrollBar*				m_pScrollBar;
	cSprite*				m_pBackGroundImg;
	cSprite*				m_pSelectedImg;
	cSprite*				m_pMoverImg;

	std::list<cGridListBoxItem*>m_listItems;

	cGridListBoxItem*			m_pSelectedItem;
	cGridListBoxItem*			m_pOverItem;
	cGridListBoxItem*			m_pDownItem;

	CsPoint					m_ptItemSize;						//아이템 크기(각 아이템의 크기를 일율적으로 한다)
	int						m_nItemExtendSize;					//그리드의 행이나 열이 바뀌게 되는 아이템 갯수
	int						m_nShowLine;						//그리드의 증가 방식에 따라서 보여주게 되는 라인의 수 m_pScrollBar->GetRenderCount() 와 동일
	int						m_nShowLineStartPos;					// 리스트 박스가 없을 경우 보여지는 라인의 위치값.
	CsPoint					m_ptItemGab;						//아이템간 갭
	bool					m_bBackOverAndSelectedImgRender;
	RenderType				m_enRenderType;						//행열 증가 방식
	StartPointType			m_enStartPointType;					//아이템 렌딩 시작점 종류
	CsPoint					m_ptMargin;							//첫 아이템과 지정된 스타트 포인트와의 간격
	bool					m_busePressedMoveEvent;		// 누른 채로 마우스 무브이벤트를 지정할 것인지
	bool					m_bAutoSelection;
};
