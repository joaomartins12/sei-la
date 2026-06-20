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

cGridListBoxItem::cGridListBoxItem( UINT item_ID, CsPoint ptSize, bool disabled ):
m_itemID(item_ID),
m_bSelected(false),
m_pItem(NULL),
m_ptItemSize(ptSize),
m_pUserData(NULL),
m_bVisible(true)
{
}

cGridListBoxItem::cGridListBoxItem():
m_itemID(-1),
m_bSelected(false),
m_pItem(NULL),
m_ptItemSize(CsPoint::ZERO),
m_pUserData(NULL),
m_bVisible(true)
{
}

cGridListBoxItem::~cGridListBoxItem(void)
{
	Delete();
}

void cGridListBoxItem::ResetDevice()
{
	if(m_pItem)
		m_pItem->ResetDevice();
}

void cGridListBoxItem::SetAlpha( float fAlpha )
{
	if(m_pItem)
		m_pItem->SetAlpha(fAlpha);
}

void cGridListBoxItem::SetItem( cString* pSetItem )
{
	SAFE_POINTER_RET( pSetItem );	
	SAFE_NIDELETE(m_pItem);

	m_pItem = pSetItem;
	m_ptItemSize.y = m_pItem->GetMaxSize().y;
}

cString* cGridListBoxItem::GetItem()	const
{
	return	m_pItem;
}

void cGridListBoxItem::Delete()
{
	SAFE_NIDELETE(m_pItem);
	SAFE_DELETE( m_pUserData );
}

void cGridListBoxItem::Update(float const& fDeltaTime)
{
	SAFE_POINTER_RET( m_pItem );
	m_pItem->Update( fDeltaTime );
}

//----------------------------------------------------------------------------------------------
void cGridListBoxItem::Render( CsPoint csPos )
{
	m_ptLastRenderPos = csPos;

	if( m_pItem )
		m_pItem->Render_Overlab( csPos );
}

UINT cGridListBoxItem::getID(void) const			
{ 
	return m_itemID; 
}

bool cGridListBoxItem::isSelected(void) const		
{ 
	return m_bSelected; 
}

void cGridListBoxItem::setID(UINT nItemID)			
{ 
	m_itemID = nItemID; 
}

void cGridListBoxItem::setSelected(bool setting)	
{ 
	m_bSelected = setting; 
}

void cGridListBoxItem::SetUserData( CUSTOMDATA* pUserData )
{
	SAFE_DELETE( m_pUserData );
	m_pUserData = pUserData;
}

CUSTOMDATA* cGridListBoxItem::GetUserData() const
{
	return m_pUserData;
}

void cGridListBoxItem::OnMouseOver()
{
	if( m_pItem )
		m_pItem->OnMouseOver();
}

void cGridListBoxItem::OnMouseLeave()
{
	if( m_pItem )
		m_pItem->OnMouseLeave();
}

void cGridListBoxItem::OnMouseBtnDown()
{
	if( m_pItem )
		m_pItem->OnMouseDown();
}

void cGridListBoxItem::OnMouseBtnUp()
{
	if( m_pItem )
		m_pItem->OnMouseUp();
}

void cGridListBoxItem::onMousePressed()
{
	if( m_pItem )
		m_pItem->onMousePressed();
}

CsPoint cGridListBoxItem::GetWorldPos() const
{
// 	if( m_pItem )
// 		return m_pItem->GetWorldPos();
// 	return CsPoint::ZERO;
	return m_ptLastRenderPos;
}

CsPoint cGridListBoxItem::getItemSize() const		
{ 
	return m_ptItemSize; 
}
void cGridListBoxItem::setItemSize( CsPoint const& nItemSize )
{
	m_ptItemSize = nItemSize;
}

void cGridListBoxItem::SetVisible( bool bVisible ) const
{
	m_bVisible = bVisible;
}

bool cGridListBoxItem::GetVisible() const
{
	return m_bVisible;
}

void cGridListBoxItem::SetEnable( bool bEnable ) const
{
	m_bEnable=bEnable;
	if( m_pItem )
		m_pItem->SetEnable(bEnable);
}

bool cGridListBoxItem::GetEnable() const
{
	return m_bEnable;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------
cGridListBox::cGridListBox(void)
:m_pScrollBar(NULL), m_pBackGroundImg(NULL)
, m_pSelectedItem(NULL), m_ptItemSize(CsPoint::ZERO), m_nItemExtendSize(1), m_pOverItem(NULL),
m_pDownItem(NULL), m_ptItemGab(CsPoint::ZERO), m_pSelectedImg(NULL), m_pMoverImg(NULL)
, m_bBackOverAndSelectedImgRender(true), m_enRenderType(LowRightDown), m_enStartPointType(LeftTop), m_nShowLine(0), m_ptMargin(CsPoint::ZERO)
,m_nShowLineStartPos(0), m_busePressedMoveEvent(false),m_bAutoSelection(true),m_bEnable(true)
{
	m_listItems.clear();
}

cGridListBox::~cGridListBox(void)
{
	Delete();
}

void cGridListBox::
Init( cWindow* pParent, CsPoint pos, CsPoint size, CsPoint gapSize, CsPoint itemSize,
	 RenderType enRenderType, StartPointType enStartPointType, bool bApplyWindowSize, int nItemColumnSize )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );
	m_nItemExtendSize = nItemColumnSize;
	m_enRenderType = enRenderType;
	m_enStartPointType = enStartPointType;
	
	SetItemGab(gapSize);
	SetItemSize( itemSize );
	AutoSetExtendSize();
}

void cGridListBox::
Init( cWindow* pParent, CsPoint pos, CsPoint size, CsPoint gapSize, CsPoint itemSize,
	 RenderType enRenderType, StartPointType enStartPointType, char* cBGImagePath, bool bApplyWindowSize, int nItemColumnSize )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );
	SetBackGorundImg( cBGImagePath );
	m_nItemExtendSize = nItemColumnSize;
	m_enRenderType = enRenderType;
	m_enStartPointType = enStartPointType;
	
	SetItemGab(gapSize);
	SetItemSize( itemSize );
	AutoSetExtendSize();
}

void cGridListBox::SetBackGorundImg( char* cImagePath )
{
	SetBackGorundImg( cImagePath, CsPoint::ZERO, GetSize() );
}

