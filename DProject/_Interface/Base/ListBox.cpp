//---------------------------------------------------------------------------
//
// 파일명 : GUIList.cpp
// 작성일 : 2009.6.16
// 작성자 : Th
// 설  명 :
//
//---------------------------------------------------------------------------
#include "stdAfx.h"
#include "ListBox.h"

cListBoxItem::cListBoxItem( UINT item_ID, bool disabled ):
m_itemID(item_ID),
m_bDisabled(disabled),
m_bSelected(false),
m_pItem(NULL),
m_nItemHeight(0),
m_pUserData(NULL),
m_bVisible(true),
bScrollVisible(false)
{
}

cListBoxItem::cListBoxItem():
m_itemID(-1),
m_bDisabled(false),
m_bSelected(false),
m_pItem(NULL),
m_nItemHeight(0),
m_pUserData(NULL)
{
}

cListBoxItem::~cListBoxItem(void)
{
	Delete();
}

void cListBoxItem::ResetDevice()
{
	if(m_pItem)
		m_pItem->ResetDevice();
}

void cListBoxItem::SetAlpha( float fAlpha )
{
	if(m_pItem)
		m_pItem->SetAlpha(fAlpha);
}

void cListBoxItem::SetItem( cString* pSetItem )
{
	SAFE_POINTER_RET( pSetItem );	
	SAFE_NIDELETE(m_pItem);

	m_pItem = pSetItem;
	m_nItemHeight = m_pItem->GetMaxSize().y;
}

cString* cListBoxItem::GetItem()	const
{
	return	m_pItem;
}

void cListBoxItem::Delete()
{
	SAFE_NIDELETE(m_pItem);
	SAFE_DELETE( m_pUserData );
}
//----------------------------------------------------------------------------------------------
void cListBoxItem::SetVisible(bool bValue)
{
	m_bVisible = bValue;
}

bool cListBoxItem::GetVisible() const
{
	return m_bVisible;
}

void cListBoxItem::SetScrollVisible(bool bValue)
{
	bScrollVisible = bValue;
}	

bool cListBoxItem::GetScrollVisible() const
{
	return bScrollVisible;
}

void cListBoxItem::Render( CsPoint csPos )
{
	if( !m_bVisible )
		return;

	if( m_pItem )
		m_pItem->Render_Overlab( csPos );
}

void cListBoxItem::Update(float const& fDeltaTime)
{
	SAFE_POINTER_RET( m_pItem );
	m_pItem->Update( fDeltaTime );
}

UINT cListBoxItem::getID(void) const			
{ 
	return m_itemID; 
}

bool cListBoxItem::isSelected(void) const		
{ 
	return m_bSelected; 
}

bool cListBoxItem::isDisabled(void) const		
{ 
	return m_bDisabled; 
}

void cListBoxItem::setID(UINT nItemID)			
{ 
	m_itemID = nItemID; 
}

int cListBoxItem::getItemHeight() const		
{ 
	return m_nItemHeight; 
}

void cListBoxItem::setSelected(bool setting)	
{ 
	m_bSelected = setting; 

	if( m_pItem )
		m_pItem->SetControlOnMode( setting );
}

void cListBoxItem::SetUserData( CUSTOMDATA* pUserData )
{
	SAFE_DELETE( m_pUserData );
	m_pUserData = pUserData;
}

CUSTOMDATA* cListBoxItem::GetUserData() const
{
	return m_pUserData;
}
//////////////////////////////////////////////////////////////////////////
void cListBoxItem::OnMouseOver()
{
	if( m_pItem )
		m_pItem->OnMouseOver();
}

void cListBoxItem::OnMouseLeave()
{
	if( m_pItem )
		m_pItem->OnMouseLeave();
}

void cListBoxItem::OnMouseBtnDown()
{
	if( m_pItem )
		m_pItem->OnMouseDown();
}

void cListBoxItem::OnMouseBtnUp()
{
	if( m_pItem )
		m_pItem->OnMouseUp();
}

void cListBoxItem::onMousePressed()
{
	if( m_pItem )
		m_pItem->onMousePressed();
}
//////////////////////////////////////////////////////////////////////////
CsPoint cListBoxItem::GetWorldPos() const
{
	if( m_pItem )
		return m_pItem->GetWorldPos();
	return CsPoint::ZERO;
}

