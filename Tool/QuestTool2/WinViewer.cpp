
#include "stdafx.h"
#include "WinViewer.h"
#define CHECK_TIME	111
WinViewer*		g_pViewer = NULL;

bool	ResetDevice(bool bBeforeReset, void* pvData)
{
	if( bBeforeReset == true )
	{
		if( g_pFont )
			g_pFont->Release();
	}
	else
	{
		if( g_pFont )
#ifdef VERSION_TH
			g_pFont->Init( ".\\Data\\Font\\tahoma.ttf" );
#elif VERSION_TW
			g_pFont->Init( ".\\Data\\Font\\msjh.ttf" );
#elif VERSION_HK
			g_pFont->Init( ".\\Data\\Font\\msjh.ttf" );
#elif VERSION_USA
			g_pFont->Init( ".\\Data\\Font\\tahoma.ttf" );
#else
			g_pFont->Init( ".\\Data\\Font\\NanumGothicBold.ttf" );
#endif	

		if( g_pViewer )
			g_pViewer->_ResetDevice();

		g_pTop->InvalidateRect( NULL );
	}
	return true;
}


NiDX9RendererPtr g_pRenderer = NULL;
IMPLEMENT_DYNCREATE(WinViewer, CView)

WinViewer::WinViewer()
{

}

void WinViewer::OnDraw(CDC* /*pDC*/)
{	
}

void WinViewer::_SetViewState( eVIEWSTATE eState )
{
	_SetClickState( CS_SELECT );
	m_eViewState = eState;

	switch( m_eViewState )
	{
	case VS_VIEWER:
		m_dlgList.ShowWindow( SW_HIDE );
		m_dlgSimple.ShowWindow( SW_HIDE );
		break;
	case VS_LIST:
		m_dlgList.ShowWindow( SW_SHOW );
		m_dlgSimple.ShowWindow( SW_HIDE );
		break;
	case VS_SIMPLE:
		m_dlgList.ShowWindow( SW_HIDE );
		m_dlgSimple.ShowWindow( SW_SHOW );
		break;
	default:
		assert_cs( false );
	}
}

void WinViewer::_Init()
{
	m_hCursor[ CS_SELECT ] = LoadCursor( NULL, IDC_ARROW );
	m_hCursor[ CS_LINK_PARENT ] = LoadCursor( theApp.m_hInstance, MAKEINTRESOURCE( IDC_CUR_LINK_PARENT ) );
	m_hCursor[ CS_LINK_CHILD ] = LoadCursor( theApp.m_hInstance, MAKEINTRESOURCE( IDC_CUR_LINK_CHILD ) );
	m_hCursor[ CS_MOVE ] = LoadCursor( theApp.m_hInstance, MAKEINTRESOURCE( IDC_CUR_MOVE ) );
	m_hCursor[ CS_COPY ] = LoadCursor( theApp.m_hInstance, MAKEINTRESOURCE( IDC_CUR_COPY ) );

	srand( GetTickCount() );

	NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(
#ifdef NI_MEMORY_DEBUGGER
		NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(),	false)
#else
		NiExternalNew NiStandardAllocator()
#endif
		);
	NiInit(pkInitOptions);

	NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);	
	NiTexture::SetMipmapByDefault(false);
	NiSourceTexture::SetUseMipmapping(false);
	NiMaterial::SetDefaultWorkingDirectory(".\\Data\\Shaders\\Generated");	
	NiSourceTexture::SetDestroyAppDataFlag( false );

	g_pRenderer = NiDX9Renderer::Create( g_ptScreen.x, g_ptScreen.y,
		NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL, GetSafeHwnd(), GetSafeHwnd(), 0,
		NiDX9Renderer::DEVDESC_PURE, NiDX9Renderer::FBFMT_UNKNOWN, NiDX9Renderer::DSFMT_D24S8, NiDX9Renderer::PRESENT_INTERVAL_ONE,
		NiDX9Renderer::SWAPEFFECT_COPY,	NiDX9Renderer::FBMODE_MULTISAMPLES_2, 1, NiDX9Renderer::REFRESHRATE_DEFAULT);

	g_pRenderer->SetBackgroundColor( NiColor( 0.5f, 0.5f, 0.5f) );
	g_pRenderer->AddResetNotificationFunc( ::ResetDevice, NULL );

	//전체 이벤트 데이터 관리
	g_pEventDataMng = NiNew CEventDataMng;
	g_pEventDataMng->Init();

	char cShader[ MAX_PATH ] = ".\\Data\\Shaders";
	char* apcShaderDirectories[1];
	apcShaderDirectories[0] = cShader;
	m_ShaderHelper.SetupShaderSystem( NULL, 0, apcShaderDirectories, 1 );


	cStringAnalysis::GlobalInit();
	cFont::GlobalInit();
	cSprite::GlobalInit();

	m_Minimap.Init();
	m_NodeMng.Init();
	m_QuestMng.Init();	

	m_bMoveMouseDown = false;
	m_bMinimapPress = false;
	m_ptCurPos = CsPoint::ZERO;
	m_vCurPos_Float = NiPoint2::ZERO;
	m_dwMovePlag = KM_NONE;

	_SetClickState( CS_SELECT );



	m_eViewState = VS_VIEWER;
	CRect rc;
	GetWindowRect( &rc );
	// 윈도우 연결
	m_dlgList.Create( _DlgViewList::IDD, this );
	m_dlgList.SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(), 0 );
	m_dlgList._Init();
	m_dlgList.ShowWindow( SW_HIDE );

	m_dlgSimple.Create( _DlgViewSimple::IDD, this );
	m_dlgSimple.SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(), 0 );
	m_dlgSimple._Init();
	m_dlgSimple.ShowWindow( SW_HIDE );
}

