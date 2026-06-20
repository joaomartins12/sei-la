// _DlgMTPlayer.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTPlayer.h"


// _DlgMTPlayer 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTPlayer, CDialog)

_DlgMTPlayer::_DlgMTPlayer(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTPlayer::IDD, pParent),
	m_fPlayerSpeed(1.0f)
{

}

_DlgMTPlayer::~_DlgMTPlayer()
{
}

// =================================================================================================
//
//		virtual
//
// =================================================================================================
void _DlgMTPlayer::_PreSave( const char* cFile, bool bAutoSave )
{
	// 패쓰엔젠 리셋
	OnBnClickedButtonApplyCollision();
}

void _DlgMTPlayer::_Delete()
{
	_DlgBase::_Delete();

	m_FogColor.Delete();
	m_BGColor.Delete();
}

void _DlgMTPlayer::_UpdateView()
{
}

void _DlgMTPlayer::_PreRenderView()
{
}

void _DlgMTPlayer::_OnLButtonDown( CPoint pt )
{
	if( m_bViewPlayer == false )
	{
		NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
		if( pRecord != NULL )
		{
			m_bViewPlayer = true;
			g_Actor.ResetVisible( pRecord->GetIntersection() );
		}
	}
	else
	{
		if( nsCsGBTerrain::g_pCurRoot->Pick_IsEnable( CAMERA_ST.GetCameraObj(), pt.x, pt.y ) )
		{
			NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
			if( pRecord != NULL )
			{
				g_Actor.SetMoveDest( pRecord->GetIntersection() );
			}
		}				
	}
}

void _DlgMTPlayer::__ResistSkyBoxPath()
{
	TCHAR sz[ MAX_PATH ];
	M2W( sz, nsCsGBTerrain::g_pCurRoot->GetSkyBox()->GetInfo()->s_cSkyBox, MAX_PATH );
	m_edit_SkyBoxPath.SetWindowText( sz );
}

void _DlgMTPlayer::__ChangeFogColor()
{
	m_FogColor.Create( &m_static_FogColor, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_FogColor );
}

void _DlgMTPlayer::__ChangeBGColor()
{
	m_BGColor.Create( &m_static_BGColor, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_BackGroundColor );
}

void _DlgMTPlayer::__ReadFogValue()
{
	m_sld_FogStart.SetPos( (int)( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fFogStart*0.01f ) );
	m_sld_FogEndObject.SetPos( (int)( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fFogEndObject*0.01f ) );
	m_sld_FogEndTerrain.SetPos( (int)( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fFogEndTerrain*0.01f ) );
	_Int2Control( &m_edit_FogStart, m_sld_FogStart.GetPos() );
	_Int2Control( &m_edit_FogEndObject, m_sld_FogEndObject.GetPos() );
	_Int2Control( &m_edit_FogEndTerrain, m_sld_FogEndTerrain.GetPos() );
}

void _DlgMTPlayer::__ReadClipValue()
{
	m_sld_ClipCell.SetPos( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nClipCellObject );
	m_sld_ClipObject.SetPos( (int)( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fClipObject_Far*0.01f ) );
	m_sld_ClipTerrain.SetPos( (int)( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fClipTerrain_Far*0.01f ) );
	_Int2Control( &m_edit_ClipCell, m_sld_ClipCell.GetPos() );
	_Int2Control( &m_edit_ClipObject, m_sld_ClipObject.GetPos() );
	_Int2Control( &m_edit_ClipTerrain, m_sld_ClipTerrain.GetPos() );
}