void cGridListBox::SetBackGorundImg( char* cImagePath, CsPoint const& pos, CsPoint const& size )
{
	SAFE_POINTER_RET( cImagePath );

	if( m_pBackGroundImg )
		SAFE_NIDELETE( m_pBackGroundImg );

	m_pBackGroundImg = NiNew cSprite;
	if( m_pBackGroundImg )
		m_pBackGroundImg->Init( this,pos, size, cImagePath, false );
}

void cGridListBox::SetMouseOverImg( char* cImagePath )
{
	SetMouseOverImg( cImagePath, CsPoint::ZERO, m_ptItemSize);
}

void cGridListBox::SetMouseOverImg( char* cImagePath, CsPoint const& pos, CsPoint const& size )
{
	SAFE_POINTER_RET( cImagePath );
	SAFE_NIDELETE( m_pMoverImg );

	m_pMoverImg = NiNew cSprite;
	if( m_pMoverImg )
		m_pMoverImg->Init( NULL, pos, size, cImagePath, false );
}

void cGridListBox::SetMouseOverImg(cImage* pImage)
{
	SAFE_NIDELETE( m_pMoverImg );
	m_pMoverImg = pImage;
	if( m_pMoverImg )
		m_pMoverImg->SetSize( m_ptItemSize );
}

void cGridListBox::SetSelectedImg( char* cImagePath )
{
	SetSelectedImg( cImagePath, CsPoint::ZERO, m_ptItemSize );
}

void cGridListBox::SetSelectedImg( char* cImagePath, CsPoint const& pos, CsPoint const& size )
{
	SAFE_POINTER_RET( cImagePath );
	SAFE_NIDELETE( m_pSelectedImg );

	m_pSelectedImg = NiNew cSprite;
	if( m_pSelectedImg )
		m_pSelectedImg->Init( NULL, pos, size, cImagePath, false );
}

void cGridListBox::SetSelectedImg(cImage* pImage)
{
	SAFE_NIDELETE( m_pSelectedImg );
	m_pSelectedImg = pImage;
	if( m_pSelectedImg )
		m_pSelectedImg->SetSize( m_ptItemSize );
}

void cGridListBox::UpdateRects()
{
// 	if( m_pScrollBar )
// 	{
// 		if( m_pSelectedImg )
// 		{
// 			CsPoint pSize = m_pSelectedImg->GetSize();
// 			m_pSelectedImg->SetSize( CsPoint( pSize.x - m_pScrollBar->GetSize().x, pSize.y ) );
// 		}
// 
// 		if( m_pMoverImg )
// 		{
// 			CsPoint pSize = m_pMoverImg->GetSize();
// 			m_pMoverImg->SetSize( CsPoint( pSize.x - m_pScrollBar->GetSize().x, pSize.y ) );
// 		}
// 	}
}

void cGridListBox::Delete()
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

void cGridListBox::SetAlpha( float fAlpha )
{
	if( m_pBackGroundImg )	
		m_pBackGroundImg->SetAlpha( fAlpha );

	if( m_pScrollBar )
		m_pScrollBar->SetAlpha( fAlpha );

	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		if( (*it) )
			(*it)->SetAlpha( fAlpha );
	}
}

void cGridListBox::onMouseLeave()
{
	if( m_pOverItem )
	{
		m_pOverItem->OnMouseLeave();
		m_pOverItem = NULL;
	}
	m_pDownItem = NULL;
	OnEvent( GRID_MOUSE_LEAVE, m_pDownItem );
}

int	cGridListBox::Update_ForMouse( CsPoint ptParent )
{
	if( !m_bVisible )
		return false;

	if( !PtInWindowRect( ptParent ) )
	{
		onMouseLeave();
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
			onMouseLeave();
			return true;
		}
	}

	switch( CURSOR_ST.GetButtonState() )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		onMouseLeave();
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

void cGridListBox::Update(float const& fDeltaTime)
{
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		(*it)->Update( fDeltaTime );
	}
}

//----------------------------------------------------------------------------------------------------------
bool cGridListBox::onMouseButtonDown(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cGridListBoxItem* pItem = getItemAtPoint( pt );
	if( pItem == NULL)
		return false;
	
	if( !pItem->GetVisible() )
		return false;

	if( !pItem->GetEnable() )
		return true;

	m_pDownItem = pItem;

	pItem->OnMouseBtnDown();
	OnEvent( GRID_MOUSE_DOWN_ON_ITEM, m_pDownItem );
	return true;
}

void	cGridListBox::releaseSelection(void)
{
	if( m_pSelectedItem != NULL)
	{
		m_pSelectedItem->setSelected(false);
		m_pSelectedItem = NULL;
	}

}