void WinViewer::_Delete()
{
	g_pEventDataMng->Delete();
	SAFE_NIDELETE( g_pEventDataMng );

	m_Minimap.Delete();
	m_NodeMng.Delete();
	m_QuestMng.Delete();

	cFont::GlobalRelease();
	cSprite::GlobalShotDown();
	cStringAnalysis::GlobalShotDown();

	g_pRenderer = 0;
	NiShutdown();

	const NiInitOptions* pkInitOptions = NiStaticDataManager::GetInitOptions();
	NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
	NiExternalDelete pkInitOptions;
	NiExternalDelete pkAllocator;
}

bool WinViewer::__BeginRender()
{
	if( g_pRenderer->IsDeviceLost() == true )
	{
		g_pRenderer->Recreate();
		return false;
	}

	if( g_pRenderer->BeginFrame() == false )
	{
		assert_cs( false );
	}

	bool bSuccess = g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
	assert_cs( bSuccess );
	g_pRenderer->SetScreenSpaceCameraData();
	return true;
}

bool WinViewer::__EndRender()
{
	g_pRenderer->EndUsingRenderTargetGroup();

	if( g_pRenderer->EndFrame() == false )
	{
		assert_cs( false );
		return false;
	}
	if( g_pRenderer->DisplayFrame() == false )
	{
		assert_cs( false );
		return false;
	}
	return true;
}

void WinViewer::_Update()
{
	switch( m_eViewState )
	{
	case VS_VIEWER:
		{
			static int nTick = GetTickCount();
			bool bUpdate = false;
			if( GetTickCount() - nTick > CHECK_TIME )
			{
				nTick = GetTickCount();
				bUpdate = true;
			}

			if( bUpdate )
			{
				POINT pt;
				GetCursorPos( &pt );
				ScreenToClient( &pt );	

				CRect rc;
				GetClientRect( &rc );

				if( PtInRect( &rc, pt ) == TRUE )
				{
					m_ptCursorScreen = CsPoint( pt.x, pt.y );
					m_ptCursor = m_ptCursorScreen + m_ptCurPos;
					m_NodeMng.UpdateCursor( m_ptCursor );
				}

				__UpdateMove();
				m_NodeMng.Update();
			}
		}
		break;
	case VS_SIMPLE:
	case VS_LIST:
		break;	
	default:
		assert_cs( false );
	}	
}