void _DlgMTPlayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SKYBOX_PATH, m_edit_SkyBoxPath);
	DDX_Control(pDX, IDC_STATIC_FOG_COLOR, m_static_FogColor);
	DDX_Control(pDX, IDC_SLD_FOG_START, m_sld_FogStart);
	DDX_Control(pDX, IDC_EDIT_FOG_START, m_edit_FogStart);
	DDX_Control(pDX, IDC_SLD_FOG_ENDOBJECT, m_sld_FogEndObject);
	DDX_Control(pDX, IDC_EDIT_FOG_ENDOBJECT, m_edit_FogEndObject);
	DDX_Control(pDX, IDC_SLD_FOG_ENDTERRAIN, m_sld_FogEndTerrain);
	DDX_Control(pDX, IDC_EDIT_FOG_ENDTERRAIN, m_edit_FogEndTerrain);
	DDX_Control(pDX, IDC_SLD_CLIP_CELL, m_sld_ClipCell);
	DDX_Control(pDX, IDC_EDIT_CLIP_CELL, m_edit_ClipCell);
	DDX_Control(pDX, IDC_SLD_CLIP_OBJECT, m_sld_ClipObject);
	DDX_Control(pDX, IDC_EDIT_CLIP_OBJECT, m_edit_ClipObject);
	DDX_Control(pDX, IDC_SLD_CLIP_TERRAIN, m_sld_ClipTerrain);
	DDX_Control(pDX, IDC_EDIT_CLIP_TERRAIN, m_edit_ClipTerrain);
	DDX_Control(pDX, IDC_EDIT_SKYBOX_HEIGHT, m_edit_SkyBoxHeight);
	DDX_Control(pDX, IDC_SPIN_SKYBOX_HEIGHT, m_spin_SkyBoxHeight);
	DDX_Control(pDX, IDC_STATIC_BG_COLOR, m_static_BGColor);
	DDX_Control(pDX, IDC_CHECK_SHOW_SKYBOX, m_chk_ShowSkyBox);
	DDX_Control(pDX, IDC_CHECK_VIEW_DIGIMON, m_chk_ViewDigimon);

	DDX_Control(pDX, IDC_SLIDER_RADIUS, m_PlayerMoveSpeed);

	
}


BEGIN_MESSAGE_MAP(_DlgMTPlayer, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_APPLY_COLLISION, &_DlgMTPlayer::OnBnClickedButtonApplyCollision)
	ON_BN_CLICKED(IDC_BTN_LOADSKYBOX, &_DlgMTPlayer::OnBnClickedBtnLoadskybox)
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_STATIC_FOG_COLOR, &_DlgMTPlayer::OnStnClickedStaticFogColor)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_SKYBOX_HEIGHT, &_DlgMTPlayer::OnEnChangeEditSkyboxHeight)
	ON_STN_CLICKED(IDC_STATIC_BG_COLOR, &_DlgMTPlayer::OnStnClickedStaticBgColor)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_RADIUS, &_DlgMTPlayer::OnNMReleasedcaptureSliderRadius)
END_MESSAGE_MAP()


// _DlgMTPlayer 메시지 처리기입니다.

void _DlgMTPlayer::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);
	m_bViewPlayer = false;

	if( bShow == TRUE )
	{
		__ResistSkyBoxPath();
		__ChangeFogColor();
		__ChangeBGColor();
		__ReadFogValue();
		__ReadClipValue();

		nsCsMfcFunc::Int2Control( &m_edit_SkyBoxHeight, (int)nsCsGBTerrain::g_pCurRoot->GetSkyBox()->GetInfo()->s_fHeight );
		OnBnClickedButtonApplyCollision();
	}
	else
	{
		g_Actor.ReleaseVisible();
		m_bViewPlayer = false;
	}
}

BOOL _DlgMTPlayer::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_spin_SkyBoxHeight.SetCsBuddy( &m_edit_SkyBoxHeight, 1 );	

	m_bViewPlayer = false;

	m_FogColor.Create( &m_static_FogColor, NiColor::BLACK );
	m_BGColor.Create( &m_static_BGColor, NiColor::BLACK );

	m_sld_FogStart.SetRange( 1, 999 );
	m_sld_FogEndObject.SetRange( 2, 1000 );
	m_sld_FogEndTerrain.SetRange( 2, 1000 );

	m_sld_ClipCell.SetRange( 1, 50 );
	m_sld_ClipObject.SetRange( 2, 1000 );
	m_sld_ClipTerrain.SetRange( 2, 1000 );

	m_chk_ViewDigimon.SetCheck( BST_CHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

HBRUSH _DlgMTPlayer::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = _DlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

	switch( pWnd->GetDlgCtrlID() )
	{
	case IDC_STATIC_FOG_COLOR:		hbr = m_FogColor.s_Brush;			break;
	case IDC_STATIC_BG_COLOR:		hbr = m_BGColor.s_Brush;			break;
	}
	return hbr;
}