bool cGridListBox::selectionChanged( cGridListBoxItem* pSelItem, bool bSendEvent )
{
	if( NULL == pSelItem )
		return false;

	OnEvent( GRID_CLICKED_ITEM, pSelItem ); // 눌렀던 아이템 눌러도 항시 호출

	if( !m_bAutoSelection )
		return true;

	if( m_pSelectedItem != pSelItem )
	{
		if( m_pSelectedItem )
			m_pSelectedItem->setSelected(false);

		pSelItem->setSelected(true);

		m_pSelectedItem = pSelItem;
		if( bSendEvent )
			OnEvent( GRID_SELECT_CHANGE_ITEM, m_pSelectedItem );
	}
	else
	{
		if( bSendEvent )
			OnEvent( GRID_SELECTED_ITEM, m_pSelectedItem );
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------
bool cGridListBox::onMouseButtonUp(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cGridListBoxItem* pItem = getItemAtPoint( pt );

	OnEvent( GRID_MOUSE_UP_ON_GRIDLIST, pItem );
	if( NULL == pItem || !pItem->GetVisible() || !pItem->GetEnable() )
	{
		m_pDownItem = 0;
		return true;
	}

	if( m_pDownItem != pItem )
	{
		m_pDownItem = 0;
		return true;
	}

	m_pDownItem = 0;

	pItem->OnMouseBtnUp();
	if( BtnOnEvent(ptParent) == false )
		selectionChanged( pItem );


	
	return true;
}

bool cGridListBox::onMousePressed(CsPoint ptParent)
{
	if( m_pDownItem )
		m_pDownItem->onMousePressed();

	if(m_busePressedMoveEvent)
		onMouseMove(ptParent);

	return true;
}

bool cGridListBox::onMouseMove(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cGridListBoxItem* pItem = getItemAtPoint( pt );
	OnEvent( GRID_CHANGED_MOUSEON_ITEM, pItem );
	if( pItem && pItem->GetVisible() && pItem->GetEnable() )
	{
		if( m_pOverItem )
			m_pOverItem->OnMouseLeave();
		m_pOverItem = pItem;
		m_pOverItem->OnMouseOver();
		return true;
	}

	if( m_pOverItem )
	{
		m_pOverItem->OnMouseLeave();
		m_pOverItem = 0;
	}

	return false;
}

void cGridListBox::SetItemGab(CsPoint gab)
{
	m_ptItemGab = gab;
	SetItemSize( m_ptItemSize );	
}

//----------------------------------------------------------------------------------------------------------
cGridListBoxItem* cGridListBox::getItemAtPoint(CsPoint const& pt) const
{	
	CsPoint ptSize = GetSize();
	//갭 만큼 영역 아래까지 감지 되지 않게
	//ptSize.y = ptSize.y - m_ptItemGab.y;
	//스크롤바 영역 제거
	if (m_pScrollBar != NULL)
	{
		CsPoint ptCheckBar = GetClient() + pt;
		CsRect rectScrollBar = m_pScrollBar->GetRect();
		if(rectScrollBar.PtInRect(ptCheckBar))
			return NULL;
	}
	
	CsRect renderArea( CsPoint(0,0), ptSize );
	if( renderArea.PtInRect( pt ) )
	{
		float x = static_cast< float >( renderArea.left );
		float y = static_cast< float >( renderArea.top );
		if( pt.x >= x && pt.y >= y )
		{
			int nStartIdx = 0;
			if( m_pScrollBar != NULL )
				nStartIdx = (m_pScrollBar->GetCurPosIndex()*m_nItemExtendSize);
			else 
				nStartIdx = (m_nShowLineStartPos * m_nItemExtendSize);
			return getItemFromListAtPoint(&y, pt, nStartIdx);
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------------------------------------
cGridListBoxItem* cGridListBox::getItemFromListAtPoint(float* bottomY, CsPoint const& pt, int const& nStartIdx) const
{
	std::list<cGridListBoxItem*>::const_iterator it = m_listItems.begin();
	CsPoint gridEnd(CsPoint::ZERO);
	CsPoint gridListBoxSize = GetSize();
	switch(m_enStartPointType)
	{
	case RightTop:
		{
			gridEnd.x = gridListBoxSize.x;
		}
		break;
	case RightBottom:
		{
			gridEnd = gridListBoxSize;
		}
		break;
	case LeftBottom:
		{
			gridEnd.y = gridListBoxSize.y;
		}
		break;
	case LeftTop:
	default:
		break;
	}

	
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		SAFE_POINTER_BEK( (*it) );

		if( n < nStartIdx )
			continue;
		switch(m_enRenderType)
		{
		case LowRightDown:	//LeftTop
			{
				CsPoint itemSize = (*it)->getItemSize();
				int nItemID = (*it)->getID();
				//라인 변경 조건
				if ( n == nStartIdx )
					gridEnd.y += (itemSize.y + m_ptMargin.y);
				else if ( (nItemID % m_nItemExtendSize ) == 0)
					gridEnd.y += (itemSize.y + m_ptItemGab.y);

				// 좌표 분석
				if ( pt.y >= gridEnd.y && pt.y <= (gridEnd.y + m_ptItemGab.y) )
					return NULL;//마우스가 갭 부분에 존재
				else if( pt.y < gridEnd.y )
				{
					if( (nItemID % m_nItemExtendSize ) == 0 )
						gridEnd.x += (itemSize.x + m_ptMargin.x);
					else
						gridEnd.x += (itemSize.x + m_ptItemGab.x);

					if( pt.x >= gridEnd.x && pt.x <= gridEnd.x + m_ptItemGab.x )
						return NULL;//마우스가 X갭 부분에 존재
					else if (pt.x < gridEnd.x)
						return (*it);
				}
			}
			break;
		case LowRightUp:	//LeftBottom
			{
				CsPoint itemSize = (*it)->getItemSize();
				int nItemID = (*it)->getID();
				//라인 변경 조건
				if ( n == nStartIdx )
					gridEnd.y -= (itemSize.y + m_ptMargin.y);
				else if ( (nItemID % m_nItemExtendSize ) == 0)
					gridEnd.y -= (m_ptItemGab.y + itemSize.y);

				// 좌표 분석
				if (pt.y >= gridEnd.y + itemSize.y && pt.y <= gridEnd.y + itemSize.y + m_ptItemGab.y)
					return NULL;//마우스가 갭 부분에 존재
				else if( pt.y < gridEnd.y + itemSize.y && pt.y > gridEnd.y )
				{
					if( (nItemID % m_nItemExtendSize ) == 0 )
						gridEnd.x += (itemSize.x + m_ptMargin.x);
					else
						gridEnd.x += (itemSize.x + m_ptItemGab.x);

					if( pt.x >= gridEnd.x && pt.x <= gridEnd.x + m_ptItemGab.x )
						return NULL;//마우스가 갭 부분에 존재
					else if (pt.x < gridEnd.x)
						return (*it);
				}
			}
			break;
		case LowLeftDown:	//RightTop
			{
				CsPoint itemSize = (*it)->getItemSize();
				int nItemID = (*it)->getID();
				//라인 변경 조건
				if ( n == nStartIdx )
					gridEnd.y += (itemSize.y + m_ptMargin.y);
				else if ( (nItemID % m_nItemExtendSize ) == 0)
					gridEnd.y += (m_ptItemGab.y + itemSize.y);

				// 좌표 분석
				
				if ( pt.y >= gridEnd.y && pt.y <= gridEnd.y + m_ptItemGab.y )
					return NULL;//마우스가 갭 부분에 존재
				else if( pt.y < gridEnd.y )
				{
					if( (nItemID % m_nItemExtendSize ) == 0 )
						gridEnd.x -= (itemSize.x + m_ptMargin.x);
					else
						gridEnd.x -= (itemSize.x + m_ptItemGab.x);

					if ( pt.x <= gridEnd.x && pt.x >= (gridEnd.x - m_ptItemGab.x) )
						return NULL;//마우스가 갭 부분에 존재
					else if (pt.x > gridEnd.x)
						return (*it);
				}
			}
			break;
		case LowLeftUp:		//RightBottom
			{
				CsPoint itemSize = (*it)->getItemSize();
				int nItemID = (*it)->getID();
				//라인 변경 조건
				if ( n == nStartIdx )
					gridEnd.y -= (itemSize.y + m_ptMargin.y);
				else if ( (nItemID % m_nItemExtendSize ) == 0)
					gridEnd.y -= (m_ptItemGab.y + itemSize.y);

				// 좌표 분석
				if (pt.y >= gridEnd.y + itemSize.y && pt.y <= gridEnd.y + itemSize.y + m_ptItemGab.y)
					return NULL;//마우스가 갭 부분에 존재
				else if( pt.y < gridEnd.y + itemSize.y && pt.y > gridEnd.y )
				{
					if( (nItemID % m_nItemExtendSize ) == 0 )
						gridEnd.x -= (itemSize.x + m_ptMargin.x);
					else
						gridEnd.x -= (itemSize.x + m_ptItemGab.x);

					if (pt.x <= gridEnd.x && pt.x >= gridEnd.x - m_ptItemGab.x)
						return NULL;//마우스가 갭 부분에 존재
					else if (pt.x > gridEnd.x)
						return (*it);
				}
			}
			break;
		case ColumnDownRight:	//LeftTop
			{
				CsPoint itemSize = (*it)->getItemSize();
				int nItemID = (*it)->getID();
				//라인 변경 조건
				if ( n == nStartIdx )
					gridEnd.x += (itemSize.x + m_ptMargin.x);
				else if ( (nItemID % m_nItemExtendSize ) == 0)
					gridEnd.x += (itemSize.x + m_ptItemGab.x);

				// 좌표 분석
				if ( pt.x >= gridEnd.x && pt.x <= (gridEnd.x + m_ptItemGab.x) )
					return NULL;//마우스가 갭 부분에 존재
				else if( pt.x < gridEnd.x )
				{
					if( (nItemID % m_nItemExtendSize ) == 0 )
						gridEnd.y += (itemSize.y + m_ptMargin.y);
					else
						gridEnd.y += (m_ptItemGab.y + itemSize.y);
					if ( pt.y >= gridEnd.y && pt.y <= gridEnd.y + m_ptItemGab.y )
						return NULL;//마우스가 갭 부분에 존재
					else if (pt.y < gridEnd.y)
						return (*it);
				}
			}
			break;
		case ColumnDownLeft:	//RightTop
			{
				CsPoint itemSize = (*it)->getItemSize();
				int nItemID = (*it)->getID();
				//라인 변경 조건
				if ( n == nStartIdx )
					gridEnd.x -= (itemSize.x + m_ptMargin.x);
				else if ( (nItemID % m_nItemExtendSize ) == 0)
					gridEnd.x -= (itemSize.x + m_ptItemGab.x);

				// 좌표 분석
				if (pt.x <= gridEnd.x && pt.x >= gridEnd.x - m_ptItemGab.x)
					return NULL;
				else if( pt.x > gridEnd.x )
				{
					if( (nItemID % m_nItemExtendSize ) == 0 )
						gridEnd.y += (itemSize.y + m_ptMargin.y);
					else
						gridEnd.y += (m_ptItemGab.y + itemSize.y);

					if ( pt.y >= gridEnd.y && pt.y <= (gridEnd.y + m_ptItemGab.y) )
						return NULL;//마우스가 갭 부분에 존재
					else if (pt.y < gridEnd.y)
						return (*it);
				}
			}
			break;
		case ColumnUpRight:		//LeftBottom
			{
				CsPoint itemSize = (*it)->getItemSize();
				int nItemID = (*it)->getID();
				//라인 변경 조건
				if ( n == nStartIdx )
					gridEnd.x += (itemSize.x + m_ptMargin.x);
				else if ( (nItemID % m_nItemExtendSize ) == 0)
					gridEnd.x += (itemSize.x + m_ptItemGab.x);

				// 좌표 분석
				if (pt.x >= gridEnd.x && pt.x <= gridEnd.x + m_ptItemGab.x)
					return NULL;//마우스가 갭 부분에 존재
				else if( pt.x < gridEnd.x )
				{
					if( (nItemID % m_nItemExtendSize ) == 0 )
						gridEnd.y -= (itemSize.y + m_ptMargin.y);
					else
						gridEnd.y -= (m_ptItemGab.y + itemSize.y);

					if(pt.y >= gridEnd.y + itemSize.y && pt.y <= gridEnd.y + itemSize.y + m_ptItemGab.y)
						return NULL;//마우스가 갭 부분에 존재
					else if (pt.y < gridEnd.y + itemSize.y && pt.y > gridEnd.y)
						return (*it);
				}
			}
			break;
		case ColumnUpLeft:		//RightBottom
			{
				CsPoint itemSize = (*it)->getItemSize();
				int nItemID = (*it)->getID();
				//라인 변경 조건
				if ( n == nStartIdx )
					gridEnd.x -= (itemSize.x + m_ptMargin.x);
				else if ( (nItemID % m_nItemExtendSize ) == 0)
					gridEnd.x -= (itemSize.x + m_ptItemGab.x);
				// 좌표 분석
				if(pt.x <= gridEnd.x && pt.x >= gridEnd.x - m_ptItemGab.x)
					return NULL;//마우스가 갭 부분에 존재
				else if( pt.x > gridEnd.x )
				{
					if( (nItemID % m_nItemExtendSize ) == 0 )
						gridEnd.y -= (itemSize.y + m_ptMargin.y);
					else
						gridEnd.y -= (m_ptItemGab.y + itemSize.y);
 
					if (pt.y >= gridEnd.y + itemSize.y && pt.y <= gridEnd.y + itemSize.y + m_ptItemGab.y)
						return NULL;//마우스가 갭 부분에 존재
					else if (pt.y < gridEnd.y + itemSize.y && pt.y > gridEnd.y)
						return (*it);
				}
			}
			break;
		default:
			break;
		}
	}

	return NULL;
}


bool cGridListBox::AddItem( cGridListBoxItem* pAddItem )
{
	SAFE_POINTER_RETVAL( pAddItem, false );
	int nIdx = GetItemIndex( pAddItem );
	if( -1 != nIdx )
		return false;

	CsPoint itemSize = pAddItem->getItemSize();
	if( m_ptItemSize.y < itemSize.y )
	{
		m_ptItemSize.y = itemSize.y;
		changeAllItemHeight( itemSize.y );
	}
	if( m_ptItemSize.x < itemSize.x )
	{
		m_ptItemSize.x = itemSize.x;
		changeAllItemWidth( m_ptItemSize.x );
	}
	pAddItem->setItemSize(m_ptItemSize);

	m_listItems.push_back( pAddItem );
	configureScrollbars();
	return true;
}
void cGridListBox::changeAllItemWidth(int const& nNewWidth)
{
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		CsPoint ptTemp = (*it)->getItemSize();
		ptTemp.x = nNewWidth;
		(*it)->setItemSize( ptTemp );
	}

}
void cGridListBox::changeAllItemHeight(int const& nNewHeight)
{
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		CsPoint ptTemp = (*it)->getItemSize();
		ptTemp.y = nNewHeight;
		(*it)->setItemSize( ptTemp );
	}
}

bool cGridListBox::InsertItem( int nPos, cGridListBoxItem* pAddItem )
{
	SAFE_POINTER_RETVAL( pAddItem, false );

	CsPoint itemSize = pAddItem->getItemSize();
	if( m_ptItemSize.y < itemSize.y )
	{
		m_ptItemSize.y = itemSize.y;
		changeAllItemHeight( itemSize.y );
	}
	if( m_ptItemSize.x < itemSize.x )
	{
		m_ptItemSize.x = itemSize.x;
		changeAllItemWidth( m_ptItemSize.x );
	}
	pAddItem->setItemSize(m_ptItemSize);

	bool bUpdateScrollbar = false;

	if( m_listItems.size() <= nPos )
	{
		m_listItems.push_back( pAddItem );
		bUpdateScrollbar = true;
	}
	else
	{
		std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
		for( int n = 0; it != m_listItems.end(); ++it, ++n )
		{
			if( n != nPos )
				continue;

			m_listItems.insert( it, pAddItem );
			RecountItemID(nPos);
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

void cGridListBox::RemoveItem( cGridListBoxItem* pAddItem )
{
	bool bUpdateScrollbar = false;
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0 ; it != m_listItems.end(); ++it )
	{
		if( (*it) != pAddItem )
			continue;

		if( m_pSelectedItem == (*it) )
			m_pSelectedItem = NULL;

		if( m_pOverItem == (*it) )
			m_pOverItem = NULL;

		SAFE_NIDELETE( (*it) );
		m_listItems.erase(it);
		RecountItemID(n);
		bUpdateScrollbar = true;
		pAddItem = NULL;
		break;
	}

	if( bUpdateScrollbar )
		configureScrollbars();
}

void cGridListBox::RemoveItem( int nPos )
{
	bool bUpdateScrollbar = false;
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
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
		RecountItemID(n);
		bUpdateScrollbar = true;
		break;
	}

	if( bUpdateScrollbar )
		configureScrollbars();
}

void cGridListBox::RemoveAllItem()
{
	m_pDownItem = NULL;
	m_pSelectedItem = NULL;
	m_pOverItem = NULL;
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		cGridListBoxItem* pItem = *it;
		SAFE_NIDELETE( pItem );
	}
	m_listItems.clear();

	configureScrollbars();
}

int cGridListBox::GetItemIndex(cGridListBoxItem const* pFindItem) 
{
	std::list<cGridListBoxItem*>::const_iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( (*it) == pFindItem )
			return (*it)->getID();
	}

	return -1;
}

void cGridListBox::SetScrollBar( cScrollBar* pScrollBar )
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
	csPos.y = -7;					//스크롤 위치가 살짝 어색해서 보정해줌 20170530 for sealmaster
	csScSize.y = csScSize.y - (pScrollBar->GetDefaultBtnSize().y*2) ;
	m_pScrollBar->ReSize( csScSize, CsRect( CsPoint::ZERO, csScSize ) );
	m_pScrollBar->SetPos( csPos );

	if( m_ptItemSize.y > 0 )
	{
		SetScrollBarRenderCount();
	}

	m_pScrollBar->ResetDevice();
	UpdateRects();
	configureScrollbars();
}