void cListBoxItem::SetItemHeight( int const& nItemHeigh )
{
	m_nItemHeight = nItemHeigh;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------
cListBox::cListBox(void)
:m_pScrollBar(NULL),m_pBackGroundImg(NULL)
,m_pSelectedItem(NULL),m_nItemHeight(0),m_pOverItem(NULL),
m_pDownItem(NULL),m_CsItemGab(CsPoint::ZERO),m_pSelectedImg(NULL), m_pMoverImg(NULL)
,m_bBackOverAndSelectedImgRender(true),m_bItemHideRender(false),m_bManaulSelectChange(false)
,m_bAutoItemSelectStateChange(false),m_bEnable(true)
{
	m_listItems.clear();
}

cListBox::~cListBox(void)
{
	Delete();
}

void cListBox::Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );
}

void cListBox::Init( cWindow* pParent, CsPoint pos, CsPoint size, char* cBGImagePath, bool bApplyWindowSize )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );
	SetBackGorundImg( cBGImagePath );
}

void cListBox::SetBackGorundImg( char* cImagePath, float fAlpha )
{
	SAFE_POINTER_RET( cImagePath );

	if( m_pBackGroundImg )
		SAFE_NIDELETE( m_pBackGroundImg );

	m_pBackGroundImg = NiNew cSprite;
	if( m_pBackGroundImg )
	{
		m_pBackGroundImg->Init( this, CsPoint( 0, 0), GetSize(), cImagePath, false );
		m_pBackGroundImg->SetAlpha( fAlpha );
	}
}

void cListBox::SetMouseOverImg( char* cImagePath, float fAlpha )
{
	SAFE_POINTER_RET( cImagePath );
	SAFE_NIDELETE( m_pMoverImg );

	m_pMoverImg = NiNew cSprite;
	if( m_pMoverImg )
	{
		m_pMoverImg->Init( NULL, CsPoint( 0, 0), CsPoint(GetSize().x, m_nItemHeight), cImagePath, false );
		m_pMoverImg->SetAlpha( fAlpha );
	}
}

void cListBox::SetMouseOverImg(cImage* pImage, bool bSizeFix)
{
	SAFE_NIDELETE( m_pMoverImg );
	m_pMoverImg = pImage;
	if( m_pMoverImg && !bSizeFix )
		m_pMoverImg->SetSize( CsPoint(GetSize().x, m_nItemHeight) );
}

void cListBox::SetSelectedImg( char* cImagePath, float fAlpha )
{
	SAFE_POINTER_RET( cImagePath );
	SAFE_NIDELETE( m_pSelectedImg );

	m_pSelectedImg = NiNew cSprite;
	if( m_pSelectedImg )
	{
		m_pSelectedImg->Init( NULL, CsPoint( 0, 0), CsPoint(GetSize().x, m_nItemHeight), cImagePath, false );
		m_pSelectedImg->SetAlpha( fAlpha );
	}
}

void cListBox::SetSelectedImg(cImage* pImage, bool bSizeFix)
{
	SAFE_NIDELETE( m_pSelectedImg );
	m_pSelectedImg = pImage;
	if( m_pSelectedImg && !bSizeFix )
		m_pSelectedImg->SetSize( CsPoint(GetSize().x, m_nItemHeight) );
}

void cListBox::UpdateRects()
{
// 	CsPoint pSize = GetSize() - m_CsMargin;
// 	pSize.y = m_nItemHeight;
// 	if( m_pScrollBar )
// 		pSize.x -= m_pScrollBar->GetSize().x;

	if( m_pScrollBar )
	{
		if( m_pSelectedImg )
		{
			CsPoint pSize = m_pSelectedImg->GetSize();
			m_pSelectedImg->SetSize( CsPoint( pSize.x - m_pScrollBar->GetSize().x, pSize.y ) );
		}

		if( m_pMoverImg )
		{
			CsPoint pSize = m_pMoverImg->GetSize();
			m_pMoverImg->SetSize( CsPoint( pSize.x - m_pScrollBar->GetSize().x, pSize.y ) );
		}
	}
}

void cListBox::Delete()
{
	m_pDownItem = NULL;
	m_pSelectedItem = NULL;
	m_pOverItem = NULL;

	SAFE_NIDELETE( m_pScrollBar );
	SAFE_NIDELETE( m_pBackGroundImg );
	SAFE_NIDELETE( m_pSelectedImg );
	SAFE_NIDELETE( m_pMoverImg );

	RemoveAllItem();
}

