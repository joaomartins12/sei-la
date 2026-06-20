#include "stdafx.h"
#include "TreeBox.h"

#define	ONE_TREE_SIZE 22

cTreeBoxItem::cTreeBoxItem( UINT item_ID, bool disabled ):
m_itemID(item_ID),
m_bDisabled(disabled),
m_bIsOpen(false),
m_bSelected(false),
m_pItem(NULL),
m_nItemHeight(0),
m_pUserDate(NULL),
m_pOpenBtn(NULL),
m_pCloseBtn(NULL)
{
	m_pOpenDataPos = CsPoint::ZERO;
	m_pCloseDataPos = CsPoint::ZERO;
	m_listItems.clear();
}

cTreeBoxItem::cTreeBoxItem():
m_itemID(-1),
m_bDisabled(false),
m_bIsOpen(false),
m_bSelected(false),
m_pItem(NULL),
m_nItemHeight(0),
m_pUserDate(NULL),
m_pOpenBtn(NULL),
m_pCloseBtn(NULL)
{
	m_pOpenDataPos = CsPoint::ZERO;
	m_pCloseDataPos = CsPoint::ZERO;
	m_listItems.clear();
}

cTreeBoxItem::~cTreeBoxItem(void)
{
	Delete();
}

void cTreeBoxItem::SetUserData(CUSTOMDATA* pData)
{
	if( m_pUserDate )
		delete m_pUserDate;
	m_pUserDate = pData;
}

CUSTOMDATA* cTreeBoxItem::GetUserData() const
{
	return m_pUserDate;
}

void cTreeBoxItem::ResetDevice()
{
	if(m_pItem)
		m_pItem->ResetDevice();

	if( m_pCloseBtn )
		m_pCloseBtn->ResetDevice();

	if( m_pOpenBtn )
		m_pOpenBtn->ResetDevice();

	LBItemList::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
		(*it)->ResetDevice();
}

void cTreeBoxItem::SetAlpha( float fAlpha )
{
	if(m_pItem)
		m_pItem->SetAlpha(fAlpha);

	LBItemList::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
		(*it)->SetAlpha(fAlpha);
}

void cTreeBoxItem::SetItem( cString* pSetItem )
{
	SAFE_POINTER_RET( pSetItem );	
	SAFE_NIDELETE(m_pItem);

	m_pItem = pSetItem;
	m_nItemHeight = m_pItem->GetMaxSize().y;
}

cString* cTreeBoxItem::GetItem()
{
	return	m_pItem;
}

void cTreeBoxItem::AddChildItem( cTreeBoxItem* pAddItem )
{
	m_listItems.push_back(pAddItem);
}

void cTreeBoxItem::Delete()
{
	SAFE_NIDELETE(m_pItem);
	SAFE_NIDELETE(m_pOpenBtn);
	SAFE_NIDELETE(m_pCloseBtn);
	SAFE_DELETE( m_pUserDate );

	LBItemList::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
		(*it)->Delete();
}
//----------------------------------------------------------------------------------------------
void cTreeBoxItem::Render( CsPoint csPos )
{
	if( m_pItem )
		m_pItem->Render_Overlab( csPos );

	if( m_bIsOpen )
	{
		if( m_pCloseBtn )
		{
			m_pCloseBtn->SetPos( csPos + m_pCloseDataPos );
			m_pCloseBtn->Render();
		}
	}
	else
	{
		if( m_pOpenBtn )
		{
			m_pOpenBtn->SetPos( csPos + m_pOpenDataPos );
			m_pOpenBtn->Render();
		}
	}
}

UINT cTreeBoxItem::getID(void) const			
{ 
	return m_itemID; 
}

bool cTreeBoxItem::isSelected(void) const		
{ 
	return m_bSelected; 
}

bool cTreeBoxItem::isDisabled(void) const		
{ 
	return m_bDisabled; 
}

void cTreeBoxItem::setID(UINT nItemID)			
{ 
	m_itemID = nItemID; 
}

bool cTreeBoxItem::getIsOpen(void)				
{ 
	return m_bIsOpen; 
}
void cTreeBoxItem::setIsOpen(bool bOpen)		
{ 
	if( getItemCount() == 0 )
		return;

	m_bIsOpen = bOpen;
}

void cTreeBoxItem::toggleIsOpen(void)			
{ 
	setIsOpen( !m_bIsOpen );
}