cGridListBoxItem const*	cGridListBox::GetItemFormGridItemID( int const& nGridID ) const
{
	std::list<cGridListBoxItem*>::const_iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( (*it)->getID() == nGridID )
			return (*it);
	}
	return NULL;
}

cGridListBoxItem const*	cGridListBox::GetItemFormPos( int nPos ) const
{
	std::list<cGridListBoxItem*>::const_iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( n == nPos )
			return (*it);
	}
	return NULL;
}

size_t cGridListBox::GetItemCount() const
{
	return m_listItems.size();
}

cGridListBoxItem const* cGridListBox::GetSelectedItem() const
{
	return m_pSelectedItem;
}

int cGridListBox::GetSelectedItemIdx()
{
	SAFE_POINTER_RETVAL( m_pSelectedItem, -1 );

	return GetItemIndex( m_pSelectedItem );
}

void cGridListBox::SetSelectMode(int MainID, int SubID)
{
	std::list<cGridListBoxItem*>::const_iterator	MainIt;

	int	MainCnt=0;
	for(MainIt = m_listItems.begin(); MainIt != m_listItems.end(); ++MainIt )
	{
		cGridListBoxItem*	pMainList =	*MainIt;
		cString*	pStr	= pMainList->GetItem();
		SAFE_POINTER_CON(pStr);
		cString::sBUTTON*	pButton	= (cString::sBUTTON*)pStr->FindElement(cString::sELEMENT::BUTTON);
		SAFE_POINTER_CON(pButton);
		if(MainID == MainCnt)
			pButton->s_pButton->SetMouseOnMode(true);
		else
			pButton->s_pButton->SetMouseOnMode(false);

		MainCnt++;
	}
}