void cListBox::SetAlpha( float fAlpha )
{
	if( m_pBackGroundImg )	
		m_pBackGroundImg->SetAlpha( fAlpha );

	if( m_pScrollBar )
		m_pScrollBar->SetAlpha( fAlpha );

	std::list<cListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		if( (*it) )
			(*it)->SetAlpha( fAlpha );
	}
}

void cListBox::OnMouseLeave()
{
	if( m_pOverItem )
	{
		m_pOverItem->OnMouseLeave();
		m_pOverItem = NULL;
	}

	m_pDownItem = NULL;
}

void cListBox::Update(float const& fDeltaTime)
{
	std::list<cListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		(*it)->Update( fDeltaTime );
	}
}

int	cListBox::Update_ForMouse( CsPoint ptParent )
{
	if( !m_bVisible )
		return false;

	if( !PtInWindowRect( ptParent ) )
	{
		OnMouseLeave();
		return false;
	}

	if( !m_bEnable )
		return true;
	
	if( m_pScrollBar )
	{
		if( CURSOR_ST.GetWheel() != INVALIDE_WHEEL )
		{
			int nWheel = (int)( CURSOR_ST.GetWheel()*0.01f );
			m_pScrollBar->SetMouseWheelMove( nWheel );
			CURSOR_ST.ResetWheel();
		}
		if( m_pScrollBar->PtInWindowRect( ptParent ) )
		{
			m_pScrollBar->Update_ForMouse();
			OnMouseLeave();
			return true;
		}
	}

 	switch( CURSOR_ST.GetButtonState() )
 	{
 	case CURSOR::BUTTON_OUTWINDOW:
 		OnMouseLeave();
 		return false;
 	case CURSOR::LBUTTON_DOWN :
 		return onMouseButtonDown( ptParent );
 	case CURSOR::LBUTTON_UP :
 		return onMouseButtonUp( ptParent );
 	case CURSOR::BUTTON_NONE:
 		return onMouseMove( ptParent );
 	case CURSOR::LBUTTON_PRESS:
 		return onMousePressed(ptParent);
 	case CURSOR::RBUTTON_DOWN :
 		return onMouseRButtonDown( ptParent );
 	case CURSOR::RBUTTON_UP :
 		return onMouseRButtonUp( ptParent );
 	}
 
 	return true;
}

//----------------------------------------------------------------------------------------------------------
bool cListBox::onMouseButtonDown(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cListBoxItem* item = getItemAtPoint( pt );
	if( 0 == item )
		return false;

	m_pDownItem = item;

	if( item->GetVisible() )
	{
		item->OnMouseBtnDown();
		return true;
	}
	return false;
}

void cListBox::UnSelectionItem()
{
	SAFE_POINTER_RET( m_pSelectedItem );
	m_pSelectedItem->setSelected(false);
	m_pSelectedItem = NULL;
}


void cListBox::UnSelectionItem(int const& nIdx)
{
	cListBoxItem const* pItem = GetItemFormPos( nIdx );
	SAFE_POINTER_RET( pItem );
	if( pItem == m_pSelectedItem )
		UnSelectionItem();
}

void cListBox::UnSelectionItem(cListBoxItem const* pFindItem)
{
	if( m_pSelectedItem == pFindItem )
		UnSelectionItem();
}

void cListBox::SetVisibleItemAll( bool bValue )
{
	std::list<cListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		(*it)->SetVisible( bValue );
	}
	configureScrollbars();
}

void cListBox::SetVisibleItem( bool bValue, int const& nIdx, bool bUpdateScrollbar )
{
	std::list<cListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( n == nIdx )
		{
			(*it)->SetVisible( bValue );
			break;
		}
	}

	if( bUpdateScrollbar )
		configureScrollbars();
}

void cListBox::SetVisibleItems( bool bValue, std::list<int> const& nIdx )
{
	std::list<int>::const_iterator it = nIdx.begin();
	for( ; it != nIdx.end(); ++it )
		SetVisibleItem( bValue, (*it), false );

	configureScrollbars();
}

