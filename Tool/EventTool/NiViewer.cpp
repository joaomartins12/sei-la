
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
	
	g_nScreenWidth = rc.right - rc.left;
	g_nScreenHeight = rc.bottom - rc.top;

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
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void NiViewer::OnRButtonDown(UINT nFlags, CPoint point)
{
	g_Camera.OnRButtonDown( point );
	SetCapture();

	CView::OnRButtonDown(nFlags, point);
}

void NiViewer::OnRButtonUp(UINT nFlags, CPoint point)
{
	g_Camera.OnRButtonUp( point );
	ReleaseCapture();

	CView::OnRButtonUp(nFlags, point);
}

void NiViewer::OnMouseMove(UINT nFlags, CPoint point)
{
	g_Camera.OnMouseMove( point );

	if( m_bClickAxis )
	{
		CPoint ptCursor;
		GetCursorPos( &ptCursor );
		
		switch( m_eAR )
		{
		case AR_X:				
			g_EventScene.m_vEffectPoint.z +=  (m_ptPos.y - ptCursor.y) * 2.5f;				
			break;
		case AR_Y:				
			g_EventScene.m_vEffectPoint.x +=  (m_ptPos.x - ptCursor.x) * 2.5f;								
			break;
		case AR_Z:				
			g_EventScene.m_vEffectPoint.y +=  (ptCursor.y - m_ptPos.y) * 2.5f;								
			break;
		}			
		m_ptPos = ptCursor;
		return;		
	}

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( g_Camera.GetCameraObj(), point.x, point.y );		
	if( pRecord == NULL )
		return;

	if( g_pMainWnd->_GetDlgController()->_GetNewEvent() )
	{
		g_pMainWnd->_GetDlgController()->_GetTarget()->SetPos( pRecord->GetIntersection() );			
	}
	else if( g_pMainWnd->_GetDlgController()->m_bEffectPos )
	{
		g_EventScene.m_vEffectPoint = pRecord->GetIntersection();
	}
	else if( g_pMainWnd->_GetDlgController()->m_bMoveEvent )
	{
		g_EventScene.SetMovePoint( pRecord->GetIntersection() );	
	}
	else if( g_pMainWnd->_GetDlgController()->m_bArea )
	{	
		NiPoint3 vpos = pRecord->GetIntersection();
		vpos.z += g_pMainWnd->_GetDlgController()->_GetAreaRad() / 7;

		g_EventScene.SetAreaPoint( vpos );		
	}
	else if( g_pMainWnd->_GetDlgController()->m_bCharCamera )
	{
		g_EventScene.SetMovePoint( pRecord->GetIntersection() );
	}

	CView::OnMouseMove(nFlags, point);
}

BOOL NiViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	g_Camera.OnMouseWheel( zDelta );

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void NiViewer::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if( g_pRenderer )
	{
		g_pRenderer->Recreate( cx, cy );

		g_Camera.ChangeStandardFov( (float)cx, (float)cy );
	}
}

void NiViewer::OnMButtonDown(UINT nFlags, CPoint point)
{
	g_Camera.OnMButtonDown( point );	

	SetCapture();

	CView::OnMButtonDown(nFlags, point);
}

void NiViewer::OnMButtonUp(UINT nFlags, CPoint point)
{
	g_Camera.OnMButtonUp( point );

	ReleaseCapture();

	CView::OnMButtonUp(nFlags, point);
}