void _DlgMTPlayer::OnBnClickedButtonApplyCollision()
{
	NiPoint3 ptPath = NiPoint3::ZERO;

	// 지형 등록
	int nErrorIndex = nsCsGBTerrain::g_pCurRoot->ApplyTerrainPath();	

	// 오브젝트 등록
	int nTotalLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	CsGBObject::sINFO* pObjectInfo = NULL;
	char cCsp[ MAX_PATH ];
	size_t nLen;

	NiTransform trVertPos;

	for( int i=0; i<nTotalLeafCount; ++i )
	{
		MAP_GBTERRAIN_OBJECT* pMapObject = nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMapObject->begin();
		for( ; it!=pMapObject->end(); ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;

			pObjectInfo = it->second->GetInfo();
			// ==== Object Path Engine 계산
			strcpy_s( cCsp, OBJECT_PATH_LEN, pObjectInfo->s_cObjectPath );
			nLen = strlen( cCsp );
			strcpy_s( &cCsp[ nLen - 3 ], MAX_PATH-(nLen-3), "csp" );
			if( _access_s( cCsp, 0 ) != 0 )
			{
				CsGBObj_Portal* pPortalObj = (CsGBObj_Portal*)it->second;
				if( pPortalObj->GetType() == CsGBObject::OT_PORTAL )
				{
					if( pObjectInfo->s_PathInfo.s_uiPEGroupCount != 0 )
					{
						pObjectInfo->s_PathInfo.Delete();
						pObjectInfo->s_PathInfo.Reset();
					}
					continue;
				}

				if( pObjectInfo->s_PathInfo.s_uiPEGroupCount == 0 )
				{
					pObjectInfo->s_PathInfo.Delete();
					pObjectInfo->s_PathInfo.Reset();
				}
				else
				{
					nsCsGBTerrain::g_ObjectResMgr.SetObjPath( pObjectInfo );

					int nOffset = 0;

					for( UINT i=0; i<pObjectInfo->s_PathInfo.s_uiPEGroupCount; ++i )
					{
						// ==== Object Path Engine 적용
						if( g_pCsPathEngine->SetShape( pObjectInfo->s_PathInfo.s_pptTranse[ i ].x,
							pObjectInfo->s_PathInfo.s_pptTranse[ i ].y,
							pObjectInfo->s_PathInfo.s_pPECount[ i ],
							&pObjectInfo->s_PathInfo.s_pData[ nOffset ] ) == false )
						{
							char cMsg[ 512 ];
							sprintf_s( cMsg, 512, "%s\n\n오브젝트의 PathEngine 선이 잘못 되었습니다.", pObjectInfo->s_cObjectPath );
							MessageBoxA( NULL, cMsg, NULL, MB_OK );
						}
						nOffset += pObjectInfo->s_PathInfo.s_pPECount[ i ]*2;
					}
				}
				continue;
			}

			FILE* fp;
			fopen_s( &fp, cCsp, "rb" );
			assert_cs( fp );

			DWORD dwVersion;
			fread( &dwVersion, sizeof( DWORD ), 1, fp );
			if( dwVersion == CSPATHENGINE_EXPORTER_VERSION )
			{
				trVertPos = pObjectInfo->s_trObject;
				trVertPos.m_Translate = NiPoint3::ZERO;

				// 그룹 갯수
				fread( &pObjectInfo->s_PathInfo.s_uiPEGroupCount, sizeof( UINT ), 1, fp );
				pObjectInfo->s_PathInfo.s_pPECount = csnew USHORT[ pObjectInfo->s_PathInfo.s_uiPEGroupCount ];
				pObjectInfo->s_PathInfo.s_pptTranse = NiNew NiPoint2[ pObjectInfo->s_PathInfo.s_uiPEGroupCount ];

				// 총 갯수
				fread( &pObjectInfo->s_PathInfo.s_uiTotalPECount, sizeof( UINT ), 1, fp );
				pObjectInfo->s_PathInfo.s_pData = csnew long[ 2*pObjectInfo->s_PathInfo.s_uiTotalPECount ];

				int nOffset = 0;
				NiPoint3 ptOffset;
				for( UINT i=0; i<pObjectInfo->s_PathInfo.s_uiPEGroupCount; ++i )
				{
					// 옵셋
					fread( &ptOffset, sizeof( float ), 2, fp );
					ptOffset.z = 0.0f;
					ptOffset = pObjectInfo->s_trObject.m_Rotate*ptOffset*pObjectInfo->s_trObject.m_fScale;

					// 위치값
					pObjectInfo->s_PathInfo.s_pptTranse[ i ].x = pObjectInfo->s_trObject.m_Translate.x + ptOffset.x;
					pObjectInfo->s_PathInfo.s_pptTranse[ i ].y = pObjectInfo->s_trObject.m_Translate.y + ptOffset.y;

					// 버택스 갯수				
					fread( &pObjectInfo->s_PathInfo.s_pPECount[ i ], sizeof( USHORT ), 1, fp );
					for( UINT uiPECount=0; uiPECount<pObjectInfo->s_PathInfo.s_pPECount[ i ]; ++uiPECount )
					{					
						fread( &ptPath, sizeof( float ), 2, fp );
						ptPath.z = 0.0f;
						ptPath = trVertPos*ptPath;

						pObjectInfo->s_PathInfo.s_pData[ nOffset + uiPECount*2 + 0 ] = (long)( ptPath.x + 0.5f );
						pObjectInfo->s_PathInfo.s_pData[ nOffset + uiPECount*2 + 1 ] = (long)( ptPath.y + 0.5f );
					}

					// ==== Object Path Engine 적용
					if( g_pCsPathEngine->SetShape(	pObjectInfo->s_PathInfo.s_pptTranse[ i ].x,
						pObjectInfo->s_PathInfo.s_pptTranse[ i ].y,
						pObjectInfo->s_PathInfo.s_pPECount[ i ],
						&pObjectInfo->s_PathInfo.s_pData[ nOffset ] ) == false )
					{
						char cMsg[ 512 ];
						sprintf_s( cMsg, 512, "%s\n\n오브젝트의 PathEngine 선이 잘못 되었습니다.", pObjectInfo->s_cObjectPath );
						MessageBoxA( NULL, cMsg, NULL, MB_OK );
					}
					nOffset += pObjectInfo->s_PathInfo.s_pPECount[ i ]*2;
				}		
			}
			else
			{
				char cMsg[ 512 ];
				sprintf_s( cMsg, 512, "%s\n\n오브젝트의 csp 파일 버젼이 예전 버전입니다.", pObjectInfo->s_cObjectPath );
				MessageBoxA( NULL, cMsg, NULL, MB_OK );
			}
			fclose( fp );
		}
	}

	if( nErrorIndex != -1 )
	{
		GET_MV_MAINTAB_DLG->_GetDlgPath()->_SetCurTerrainPath( nErrorIndex );
	}
}