size_t cTreeBoxItem::getItemCount(void) const	
{ 
	return m_listItems.size(); 
}

cTreeBoxItem::LBItemList& cTreeBoxItem::getItemList(void)			
{ 
	return m_listItems; 
}
int cTreeBoxItem::getItemHeight() const		
{ 
	return m_nItemHeight; 
}

void cTreeBoxItem::setSelected(bool setting)	
{ 
	m_bSelected = setting; 
	if( m_pItem )
		m_pItem->SetControlOnMode(m_bSelected);
}

void cTreeBoxItem::SetOpenButton(cButton* pOpenBtn, CsPoint csDeltaPos)
{
	SAFE_NIDELETE( m_pOpenBtn );
	m_pOpenBtn = pOpenBtn;
	m_pOpenDataPos = csDeltaPos;
}

void cTreeBoxItem::SetCloseButton(cButton* pCLoseBtn, CsPoint csDeltaPos)
{
	SAFE_NIDELETE( m_pCloseBtn );
	m_pCloseBtn = pCLoseBtn;
	m_pCloseDataPos = csDeltaPos;
}

void cTreeBoxItem::OnMouseOver()
{
	if( m_pItem )
		m_pItem->OnMouseOver();

	if( m_bIsOpen )
	{
		if( m_pCloseBtn )
			m_pCloseBtn->OnMouseOver();
	}
	else
	{
		if( m_pOpenBtn )
			m_pOpenBtn->OnMouseOver();
	}
}

void cTreeBoxItem::OnMouseLeave()
{
	if( m_pItem )
		m_pItem->OnMouseLeave();

	if( m_bIsOpen )
	{
		if( m_pCloseBtn )
			m_pCloseBtn->OnMouseLeave();
	}
	else
	{
		if( m_pOpenBtn )
			m_pOpenBtn->OnMouseLeave();
	}
}

void cTreeBoxItem::OnMouseBtnDown()
{
	if( m_pItem )
		m_pItem->OnMouseDown();

	if( m_bIsOpen )
	{
		if( m_pCloseBtn )
			m_pCloseBtn->OnMouseDown();
	}
	else
	{
		if( m_pOpenBtn )
			m_pOpenBtn->OnMouseDown();
	}
}

void cTreeBoxItem::OnMouseBtnUp()
{
	if( m_pItem )
		m_pItem->OnMouseUp();

	if( m_bIsOpen )
	{
		if( m_pCloseBtn )
			m_pCloseBtn->OnMouseUp();
	}
	else
	{
		if( m_pOpenBtn )
			m_pOpenBtn->OnMouseUp();
	}
}

void cTreeBoxItem::onMousePressed()
{
	if( m_pItem )
		m_pItem->onMousePressed();
}

void cTreeBoxItem::SetControlOnMode(bool bValue)
{
	if( m_pItem )
		m_pItem->SetControlOnMode( bValue );
}

void cTreeBoxItem::DeleteAllChildItem()
{
	LBItemList::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
		SAFE_NIDELETE( *it );

	m_listItems.clear();
}

std::wstring cTreeBoxItem::GetItemText()
{
	cString::sELEMENT* pElement = m_pItem->FindElement( cString::sELEMENT::TEXT );
	SAFE_POINTER_RETVAL( pElement, std::wstring() );
	cString::sTEXT* pText = dynamic_cast< cString::sTEXT* >( pElement );
	SAFE_POINTER_RETVAL( pText, std::wstring() );

	return std::wstring( pText->s_Text.GetText() );
}

void cTreeBoxItem::GetItemList( std::list<cTreeBoxItem*>& BoxItemList )
{
	SAFE_POINTER_RET( m_pItem );
	LBItemList::const_iterator it =	m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		BoxItemList.push_back( *it );
		if( (*it)->getItemCount() > 0 )
		{
			(*it)->GetItemList( BoxItemList );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

cTreeBox::cTreeBox() : 
m_pScrollBar(NULL),
m_pBackGroundImg(NULL),
m_bAutoDeleted(true),
m_CsChildItemGab(CsPoint::ZERO),
m_CsItemGab(CsPoint::ZERO),
m_nItemHeight(ONE_TREE_SIZE),
m_MouseOverItem(NULL),
m_SelectedItem(NULL),
m_CsMargin(CsPoint::ZERO),
m_MouseDownItem(NULL),
m_bOnlyOpenMode(false),
m_bEnalbe(true)
{
	m_listItems.clear();
}

cTreeBox::~cTreeBox()
{
	Delete();
}

void cTreeBox::Delete()
{
	m_MouseDownItem = 0;
	SAFE_NIDELETE( m_pScrollBar );
	SAFE_NIDELETE( m_pBackGroundImg );
	RemoveAllItem();
}

void cTreeBox::Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );
}