void cListBox::SetVisibleItems( std::map<int, bool> const& nIdx )
{
	std::map<int, bool>::const_iterator it = nIdx.begin();
	for( ; it != nIdx.end(); ++it )
		SetVisibleItem( it->second, it->first, false );

	configureScrollbars();
}

bool cListBox::selectionChanged( cListBoxItem* pSelItem, bool bSendEvent )
{
	if( NULL == pSelItem )
		return false;
	
	if( !m_bManaulSelectChange )
	{
		if( changeSelectItem( pSelItem ) )
		{
			if( bSendEvent )
				OnEvent( LIST_SELECT_CHANGE_ITEM, m_pSelectedItem );
		}
		else
		{
			if( bSendEvent )
				OnEvent( LIST_SELECTED_ITEM, m_pSelectedItem );
		}
	}
	else
	{
		if( m_pSelectedItem != pSelItem )
		{
			if( bSendEvent )
				OnEvent( LIST_SELECT_CHANGE_ITEM, pSelItem );
		}
		else
		{
			if( bSendEvent )
				OnEvent( LIST_SELECTED_ITEM, m_pSelectedItem );
		}
	}


	return true;
}

bool cListBox::changeSelectItem( cListBoxItem* pSelItem )
{
	if( m_pSelectedItem == pSelItem )
		return false;

	if( m_bAutoItemSelectStateChange )
	{
		if( m_pSelectedItem )
			m_pSelectedItem->setSelected(false);

		if( pSelItem )
			pSelItem->setSelected(true);
	}

	m_pSelectedItem = pSelItem;
	return true;
}

//----------------------------------------------------------------------------------------------------------
bool cListBox::onMouseButtonUp(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cListBoxItem* item = getItemAtPoint( pt );
	if( NULL == item )
	{
		m_pDownItem = 0;
		return false;
	}

	if( m_pDownItem != item )
	{
		m_pDownItem = 0;
		return true;
	}

	m_pDownItem = 0;

	if( item->GetVisible() )
		item->OnMouseBtnUp();
	
	selectionChanged( item );	
	return true;
}

bool cListBox::onMousePressed(CsPoint ptParent)
{
	if( m_pDownItem && m_pDownItem->GetVisible() )
		m_pDownItem->onMousePressed();

	return true;
}

bool cListBox::onMouseMove(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cListBoxItem* pItem = getItemAtPoint( pt );
	if( pItem )
	{
		m_pOverItem = pItem;

		if( m_pOverItem->GetVisible() )
		{
			m_pOverItem->OnMouseOver();
			OnEvent( LIST_ITEM_MOUSE_OVER, m_pOverItem );
			return true;
		}
	}

	if( m_pOverItem )
		m_pOverItem = 0;

	return false;
}

void cListBox::SetMargin(CsPoint margin)
{
	m_CsMargin = margin;
}

void cListBox::SetItemGab(CsPoint gab)
{
	m_CsItemGab = gab;
}

