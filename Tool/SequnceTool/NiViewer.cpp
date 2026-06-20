
#include "stdafx.h"
#include "NiViewer.h"


IMPLEMENT_DYNCREATE(NiViewer, CView)

NiViewer::NiViewer()
{

}
void NiViewer::OnDraw(CDC* /*pDC*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void NiViewer::_Update()
{
	POINT pt;
	GetCursorPos( &pt );
	ScreenToClient( &pt );
	
	CRect rc;
	GetClientRect( &rc );

	if( PtInRect( &rc, pt ) == TRUE )
		SetFocus();
}

BEGIN_MESSAGE_MAP(NiViewer, CView)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void NiViewer::OnRButtonDown(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnRButtonDown( point );
	SetCapture();

	CView::OnRButtonDown(nFlags, point);
}

void NiViewer::OnRButtonUp(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnRButtonUp( point );
	ReleaseCapture();

	CView::OnRButtonUp(nFlags, point);
}

void NiViewer::OnMouseMove(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnMouseMove( point );

	CView::OnMouseMove(nFlags, point);
}

BOOL NiViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( ::GetKeyState( VK_CONTROL ) & 0x01 )
		zDelta -= 110;

	CAMERA_ST.OnMouseWheel( zDelta );

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void NiViewer::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if( g_pRenderer )
	{
		g_pRenderer->Recreate( cx, cy );

		CAMERA_ST.ChangeStandardFov( (float)cx, (float)cy );
	}
}

void NiViewer::OnMButtonDown(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnMButtonDown( point );

	SetCapture();

	CView::OnMButtonDown(nFlags, point);
}

void NiViewer::OnMButtonUp(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnMButtonUp( point );

	ReleaseCapture();

	CView::OnMButtonUp(nFlags, point);
}

void NiViewer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CsC_AvObject* pTarget = NULL;
	switch(nChar)
	{
	case VK_LEFT:
		pTarget = g_pMainWnd->_GetDlgAnimation()->_GetTarget();
		if( pTarget != NULL )
		{
			pTarget->SetRotation( pTarget->GetCurRot() - 0.2f, true );			
		}
		break;
	case VK_RIGHT:
		pTarget = g_pMainWnd->_GetDlgAnimation()->_GetTarget();
		if( pTarget != NULL )
		{
			pTarget->SetRotation( pTarget->GetCurRot() + 0.2f, true );			
		}
		break;
	case VK_ADD:
		{
			pTarget = g_pMainWnd->_GetDlgAnimation()->_GetTarget();
			if( pTarget->GetModelID() / 10000 != 8 )
				break;

			static DWORD nCostumeID = 0;

			CsItem::MAP* pMap = nsCsFileTable::g_pItemMng->GetItemMap();
			CsItem::MAP_IT it = pMap->begin();
			CsItem::MAP_IT itEnd = pMap->end();
			if( nCostumeID != 0 )
			{
				it = pMap->find( nCostumeID );
				++it;
			}

			bool bFind = false;
			for( ; it!=itEnd; ++it )
			{
				if( it->second->GetInfo()->s_nType_L == 26 )
				{
					bFind = true;
					nCostumeID = it->second->GetInfo()->s_dwItemID;

					CsC_PartObject::sCHANGE_PART_INFO cp;
					cp.s_nFileTableID = nCostumeID;
					cp.s_nPartIndex = nsPART::Costume;
					( (CsC_PartObject*)pTarget )->ChangePart( &cp );
					break;
				}
			}

			if( bFind == false )
			{
				CsC_PartObject::sCHANGE_PART_INFO cp;
				cp.s_nFileTableID = 0;
				cp.s_nPartIndex = nsPART::Costume;
				( (CsC_PartObject*)pTarget )->ChangePart( &cp );
				nCostumeID = 0;
			}			
		}
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