void cTreeBox::Init( cWindow* pParent, CsPoint pos, CsPoint size, char* cBGImagePath, bool bApplyWindowSize )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );
	SetBackGorundImg( cBGImagePath );
}

void cTreeBox::SetScrollBar( cScrollBar* pScrollbar )
{
	SAFE_NIDELETE( m_pScrollBar );
	m_pScrollBar = pScrollbar;
	
	SAFE_POINTER_RET( m_pScrollBar );

	m_pScrollBar->SetParentWindow( this );

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
		int nShowCount = csScSize.y / m_nItemHeight;
		m_pScrollBar->SetRenderCount( nShowCount );
	}

	m_pScrollBar->ResetDevice();
	UpdateRects();
	configureScrollbars();
}

void cTreeBox::SetBackGorundImg( char* cImagePath )
{
	SAFE_POINTER_RET( cImagePath );

	if( m_pBackGroundImg )
		SAFE_NIDELETE( m_pBackGroundImg );

	m_pBackGroundImg = NiNew cSprite;
	if( m_pBackGroundImg )
		m_pBackGroundImg->Init( this, CsPoint( 0, 0), GetSize(), cImagePath, false );
}

void cTreeBox::SetAlpha( float fAlpha )
{
	if( m_pBackGroundImg )	
		m_pBackGroundImg->SetAlpha( fAlpha );

	if( m_pScrollBar )
		m_pScrollBar->SetAlpha( fAlpha );

	LBItemList::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		if( (*it) )
			(*it)->SetAlpha( fAlpha );
	}
}

void cTreeBox::RemoveAllItem()
{
	m_MouseOverItem = NULL;
	m_MouseDownItem = NULL;
	m_SelectedItem = NULL;

	LBItemList::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
		SAFE_NIDELETE( (*it) );

	m_listItems.clear();
	configureScrollbars();
}

void cTreeBox::RemoveItem( const int nIdx )
{
	LBItemList::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( n == nIdx )
		{
			if ( (*it) == m_MouseOverItem )
				m_MouseOverItem = NULL;
			if ( (*it) == m_MouseDownItem )
				m_MouseDownItem = NULL;
			if ( (*it) == m_SelectedItem )
				m_SelectedItem = NULL;

			SAFE_NIDELETE( *it );
			m_listItems.erase( it );
			break;
		}
	}
	configureScrollbars();
}

void cTreeBox::OnMouseLeave()
{
	if( m_MouseOverItem )
	{
		m_MouseOverItem->OnMouseLeave();
		m_MouseOverItem = 0;
	}
	if( m_MouseDownItem )
		m_MouseDownItem = 0;

	OnEvent( TreeEvent_MouseOverItem_Release, NULL );
}

int cTreeBox::Update_ForMouse( CsPoint ptParent )
{
	if( !PtInWindowRect( ptParent ) )
	{
		OnMouseLeave();
		return false;
	}

	if( !m_bEnalbe )
		return true;

	if( m_pScrollBar )
	{
		if( CURSOR_ST.GetWheel() != INVALIDE_WHEEL )
		{
			int nWheel = (int)( CURSOR_ST.GetWheel()*0.01f );
			m_pScrollBar->SetMouseWheelMove( nWheel );
		}

		if( m_pScrollBar->PtInWindowRect( ptParent ) )
		{
			m_pScrollBar->Update_ForMouse();
			OnMouseLeave();
			return true;
		}
	}

	CsPoint pt = MousePosToWindowPos( ptParent );
	switch( CURSOR_ST.GetButtonState() )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		return false;
	case CURSOR::LBUTTON_DOWN :
		return onMouseButtonDown( pt );
	case CURSOR::LBUTTON_UP :
		return onMouseButtonUp( pt );
	case CURSOR::BUTTON_NONE:
		return onMouseMove( pt );
	case CURSOR::LBUTTON_PRESS :
		return onMousePressed( pt );
	}

	return true;
}