void _DlgMTPlayer::OnBnClickedBtnLoadskybox()
{
	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"SkyBox File(*.nif)\0*.nif\0", g_Resist.GetSkyBox()->s_szSkyBoxPath ) == false )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"파일은 현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	// 스카이 박스가 맞는지 검사해본다.
	{
		char cCheck[ MAX_PATH ] = {0, };
		W2M( cCheck, szOut, MAX_PATH );
		NiStream stream;
		stream.Load( cCheck );
		NiNodePtr pNiNode = (NiNode*)stream.GetObjectAt( 0 );
		UINT uiChildCount = pNiNode->GetChildCount();
		NiGeometryPtr pNiGeom = NULL;
		for( UINT i=0; i<uiChildCount; ++i )
		{		
			if( NiIsKindOf( NiGeometry, pNiNode->GetAt( i ) ) == false )
			{
				MessageBox( L"선택된 파일은 스카이박스 오브젝트가 아닙니다.", NULL, MB_OK );
				pNiNode = 0;
				return;
			}			
		}
		pNiNode = 0;
	}	

	TCHAR szRootFullPath[ MAX_PATH ] = {0, };
	nsCSFILE::GetRelativePath( szRootFullPath, MAX_PATH, szOut );
	nsCSFILE::GetFilePathOnly( g_Resist.GetSkyBox()->s_szSkyBoxPath, MAX_PATH, szRootFullPath );
	W2M( nsCsGBTerrain::g_pCurRoot->GetSkyBox()->GetInfo()->s_cSkyBox, szRootFullPath, MAX_PATH );
	__ResistSkyBoxPath();

	nsCsGBTerrain::g_pCurRoot->GetSkyBox()->Delete();
	nsCsGBTerrain::g_pCurRoot->LoadSkyBox();
}

void _DlgMTPlayer::OnStnClickedStaticFogColor()
{
	if( m_FogColor.OpenDlg( &m_static_FogColor ) == true )
	{
		nsCsGBTerrain::g_pCurRoot->GetInfo()->s_FogColor = m_FogColor.s_Color;
		nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFogColor( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_FogColor );
	}	
}