//----------------------------------------------------------------------------------------------------------
cListBoxItem* cListBox::getItemAtPoint(CsPoint const& pt) const
{	
	CsRect renderArea( CsPoint(0,0),GetSize());
	if( renderArea.PtInRect( pt ) )
	{
		float x = static_cast< float >( renderArea.left ) + m_CsMargin.x;
		float y = static_cast< float >( renderArea.top ) + m_CsMargin.y;
		if( pt.x >= x && pt.y >= y )
		{
			int nStartIdx = 0;
			if( m_pScrollBar )
				nStartIdx = m_pScrollBar->GetCurPosIndex();
			return getItemFromListAtPoint(&y, pt, nStartIdx);
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------------------------------------
cListBoxItem* cListBox::getItemFromListAtPoint(float* bottomY, CsPoint const& pt, int const& nStartIdx) const
{
	std::list<cListBoxItem*>::const_iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		SAFE_POINTER_BEK( (*it) );

		if( n < nStartIdx )
			continue;

		*bottomY += (*it)->getItemHeight();

		if( pt.y < *bottomY )
			return (*it);

		*bottomY += m_CsItemGab.y;		
	}

	return NULL;
}


bool cListBox::AddItem( cListBoxItem* pAddItem )
{
	SAFE_POINTER_RETVAL( pAddItem, false );
	int nIdx = GetItemIndex( pAddItem );
	if( -1 != nIdx )
		return false;

	if( m_nItemHeight < pAddItem->getItemHeight() )
	{
		m_nItemHeight = pAddItem->getItemHeight();
		changeAllItemHeight( m_nItemHeight );
	}
	else
		pAddItem->SetItemHeight( m_nItemHeight );

	pAddItem->SetItemHeight( pAddItem->getItemHeight() );

	m_listItems.push_back( pAddItem );
	configureScrollbars();
	return true;
}

bool cListBox::AddItemMultiHeight( cListBoxItem* pAddItem, int minusHeight )
{
	SAFE_POINTER_RETVAL( pAddItem, false );
	int nIdx = GetItemIndex( pAddItem );
	if( -1 != nIdx )
		return false;

	pAddItem->SetItemHeight( pAddItem->getItemHeight() - minusHeight );

	m_listItems.push_back( pAddItem );
	configureScrollbars();
	return true;
}


void cListBox::changeAllItemHeight(int const& nNewHeight)
{
	std::list<cListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
		(*it)->SetItemHeight( nNewHeight );
}

bool cListBox::InsertItem( int nPos, cListBoxItem* pAddItem )
{
	SAFE_POINTER_RETVAL( pAddItem, false );

	if( m_nItemHeight < pAddItem->getItemHeight() )
	{
		m_nItemHeight = pAddItem->getItemHeight();
		changeAllItemHeight( m_nItemHeight );
	}
	else
		pAddItem->SetItemHeight( m_nItemHeight );

	bool bUpdateScrollbar = false;

	if( m_listItems.size() <= nPos )
	{
		m_listItems.push_back( pAddItem );
		bUpdateScrollbar = true;
	}
	else
	{
		std::list<cListBoxItem*>::iterator it = m_listItems.begin();
		for( int n = 0; it != m_listItems.end(); ++it, ++n )
		{
			if( n != nPos )
				continue;

			m_listItems.insert( it, pAddItem );
			bUpdateScrollbar = true;
			break;
		}
	}

	if( bUpdateScrollbar )
	{
		configureScrollbars();
		return true;
	}

	return false;
}

void cListBox::RemoveItem( cListBoxItem* pAddItem )
{
	bool bUpdateScrollbar = false;
	std::list<cListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		if( (*it) != pAddItem )
			continue;

		if( m_pSelectedItem == (*it) )
			m_pSelectedItem = NULL;

		if( m_pOverItem == (*it) )
			m_pOverItem = NULL;

		SAFE_NIDELETE( (*it) );
		m_listItems.erase(it);
		bUpdateScrollbar = true;
		pAddItem = NULL;
		break;
	}

	if( bUpdateScrollbar )
		configureScrollbars();
}

void cListBox::RemoveItem( int nPos )
{
	bool bUpdateScrollbar = false;
	std::list<cListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( n != nPos )
			continue;

		if( m_pSelectedItem == (*it) )
			m_pSelectedItem = NULL;

		if( m_pOverItem == (*it) )
			m_pOverItem = NULL;

		SAFE_NIDELETE( (*it) );
		m_listItems.erase(it);
		bUpdateScrollbar = true;
		break;
	}

	if( bUpdateScrollbar )
		configureScrollbars();
}

void cListBox::RemoveAllItem()
{
	m_pDownItem = NULL;
	m_pSelectedItem = NULL;
	m_pOverItem = NULL;
	std::list<cListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		cListBoxItem* pItem = *it;
		SAFE_NIDELETE( pItem );
	}
	m_listItems.clear();

	configureScrollbars();
}

int cListBox::GetItemIndex(cListBoxItem const* pFindItem) const
{
	std::list<cListBoxItem*>::const_iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( (*it) == pFindItem )
			return n;
	}

	return -1;
}

void cListBox::SetScrollBar( cScrollBar* pScrollBar )
{
	if( m_pScrollBar )
		SAFE_NIDELETE( m_pScrollBar );
	m_pScrollBar = pScrollBar;
	SAFE_POINTER_RET( m_pScrollBar );

	m_pScrollBar->SetParentWindow(this);

	CsPoint csSize = GetSize();
	CsPoint csScSize = m_pScrollBar->GetSize();

	CsPoint csPos;
	csPos.x = csSize.x - csScSize.x;
	csPos.y = 0;	
	csScSize.y = csSize.y;
	m_pScrollBar->ReSize( csScSize, CsRect( CsPoint::ZERO, csScSize ) );
	m_pScrollBar->SetPos( csPos );

	if( m_nItemHeight > 0 )
	{
		int nShowCount = csScSize.y / (m_nItemHeight + m_CsItemGab.y);
		m_pScrollBar->SetRenderCount( nShowCount );
	}

	m_pScrollBar->ResetDevice();
	UpdateRects();
	configureScrollbars();
}