void cGridListBox::SetItemSize( CsPoint ptItemSize )
{
	m_ptItemSize = ptItemSize;
	CsPoint ptSize = GetSize();
	switch( m_enRenderType )
	{
	case LowRightDown:			//키패드 3번 방향으로 확장
	case LowRightUp:				//키패드 9번 방향으로 확장
	case LowLeftDown:			//키패드 1번 방향으로 확장
	case LowLeftUp:				//키패드 7번 방향으로 확장
		{
			if(m_ptItemSize.y + m_ptItemGab.y != 0)
				m_nShowLine = (ptSize.y + m_ptItemGab.y) / (m_ptItemSize.y + m_ptItemGab.y);
		}
		break;
	case ColumnDownRight:		//키패드 3번 방향으로 확장
	case ColumnDownLeft:			//키패드 1번 방향으로 확장
	case ColumnUpRight:			//키패드 9번 방향으로 확장
	case ColumnUpLeft:			//키패드 7번 방향으로 확장
		{
			if(m_ptItemSize.x + m_ptItemGab.x != 0)
				m_nShowLine = (ptSize.x + m_ptItemGab.x) / (m_ptItemSize.x + m_ptItemGab.x);
		}
		break;	
	}
}

void cGridListBox::ChangeShowLinePos( int const& nLineNumber )
{
	if( m_pScrollBar )
	{
		m_nShowLineStartPos = 0;
		return;
	}

	size_t nItemTotalCount = GetItemCount();
	int nMaxLineCount = nItemTotalCount / m_nItemExtendSize;
	if( nItemTotalCount % m_nItemExtendSize > 0 )
		nMaxLineCount++;


	if( nLineNumber < 0 || nMaxLineCount <= nLineNumber )
		return;

	m_nShowLineStartPos = nLineNumber;
}