void WinViewer::__UpdateMove()
{
	if( m_dwMovePlag == KM_NONE )
		return;

#define MOVE_SPEED		1000
	if( m_dwMovePlag & KM_LEFT )
	{
		m_vCurPos_Float.x -= g_fDeltaTime*MOVE_SPEED;
	}
	if( m_dwMovePlag & KM_UP )
	{
		m_vCurPos_Float.y -= g_fDeltaTime*MOVE_SPEED;
	}
	if( m_dwMovePlag & KM_RIGHT )
	{
		m_vCurPos_Float.x += g_fDeltaTime*MOVE_SPEED;
	}
	if( m_dwMovePlag & KM_DOWN )
	{
		m_vCurPos_Float.y += g_fDeltaTime*MOVE_SPEED;
	}

	m_ptCurPos.x = (int)m_vCurPos_Float.x;
	m_ptCurPos.y = (int)m_vCurPos_Float.y;

	__CalMove();
}

void WinViewer::_Render()
{
	switch( m_eViewState )
	{
	case VS_VIEWER:
		{
			static int nTick = GetTickCount();
			bool bRend = false;
			if( GetTickCount() - nTick > CHECK_TIME )
			{
				nTick = GetTickCount();
				bRend = true;
			}

			if( bRend )
			{
				if( __BeginRender() == false )
					return;

				// 랜더 구문
				m_NodeMng.Render( m_ptCurPos );
				m_Minimap.Render();

				__EndRender();
			}
		}
		break;
	case VS_LIST:
	case VS_SIMPLE:
		break;
	default:
		assert_cs( false );
	}
}


void WinViewer::_ResetDevice()
{
	m_NodeMng.ResetDevice();
	m_Minimap.ResetDevice();
}