void _DlgMTPlayer::OnStnClickedStaticBgColor()
{
	if( m_BGColor.OpenDlg( &m_static_BGColor ) == true )
	{
		nsCsGBTerrain::g_pCurRoot->GetInfo()->s_BackGroundColor = m_BGColor.s_Color;
		g_pRenderer->SetBackgroundColor( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_BackGroundColor );
	}	
}


void _DlgMTPlayer::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	if( pScrollBar->GetSafeHwnd() == m_sld_FogStart.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_FogStart, m_sld_FogStart.GetPos() );
		pInfo->s_fFogStart = m_sld_FogStart.GetPos() * 100.0f;
		if( m_sld_FogStart.GetPos() > m_sld_FogEndObject.GetPos() )
		{
			m_sld_FogEndObject.SetPos( m_sld_FogStart.GetPos() + 1 );
			_Int2Control( &m_edit_FogEndObject, m_sld_FogEndObject.GetPos() );
			pInfo->s_fFogEndObject = m_sld_FogEndObject.GetPos() * 100.0f;
		}
		if( m_sld_FogStart.GetPos() > m_sld_FogEndTerrain.GetPos() )
		{
			m_sld_FogEndTerrain.SetPos( m_sld_FogStart.GetPos() + 1 );
			_Int2Control( &m_edit_FogEndTerrain, m_sld_FogEndTerrain.GetPos() );
			pInfo->s_fFogEndTerrain = m_sld_FogEndTerrain.GetPos() * 100.0f;
		}
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_FogEndObject.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_FogEndObject, m_sld_FogEndObject.GetPos() );
		pInfo->s_fFogEndObject = m_sld_FogEndObject.GetPos() * 100.0f;
		if( m_sld_FogEndObject.GetPos() < m_sld_FogStart.GetPos() )
		{
			m_sld_FogStart.SetPos( m_sld_FogEndObject.GetPos() - 1 );
			_Int2Control( &m_edit_FogStart, m_sld_FogStart.GetPos() );
			pInfo->s_fFogStart = m_sld_FogStart.GetPos() * 100.0f;
		}
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_FogEndTerrain.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_FogEndTerrain, m_sld_FogEndTerrain.GetPos() );
		pInfo->s_fFogEndTerrain = m_sld_FogEndTerrain.GetPos() * 100.0f;
		if( m_sld_FogEndTerrain.GetPos() < m_sld_FogStart.GetPos() )
		{
			m_sld_FogStart.SetPos( m_sld_FogEndTerrain.GetPos() - 1 );
			_Int2Control( &m_edit_FogStart, m_sld_FogStart.GetPos() );
			pInfo->s_fFogStart = m_sld_FogStart.GetPos() * 100.0f;
		}
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_ClipCell.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_ClipCell, m_sld_ClipCell.GetPos() );
		pInfo->s_nClipCellObject = m_sld_ClipCell.GetPos();
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_ClipObject.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_ClipObject, m_sld_ClipObject.GetPos() );
		pInfo->s_fClipObject_Far = m_sld_ClipObject.GetPos() * 100.0f;
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_ClipTerrain.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_ClipTerrain, m_sld_ClipTerrain.GetPos() );
		pInfo->s_fClipTerrain_Far = m_sld_ClipTerrain.GetPos() * 100.0f;
	}

	if( pInfo->s_fFogStart < CAMERA_ST.GetCameraInfo()->s_fNearPlane )
	{
		pInfo->s_fFogStart = CAMERA_ST.GetCameraInfo()->s_fNearPlane;
	}
	
	nsCsGBTerrain::g_pCurRoot->CalFogDateValue();

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

void _DlgMTPlayer::OnEnChangeEditSkyboxHeight()
{
	nsCsGBTerrain::g_pCurRoot->GetSkyBox()->SetHeight( (float)nsCsMfcFunc::Control2Int( &m_edit_SkyBoxHeight ) );

	if( g_Actor.IsEnableActor() )
		nsCsGBTerrain::g_pCurRoot->GetSkyBox()->SetViewerPos( g_Actor.GetPos() );
}


void _DlgMTPlayer::OnNMReleasedcaptureSliderRadius(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	float fRate = (float)m_PlayerMoveSpeed.GetPos() / 50.0f + 0.2f;
	g_Actor.SetMoveRate(fRate);
}