bool cTreeBox::selectionChanged( cTreeBoxItem* pSelItem )
{
	if( NULL == pSelItem )
		return false;
	if(m_bOnlyOpenMode == true && GetItemIdx(pSelItem) != -1)
		closeAllItem();

 	pSelItem->toggleIsOpen();

	OnEvent( TreeEvent_ItemToggle_Changed, pSelItem );

	configureScrollbars();

	if( pSelItem->getItemCount() != 0 )
		return true;

	if( m_SelectedItem != pSelItem )
	{
		if( m_SelectedItem )
			m_SelectedItem->setSelected(false);

		pSelItem->setSelected(true);
		m_SelectedItem = pSelItem;
		OnEvent( TreeEvent_Selection_Changed, m_SelectedItem );
	}
	return true;
}

bool cTreeBox::onMousePressed(CsPoint ptParent)
{
	if( m_MouseDownItem )
		m_MouseDownItem->onMousePressed();

	return true;
}

//----------------------------------------------------------------------------------------------------------
bool cTreeBox::onMouseButtonDown(CsPoint ptParent)
{
	cTreeBoxItem* item = getItemAtPoint( ptParent );
	if( 0 == item )
	{
		m_MouseDownItem = 0;
		return true;
	}

	m_MouseDownItem = item;

	item->OnMouseBtnDown();
	return true;
}

//----------------------------------------------------------------------------------------------------------
bool cTreeBox::onMouseButtonUp(CsPoint ptParent)
{
	cTreeBoxItem* item = getItemAtPoint( ptParent );
	if( 0 == item )
	{
		m_MouseDownItem = 0;
		return true;
	}

	if( m_MouseDownItem != item )
	{
		m_MouseDownItem = 0;
		return true;
	}

	item->OnMouseBtnUp();

	BtnOnEvent(ptParent);
	selectionChanged( item );
	return true;
}

bool cTreeBox::onMouseMove(CsPoint ptParent)
{
	cTreeBoxItem* pItem = getItemAtPoint( ptParent );
	if( pItem )
	{
		pItem->OnMouseOver();
		if( m_MouseOverItem != pItem )
		{
			if( m_MouseOverItem )
				m_MouseOverItem->OnMouseLeave();
			OnEvent( TreeEvent_MouseOverItem_Change, pItem );
			m_MouseOverItem = pItem;
		}
		return true;
	}

	if( m_MouseOverItem )
	{
		if( m_MouseOverItem )
			m_MouseOverItem->OnMouseLeave();
		m_MouseOverItem = 0;
		OnEvent( TreeEvent_MouseOverItem_Release, NULL );
	}
	return true;
}

bool cTreeBox::AddItem( cTreeBoxItem* pAddItem )
{
	SAFE_POINTER_RETVAL( pAddItem, false );

	if( GetItemIdx( pAddItem ) != -1 )
		return false;

	m_listItems.push_back( pAddItem );
	configureScrollbars();
	return true;
}

int cTreeBox::GetItemIdx( cTreeBoxItem const * pAddItem ) const
{
	LBItemList::const_iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( (*it) == pAddItem )
			return n;
	}
	return -1;
}

cTreeBoxItem const * cTreeBox::GetItem( int const& nIdx ) const
{
	LBItemList::const_iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( n == nIdx )
			return (*it);
	}
	return NULL;
}

void cTreeBox::SetSelectMode(int MainID, int SubID)
{
	LBItemList::const_iterator	MainIt = m_listItems.begin();	
	for( int MainCnt=0; MainIt != m_listItems.end(); ++MainIt, ++MainCnt )
	{
		cTreeBoxItem*	pMainList =	*MainIt;
		SAFE_POINTER_CON( pMainList );

		LBItemList::const_iterator	SubIt =  pMainList->getItemList().begin();
		for(int	SubCnt=0; SubIt != pMainList->getItemList().end(); ++SubIt, ++SubCnt )
		{
			if(MainID == MainCnt && SubID == SubCnt)
				(*SubIt)->SetControlOnMode( true );
			else
				(*SubIt)->SetControlOnMode( false );
		}
	}
}