cListBoxItem const*	cListBox::GetItemFormPos( int nPos ) const
{
	std::list<cListBoxItem*>::const_iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( n == nPos )
			return (*it);
	}
	return NULL;
}

size_t cListBox::GetItemCount() const
{
	return m_listItems.size();
}

cListBoxItem const* cListBox::GetSelectedItem() const
{
	return m_pSelectedItem;
}

int cListBox::GetSelectedItemIdx() const
{
	SAFE_POINTER_RETVAL( m_pSelectedItem, -1 );

	return GetItemIndex( m_pSelectedItem );
}

// void cListBox::SetSelectMode(int MainID, int SubID)
// {
// 	std::list<cListBoxItem*>::const_iterator	MainIt = m_listItems.begin();
// 	
// 	for(int	MainCnt=0; MainIt != m_listItems.end(); ++MainIt, ++MainCnt )
// 	{
// 		cListBoxItem*	pMainList =	*MainIt;
// 		SAFE_POINTER_CON( pMainList );
// 
// 		if(MainID == MainCnt)
// 			pMainList->setSelected(true);
// 		else
// 			pMainList->setSelected(false);
// 	}
// }

void cListBox::SetItemHeight( int nItemHeight )
{
	m_nItemHeight = nItemHeight;
}

void cListBox::Render()
{
	if( !m_bVisible )
		return;

	if( m_pBackGroundImg )
		m_pBackGroundImg->Render();

	if( !m_listItems.empty() )
	{
		int nStartIdx = 0;
		if( m_pScrollBar )
			nStartIdx = m_pScrollBar->GetCurPosIndex();

		CsPoint pos = GetClient();
		CsPoint csSize = GetSize();
		CsPoint csTotalSize = pos + csSize;

		std::list<cListBoxItem*>::iterator it = m_listItems.begin();
		for( int n = 0; it != m_listItems.end(); ++it, ++n )
		{
			SAFE_POINTER_BEK( (*it) );

			if( n < nStartIdx )
				continue;

			if( !m_bItemHideRender )// 숨긴 아이템을 그리지 않는 경우
			{
				if( !(*it)->GetVisible() )
					continue;
			}	

			(*it)->SetScrollVisible(false);

			if( pos.y + (*it)->getItemHeight() > csTotalSize.y)
				break;

			if( m_bBackOverAndSelectedImgRender )
			{
				if( m_pSelectedItem && m_pSelectedItem == (*it) )
				{
					if( m_pSelectedImg )
						m_pSelectedImg->Render( pos );
				}else if(  m_pOverItem && m_pOverItem == (*it) )
				{
					if( m_pMoverImg )
						m_pMoverImg->Render( pos );
				}
			}

			(*it)->SetScrollVisible(true);

			(*it)->Render(pos + m_CsMargin);

			if( !m_bBackOverAndSelectedImgRender )
			{
				if( m_pSelectedItem && m_pSelectedItem == (*it) )
				{
					if( m_pSelectedImg )
						m_pSelectedImg->Render( pos );
				}else if(  m_pOverItem && m_pOverItem == (*it) )
				{
					if( m_pMoverImg )
						m_pMoverImg->Render( pos );
				}
			}

			pos.y += (*it)->getItemHeight() + m_CsItemGab.y;
		}
	}

	if( m_pScrollBar )
		m_pScrollBar->Render();
}

void cListBox::ResetDevice()
{
	cWindow::ResetDevice();

	if( m_pScrollBar )
		m_pScrollBar->ResetDevice();

	if( m_pBackGroundImg )
		m_pBackGroundImg->ResetDevice();

	if( m_pSelectedImg )
		m_pSelectedImg->ResetDevice();

	if( m_pMoverImg )
		m_pMoverImg->ResetDevice();

	std::list<cListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		if( (*it) )
			(*it)->ResetDevice();
	}
}