void cGridListBox::Render()
{
	if( !m_bVisible )
		return;

	if( m_pBackGroundImg )
		m_pBackGroundImg->Render();

	if( !m_listItems.empty() )
	{
		int nShowIdx = 0;
		if( m_pScrollBar )
			nShowIdx = m_pScrollBar->GetCurPosIndex();
		else
			nShowIdx = m_nShowLineStartPos ;

		nShowIdx *= m_nItemExtendSize;

		RenderFromType(nShowIdx);
	}

	if( m_pScrollBar )
		m_pScrollBar->Render();
}

void cGridListBox::ResetDevice()
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

	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( ; it != m_listItems.end(); ++it )
	{
		if( (*it) )
			(*it)->ResetDevice();
	}
}

//----------------------------------------------------------------------------------------------------------
void cGridListBox::configureScrollbars()
{
	SAFE_POINTER_RET( m_pScrollBar );

	float totalHeight = getTotalItemHeight();
	bool bEnableScrollBar = (totalHeight > m_ptSize.y );
	if( !bEnableScrollBar )
	{
		m_pScrollBar->SetRange( CsPoint::ZERO );	
		return;
	}
	UINT nShowItemSize = GetVisibleItemCount()/m_nItemExtendSize;
	if(nShowItemSize*m_nItemExtendSize < GetVisibleItemCount())
		nShowItemSize++;

	m_pScrollBar->SetRange( CsPoint(0, nShowItemSize) );		
}

//----------------------------------------------------------------------------------------------------------
float cGridListBox::getTotalItemHeight(void) const
{
	float heightSum = m_ptMargin.y;

	std::list<cGridListBoxItem*>::const_iterator it = m_listItems.begin();
	for(int i = 0; it != m_listItems.end(); ++it, ++i )
	{
		if( (i%m_nItemExtendSize) == 0)
		{
			heightSum += (*it)->getItemSize().y + m_ptItemGab.y;
		}
	}

	return heightSum;
}

cGridListBoxItem const*	cGridListBox::GetMouseOverItem() const
{
	return m_pOverItem;
}

void cGridListBox::ChangeSelectFront()
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
}

void cGridListBox::ChangeSelectNext()
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
}

void cGridListBox::SetSelectedItemFromIdx( int const& nSelIdx, bool bSendEvent )
{
	cGridListBoxItem * pNewSelItem = const_cast<cGridListBoxItem*>(GetItemFormPos( nSelIdx ));
	if( pNewSelItem )
		selectionChanged( pNewSelItem, bSendEvent );	
}

void cGridListBox::SetBackOverAndSelectedImgRender(bool bValue)
{				
	m_bBackOverAndSelectedImgRender = bValue;
}
void cGridListBox::SetUsePressedMoveEvent(bool bValue)
{
	m_busePressedMoveEvent = bValue;
}

void cGridListBox::SetScrollBarFirstPos()
{
	SAFE_POINTER_RET( m_pScrollBar );

	m_pScrollBar->SetCurPosIndex( 0 );
}

void cGridListBox::SetScrollBarEndPos()
{
	SAFE_POINTER_RET( m_pScrollBar );

	m_pScrollBar->SetCurPosIndex( m_pScrollBar->GetMaxPosIndex() );
}

void cGridListBox::SetScrollBarPos( int const& nVisibleIndex )
{
	SAFE_POINTER_RET( m_pScrollBar );

	if( nVisibleIndex >= GetVisibleItemCount() )
		return;

	if( 0 == m_nItemExtendSize )
		return;

	int nRenderPos = nVisibleIndex / m_nItemExtendSize;
	if( m_pScrollBar->GetMaxPosIndex() < nRenderPos )
		nRenderPos = m_pScrollBar->GetMaxPosIndex();

	m_pScrollBar->SetCurPosIndex( nRenderPos );
}