BEGIN_MESSAGE_MAP(WinViewer, CView)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void WinViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	if( m_Minimap.OnMouseDown( point ) == true )
	{
		m_bMinimapPress = true;
		m_Minimap.OnLBtnPress( point );
	}
	else
	{
		CsPoint ptClick = CsPoint( point.x + m_ptCurPos.x, point.y + m_ptCurPos.y ) ;

		switch( m_eClickState )
		{
		case CS_SELECT:
			m_NodeMng.OnLBtnDown( ptClick );
			break;
		case CS_LINK_PARENT:
			{
				cNode* pSelectNode = m_NodeMng.FindNode( ptClick );
				CsPoint ptPitch = pSelectNode->CalPitch( ptClick );
				cQuestObj* pSelectObject = pSelectNode->GetQuest_FromPitch( ptPitch );
				if( pSelectObject != NULL )
				{
					if( pSelectObject != m_pPopupObject )
					{
						pSelectObject->AddChild( m_pPopupObject );
						m_pPopupObject->AddParent( pSelectObject );
						m_pPopupObject->GetFT()->AddPreRequire( CsQuestPreRequire::QUEST, pSelectObject->GetFT()->GetUniqID(), 1 );

						pSelectObject->ResetMiddleTail();
						m_pPopupObject->ResetMiddleTail();

						g_pDiscript->_SetSelQuest( m_pPopupObject );						

						_SetClickState( CS_SELECT );
					}
					else
					{
						CsMessageBox( MB_OK, L"같은 퀘스트는 부모가 될수 없습니다." );
					}					
				}
				else
				{
					CsMessageBox( MB_OK, L"퀘스트를 클릭 하셔야 합니다." );
				}
			}
			break;
		case CS_LINK_CHILD:
			{
				cNode* pSelectNode = m_NodeMng.FindNode( ptClick );
				CsPoint ptPitch = pSelectNode->CalPitch( ptClick );
				cQuestObj* pSelectObject = pSelectNode->GetQuest_FromPitch( ptPitch );
				if( pSelectObject != NULL )
				{
					if( pSelectObject != m_pPopupObject )
					{
						pSelectObject->AddParent( m_pPopupObject );
						m_pPopupObject->AddChild( pSelectObject );
						pSelectObject->GetFT()->AddPreRequire( CsQuestPreRequire::QUEST, m_pPopupObject->GetFT()->GetUniqID(), 1 );

						pSelectObject->ResetMiddleTail();
						m_pPopupObject->ResetMiddleTail();

						g_pDiscript->_SetSelQuest( pSelectObject );

						_SetClickState( CS_SELECT );
					}
					else
					{
						CsMessageBox( MB_OK, L"같은 퀘스트는 자식이 될수 없습니다." );
					}					
				}
				else
				{
					CsMessageBox( MB_OK, L"퀘스트를 클릭 하셔야 합니다." );
				}
			}
			break;
		case CS_MOVE:
			{
				cNode* pSelectNode = m_NodeMng.FindNode( ptClick );
				CsPoint ptPitch = pSelectNode->CalPitch( ptClick );
				cQuestObj* pSelectObject = pSelectNode->GetQuest_FromPitch( ptPitch );
				if( pSelectNode->GetNodeIndex() != m_pPopupObject->GetNodeIndex() )				
				{
					CsMessageBox( MB_OK, L"같은 노드로 움직이셔야 합니다." );					
				}
				else if( pSelectObject != NULL )
				{
					if( pSelectObject != m_pPopupObject )
					{
						CsPoint ptPitch = pSelectObject->GetFT()->GetToolPos();
						pSelectObject->GetFT()->SetToolPos( m_pPopupObject->GetFT()->GetToolPos() );
						m_pPopupObject->GetFT()->SetToolPos( ptPitch );

						// 부모들 미들 리셋, 자식들 미들리셋 본인 미들리셋
						pSelectObject->ResetMiddleTail_ParentChild();
						pSelectObject->ResetMiddleTail();
						m_pPopupObject->ResetMiddleTail_ParentChild();
						m_pPopupObject->ResetMiddleTail();

						pSelectNode->ResetNodeSize();
						m_NodeMng.ResetSize();
					}
					_SetClickState( CS_SELECT );
				}
				else
				{
					m_pPopupObject->GetFT()->SetToolPos( ptPitch );

					// 부모들 미들 리셋, 자식들 미들리셋 본인 미들리셋
					m_pPopupObject->ResetMiddleTail_ParentChild();
					m_pPopupObject->ResetMiddleTail();

					pSelectNode->ResetNodeSize();
					m_NodeMng.ResetSize();

					_SetClickState( CS_SELECT );
				}
			}
			break;
		case CS_COPY:
			{
				cNode* pSelectNode = m_NodeMng.FindNode( ptClick );
				CsPoint ptPitch = pSelectNode->CalPitch( ptClick );
				cQuestObj* pSelectObject = pSelectNode->GetQuest_FromPitch( ptPitch );
				if( pSelectNode->GetNodeIndex() != m_pPopupObject->GetNodeIndex() )				
				{
					CsMessageBox( MB_OK, L"같은 노드로 움직이셔야 합니다." );
				}
				else if( pSelectObject != NULL )
				{
					CsMessageBox( MB_OK, L"복사는 빈 패치로만 가능합니다." );
				}
				else
				{
					CsQuest* pNewFT = nsCsFileTable::g_pQuestMng->AddQuest( m_pPopupObject->GetFT(), nsCsFileTable::g_pQuestMng->GetLastUniqID() );
					pNewFT->SetToolPos( ptPitch );
					cQuestObj* pObj = m_QuestMng.AddQuest( pNewFT );
					pObj->ResetMiddleTail_ParentChild();

					g_pDiscript->_SetSelQuest( pObj );
					_SetClickState( CS_SELECT );
				}
			}
			break;
		default:
			assert_cs( false );
		}
	}

	CView::OnLButtonDown(nFlags, point);
}

void WinViewer::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	m_bMinimapPress = false;

	CView::OnLButtonUp(nFlags, point);
}


void WinViewer::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	if( m_Minimap.OnMouseDown( point ) == true )
	{

	}
	else
	{

	}

	CView::OnRButtonDown(nFlags, point);
}

void WinViewer::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	CView::OnRButtonUp(nFlags, point);
}


void WinViewer::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_ptMoveMouse = CsPoint( point.x, point.y );
	m_bMoveMouseDown = true;

	SetCapture();
	CView::OnMButtonDown(nFlags, point);
}

void WinViewer::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_bMoveMouseDown = false;

	ReleaseCapture();

	CView::OnMButtonUp(nFlags, point);
}