//----------------------------------------------------------------------------------------------------------
void cListBox::configureScrollbars()
{
	SAFE_POINTER_RET( m_pScrollBar );

	float totalHeight = getTotalItemHeight();
	bool bEnableScrollBar = (totalHeight > m_ptSize.y );
	if( !bEnableScrollBar )
	{
		m_pScrollBar->SetRange( CsPoint::ZERO );	
		return;
	}

	UINT nShowItemSize = m_listItems.size();	
	m_pScrollBar->SetRange( CsPoint(0, nShowItemSize) );		
}

//----------------------------------------------------------------------------------------------------------
float cListBox::getTotalItemHeight(void) const
{
	float heightSum = 0;
	int nVisibleItemCount = 0;
	std::list<cListBoxItem*>::const_iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{	
		if( !m_bItemHideRender && !(*it)->GetVisible() )
			continue;

		heightSum += (*it)->getItemHeight();
		nVisibleItemCount++;
	}

	if( nVisibleItemCount > 1 )
		heightSum += (nVisibleItemCount - 1) * m_CsItemGab.y;

	return heightSum;
}

cListBoxItem const*	cListBox::GetMouseOverItem() const
{
	return m_pOverItem;
}

cListBoxItem const*	cListBox::GetMouseDownItem() const
{
	return m_pDownItem;
}

void cListBox::ChangeSelectFront()
{
	if( m_listItems.empty() )
		return;

	int nCurrentSelIdx = GetSelectedItemIdx();
	int nForntSelIdx = 0;
	if( NULL == m_pSelectedItem )
		nForntSelIdx = GetItemCount() - 1;
	else
	{
		nForntSelIdx = nCurrentSelIdx;
		nForntSelIdx--;
	}
	
	if( nForntSelIdx < 0 )
		return;
		//nForntSelIdx = GetItemCount() - 1;

	if( nForntSelIdx == nCurrentSelIdx )
		return;

	SetSelectedItemFromIdx( nForntSelIdx );
	ShowSelectItemScrollBar();
}

void cListBox::ChangeSelectNext()
{
	if( m_listItems.empty() )
		return;

	int nCurrentSelIdx = GetSelectedItemIdx();
	int nNextSelIdx = 0;
	if( NULL == m_pSelectedItem )
		nNextSelIdx = 0;
	else
	{
		nNextSelIdx = nCurrentSelIdx;
		nNextSelIdx++;
	}

	if( nNextSelIdx >= GetItemCount() )
		return;
		//nNextSelIdx = 0;

	if( nNextSelIdx == nCurrentSelIdx )
		return;

	SetSelectedItemFromIdx( nNextSelIdx );
	ShowSelectItemScrollBar();
}

void cListBox::ShowSelectItemScrollBar()
{
	SAFE_POINTER_RET( m_pScrollBar );
	int nMaxRenderCount = m_pScrollBar->GetRenderCount();
	if( nMaxRenderCount < 1 )
		return;

	nMaxRenderCount -= 1;	// 인덱스가 0번부터 시작하기 때문에 -1을 함.

	int nCurrentSelIdx = GetSelectedItemIdx();
	int nStartIdx = m_pScrollBar->GetCurPosIndex();
	int nShowLineMaxIdx = nStartIdx + nMaxRenderCount;

	if( nStartIdx > nCurrentSelIdx )
		m_pScrollBar->SetCurPosIndex( nCurrentSelIdx );
	else if( nShowLineMaxIdx < nCurrentSelIdx )
		m_pScrollBar->SetCurPosIndex( nCurrentSelIdx - nMaxRenderCount );
}

void cListBox::SetSelectedItemFromIdx( int const& nSelIdx, bool bSendEvent )
{
	cListBoxItem * pNewSelItem = const_cast<cListBoxItem*>(GetItemFormPos( nSelIdx ));
	SAFE_POINTER_RET( pNewSelItem );
	if( changeSelectItem( pNewSelItem ) )
	{
		if( bSendEvent )
			OnEvent( LIST_SELECT_CHANGE_ITEM, m_pSelectedItem );
	}
	else
	{
		if( bSendEvent )
			OnEvent( LIST_SELECTED_ITEM, m_pSelectedItem );
	}
}