void NiViewer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	NiMaterialProperty* pMaterialProp;
	pMaterialProp = (NiMaterialProperty*)g_EventScene.m_pAxisNiNode.m_pNiNode->GetAt( 3 )->GetProperty( NiMaterialProperty::GetType() );
	pMaterialProp->SetEmittance( NiColor( 0.0f, 1.0f, 0.0f ) );
	pMaterialProp = (NiMaterialProperty*)g_EventScene.m_pAxisNiNode.m_pNiNode->GetAt( 4 )->GetProperty( NiMaterialProperty::GetType() );
	pMaterialProp->SetEmittance( NiColor( 1.0f, 0.0f, 0.0f ) );
	pMaterialProp = (NiMaterialProperty*)g_EventScene.m_pAxisNiNode.m_pNiNode->GetAt( 5 )->GetProperty( NiMaterialProperty::GetType() );
	pMaterialProp->SetEmittance( NiColor( 0.0f, 0.0f, 1.0f ) );

	m_bClickAxis = false;

	ReleaseCapture();
	
	CView::OnLButtonUp(nFlags, point);
}

void NiViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	CsC_AvObject* pMoveObj = g_pMainWnd->_GetDlgController()->_GetTarget();	

	NiPoint3 origin, dir;
	g_Camera.WindowPointToRay( point.x, point.y, origin, dir );

	CsGBPick pick;

	if( pick.Pick_IsBound( g_EventScene.m_pAxisNiNode.m_pNiNode, origin, dir ) == true )
	{
		for( int i=0; i<=2; ++i )
		{
			pick.Pick( g_EventScene.m_pAxisNiNode.m_pNiNode->GetAt( i ), origin, dir, true );
		}
		pick.Pick( g_EventScene.m_pAxisPlaneNiNode.m_pNiNode, origin, dir, true );

		NiPick::Record* pRecord = pick.GetRecord();
		if( pRecord != NULL )
		{
			if( nsCSGBFUNC::GetRootNiNode( pRecord->GetAVObject() ) != g_EventScene.m_pAxisPlaneNiNode.m_pNiNode )
			{
				NiAVObject* pObject = _PickAxisToRenderAxis( pRecord->GetAVObject() );
				NiMaterialProperty* pMaterialProp = (NiMaterialProperty*)pObject->GetProperty( NiMaterialProperty::GetType() );
				_SetEnumAxisRotation( pMaterialProp->GetEmittance() );
				pMaterialProp->SetEmittance( NiColor( 1.0f, 1.0f, 0.0f ) );
				m_bClickAxis = true;
				m_ptPos = point;
			}			
		}
	}

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( g_Camera.GetCameraObj(), point.x, point.y );	

	if( pRecord == NULL )
		return;

	// 캐릭터 카메라모드 이동
	if( g_pMainWnd->_GetDlgController()->_IsChrCamera() )
	{	
		if( g_EventScene.GetCameraTarget() != NULL )
		{
			if( g_pCsPathEngine->PathTest( g_EventScene.GetCameraTarget()->GetProp_Path()->GetPath(), pRecord->GetIntersection().x, pRecord->GetIntersection().y ) == false )
				return;

			g_EventScene.GetCameraTarget()->GetProp_Path()->SetPath( pRecord->GetIntersection() );		
			return;
		}
	}

	//신규 이벤트 초기 좌표 설정
	if( g_pMainWnd->_GetDlgController()->_GetNewEvent() )
	{	
		g_pMainWnd->_GetDlgController()->_SetNewEvent( false );
		g_pMainWnd->_GetDlgController()->_SetNewRot( pMoveObj->GetCurRot() );
		g_pMainWnd->_GetDlgController()->_SetNewPos( pMoveObj->GetPos() );
		pMoveObj->GetProp_Animation()->SetStopFrame( false );

		g_EventScene.ReleaseMovePoint();		
		g_pMainWnd->_GetDlgController()->m_bMouseMove = false;		
	}
	//이펙트 위치 설정
	else if( g_pMainWnd->_GetDlgController()->m_bEffectPos )
	{	
		g_pMainWnd->_GetDlgController()->_SetEffectPos( pRecord->GetIntersection() );	
		g_pMainWnd->_GetDlgController()->m_bMouseMove = false;
	}	
	// 위치이동 이벤트 이동 위치설정
	else if( g_pMainWnd->_GetDlgController()->m_bMoveEvent )
	{
		g_EventScene.SetMovePoint( pRecord->GetIntersection() );		
		g_pMainWnd->_GetDlgController()->_SetMoveEvent( pRecord->GetIntersection() );
		g_pMainWnd->_GetDlgController()->m_bMoveEvent = false;
		g_pMainWnd->_GetDlgController()->m_bMouseMove = false;	
	}
	// 영역 설정 위치
	else if( g_pMainWnd->_GetDlgController()->m_bArea )
	{		
		NiPoint3 vpos = pRecord->GetIntersection();
		vpos.z += g_pMainWnd->_GetDlgController()->_GetAreaRad() / 7;

		g_EventScene.SetAreaPoint( vpos );
		g_pMainWnd->_GetDlgController()->m_bArea = false;	
		g_pMainWnd->_GetDlgController()->m_bMouseMove = false;
	}
	// 캐릭터 카메라 대상 위치
	else if( g_pMainWnd->_GetDlgController()->m_bCharCamera )
	{		
		NiPoint3 vpos = pRecord->GetIntersection();
		g_EventScene.SetCameraTargetPos( vpos );
		g_EventScene.ResetCharCamera();

		g_pMainWnd->_GetDlgController()->m_bCharCamera = false;	
		g_pMainWnd->_GetDlgController()->m_bMouseMove = false;

		g_pMainWnd->_GetDlgController()->_SetStartCharEvent();
	}

	CView::OnLButtonDown(nFlags, point);
}