bool cTreeBox::darwItemList( LBItemList& itemList, CsPoint& itemPos, CsRect const& rcRender, int const& nStartIdx, int & nCount )
{
	if( itemList.empty() )
		return false;


	LBItemList::iterator it = itemList.begin();
	for( ; it != itemList.end(); ++it )
	{
		cTreeBoxItem*	pBoxItem = *it;
		SAFE_POINTER_BEK( pBoxItem );

		int	ItemHeight = pBoxItem->getItemHeight();

		if( itemPos.y > (rcRender.bottom - m_CsMargin.y -ItemHeight) )
			return false;

		bool bRender = true;
		if( nCount < nStartIdx )
			bRender = false;

		nCount++;

		if( bRender )
			pBoxItem->Render( itemPos );

		if( pBoxItem->getItemCount() > 0 )
		{
			if( pBoxItem->getIsOpen() )
			{
				if( bRender )
					itemPos.y += (ItemHeight + m_CsItemGab.y);

				itemPos.x += m_CsChildItemGab.x;
				bool bValue = darwItemList(pBoxItem->getItemList(), itemPos, rcRender, nStartIdx, nCount);
				itemPos.x -= m_CsChildItemGab.x;

				if( !bValue )
					return false;
			}
			else
			{
				if( bRender )
					itemPos.y += (ItemHeight  + m_CsItemGab.y);
			}
		}
		else
		{
			if( bRender )
				itemPos.y += (ItemHeight  + m_CsItemGab.y);
		}
	}

	return true;
}
void cTreeBox::Render()
{
	if( !m_bVisible )
		return;

	if( m_pBackGroundImg )
		m_pBackGroundImg->Render();

	CsPoint csPos = GetClient();
	csPos += m_CsMargin;
	int nStartIdx = 0;
	int nRenderCount = 0;
	if( m_pScrollBar )
		nStartIdx = m_pScrollBar->GetCurPosIndex();

	darwItemList( m_listItems, csPos, GetRect(), nStartIdx, nRenderCount );

	if( m_pScrollBar )
		m_pScrollBar->Render();
}

void cTreeBox::ResetDevice()
{
	cWindow::ResetDevice();

	if( m_pScrollBar )
		m_pScrollBar->ResetDevice();

	if( m_pBackGroundImg )
		m_pBackGroundImg->ResetDevice();

	LBItemList::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
		(*it)->ResetDevice();
}

size_t	cTreeBox::GetItemCount() const
{
	return m_listItems.size();
}