bool cGridListBox::onMouseRButtonDown(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cGridListBoxItem* item = getItemAtPoint( pt );
	if( 0 == item )
		return false;

	m_pDownItem = item;
	return true;

}
bool cGridListBox::onMouseRButtonUp(CsPoint ptParent)
{
	CsPoint pt = MousePosToWindowPos( ptParent );
	cGridListBoxItem* item = getItemAtPoint( pt );
	if ( item != null && item == m_pDownItem)
	{
		OnEvent( GRID_SELECTED_ITEM_RIGHT, m_pOverItem );
		m_pDownItem = NULL;
		return true;
	}
	m_pDownItem = NULL;
	return false;

}
void cGridListBox::RecountItemID(int nPos)
{
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if (n < nPos)
			continue;
		else
		{
			(*it)->setID(n);
			//cString 멀티버튼을 가지면 버튼이 cstring이 속한 Item의 아이디를 가지고 있는다.
			std::list< cString::sELEMENT* >::iterator elemnetList = (*it)->GetItem()->Get_sTextList();
			int listSize = (*it)->GetItem()->GetListSize();
			for(int i = 0; i < listSize; ++i, ++elemnetList)
			{
				cString::sELEMENT::eTYPE tempType = (*elemnetList)->GetType();
				if(tempType == cString::sELEMENT::BUTTON)
				{
					cString::sBUTTON* pButton = static_cast<cString::sBUTTON*>( (*elemnetList) );
					pButton->SetValue(n);
				}
			}
		}
	}
}
void cGridListBox::SetRenderType( RenderType renderType )
{
	m_enRenderType = renderType;
}
cGridListBox::RenderType cGridListBox::GetRenderType()
{
	return m_enRenderType;

}
void cGridListBox::SetStartPointType( StartPointType startPointType )
{
	m_enStartPointType = startPointType;
}
cGridListBox::StartPointType cGridListBox::GetStartPointType()
{
	return m_enStartPointType;
}

void cGridListBox::RenderFromType(int const& nRenderStartIdx)
{
	CsPoint ptLeftTop = GetClient();
	CsPoint ptSize = GetSize();
	CsPoint ptRightBottom = ptLeftTop + ptSize;
	CsPoint ptRenderPos = ptLeftTop;

	switch(m_enStartPointType)
	{
	case RightTop:
		ptRenderPos.x = ptRightBottom.x - m_ptItemSize.x - m_ptMargin.x;
		ptRenderPos.y += m_ptMargin.y;
		break;
	case RightBottom:
		ptRenderPos = ptRightBottom - m_ptItemSize - m_ptMargin;
		break;
	case LeftBottom:
		ptRenderPos.y = ptRightBottom.y - m_ptItemSize.y - m_ptMargin.y;
		ptRenderPos.x += m_ptMargin.x;
		break;
	case LeftTop:
		ptRenderPos += m_ptMargin;
	default:
		break;
	}
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		SAFE_POINTER_BEK( (*it) );

		if( n < nRenderStartIdx )
			continue;

		//줄이 바뀔때 && 최대 그리는 아이템 행/열 줄바꿈 횟수 확인
		if( n % m_nItemExtendSize == 0 && ( (n-nRenderStartIdx)/m_nItemExtendSize ) == m_nShowLine )
			break;

		if( m_bBackOverAndSelectedImgRender )
		{
			if( m_pSelectedItem && m_pSelectedItem == (*it) )
			{
				if( m_pSelectedImg )
					m_pSelectedImg->Render( ptRenderPos );
			}else if(  m_pOverItem && m_pOverItem == (*it) )
			{
				if( m_pMoverImg )
					m_pMoverImg->Render( ptRenderPos );
			}
		}
		if ( (*it)->GetVisible() == true )
			(*it)->Render(ptRenderPos);

		if( !m_bBackOverAndSelectedImgRender )
		{
			if( m_pSelectedItem && m_pSelectedItem == (*it) )
			{
				if( m_pSelectedImg )
					m_pSelectedImg->Render( ptRenderPos );
			}else if(  m_pOverItem && m_pOverItem == (*it) )
			{
				if( m_pMoverImg )
					m_pMoverImg->Render( ptRenderPos );
			}
		}
		
		switch(m_enRenderType)
		{
		case LowRightDown:	//LeftTop
			{
				ptRenderPos.x += (*it)->getItemSize().x + m_ptItemGab.x;
				if ( (n%m_nItemExtendSize) == m_nItemExtendSize - 1 )
				{
					ptRenderPos.x = ptLeftTop.x + m_ptMargin.x;
					ptRenderPos.y += (*it)->getItemSize().y + m_ptItemGab.y;
				}
			}
			break;
		case LowRightUp:	//LeftBottom
			{
				ptRenderPos.x += (*it)->getItemSize().x + m_ptItemGab.x;
				if ( (n%m_nItemExtendSize) == m_nItemExtendSize - 1 )
				{
					ptRenderPos.x = ptLeftTop.x + m_ptMargin.x;
					ptRenderPos.y -= (*it)->getItemSize().y + m_ptItemGab.y;
				}
			}
			break;
		case LowLeftDown:	//RightTop
			{
				ptRenderPos.x -= (*it)->getItemSize().x + m_ptItemGab.x;
				if ( (n%m_nItemExtendSize) == m_nItemExtendSize - 1 )
				{
					ptRenderPos.x = ptRightBottom.x - m_ptItemSize.x - m_ptMargin.x;
					ptRenderPos.y += (*it)->getItemSize().y + m_ptItemGab.y;
				}
			}
			break;
		case LowLeftUp:		//RightBottom
			{
				ptRenderPos.x -= (*it)->getItemSize().x + m_ptItemGab.x;
				if ( (n%m_nItemExtendSize) == m_nItemExtendSize - 1 )
				{
					ptRenderPos.x = ptRightBottom.x - m_ptItemSize.x - m_ptMargin.x;
					ptRenderPos.y -= (*it)->getItemSize().y + m_ptItemGab.y;
				}
			}
			break;
		case ColumnDownRight:	//LeftTop
			{
				ptRenderPos.y += (*it)->getItemSize().y + m_ptItemGab.y;
				if ( (n%m_nItemExtendSize) == m_nItemExtendSize - 1 )
				{
					ptRenderPos.y = ptLeftTop.y + m_ptMargin.y;
					ptRenderPos.x += (*it)->getItemSize().x + m_ptItemGab.x;
				}
			}
			break;
		case ColumnDownLeft:	//RightTop
			{
				ptRenderPos.y += (*it)->getItemSize().y + m_ptItemGab.y;
				if ( (n%m_nItemExtendSize) == m_nItemExtendSize - 1 )
				{
					ptRenderPos.y = ptLeftTop.y + m_ptMargin.y;
					ptRenderPos.x -= (*it)->getItemSize().x + m_ptItemGab.x;
				}
			}
			break;
		case ColumnUpRight:		//LeftBottom
			{
				ptRenderPos.y -= (*it)->getItemSize().y + m_ptItemGab.y;
				if ( (n%m_nItemExtendSize) == m_nItemExtendSize - 1 )
				{
					ptRenderPos.y = ptRightBottom.y - m_ptItemSize.y - m_ptMargin.y;
					ptRenderPos.x += (*it)->getItemSize().x + m_ptItemGab.x;
				}
			}
			break;
		case ColumnUpLeft:		//RightBottom
			{
				ptRenderPos.y -= ((*it)->getItemSize().y + m_ptItemGab.y);
				if ( (n%m_nItemExtendSize) == m_nItemExtendSize - 1 )
				{
					ptRenderPos.y = ptRightBottom.y - m_ptItemSize.y - m_ptMargin.y;
					ptRenderPos.x -= ((*it)->getItemSize().x + m_ptItemGab.x);
				}
			}
			break;
		default:
			break;
		}
	}	
}
void cGridListBox::AutoSetExtendSize()
{
	if( m_nItemExtendSize <= 1 && (m_ptItemGab.x + m_ptItemSize.x) != 0 && (m_ptItemGab.y + m_ptItemSize.y) != 0 )
	{
		if( m_enRenderType == LowRightDown ||
			m_enRenderType == LowRightUp ||
			m_enRenderType == LowLeftDown ||
			m_enRenderType == LowLeftUp
			)
		{
			int nItemCheckSizeX = m_ptItemSize.x + m_ptMargin.x;
			for(int i = 1; nItemCheckSizeX <= GetSize().x; ++i, nItemCheckSizeX += m_ptItemSize.x)
			{
				m_nItemExtendSize = i;
				nItemCheckSizeX += m_ptItemGab.x;
			}
		}
		else
		{
			int nItemCheckSizeY = m_ptItemSize.y + m_ptMargin.y;
			for(int i = 1; nItemCheckSizeY <= GetSize().y; ++i, nItemCheckSizeY += m_ptItemSize.y)
			{
				m_nItemExtendSize = i;
				nItemCheckSizeY += m_ptItemGab.y;
			}
		}
	}
}
void cGridListBox::SetScrollBarRenderCount()
{
	CsPoint ptSize = GetSize();
	if( m_enRenderType == LowRightDown ||
		m_enRenderType == LowRightUp ||
		m_enRenderType == LowLeftDown ||
		m_enRenderType == LowLeftUp
		)
	{
		int nShowCount = (ptSize.y - m_ptMargin.y) / (m_ptItemSize.y + m_ptItemGab.y);
		m_pScrollBar->SetRenderCount( nShowCount );
		m_nShowLine = nShowCount;
	}
	else
	{
		int nShowCount = (ptSize.x - m_ptMargin.x) / (m_ptItemSize.x + m_ptItemGab.x);
		m_pScrollBar->SetRenderCount( nShowCount );
		m_nShowLine = nShowCount;
	}
}
bool cGridListBox::BtnOnEvent(CsPoint pt)
{
	//버튼이 존재할때 해당 버튼이 눌리는 것을 체크
	CsPoint windowPt = MousePosToWindowPos( pt );
	cGridListBoxItem* item = getItemAtPoint( windowPt );

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
			if( buttonRect.PtInRect(pt) && bVisible == true)
			{
				OnEvent(GRID_BUTTON_UP, button);
				return true;
			}
		}
	}
	return false;
}