void WinViewer::OnMouseMove(UINT nFlags, CPoint point)
{
	m_ptCursor = CsPoint( point.x, point.y );

	if( m_bMoveMouseDown == true )
	{
		CsPoint ptDelta;
		ptDelta.x = m_ptMoveMouse.x - point.x;
		ptDelta.y = m_ptMoveMouse.y - point.y;
		m_ptMoveMouse = CsPoint( point.x, point.y );

		m_ptCurPos += ptDelta;
		m_vCurPos_Float.x = (float)m_ptCurPos.x;
		m_vCurPos_Float.y = (float)m_ptCurPos.y;

		__CalMove();
	}
	else if( m_bMinimapPress == true )
	{
		m_Minimap.OnLBtnPress( point );
	}

	CView::OnMouseMove(nFlags, point);
}

void WinViewer::__CalMove()
{
	cNode* pLastNode = m_NodeMng.GetNode( m_NodeMng.GetNodeCount() - CsPoint( 1, 1 ) );
	if( m_ptCurPos.x < 0 )
	{
		m_ptCurPos.x = 0;
		m_vCurPos_Float.x = (float)m_ptCurPos.x;
	}
	if( m_ptCurPos.y < 0 )
	{
		m_ptCurPos.y = 0;
		m_vCurPos_Float.y = (float)m_ptCurPos.y;
	}


	int nTemp = pLastNode->GetNodePos().x + pLastNode->GetPitchCount().x*PITCH_SIZE_X;
	if( g_ptScreen.x < nTemp )
	{
		if( m_ptCurPos.x > nTemp - g_ptScreen.x )
		{
			m_ptCurPos.x = nTemp - g_ptScreen.x;
			m_vCurPos_Float.x = (float)m_ptCurPos.x;
		}
	}
	else
	{
		m_ptCurPos.x = 0;
		m_vCurPos_Float.x = (float)m_ptCurPos.x;
	}

	nTemp = pLastNode->GetNodePos().y + pLastNode->GetPitchCount().y*PITCH_SIZE_Y;
	if( g_ptScreen.y < nTemp )
	{
		if( m_ptCurPos.y > nTemp - g_ptScreen.y )
		{
			m_ptCurPos.y = nTemp - g_ptScreen.y;
			m_vCurPos_Float.y = (float)m_ptCurPos.y;
		}
	}
	else
	{
		m_ptCurPos.y = 0;
		m_vCurPos_Float.y = (float)m_ptCurPos.y;
	}
}

void WinViewer::_SetCurPos( CsPoint ptPos )
{
	m_ptCurPos = ptPos;
	m_vCurPos_Float.x = (float)m_ptCurPos.x;
	m_vCurPos_Float.y = (float)m_ptCurPos.y;

	__CalMove();
}

void WinViewer::_SetSelectCenter()
{
	if( g_pDiscript->_GetSelQuest() == NULL )
		return;

	if( m_eViewState == VS_LIST )
		return;

	_Update();
	CsPoint ptNodeIndex = g_pDiscript->_GetSelQuest()->GetNodeIndex();
	CsPoint ptNodePos = m_NodeMng.GetNode( ptNodeIndex )->GetNodePos() - g_ptScreen/2 + g_pDiscript->_GetSelQuest()->GetPitchPos();
	ptNodePos.x += PITCH_SIZE_X/2;
	ptNodePos.y += 40;
	_SetCurPos( ptNodePos );
}

BOOL WinViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void WinViewer::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	g_ptScreen = CsPoint( cx, cy );
	if( g_pRenderer )
	{		
		g_pRenderer->Recreate( cx, cy );		
	}
}



BOOL WinViewer::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		m_pPopupObject = g_pDiscript->_GetSelQuest();
		
		if( ( IsCTRLPressed() == true ) && m_pPopupObject )
		{
			switch( pMsg->wParam )
			{
			case 'X':
				_SetClickState( CS_MOVE );
				return TRUE;
			case 'C':
				_SetClickState( CS_COPY );
				return TRUE;
			}
		}
	}

	return CView::PreTranslateMessage(pMsg);
}