void cListBox::SetSelectedItemFromItemPtr( cListBoxItem * pFindItem, bool bSendEvent )
{
	int nItemIdx = GetItemIndex( pFindItem );
	if( -1 == nItemIdx )
		return;

	if( changeSelectItem( pFindItem ) )
	{
		if( bSendEvent )
			OnEvent( LIST_SELECT_CHANGE_ITEM, m_pSelectedItem );
	}
	else
	{
		if( bSendEvent )
			OnEvent( LIST_SELECTED_ITEM, m_pSelectedItem );
	}
}

void cListBox::SetBackOverAndSelectedImgRender(bool bValue)
{				
	m_bBackOverAndSelectedImgRender = bValue;
}

CsPoint cListBox::GetMargin() const
{
	return m_CsMargin;
}

void cListBox::SetScrollBarFirstPos()
{
	SAFE_POINTER_RET( m_pScrollBar );

	m_pScrollBar->SetCurPosIndex( 0 );
}

void cListBox::SetScrollBarEndPos()
{
	SAFE_POINTER_RET( m_pScrollBar );

	m_pScrollBar->SetCurPosIndex( m_pScrollBar->GetMaxPosIndex() );
}

bool cListBox::onMouseRButtonDown(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cListBoxItem* item = getItemAtPoint( pt );
	if( 0 == item )
		return false;

	if( !item->GetVisible() )
		return false;
	
	m_pDownItem = item;
	OnEvent( LIST_SELECTED_ITEM_RIGHT_DOWN, m_pDownItem );
	return true;

}
bool cListBox::onMouseRButtonUp(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cListBoxItem* item = getItemAtPoint( pt );
	OnEvent( LIST_ITEM_RIGHT_UP, item );
	if ( item != null && item == m_pDownItem)
	{
		OnEvent( LIST_SELECTED_ITEM_RIGHT_UP, m_pSelectedItem );
		m_pDownItem = NULL;
		return true;
	}
	m_pDownItem = NULL;
	return false;
}

void cListBox::SetHideItemRender( bool bValue )
{
	m_bItemHideRender = bValue;
}


void cListBox::SetManaulSelectChange( bool bValue )
{
	m_bManaulSelectChange = bValue;
}

bool cListBox::IsManaulSelectChange() const
{
	return m_bManaulSelectChange;
}

CsPoint cListBox::GetItemtoMousePos( CsPoint localPos, cListBoxItem * pFindItem )
{
	CsPoint pt = MousePosToWindowPos( localPos );
	CsPoint pickItemPos;
	float pickY = getItemAtPoint( pt, pFindItem );
	return CsPoint( pt.x, pickY );
}

float cListBox::getItemAtPoint(CsPoint const& pt, cListBoxItem const * pFindItem )
{	
	CsRect renderArea( CsPoint(0,0),GetSize());
	if( renderArea.PtInRect( pt ) )
	{
		float x = static_cast< float >( renderArea.left ) + m_CsMargin.x;
		float y = static_cast< float >( renderArea.top ) + m_CsMargin.y;
		if( pt.x >= x && pt.y >= y )
		{
			int nStartIdx = 0;
			if( m_pScrollBar )
				nStartIdx = m_pScrollBar->GetCurPosIndex();

			CsPoint csLocalPt = pt;
			cListBoxItem * pItem = getItemFromListAtPoint(csLocalPt, nStartIdx);
			if( pItem == pFindItem )
				return csLocalPt.y;
		}
	}

	return 0.0f;
}
//----------------------------------------------------------------------------------------------------------
cListBoxItem* cListBox::getItemFromListAtPoint(CsPoint & localpt, int const& nStartIdx)
{
	std::list<cListBoxItem*>::const_iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		SAFE_POINTER_BEK( (*it) );

		if( n < nStartIdx )
			continue;

		if( localpt.y < (*it)->getItemHeight() )
			return (*it);

		localpt.y -= ((*it)->getItemHeight() + m_CsItemGab.y);
	}

	return NULL;
}

void cListBox::SetScrollBarRenderCount(int cnt)
{
	SAFE_POINTER_RET( m_pScrollBar );
	m_pScrollBar->SetRenderCount(cnt);
}

void cListBox::SetAutoItemSelectStateChange(bool bValue)
{
	m_bAutoItemSelectStateChange = bValue;
}

void cListBox::SetEnable( bool bValue )
{
	m_bEnable = bValue;
}