void NiViewer::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.		

	CView::OnChar(nChar, nRepCnt, nFlags);
}

void NiViewer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.	

	switch( nChar )
	{
	case 'A': case 'a':				
		break;
	case 'D': case 'd':
		//g_Camera._SetDeltaTranslate_OnCamera( NiPoint3( 0.0f, -100.0f, 0.0f) );
		break;	
	case 'W': case 'w':
		//g_Camera.SetDeltaDist( -100 );
		break;
	case 'S': case 's':
		//g_Camera.SetDeltaDist( 100 );
		break;
	case VK_LEFT:
		if( g_pMainWnd->_GetDlgController()->_GetTarget() )
			g_pMainWnd->_GetDlgController()->_GetTarget()->SetRotation_AniDelta( -0.1f );
		break;
	case VK_RIGHT:
		if( g_pMainWnd->_GetDlgController()->_GetTarget() )
			g_pMainWnd->_GetDlgController()->_GetTarget()->SetRotation_AniDelta( 0.1f );
		break;
	case VK_UP:
		if( g_pMainWnd->_GetDlgController()->_GetTarget() )
			g_pMainWnd->_GetDlgController()->_GetTarget()->SetRotation_AniDelta( -0.02f );
		break;
	case VK_DOWN:
		if( g_pMainWnd->_GetDlgController()->_GetTarget() )
			g_pMainWnd->_GetDlgController()->_GetTarget()->SetRotation_AniDelta( 0.02f );
		break;

	}	

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void NiViewer::_SetEnumAxisRotation( NiColor emittance )
{
	if( emittance == NiColor( 0.0f, 1.0f, 0.0f ) )
	{
		m_eAR = AR_Z;
	}
	else if( emittance == NiColor( 1.0f, 0.0f, 0.0f ) )
	{
		m_eAR = AR_Y;
	}
	else if( emittance == NiColor( 0.0f, 0.0f, 1.0f ) )
	{
		m_eAR = AR_X;
	}
	else
	{
		assert_cs( false )
	}
}

NiAVObject* NiViewer::_PickAxisToRenderAxis( NiAVObject* pPickAxis )
{
	for( int i=0; i<=2; ++i )
	{
		if( g_EventScene.m_pAxisNiNode.m_pNiNode->GetAt( i ) == pPickAxis )
			return g_EventScene.m_pAxisNiNode.m_pNiNode->GetAt( i + 3 );
	}
	assert_cs( false );
	return NULL;
}