//----------------------------------------------------------------------------------------------------------
cTreeBoxItem* cTreeBox::getItemAtPoint(CsPoint const& pt) const
{	
	CsRect renderArea( CsPoint(0,0),GetSize());
	if( renderArea.PtInRect( pt ) )
	{
		float y = static_cast< float >( renderArea.top ) + m_CsMargin.y;
		if( pt.y >= y )
		{
			int nStartIdx = 0;	int nCount = 0;
			if( m_pScrollBar )
				nStartIdx = m_pScrollBar->GetCurPosIndex();
			return getItemFromListAtPoint(m_listItems, &y, pt, nStartIdx, nCount);
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------------------------------------
cTreeBoxItem* cTreeBox::getItemFromListAtPoint(const LBItemList& itemList, float* bottomY, CsPoint const& pt, int const& nStartIdx, int & nCount) const
{
	LBItemList::const_iterator it = itemList.begin();
	for( ; it != itemList.end(); ++it )
	{
		SAFE_POINTER_BEK( (*it) );

		bool bSkip = false;
		if( nStartIdx > nCount )
			bSkip = true;

		if( !bSkip )
		{
			*bottomY += (*it)->getItemHeight();
			if( pt.y < *bottomY && !(*it)->isDisabled() )
				return (*it);

			*bottomY += m_CsItemGab.y;
		}

		nCount++;

		if( (*it)->getItemCount() > 0 )
		{
			if( (*it)->getIsOpen() )
			{
				cTreeBoxItem* foundPointedAtTree = getItemFromListAtPoint((*it)->getItemList(), bottomY, pt, nStartIdx, nCount);
				if( 0 != foundPointedAtTree )
					return foundPointedAtTree;
			}
		}
	}

	return NULL;
}

bool cTreeBox::isAutoDeleted(void) const	
{ 
	return m_bAutoDeleted; 
}

void cTreeBox::setAutoDeleted(bool setting)
{ 
	m_bAutoDeleted = setting; 
}

void cTreeBox::setChildItemGab(CsPoint gab)
{ 
	m_CsChildItemGab = gab; 
}

void cTreeBox::setItemGab(CsPoint gab)
{ 
	m_CsItemGab = gab; 
}

void cTreeBox::setMargin(CsPoint gab)
{
	m_CsMargin = gab;
	UpdateRects();
}

void cTreeBox::UpdateRects()
{
	CsPoint pSize = GetSize() - m_CsMargin;
	pSize.y = m_nItemHeight;
	if( m_pScrollBar )
		pSize.x -= m_pScrollBar->GetSize().x;
}
//----------------------------------------------------------------------------------------------------------
void cTreeBox::configureScrollbars()
{
	SAFE_POINTER_RET( m_pScrollBar );

	float totalHeight = getTotalItemHeight();
 	bool bEnableScrollBar = (totalHeight > m_ptSize.y );
	if( !bEnableScrollBar )
	{
		m_pScrollBar->SetRange( CsPoint::ZERO );	
		return;
	}

	UINT nShowItemSize = 0;
	getShowItemSize( m_listItems, &nShowItemSize );
	m_pScrollBar->SetRange( CsPoint(0, nShowItemSize) );		
}

//----------------------------------------------------------------------------------------------------------
float cTreeBox::getTotalItemHeight(void) const
{
	float heightSum = 0;
	getTotalItemsInListHeight(m_listItems, &heightSum);
	return heightSum;
}
//----------------------------------------------------------------------------------------------------------
void cTreeBox::getTotalItemsInListHeight(const LBItemList& itemList, float* heightSum) const
{
	LBItemList::const_iterator it = itemList.begin();
	for( ; it != itemList.end(); ++it )
	{
		SAFE_POINTER_BEK( (*it) );
		*heightSum += ((*it)->getItemHeight() + m_CsItemGab.y);
		if( (*it)->getIsOpen() && ((*it)->getItemCount() > 0) )
			getTotalItemsInListHeight((*it)->getItemList(), heightSum);
	}
}
//----------------------------------------------------------------------------------------------------------
// 보여지는 아이템의 갯수
void cTreeBox::getShowItemSize(const LBItemList& itemList, UINT* nCount)
{
	LBItemList::const_iterator it = itemList.begin();
	for( ; it != itemList.end(); ++it )
	{
		SAFE_POINTER_BEK( (*it) );
		(*nCount)++;
		if( (*it)->getIsOpen() )
			getShowItemSize( (*it)->getItemList(), nCount );
	}
}

void cTreeBox::GetItemList( std::list<cTreeBoxItem*>& BoxItemList )
{
	LBItemList::const_iterator it =	m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		BoxItemList.push_back( *it );
		if( (*it)->getItemCount() > 0 )
		{
			(*it)->GetItemList( BoxItemList );
		}
	}
}
void cTreeBox::setItemHeight(int itemHeight)
{
	m_nItemHeight = itemHeight;
}
void cTreeBox::SetOnlyOpenMode(bool bMode)
{
	m_bOnlyOpenMode = bMode;
}
void cTreeBox::closeAllItem()
{
	LBItemList::iterator itr = m_listItems.begin();
	for (;itr != m_listItems.end(); ++itr)
		(*itr)->setIsOpen(false);
}

bool cTreeBox::BtnOnEvent(CsPoint pt)
{
	//버튼이 존재할때 해당 버튼이 눌리는 것을 체크
	cTreeBoxItem* item = getItemAtPoint( pt );

	SAFE_POINTER_RETVAL(item, false);
	std::list< cString::sELEMENT* >::iterator elemnetList = item->GetItem()->Get_sTextList();
	int listSize = item->GetItem()->GetListSize();
	for(int i = 0; i < listSize; ++i, ++elemnetList)
	{
		cString::sELEMENT::eTYPE tempType = (*elemnetList)->GetType();
		if(tempType == cString::sELEMENT::BUTTON)
		{
			cString::sBUTTON* button = (cString::sBUTTON*) (*elemnetList);
			CsPoint buttonPt = button->GetWorldPos();
			CsPoint size = buttonPt + button->s_ptSize;
			CsRect buttonRect(buttonPt, size);

			bool bVisible = button->s_pButton->GetVisible();
			if( buttonRect.PtInRect(pt + GetClient()) && bVisible == true)
			{
				OnEvent(TreeEvent_Item_MouseUp, button);
				return true;
			}
		}
	}
	return false;
}

void cTreeBox::SetEnable( bool bValue )
{
	m_bEnalbe = bValue;
}