void WinViewer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch( nChar )
	{
	case VK_LEFT:
		m_dwMovePlag |= KM_LEFT;
		break;
	case VK_UP:
		m_dwMovePlag |= KM_UP;
		break;
	case VK_RIGHT:
		m_dwMovePlag |= KM_RIGHT;
		break;
	case VK_DOWN:
		m_dwMovePlag |= KM_DOWN;
		break;
	case VK_ESCAPE:
		_SetClickState( CS_SELECT );
		g_pDiscript->_SetSelQuest( NULL );
		break;
	}		

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void WinViewer::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch( nChar )
	{
	case VK_LEFT:
		m_dwMovePlag &= ~KM_LEFT;
		break;
	case VK_UP:
		m_dwMovePlag &= ~KM_UP;
		break;
	case VK_RIGHT:
		m_dwMovePlag &= ~KM_RIGHT;
		break;
	case VK_DOWN:
		m_dwMovePlag &= ~KM_DOWN;
		break;
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void WinViewer::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	m_dwMovePlag = KM_NONE;
}


void WinViewer::OnContextMenu(CWnd* pWnd, CPoint point)
{
	_SetClickState( CS_SELECT );

	if( m_eViewState != VS_VIEWER )
		return;

	m_ptPopupPos = CsPoint( point.x, point.y ) + m_ptCurPos;
	ScreenToClient( (POINT*)&m_ptPopupPos );

	cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
	CsPoint ptPicth = pNode->CalPitch( m_ptPopupPos );
	m_pPopupObject = pNode->GetQuest_FromPitch( ptPicth );

	g_pDiscript->_SetSelQuest( m_pPopupObject );
	_Render();

	HMENU hMenu = LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE( IDR_MENU_TARGET ) );
	HMENU hPopup = GetSubMenu( hMenu, m_pPopupObject == NULL ? 0 : 1 );
	if( m_pPopupObject == NULL )
	{		
		// 행이 비여있는지 체크
		if( pNode->IsExistPitch_X( ptPicth ) == true )
		{
			EnableMenuItem( hPopup, ID_DELETE_COL, MF_GRAYED );
		}
		else
		{
			EnableMenuItem( hPopup, ID_DELETE_COL, MF_ENABLED );
		}

		// 열이 비여 있는지 체크
		if( pNode->IsExistPitch_Y( ptPicth ) == true )
		{
			EnableMenuItem( hPopup, ID_DELETE_ROW, MF_GRAYED );
		}
		else
		{
			EnableMenuItem( hPopup, ID_DELETE_ROW, MF_ENABLED );
		}
	}


	TrackPopupMenu( hPopup, TPM_LEFTALIGN, point.x, point.y, 0, GetSafeHwnd(), NULL );	

	DestroyMenu( hMenu );
}