int cGridListBox::GetVisibleItemCount()
{
	int nVisibleCount = 0;
	std::list<cGridListBoxItem*>::iterator itr = m_listItems.begin();
	for(; itr != m_listItems.end(); ++itr)
	{
		if( (*itr)->GetVisible() == true )
			++nVisibleCount;
	}
	return nVisibleCount;
}


int cGridListBox::GetShowLineStartPos() const
{
	return m_nShowLineStartPos;
}

int cGridListBox::GetItemExtendSize() const
{
	return m_nItemExtendSize;
}

void cGridListBox::ResetItemExtendSize()
{
	m_nItemExtendSize = 0;
	AutoSetExtendSize();
}

bool VisibleComp(cGridListBoxItem* first, cGridListBoxItem* second)
{
	//first->GetVisible();
	if (first->GetVisible() == true && second->GetVisible() == false)
		return true;
// 	else if ( first->GetVisible() == false && second->GetVisible() == true)
// 		return false;
// 	else if (first->GetVisible() == true && second->GetVisible() == true)
// 		return false;
	else
		return false;
}

void cGridListBox::VisibleSort()
{
	m_listItems.sort(VisibleComp);
	RecountItemID(0);
	configureScrollbars();
}

std::list<cGridListBoxItem*>* cGridListBox::GetListItem()
{
	return &m_listItems;
}
void cGridListBox::SetMargin(CsPoint ptMargin)
{
	m_ptMargin = ptMargin;
}

CsPoint cGridListBox::GetMargin()
{
	return m_ptMargin;
}

void cGridListBox::SetVisibleItemCount( int const& nCount )
{ 
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( n < nCount )
			(*it)->SetVisible(true);
		else
			(*it)->SetVisible(false);
	}
}

void cGridListBox::SetVisibleControl( bool bVsible, int const& nIdx )
{ 
	std::list<cGridListBoxItem*>::iterator it = m_listItems.begin();
	for( int n = 0; it != m_listItems.end(); ++it, ++n )
	{
		if( (*it)->getID() != nIdx )
			continue;
		
		(*it)->SetVisible(bVsible);
		if( !bVsible )
		{
			if( m_pSelectedItem && m_pSelectedItem == (*it) )
				releaseSelection();
		}
		break;
	}
}

void cGridListBox::SetVisibleControl( bool bVsible, cGridListBoxItem const * pControl )
{ 
	SAFE_POINTER_RET( pControl );

	pControl->SetVisible( bVsible );
	if( m_pSelectedItem && m_pSelectedItem == pControl )
		releaseSelection();
}

void cGridListBox::SetAutoSelection( bool bValue )
{
	 m_bAutoSelection = bValue;
	 if( !m_bAutoSelection )
		 releaseSelection();
}

void cGridListBox::SetEnable( bool bValue )
{
	m_bEnable = bValue;
}