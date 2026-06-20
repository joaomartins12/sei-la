// _DlgNotice.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgNotice.h"


// _DlgNotice 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgNotice, CDialog)

_DlgNotice::_DlgNotice( eNOTICE_TYPE ent, void* pConstant1, void* pConstant2/*=NULL*/, void* pConstant3/*=NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(_DlgNotice::IDD, pParent)
{
	m_eNoticeType = ent;
	m_pConstant1 = pConstant1;
	m_pConstant2 = pConstant2;
	m_pConstant3 = pConstant3;
}

_DlgNotice::~_DlgNotice()
{
}

void _DlgNotice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_NOTICE, m_static_Notice);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_pgBar);
}


BEGIN_MESSAGE_MAP(_DlgNotice, CDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// _DlgNotice 메시지 처리기입니다.

BOOL _DlgNotice::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bInit = false;
	m_pgBar.ShowWindow( SW_HIDE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgNotice::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	SetTimer( 0, 1, NULL );
}

void _DlgNotice::OnTimer(UINT_PTR nIDEvent)
{
	switch( m_eNoticeType )
	{
	case NT_SHADOWMAP:		__ShadowMap_Update();		break;
	case NT_SAVE:			__Save_Update();			break;
	case NT_LOAD:			__Load_Update();			break;
	case NT_PLAY:			__Play_Update();			break;
	case NT_HEIGHT_FIELD:	__HeightField_Update();		break;
	case NT_SAVE_MINIMAP:	__SaveMinimap_Update();		break;
	case NT_SAVE_EMRFILE:	__SaveEMRFile_Update();		break;
	case NT_SM_BLUR:		__ShadowMapBlur_Update();	break;
	default:				assert_cs( false );
	}

	CDialog::OnTimer(nIDEvent);
}

void _DlgNotice::__ShadowMap_Update()
{
	static int nStep = 0;
	static sCS_BMPDATA* pBmpData;
	static D3DXMATRIX* pLVP;
	static D3DXMATRIX* pLVPT;	

	if( m_bInit == false )
	{
		m_nValue1 = 0;
		m_nValue2 = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;

		SetWindowText( L"Apply Shadow" );
		TCHAR szMsg[ MAX_PATH ];
		_stprintf_s( szMsg, MAX_PATH, L"지형 쉐도우맵을 위한 이미지 데이타 생성 중입니다." );
		m_static_Notice.SetWindowText( szMsg );		

		m_bInit = true;

		nStep = 0;
		g_pShadow->Tool_InitApplyTRShadowMap( &pBmpData, &pLVP, &pLVPT );

		m_pgBar.ShowWindow( SW_SHOW );
		m_pgBar.SetRange( 0, m_nValue2 );
		m_pgBar.SetPos( 1 );		
	}
	else if( nStep == 0 )
	{
		m_pgBar.SetPos( m_nValue1 + 1 );

		TCHAR szMsg[ MAX_PATH ];
		_stprintf_s( szMsg, MAX_PATH, L"지형 쉐도우맵 ( 라이트맵 포함 ) 생성중 ... %d / %d", m_nValue1+1, m_nValue2 );
		m_static_Notice.SetWindowText( szMsg );

		if( nsCsGBTerrain::g_pCurRoot->GetNode( m_nValue1 )->IsVisible() == true )
		{
			g_pShadow->Tool_ApplyTRShadowMap( m_nValue1, pBmpData, pLVP, pLVPT );			
		}

		if( ++m_nValue1 == m_nValue2 )
		{
			nStep = 1;			
			m_static_Notice.SetWindowText( L"쉐도우맵 경계선 복사중 ..." );
			m_pgBar.ShowWindow( SW_HIDE );
		}
	}
	else if( nStep == 1 )
	{
		g_pShadow->Tool_CopyBorderShadowMap();
		// 쉐이더 다시 원래 대로 복귀
		g_Sorting.SetShader_GetterTRObject( nsCsGBTerrain::g_pRenderer->GetDefaultMaterial() );

		nStep = 2;		
		m_static_Notice.SetWindowText( L"베이스 오브젝트에 그림자 생성중 ..." );
	}
	else if( nStep == 2 )
	{
		g_pShadow->Tool_ApplyDMObjectShadowMap();

		m_static_Notice.SetWindowText( L"쉐도우맵 생성을 위한 데이터 삭제중" );
		nStep = 3;		
	}
	else if( nStep == 3 )
	{
		g_pShadow->Tool_EndApplyTRShadowMap( &pBmpData, &pLVP, &pLVPT );
		OnOK();
	}
}

void _DlgNotice::__Save_Update()
{
	TCHAR szMsg[ MAX_PATH ];
	_stprintf_s( szMsg, MAX_PATH, L"저장 중입니다.\n시간이 조금 걸리니 기다려주세요~!" );
	m_static_Notice.SetWindowText( szMsg );	

	if( m_bInit == false )
	{
		SetWindowText( L"Save" );
		m_bInit = true;
	}
	else
	{
		KillTimer( 0 );

		char cSavePath[ MAX_PATH ] = {0, };
		W2M( cSavePath, (LPCTSTR)m_pConstant1, MAX_PATH );

		nsCsGBTerrain::g_pCurRoot->PreSave( cSavePath );
		GET_MV_MAINTAB_DLG->_PreSave( cSavePath, m_pConstant2 ? *(bool*)m_pConstant2 : false );

		// 파일 테이블 세이브
		if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID != 0 )
		{
			if( m_pConstant2 == NULL )
			{
				nsCsFileTable::g_pMapMng->Save( g_Config.GetEnvironment()->s_bWriteExcel, g_Config.GetEnvironment()->s_bWriteList,
					g_Config.GetEnvironment()->s_bWritePortal, g_Config.GetEnvironment()->s_bWriteCharLight );
			}
			else
			{
				nsCsMapTable::g_pMapTableObjMng->Save();
			}
		}

		// 오브젝트 패쓰엔진 재등록
		GET_MV_MAINTAB_DLG->_GetDlgPlayer()->OnBnClickedButtonApplyCollision();

		// 세이브
		nsCsGBTerrain::g_pCurRoot->Save( (LPCTSTR)m_pConstant1 );

		// 세이브 했으니 자동세이브 리셋
		GET_MV_VIEW->_ResetAutoSave();

		OnOK();
	}
}

void _DlgNotice::__Load_Update()
{
	TCHAR szMsg[ MAX_PATH ];
	_stprintf_s( szMsg, MAX_PATH, L"로드 중입니다.\n시간이 조금 걸리니 기다려주세요~!" );
	m_static_Notice.SetWindowText( szMsg );	

	if( m_bInit == false )
	{
		SetWindowText( L"Load" );
		m_bInit = true;
	}
	else
	{		
		KillTimer( 0 );

		char cLoadPath[ MAX_PATH ] = {0, };
		W2M( cLoadPath, (LPCTSTR)m_pConstant1, MAX_PATH );

		//// 지형 파일인지 체크해본다.
		//NiStream stream;
		//stream.Load( cLoadPath );		
		//if( ( stream.GetObjectCount() == 1 )||( NiIsKindOf( CsNiNode, stream.GetObjectAt( 1 ) ) == false ) )
		//{
		//	MessageBox( L"선택된 파일은 지형이 아닙니다.", NULL, MB_OK );
		//	CDialog::OnCancel();
		//	return;
		//}
		
		if( g_MapMng.CreateRoot( cLoadPath, nsCsGBTerrain::CT_FILELOAD_BYTOOL ) == false )
		{			
			CDialog::OnCancel();
			return;
		}

		nsCsGBTerrain::g_pCurRoot->ResetOption();

		// 모델 로드
		//g_ModelMng.LoadNpc( cLoadPath );
		OnOK();
	}
}

void _DlgNotice::__Play_Update()
{
	TCHAR szMsg[ MAX_PATH ];
	_stprintf_s( szMsg, MAX_PATH, L"로드 중입니다.\n시간이 조금 걸리니 기다려주세요~!" );
	m_static_Notice.SetWindowText( szMsg );	

	if( m_bInit == false )
	{
		SetWindowText( L"Load_ForPlay" );
		m_bInit = true;
	}
	else
	{
		KillTimer( 0 );

		char cLoadPath[ MAX_PATH ] = {0, };
		W2M( cLoadPath, (LPCTSTR)m_pConstant1, MAX_PATH );
		if( g_MapMng.CreateRoot( cLoadPath, nsCsGBTerrain::CT_FILELOAD ) == false )
		{			
			CDialog::OnCancel();
			return;
		}
		OnOK();
	}
}

void _DlgNotice::__HeightField_Update()
{
	TCHAR szMsg[ MAX_PATH ];
	_stprintf_s( szMsg, MAX_PATH, L"헤이트 필드 생성 중입니다." );
	m_static_Notice.SetWindowText( szMsg );

	if( m_bInit == false )
	{
		SetWindowText( L"Create Height Field" );
		m_bInit = true;
	}
	else
	{
		KillTimer( 0 );
		nsCsGBTerrain::g_pCurRoot->MakeHeightField();
		//nsCsGBTerrain::g_pCurRoot->CalculateNormal();
		OnOK();
	}
}

void _DlgNotice::__SaveMinimap_Update()
{
	#define MINIMAP_SIZE		512.0f

	TCHAR szMsg[ MAX_PATH ];
	_stprintf_s( szMsg, MAX_PATH, L"미니 맵 생성 중입니다." );
	m_static_Notice.SetWindowText( szMsg );

	if( m_bInit == false )
	{
		SetWindowText( L"Make Mini Map" );
		m_bInit = true;
	}
	else
	{
		KillTimer( 0 );

		nsCsGBTerrain::eCREATE_TYPE ct = nsCsGBTerrain::g_eCreateType;

		// 원 오브젝트 맵ID
		DWORD dwOrgMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

		// 오브젝트 랜더링인가
		bool bRenderObject = nsCsGBTerrain::g_pCurRoot->GetRenderObject_ByTool();
		// 카메라 셋팅 저장
		CAMERA_ST.SaveCameraState();
		// 패쓰엔진 워닝 안뜨게
		nsCsGBTerrain::g_SaveLoad.g_bLoadPathEngineWarning = false;

		// 카메라 기본 위치로
		GET_MV_VIEW->_ResetCamera();

		CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
		float fHalfWidth = (pInfo->s_nXVertCount-1)*pInfo->s_nGridSize*0.5f;
		float fHalfHeight = (pInfo->s_nYVertCount-1)*pInfo->s_nGridSize*0.5f;

		// 확장 사이즈
		float fExHeight = 0;
		float fExWidth = 0;

		if( fHalfHeight >= fHalfWidth )
		{
			fExHeight = fHalfHeight*(VIEWER_HEIGHT*0.5f)/(MINIMAP_SIZE*0.5f);
			fExWidth = fHalfHeight*(VIEWER_HEIGHT*0.5f)/(MINIMAP_SIZE*0.5f)*CAMERA_ST.GetAspectRatio();
		}
		else
		{
			fExWidth = fHalfWidth*(VIEWER_WIDTH*0.5f)/(MINIMAP_SIZE*0.5f);
			fExHeight = fHalfWidth*(VIEWER_WIDTH*0.5f)/(MINIMAP_SIZE*0.5f)/CAMERA_ST.GetAspectRatio();
		}
		
		// 직교 투영으로
		// 맵사이즈 안에 모든 지형이 들어갈수 있도록 fHalfHeight 확장 -  전제 ( 넓이 < 높이 )
		CAMERA_ST.SetOrthoFrustum( fExWidth, fExHeight );

		// 추가된양은 = 확장양 - 기존양
		float fAddHeight =  fExHeight - fHalfHeight;		// 가장 아래에 붙이는값
		float fAddWidth = fExWidth - fHalfWidth;			// 가장 좌측에 붙이는값

		if( fHalfHeight >= fHalfWidth )
		{
			CAMERA_ST.SetDeltaTranslate( NiPoint3( fAddWidth - (fHalfHeight-fHalfWidth), -fAddHeight, 0.0f ) );
		}
		else
		{
			CAMERA_ST.SetDeltaTranslate( NiPoint3( fAddWidth, -fAddHeight-(fHalfHeight-fHalfWidth), 0.0f ) );
		}
		
		CAMERA_ST._UpdateCamera();
		CAMERA_ST.ApplyFrustumPlane( true );

		cRender::RenderToTexture( cRender::R2T_DETAILMAP );

		// dds 로 저장
		LPDIRECT3DDEVICE9 pkDevice = g_pRenderer->GetD3DDevice();
		LPDIRECT3DSURFACE9 pkDestSurface = NULL;
		HRESULT hr = pkDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pkDestSurface);
		assert_cs( SUCCEEDED(hr) );
		// Save the image to a file
		HRESULT hr2 = D3DXSaveSurfaceToFile( L"temp.dds", D3DXIFF_DDS, pkDestSurface, NULL, NULL);
		D3D_POINTER_RELEASE(pkDestSurface);

		// 해당 파일 리딩
		NiTexturingPropertyPtr pTexProp = NiNew NiTexturingProperty( "temp.dds" );
		NiSourceTexture* pTexture = (NiSourceTexture*)pTexProp->GetBaseMap()->GetTexture();
		assert_cs( NiIsKindOf( NiSourceTexture, pTexture ) == true );
		pTexture->LoadPixelDataFromFile();
		NiPixelData* pPixelData = pTexture->GetSourcePixelData();
		int nSrcWidth = pPixelData->GetWidth();
		int nSrcStride = pPixelData->GetPixelStride();
		BYTE* pPixel = (BYTE*)pPixelData->GetPixels();

		// 텍스쳐로 저장
		int nDestStride = 3;
		sCS_TARGADATA td;
		CsTarga::CreateTgaData( &td, (int)MINIMAP_SIZE, (int)MINIMAP_SIZE, nDestStride*8, 0 );
		int nPixelCount = td.s_Header.imageWidth*td.s_Header.imageHeight;

		for( int y=0; y<td.s_Header.imageHeight; ++y )
		{
			for( int x=0; x<td.s_Header.imageWidth; ++x )
			{
				int yTemp = ( td.s_Header.imageHeight-1-y)*td.s_Header.imageWidth;

				if( ( x == 0 )||( y == 0 )||( x == td.s_Header.imageWidth - 1 )||( y == td.s_Header.imageHeight - 1 ) )
				{
					td.s_pData[ ( yTemp + x )*nDestStride + 0 ] = 0;
					td.s_pData[ ( yTemp + x )*nDestStride + 1 ] = 0;
					td.s_pData[ ( yTemp + x )*nDestStride + 2 ] = 0;
				}
				else
				{
					td.s_pData[ ( yTemp + x )*nDestStride + 0 ] = pPixel[ ( y*nSrcWidth + x )*nSrcStride + 2 ];
					td.s_pData[ ( yTemp + x )*nDestStride + 1 ] = pPixel[ ( y*nSrcWidth + x )*nSrcStride + 1 ];
					td.s_pData[ ( yTemp + x )*nDestStride + 2 ] = pPixel[ ( y*nSrcWidth + x )*nSrcStride + 0 ];
				}				
			}
		}

		CsTarga::CreateFileFromTgaData( (LPCTSTR)m_pConstant1, &td );
		CsTarga::DeleteTgaData( &td );
		
		// 템프 데이터 제거
		pTexProp = 0;
		DeleteFile( L"temp.dds" );

		nsCsGBTerrain::g_SaveLoad.g_bLoadPathEngineWarning = true;
		nsCsGBTerrain::g_SaveLoad.g_bSaveBMIncludeSM = true;
		// 카메라 돌려준다
		CAMERA_ST.LoadCameraState();
		CAMERA_ST.SetStandardFrustum();
		CAMERA_ST.ApplyFrustumPlane( false );


		nsCsGBTerrain::g_eCreateType = ct;

		OnOK();
	}
}

void _DlgNotice::__SaveEMRFile_Update()
{
	TCHAR szMsg[ MAX_PATH ];
	_stprintf_s( szMsg, MAX_PATH, L"emr 파일 생성 중입니다." );
	m_static_Notice.SetWindowText( szMsg );

	if( m_bInit == false )
	{
		SetWindowText( L"Create emr file" );
		m_bInit = true;
	}
	else
	{
		KillTimer( 0 );

		_DlgMTEtc::sEMR_INFO* pEi = (_DlgMTEtc::sEMR_INFO*)m_pConstant2;

		// 오브젝트 패쓰엔진 재등록
		GET_MV_MAINTAB_DLG->_GetDlgPlayer()->OnBnClickedButtonApplyCollision();
		
		// 저장
		TCHAR szPath[ MAX_PATH ] = L".\\EMR\\";
		nsCSFILE::CreateDirectory( szPath );
		_tcscat_s( szPath, MAX_PATH, nsCSFILE::GetFileName( (TCHAR*)m_pConstant1 ) );

		// 텍스쳐
		sCS_TARGADATA td;
		int nWidth = pEi->s_nWidth/pEi->s_nGridSize + 1;
		int nHeight = pEi->s_nHeight/pEi->s_nGridSize + 1;
		CsTarga::CreateTgaData( &td, nWidth, nHeight, 24, 0xffffffff );		

		FILE* fp = NULL;
		_tfopen_s( &fp, szPath, L"wb" );
		assert_cs( fp != NULL );
		fwrite( pEi, sizeof( _DlgMTEtc::sEMR_INFO ), 1, fp );
		char cData;
		cPosition pos[ 4 ];
		pos[ 0 ].cell = pos[ 1 ].cell = pos[ 2 ].cell = pos[ 3 ].cell = 0;

		bool bOldPath = false;

		DWORD dwIndex = 0;
		for( int y=0; y<=pEi->s_nHeight; y+=pEi->s_nGridSize )
		{			
			for( int x=0; x<=pEi->s_nWidth; x+=pEi->s_nGridSize )
			{
				pos[ 0 ].x = pos[ 2 ].x = x;
				pos[ 0 ].y = pos[ 1 ].y = y;
				pos[ 3 ].x = pos[ 1 ].x = x + pEi->s_nGridSize;
				pos[ 3 ].y = pos[ 2 ].y = y + pEi->s_nGridSize;

				if( ( bOldPath == true )&&( x != 0 ) )
				{
					td.s_pData[ dwIndex + 1 ] = 0x00;
					td.s_pData[ dwIndex + 2 ] = 0x00;

					bOldPath = false;
					cData = _DlgMTEtc::ET_NOMOVE;
				}
				else if( ( bOldPath = ( g_pCsPathEngine->m_iMesh->testPointCollision( g_pCsPathEngine->m_iShapeAgent, g_pCsPathEngine->m_iContext, pos[ 3 ] ) == true ) )||
					( g_pCsPathEngine->m_iMesh->testPointCollision( g_pCsPathEngine->m_iShapeAgent, g_pCsPathEngine->m_iContext, pos[ 2 ] ) == true )||
					( g_pCsPathEngine->m_iMesh->testPointCollision( g_pCsPathEngine->m_iShapeAgent, g_pCsPathEngine->m_iContext, pos[ 1 ] ) == true )||
					( g_pCsPathEngine->m_iMesh->testPointCollision( g_pCsPathEngine->m_iShapeAgent, g_pCsPathEngine->m_iContext, pos[ 0 ] ) == true )	)
				{
					td.s_pData[ dwIndex + 1 ] = 0x00;
					td.s_pData[ dwIndex + 2 ] = 0x00;

					cData = _DlgMTEtc::ET_NOMOVE;
				}
				else
				{
					cData = _DlgMTEtc::ET_ENABLE_MOVE;
				}
				dwIndex += 3;
				fwrite( &cData, sizeof( char ), 1, fp );				
			}
		}
		fclose( fp );

		::CopyFile( szPath, (TCHAR*)m_pConstant1, FALSE );		

		size_t siLen = _tcslen( szPath );
		szPath[ siLen-3 ] = NULL;
		_tcscat_s( szPath, MAX_PATH, L"tga" );		
		CsTarga::CreateFileFromTgaData( szPath, &td );
		CsTarga::DeleteTgaData( &td );

		OnOK();
	}
}

void _DlgNotice::__ShadowMapBlur_Update()
{
	if( m_bInit == false )
	{
		SetWindowText( L"ShadowMap Bluring" );
		TCHAR szMsg[ MAX_PATH ];
		_stprintf_s( szMsg, MAX_PATH, L"쉐도우맵 블러 중입니다. 장시만 기다려 주세요" );
		m_static_Notice.SetWindowText( szMsg );	
		m_bInit = true;
	}
	else
	{
		KillTimer( 0 );

		int nBlurSize = *(int*)m_pConstant1;
		GET_MV_MAINTAB_DLG->_GetDlgShadow()->_ShadowMapBlur( nBlurSize );
		OnOK();
	}
}

void _DlgNotice::OnOK()
{
	KillTimer( 0 );
	m_pgBar.ShowWindow( SW_HIDE );

	CDialog::OnOK();
}