BOOL WinViewer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD( wParam ) )
	{
		//=======================================================================================
		//
		//		논 타겟
		//
		//=======================================================================================
	case ID_CREATE_GLOBAL:
		{
			CsQuest* pFTQuest;
			_DlgModel_Create dlg( &pFTQuest );
			if( dlg.DoModal() == IDOK )
			{
				cQuestObj* pObj = m_QuestMng.AddQuest( pFTQuest );
				g_pDiscript->_SetSelQuest( pObj );
			}
		}
		break;
	case ID_CREATE_NODE_THIS:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );

			DWORD dwMapID = 0;
			if( nsCsMapTable::g_pMapListMng->GetListMap()->size() > (size_t)pNode->GetNodeIndex().y )
				dwMapID = nsCsMapTable::g_pMapListMng->GetListIndex( pNode->GetNodeIndex().y )->GetInfo()->s_dwMapID;

			CsQuest* pFTQuest;
			_DlgModel_Create dlg( &pFTQuest, dwMapID, pNode->GetNodeIndex().x + 1 );
			if( dlg.DoModal() == IDOK )
			{
				pFTQuest->SetToolPos( pNode->CalPitch( m_ptPopupPos ) );
				cQuestObj* pObj = m_QuestMng.AddQuest( pFTQuest );
				g_pDiscript->_SetSelQuest( pObj );
			}
		}
		break;
	case ID_CREATE_NODE_AUTO:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
			DWORD dwMapID = 0;
			if( nsCsMapTable::g_pMapListMng->GetListMap()->size() > (size_t)pNode->GetNodeIndex().y )
				dwMapID = nsCsMapTable::g_pMapListMng->GetListIndex( pNode->GetNodeIndex().y )->GetInfo()->s_dwMapID;

			CsQuest* pFTQuest;
			_DlgModel_Create dlg( &pFTQuest, dwMapID, pNode->GetNodeIndex().x + 1 );
			if( dlg.DoModal() == IDOK )
			{
				cQuestObj* pObj = m_QuestMng.AddQuest( pFTQuest );
				g_pDiscript->_SetSelQuest( pObj );
			}
		}
		break;
	case ID_CREATE_INSERT_BOTTOM:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
			DWORD dwMapID = 0;
			if( nsCsMapTable::g_pMapListMng->GetListMap()->size() > (size_t)pNode->GetNodeIndex().y )
				dwMapID = nsCsMapTable::g_pMapListMng->GetListIndex( pNode->GetNodeIndex().y )->GetInfo()->s_dwMapID;

			CsQuest* pFTQuest;
			_DlgModel_Create dlg( &pFTQuest, dwMapID, pNode->GetNodeIndex().x + 1 );
			if( dlg.DoModal() == IDOK )
			{
				CsPoint ptPitch = pNode->CalPitch( m_ptPopupPos );
				// 기존에 있는거는 한칸씩 y축으로 미루자
				pNode->IncreasePitch_Y( ptPitch.y );
				pFTQuest->SetToolPos( ptPitch );
				cQuestObj* pObj = m_QuestMng.AddQuest( pFTQuest );
				g_pDiscript->_SetSelQuest( pObj );
			}
		}
		break;
	case ID_CREATE_INSERT_RIGHT:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
			
			DWORD dwMapID = 0;
			if( nsCsMapTable::g_pMapListMng->GetListMap()->size() > (size_t)pNode->GetNodeIndex().y )
				dwMapID = nsCsMapTable::g_pMapListMng->GetListIndex( pNode->GetNodeIndex().y )->GetInfo()->s_dwMapID;

			CsQuest* pFTQuest;
			_DlgModel_Create dlg( &pFTQuest, dwMapID, pNode->GetNodeIndex().x + 1 );
			if( dlg.DoModal() == IDOK )
			{
				CsPoint ptPitch = pNode->CalPitch( m_ptPopupPos );
				// 기존에 있는거는 한칸씩 x축으로 미루자
				pNode->IncreasePitch_X( ptPitch.x );
				pFTQuest->SetToolPos( ptPitch );
				cQuestObj* pObj = m_QuestMng.AddQuest( pFTQuest );
				g_pDiscript->_SetSelQuest( pObj );
			}
		}
		break;
	case ID_CREATE_INSERT_UP:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
			DWORD dwMapID = 0;
			if( nsCsMapTable::g_pMapListMng->GetListMap()->size() > (size_t)pNode->GetNodeIndex().y )
				dwMapID = nsCsMapTable::g_pMapListMng->GetListIndex( pNode->GetNodeIndex().y )->GetInfo()->s_dwMapID;

			CsQuest* pFTQuest;
			_DlgModel_Create dlg( &pFTQuest, dwMapID, pNode->GetNodeIndex().x + 1 );
			if( dlg.DoModal() == IDOK )
			{
				CsPoint ptPitch = pNode->CalPitch( m_ptPopupPos );
				++ptPitch.y;
				// 기존에 있는거는 한칸씩 y축으로 미루자
				pNode->IncreasePitch_Y( ptPitch.y );
				pFTQuest->SetToolPos( ptPitch );
				cQuestObj* pObj = m_QuestMng.AddQuest( pFTQuest );
				g_pDiscript->_SetSelQuest( pObj );
			}
		}
		break;
	case ID_CREATE_INSERT_LEFT:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
			DWORD dwMapID = 0;
			if( nsCsMapTable::g_pMapListMng->GetListMap()->size() > (size_t)pNode->GetNodeIndex().y )
				dwMapID = nsCsMapTable::g_pMapListMng->GetListIndex( pNode->GetNodeIndex().y )->GetInfo()->s_dwMapID;

			CsQuest* pFTQuest;
			_DlgModel_Create dlg( &pFTQuest, dwMapID, pNode->GetNodeIndex().x + 1 );
			if( dlg.DoModal() == IDOK )
			{
				CsPoint ptPitch = pNode->CalPitch( m_ptPopupPos );
				++ptPitch.x;
				// 기존에 있는거는 한칸씩 x축으로 미루자
				pNode->IncreasePitch_X( ptPitch.x );
				pFTQuest->SetToolPos( ptPitch );
				cQuestObj* pObj = m_QuestMng.AddQuest( pFTQuest );
				g_pDiscript->_SetSelQuest( pObj );
			}
		}
		break;
	case ID_DELETE_COL:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
			CsPoint ptPitch = pNode->CalPitch( m_ptPopupPos );
			pNode->DecreasePitch_X( ptPitch.x );
			m_NodeMng.ResetSize_AllNode();
		}
		break;
	case ID_DELETE_ROW:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
			CsPoint ptPitch = pNode->CalPitch( m_ptPopupPos );
			pNode->DecreasePitch_Y( ptPitch.y );
			m_NodeMng.ResetSize_AllNode();
		}
		break;
	case ID_INSERT_COL:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
			CsPoint ptPitch = pNode->CalPitch( m_ptPopupPos );
			pNode->IncreasePitch_X( ptPitch.x );
			m_NodeMng.ResetSize_AllNode();
		}
		break;
	case ID_INSERT_ROW:
		{
			cNode* pNode = m_NodeMng.FindNode( m_ptPopupPos );
			CsPoint ptPitch = pNode->CalPitch( m_ptPopupPos );
			pNode->IncreasePitch_Y( ptPitch.y );
			m_NodeMng.ResetSize_AllNode();
		}
		break;

		//=======================================================================================
		//
		//		타겟
		//
		//=======================================================================================
	case ID_CREATE_CHILD:
		{
			assert_cs( m_pPopupObject != NULL );

			CsQuest* pFTQuest;
			_DlgModel_Create dlg( &pFTQuest );
			if( dlg.DoModal() == IDOK )
			{
				cQuestObj* pObject = NiNew cQuestObj;
				pObject->Init( pFTQuest );

				pObject->GetFT()->AddPreRequire( CsQuestPreRequire::QUEST, m_pPopupObject->GetFT()->GetUniqID(), 1 );
				pObject->AddParent( m_pPopupObject );
				m_pPopupObject->AddChild( pObject );

				m_QuestMng.AddQuest( pObject );
				
				pObject->ResetMiddleTail();
				m_pPopupObject->ResetMiddleTail();

				g_pDiscript->_SetSelQuest( pObject );								
			}
		}
		break;

	case ID_LINK_PARENT:
		{
			_SetClickState( CS_LINK_PARENT );
		}
		break;
	case ID_LINK_CHILD:
		{
			_SetClickState( CS_LINK_CHILD );
		}
		break;
	case ID_UNLINK_PARENT:
		{
			m_pPopupObject->UnlinkParent();
			g_pDiscript->_SetSelQuest( m_pPopupObject );
		}
		break;
	case ID_UNLINK_CHILD:
		{
			m_pPopupObject->UnlinkChild();			
		}
		break;
	case ID_MOVE:
		{
			_SetClickState( CS_MOVE );
		}
		break;
	case ID_COPY:
		{
			_SetClickState( CS_COPY );
		}
		break;
	case ID_DELETE:
		{
			CsQuest* pFTQuest = m_pPopupObject->GetFT();

			m_pPopupObject->UnlinkParent();
			m_pPopupObject->UnlinkChild();
			m_NodeMng.ReleaseQuestObj( m_pPopupObject, true );
			m_QuestMng.DeleteQuestObj( m_pPopupObject );
			nsCsFileTable::g_pQuestMng->DeleteQuest( pFTQuest->GetUniqID() );

			g_pDiscript->_SetSelQuest( NULL );
		}
		break;	
	}

	return CView::OnCommand(wParam, lParam);
}


void WinViewer::_SetClickState( eCLICK_STATE cs )
{
	m_eClickState = cs;

#pragma warning( disable : 4311 )
	SetClassLong( GetSafeHwnd(), GCL_HCURSOR, (LONG)m_hCursor[ m_eClickState ] );
	::SetCursor( m_hCursor[ m_eClickState ] );
#pragma warning( default : 4311 )

	SetFocus();
}
