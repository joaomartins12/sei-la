// _DlgMTMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTMain.h"


// _DlgMTMain 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTMain, CDialog)

_DlgMTMain::_DlgMTMain(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTMain::IDD, pParent)
{
}

_DlgMTMain::~_DlgMTMain()
{
}

void _DlgMTMain::__ReadMapInfo()
{
	if( nsCsGBTerrain::g_pCurRoot == NULL )
		return;

	m_list_MapInfo.ResetContent();

	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	TCHAR szVersion[ 128 ];
	__InsertList( L"=====         현재맵          =====" );
	__InsertList( (char*)nsCsGBTerrain::g_pCurRoot->GetMapFilePath() );
	__InsertList( (char*)nsCSFILE::GetFileName( (char*)nsCsGBTerrain::g_pCurRoot->GetMapFilePath() ) );
	__InsertList( L"맵 ID", (int)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID );

	__InsertList( L"" );
	__InsertList( L"=====          버젼          =====" );
	VERSION_STRING( MAPTOOL_VERSION, szVersion, 128 );
	__InsertList( L"현재 맵툴 버젼        ", szVersion );	
	VERSION_STRING( MAP_RES_VERSION, szVersion, 128 );
	__InsertList( L"현재 맵리소스 버젼", szVersion );	
	VERSION_STRING( nsCsGBTerrain::g_pCurRoot->GetOriginalVersion(), szVersion, 128 );
	__InsertList( L"로드 맵리소스 버젼", szVersion );

	
	__InsertList( L"" );
	__InsertList( L"=====          트리          =====" );
	__InsertList( L"레벨", pRootInfo->s_nTreeLevel );
	__InsertList( L"총 노드 숫자", pRootInfo->s_nTotalLeafCount );

	__InsertList( L"" );
	__InsertList( L"=====         그리드         =====" );
	__InsertList( L"버택스간의 길이", pRootInfo->s_nGridSize/METER_SIZE );
	__InsertList( L"총 넓이", (pRootInfo->s_nXVertCount-1)*pRootInfo->s_nGridSize/METER_SIZE, (pRootInfo->s_nYVertCount-1)*pRootInfo->s_nGridSize/METER_SIZE );
	__InsertList( L"노드 넓이", (pRootInfo->s_nXVertCountInNode-1)*pRootInfo->s_nGridSize/METER_SIZE, (pRootInfo->s_nYVertCountInNode-1)*pRootInfo->s_nGridSize/METER_SIZE );

	__InsertList( L"" );
	__InsertList( L"=====         맵         =====" );
	__InsertList( L"기본사이즈", pRootInfo->s_nInitMapSize, pRootInfo->s_nInitMapSize );
	__InsertList( L"기본사용사이즈", pRootInfo->GetTextureRightFigure( 1 ) - pRootInfo->GetTextureLeftFigure( 1 ),
									pRootInfo->GetTextureBottomFigure( 1 ) - pRootInfo->GetTextureTopFigure( 1 ) );
	__InsertList( L"커버사이즈", pRootInfo->s_nInitMapCoverSize/METER_SIZE, pRootInfo->s_nInitMapCoverSize/METER_SIZE );
	__InsertList( L"BaseMap    ", pRootInfo->s_nInitMapSize*pRootInfo->s_nBaseMPPG, pRootInfo->s_nInitMapSize*pRootInfo->s_nBaseMPPG );
	__InsertList( L"AlphaMap  ", pRootInfo->s_nInitMapSize*pRootInfo->s_nAlphaMPPG, pRootInfo->s_nInitMapSize*pRootInfo->s_nAlphaMPPG );
	__InsertList( L"ShadowMap", pRootInfo->s_nInitMapSize*pRootInfo->s_nShadowMPPG, pRootInfo->s_nInitMapSize*pRootInfo->s_nShadowMPPG );

	__InsertList( L"" );
	__InsertList( L"=====          버택스          =====" );
	__InsertList( L"총 버택스 숫자", pRootInfo->s_nXVertCountInNode*pRootInfo->s_nYVertCountInNode*pRootInfo->s_nTotalLeafCount );
	__InsertList( L"X 축 총 버택스 숫자", pRootInfo->s_nXVertCount );
	__InsertList( L"Y 축 총 버택스 숫자", pRootInfo->s_nYVertCount );
	__InsertList( L"노드 버택스 숫자", pRootInfo->s_nXVertCountInNode*pRootInfo->s_nYVertCountInNode );
	__InsertList( L"X 축 노드 버택스 숫자", pRootInfo->s_nXVertCountInNode );
	__InsertList( L"Y 축 노드 버택스 숫자", pRootInfo->s_nYVertCountInNode );

	__InsertList( L"" );
	__InsertList( L"=====         높이맵         =====" );
	__InsertList( L"비율", pRootInfo->s_fHeightRate );
	__InsertList( L"맵", pRootInfo->s_szHeightMapFile );	
}

void _DlgMTMain::__InsertList( LPCTSTR str )
{
	m_list_MapInfo.InsertString( m_list_MapInfo.GetCount(), str );
}

void _DlgMTMain::__InsertList( char* c )
{
	static TCHAR szList[ 1024 ] = {0, };
	M2W( szList, c, 1024 );
	m_list_MapInfo.InsertString( m_list_MapInfo.GetCount(), szList );
}

void _DlgMTMain::__InsertList( LPCTSTR str, int val )
{
	static TCHAR szList[ 1024 ] = {0, };
	_stprintf_s( szList, 1024, L"%s   =   %d", str, val );
	m_list_MapInfo.InsertString( m_list_MapInfo.GetCount(), szList );
}

void _DlgMTMain::__InsertList( LPCTSTR str, int val, int val2 )
{
	static TCHAR szList[ 1024 ] = {0, };
	_stprintf_s( szList, 1024, L"%s   =   %d x %d", str, val, val2 );
	m_list_MapInfo.InsertString( m_list_MapInfo.GetCount(), szList );
}

void _DlgMTMain::__InsertList( LPCTSTR str, float val )
{
	static TCHAR szList[ 1024 ] = {0, };
	_stprintf_s( szList, 1024, L"%s   =   %.1f", str, val );
	m_list_MapInfo.InsertString( m_list_MapInfo.GetCount(), szList );
}

void _DlgMTMain::__InsertList( LPCTSTR str, float val, float val2 )
{
	static TCHAR szList[ 1024 ] = {0, };
	_stprintf_s( szList, 1024, L"%s   =   %.1f x %.1f", str, val, val2 );
	m_list_MapInfo.InsertString( m_list_MapInfo.GetCount(), szList );
}

void _DlgMTMain::__InsertList( LPCTSTR str, LPCTSTR str2 )
{
	static TCHAR szList[ 1024 ] = {0, };
	_stprintf_s( szList, 1024, L"%s   =   %s", str, str2 );
	m_list_MapInfo.InsertString( m_list_MapInfo.GetCount(), szList );
}

void _DlgMTMain::__InsertList( char* c1, char* c2 )
{
	static TCHAR szList[ 1024 ] = {0, };
	static char cList[ 1024 ] = {0, };
	sprintf_s( cList, 1024, "%s   =   %s", c1, c2 );
	M2W( szList, cList, 1024 );
	m_list_MapInfo.InsertString( m_list_MapInfo.GetCount(), szList );
}

void _DlgMTMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAPINFO, m_list_MapInfo);
}


BEGIN_MESSAGE_MAP(_DlgMTMain, CDialog)
	ON_BN_CLICKED(IDC_BTN_NEW, &_DlgMTMain::OnBnClickedBtnNew)
	ON_BN_CLICKED(IDC_BTN_SAVE, &_DlgMTMain::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_LOAD, &_DlgMTMain::OnBnClickedBtnLoad)	
	ON_BN_CLICKED(IDC_BTN_PATCH_NOTE, &_DlgMTMain::OnBnClickedBtnPatchNote)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_MAKE_CIFILE, &_DlgMTMain::OnBnClickedBtnMakeCifile)
	ON_BN_CLICKED(IDC_BTN_SAVEAS, &_DlgMTMain::OnBnClickedBtnSaveas)
	ON_BN_CLICKED(IDC_BTN_TR_MERGE, &_DlgMTMain::OnBnClickedBtnTrMerge)
	ON_BN_CLICKED(IDC_BTN_RELOAD, &_DlgMTMain::OnBnClickedBtnReload)
END_MESSAGE_MAP()


// _DlgMTMain 메시지 처리기입니다.

void _DlgMTMain::OnBnClickedBtnNew()
{
	CsGBTerrainRoot::sINFO RootInfo;
	RootInfo.Reset();

	_DlgNewTerrain dlgNew( &RootInfo );
	if( dlgNew.DoModal() == IDCANCEL )
		return;

	// 이미 존재 한다면
	if( ( nsCsGBTerrain::g_pCurRoot )&&( nsCsGBTerrain::g_pCurRoot->GetRootNode()->IsExistTerrain() == true ) )
	{
		if( MessageBox( L"기존에 있는것을 제거 하시고 새로 만드시겠습니까?", NULL, MB_OKCANCEL ) == IDCANCEL )
		{
			return;
		}
	}

	g_RollBack.Delete();
	g_RollBack.Init();

	GET_MV_VIEW->_PreDelete();

	g_MapMng.Delete();
	g_MapMng.Init( g_pRenderer, 8, true, true );	

	GET_MV_MAINTAB_DLG->_Reset();
	GET_MV_MAINTAB_DLG->_Resist();

	// 임시 텍스쳐가 있는지 검사 한다.
#define TMAKE_TEMP_TEXTURE_PATH		L".\\ToolData\\TTemp.tga"
	if( _taccess_s( TMAKE_TEMP_TEXTURE_PATH, 0 ) != 0 )
	{
		TCHAR szFolder[ MAX_PATH ] = {0, };
		nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, RESIST_PATH );
		// 폴더가 존재 하지 않는다면 폴더 생성
		nsCSFILE::CreateDirectory( szFolder );
		// 파일 생성
		sCS_TARGADATA td;
		CsTarga::CreateTgaData( &td, 4, 4, 24, 0xff888888 );
		CsTarga::CreateFileFromTgaData( TMAKE_TEMP_TEXTURE_PATH, &td );
		CsTarga::DeleteTgaData( &td );
	}

	GET_MV_VIEW->_PreReset();
	g_MapMng.CreateMakeTerrain( &RootInfo, TMAKE_TEMP_TEXTURE_PATH );	
	GET_MV_VIEW->_PostReset();

	__ReadMapInfo();

	// Node
	GET_MV_NODE->_ResistNode();
}

void _DlgMTMain::OnBnClickedBtnSave()
{
	if( ( nsCsGBTerrain::g_pCurRoot == NULL )||( nsCsGBTerrain::g_pCurRoot->GetRootNode()->IsExistTerrain() == false ) )
	{
		MessageBox( L"아무런 노드도 존재 하지 않습니다" );
		return;
	}

	if( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_MAKE )
	{
		OnBnClickedBtnSaveas();
		return;
	}
	
	//==== Save To File
	TCHAR szPath[ MAX_PATH ] = {0, };
	M2W( szPath, nsCsGBTerrain::g_pCurRoot->GetMapFilePath(), MAX_PATH );

	_DlgNotice dlgNotice( _DlgNotice::NT_SAVE, szPath );
	dlgNotice.DoModal();

	__ReadMapInfo();
}

void _DlgMTMain::OnBnClickedBtnSaveas()
{
	TCHAR szCurDirectory[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, szCurDirectory );
	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szCurDirectory ) == false )
	{
		MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	if( ( nsCsGBTerrain::g_pCurRoot == NULL )||( nsCsGBTerrain::g_pCurRoot->GetRootNode()->IsExistTerrain() == false ) )
	{
		MessageBox( L"아무런 노드도 존재 하지 않습니다" );
		return;
	}	

	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileSave( szOut, MAX_PATH, L"Nif File(*.nif)\0*.nif\0", g_Resist.GetMainMenu()->s_szSaveLoad ) == false )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	//==== Save To File
	_DlgNotice dlgNotice( _DlgNotice::NT_SAVE, szOut );
	dlgNotice.DoModal();

	nsCSFILE::GetFilePathOnly( g_Resist.GetMainMenu()->s_szSaveLoad, MAX_PATH, szOut );

	__ReadMapInfo();
}


void _DlgMTMain::OnBnClickedBtnLoad()
{
	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Nif File(*.nif)\0*.nif\0", g_Resist.GetMainMenu()->s_szSaveLoad ) == false )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	if( ( nsCsGBTerrain::g_pCurRoot )&&( nsCsGBTerrain::g_pCurRoot->GetRootNode()->IsExistTerrain() == true ) )	
	{
		if( MessageBox( L"기존에 있는것을 제거 하시고 로드 하시겠습니까?", NULL, MB_OKCANCEL ) == IDCANCEL )
		{
			return;
		}		
		GET_MV_MAINTAB_DLG->_Reset();
	}

	g_RollBack.Delete();
	g_RollBack.Init();

	GET_MV_VIEW->_PreDelete();

	g_MapMng.Delete();
	g_MapMng.Init( g_pRenderer, 8, true, true );

	GET_MV_VIEW->_PreReset();

	//==== Load To File
	TCHAR szRelative[ MAX_PATH ] = {0, };
	nsCSFILE::GetRelativePath( szRelative, MAX_PATH, szOut );

	_DlgNotice dlgNotice( _DlgNotice::NT_LOAD, szRelative );
	if( dlgNotice.DoModal() == IDCANCEL )
	{
		m_list_MapInfo.ResetContent();
		return;
	}	

	GET_MV_MAINTAB_DLG->_Reset();
	GET_MV_MAINTAB_DLG->_Resist();
	GET_MV_VIEW->_PostReset();

	// Node
	GET_MV_NODE->_ResistNode();

	nsCSFILE::GetFilePathOnly( g_Resist.GetMainMenu()->s_szSaveLoad, MAX_PATH, szOut );

	__ReadMapInfo();
}

void _DlgMTMain::OnBnClickedBtnPatchNote()
{
	TCHAR szPath[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, szPath );
	_tcscat_s( szPath, MAX_PATH, L"\\MapToolPatchNote.txt" );
	ShellExecute( NULL, L"open", szPath, NULL, NULL, SW_SHOW );	
}

void _DlgMTMain::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		__ReadMapInfo();
	}

	if( IS_ENABLEFRM( nsMAPTOOL::MV_VIEWER ) == true )
		GET_MV_VIEW->SetFocus();
}

void _DlgMTMain::OnBnClickedBtnMakeCifile()
{
	if( ( nsCsGBTerrain::g_pCurRoot == NULL )||( nsCsGBTerrain::g_pCurRoot->GetRootNode()->IsExistTerrain() == false ) )
	{
		MessageBox( L"지형이 없습니다." );
		return;
	}
	
	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileSave( szOut, MAX_PATH, L"_CMI File(*.cmi)\0*.cmi\0", g_Resist.GetMainMenu()->s_szCMI ) == false )
		return;

	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	assert_cs( nsCsGBTerrain::g_pCurRoot->GetFieldInfo()->s_pfHeightField != NULL );

	int nCMI_Header_Size = sizeof( sCMI_HEADER );

	sCMI_HEADER ch;
	ch.s_nRootInfo_Size = sizeof( sCMI_ROOTINFO );

	sCMI_ROOTINFO cri;
	cri.s_nXVertCount = pInfo->s_nXVertCount;
	cri.s_nYVertCount = pInfo->s_nYVertCount;
	cri.s_nTotalVertCount = pInfo->s_nXVertCount*pInfo->s_nYVertCount;
	cri.s_nGridSize = pInfo->s_nGridSize;
	
	FILE* fp = NULL;
	_tfopen_s( &fp, szOut, L"wb" );

	// ========================= 헤더	
	fwrite( &nCMI_Header_Size, sizeof( int ), 1, fp );

	// ========================= 지형
	fwrite( &ch, sizeof( sCMI_HEADER ), 1, fp );
	fwrite( &cri, sizeof( sCMI_ROOTINFO ), 1, fp );
	fwrite( nsCsGBTerrain::g_pCurRoot->GetFieldInfo()->s_pfHeightField, sizeof( float ), cri.s_nTotalVertCount, fp );

	// ========================= 오브젝트	
	int nLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	int nObjectCount = 0;

	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	for( int i=0; i<nLeafCount; ++i )
	{
		MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		for( ; it!=pMap->end(); ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;

			++nObjectCount;
		}
	}

	// 오브젝트 갯수
	fwrite( &nObjectCount, sizeof( int ), 1, fp );

	// 오브젝트 정보
	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	for( int i=0; i<nLeafCount; ++i )
	{
		MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		for( ; it!=pMap->end(); ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;

			sCMI_OBJECTINFO objInfo;
			objInfo.Reset();

			// 위치 정보
			NiTransform tr = it->second->GetNiNode()->GetWorldTransform();
			objInfo.s_ptPos = tr.m_Translate;
			
			// 회전 정보
			NiPoint3 ptRot;
			tr.m_Rotate.ToEulerAnglesZXY( objInfo.s_ptRot.z, objInfo.s_ptRot.x, objInfo.s_ptRot.y );
			
			// 박스 정보
			if( nsCSGBFUNC::CalMinMax3DBound_Local( it->second->GetNiNode(), &objInfo.s_ptMinVert, &objInfo.s_ptMaxVert ) == false )
			{
				objInfo.s_ptMinVert = it->second->GetNiNode()->GetWorldTransform().m_Translate;
				objInfo.s_ptMaxVert = it->second->GetNiNode()->GetWorldTransform().m_Translate;
			}
			
			// 이름
			strcpy_s( objInfo.s_cObjName, MAX_FILENAME, nsCSFILE::GetFileName( it->second->GetInfo()->s_cObjectPath ) );
			
			fwrite( &objInfo.s_ptPos, sizeof( float ), 3, fp );
			fwrite( &objInfo.s_ptRot, sizeof( float ), 3, fp );
			fwrite( &objInfo.s_ptMinVert, sizeof( float ), 3, fp );
			fwrite( &objInfo.s_ptMaxVert, sizeof( float ), 3, fp );
			fwrite( &objInfo.s_cObjName, sizeof( char ), MAX_FILENAME, fp );
		}
	}
	
	fclose( fp );

	nsCSFILE::GetFilePathOnly( g_Resist.GetMainMenu()->s_szCMI, MAX_PATH, szOut );
}


void _DlgMTMain::OnBnClickedBtnTrMerge()
{
	if( ( nsCsGBTerrain::g_pCurRoot == NULL )||( nsCsGBTerrain::g_pCurRoot->GetRootNode()->IsExistTerrain() == false ) )
	{
		MessageBox( L"아무런 노드도 존재 하지 않습니다" );
		return;
	}

	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Nif File(*.nif)\0*.nif\0", g_Resist.GetMainMenu()->s_szSaveLoad ) == false )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	CsPoint ptDelta( 0, 464 );

	char cOut[ MAX_PATH ] = {0, };
	W2M( cOut, szOut, MAX_PATH );

	nsCsGBTerrain::eCREATE_TYPE ct = nsCsGBTerrain::g_eCreateType;
	CsGBTerrainRoot* pCurRoot = nsCsGBTerrain::g_pCurRoot;
	CAMERA_ST.SaveCameraState();

	//======================================================================================================
	//
	// 초기화
	//
	//======================================================================================================
	GET_MV_MAINTAB_DLG->_PreViewReset();
	pCurRoot->GetSpeedCell()->Destroy();
	pCurRoot->GetSpeedCell()->Init();



	g_TempRoot.DeleteTemp();
	CsGBTerrainLeaf::m_dwObjectUniqIDConstant = 2000000000;
	if( g_TempRoot.LoadTemp( cOut, cTempRoot::TT_NONE ) == false )
		assert_cs( false );

	int nTotalLeaf = pCurRoot->GetInfo()->s_nTotalLeafCount;

	//======================================================================================================
	//
	// 맵핑 복사
	//
	//======================================================================================================
	if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount == pCurRoot->GetInfo()->s_nTotalLeafCount )
	{
		int nTotalLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
		for( int i=0; i<nTotalLeafCount; ++i )
		{
			CsGBTerrainLeaf* pSrcLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
			CsGBTerrainLeaf* pDestLeaf = pCurRoot->GetNode( i );
			// 기본 셋팅
			strcpy_s( pDestLeaf->GetNiTriInfo()->s_cDetail_Map0, MAX_PATH, pSrcLeaf->GetNiTriInfo()->s_cDetail_Map0 );
			strcpy_s( pDestLeaf->GetNiTriInfo()->s_cDetail_Map1, MAX_PATH, pSrcLeaf->GetNiTriInfo()->s_cDetail_Map1 );
			strcpy_s( pDestLeaf->GetNiTriInfo()->s_cDetail_Map2, MAX_PATH, pSrcLeaf->GetNiTriInfo()->s_cDetail_Map2 );
			strcpy_s( pDestLeaf->GetNiTriInfo()->s_cDetail_Map3, MAX_PATH, pSrcLeaf->GetNiTriInfo()->s_cDetail_Map3 );
			pDestLeaf->SetDetail_Map4_ByTool( pSrcLeaf->GetNiTriInfo()->s_cDetail_Map4 );

			for( int i=0; i<3; ++i )			
			{
				int nNTM;
				int nSrcMPPG;
				int nDestMPPG;
				switch( i )
				{
				case 0:	
					nNTM = SHADOW_MAP_SHADER_NTM;
					nDestMPPG = pCurRoot->GetInfo()->s_nShadowMPPG;
					nSrcMPPG = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nShadowMPPG;
					break;
				case 1:
					nNTM = ALPHA_MAP_SHADER_NTM;
					nDestMPPG = pCurRoot->GetInfo()->s_nAlphaMPPG;
					nSrcMPPG = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nAlphaMPPG;
					break;
				case 2:
					nNTM = EFFECT_MAP_SHADER_NTM;
					nDestMPPG = pCurRoot->GetInfo()->s_nEffectMPPG;
					nSrcMPPG = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nEffectMPPG;
					break;
				}

				NiPixelData* pSrcPixel = GET_SHADERMAP_PIXELDATA( pSrcLeaf->GetNiNode(), nNTM );
				BYTE* pSrcData = pSrcPixel->GetPixels();
				NiPixelData* pDestPixel = GET_SHADERMAP_PIXELDATA( pDestLeaf->GetNiNode(), nNTM );
				BYTE* pDestData = pDestPixel->GetPixels();
				
				int nSrcLeft = nsCsGBTerrain::g_pCurRoot->GetInfo()->GetTextureLeftFigure( nSrcMPPG );
				int nSrcTop = nsCsGBTerrain::g_pCurRoot->GetInfo()->GetTextureTopFigure( nSrcMPPG );
				int nSrcRight = nsCsGBTerrain::g_pCurRoot->GetInfo()->GetTextureRightFigure( nSrcMPPG ) + 1;
				int nSrcBottom = nsCsGBTerrain::g_pCurRoot->GetInfo()->GetTextureBottomFigure( nSrcMPPG ) + 1;
				int nSrcStride = pSrcPixel->GetPixelStride();
				int nSrcWidth = nSrcRight - nSrcLeft;
				int nSrcHeight = nSrcBottom - nSrcTop;
				BYTE* pSrcBYTE = csnew BYTE[ nSrcWidth*nSrcHeight*nSrcStride ];
				{
					int by = 0;
					for( int y=nSrcTop; y<nSrcBottom; ++y, ++by )
					{
						int bx = 0;
						for( int x=nSrcLeft; x<nSrcRight; ++x, ++bx )
						{
							for( int s=0; s<nSrcStride; ++s )
							{
								pSrcBYTE[ ( by*nSrcWidth + bx )*nSrcStride + s ] = pSrcData[ ( y*pSrcPixel->GetWidth() + x )*nSrcStride + s ];
							}
						}
					}
				}					

				// 보간
				CsImageInterpolation ip( pSrcBYTE, nSrcWidth, nSrcHeight, nSrcStride );

				int nDestLeft = pCurRoot->GetInfo()->GetTextureLeftFigure( nDestMPPG );
				int nDestTop = pCurRoot->GetInfo()->GetTextureTopFigure( nDestMPPG );
				int nDestRight = pCurRoot->GetInfo()->GetTextureRightFigure( nDestMPPG ) + 1;
				int nDestBottom = pCurRoot->GetInfo()->GetTextureBottomFigure( nDestMPPG ) + 1;
				int nDestStride = pDestPixel->GetPixelStride();
				int nDestWidth = nDestRight - nDestLeft;
				int nDestHeight = nDestBottom - nDestTop;
				BYTE* pDestBYTE = ip.Liner( nDestWidth, nDestHeight );

				{
					int by = 0;
					for( int y=nDestTop; y<nDestBottom; ++y, ++by )
					{
						int bx = 0;
						for( int x=nDestLeft; x<nDestRight; ++x, ++bx )
						{
							for( int s=0; s<nDestStride; ++s )
							{
								pDestData[ ( y*pDestPixel->GetWidth() + x )*nDestStride + s ] = pDestBYTE[ ( by*nDestWidth + bx )*nDestStride + s ];									
							}
						}
					}
				}					
				SAFE_DELETE_ARRAY( pSrcBYTE );

				pDestPixel->MarkAsChanged();
			}			
		}

		// ===== 다 만들었으면 경계선 복사 하자
		NiPixelData** ppPixelData = csnew NiPixelData*[ nTotalLeafCount ];
		for( int n=0; n<3; ++n )
		{
			int nNTM;
			int nMPPG;
			switch( n )
			{
			case 0:	
				nNTM = SHADOW_MAP_SHADER_NTM;
				nMPPG = pCurRoot->GetInfo()->s_nShadowMPPG;
				break;
			case 1:
				nNTM = ALPHA_MAP_SHADER_NTM;
				nMPPG = pCurRoot->GetInfo()->s_nAlphaMPPG;
				break;
			case 2:
				nNTM = EFFECT_MAP_SHADER_NTM;
				nMPPG = pCurRoot->GetInfo()->s_nEffectMPPG;
				break;
			}

			for( int i=0; i<nTotalLeafCount; ++i )
			{
				CsGBTerrainLeaf* pDestLeaf = pCurRoot->GetNode( i );
				ppPixelData[ i ] = (NiPixelData*)GET_SHADERMAP_PIXELDATA( pDestLeaf->GetNiNode(), nNTM );
			}
			pCurRoot->GetInfo()->CopyTextureBorder( nMPPG, ppPixelData );
		}		
		SAFE_DELETE_ARRAY( ppPixelData );
	}

	//======================================================================================================
	//
	// 라이트 정보를 가져 온다
	//
	//======================================================================================================
	// 현재 라이트 전부 제거
	int nLightCount = pCurRoot->GetLightMng()->GetLightCount();
	for( int i=0; i<nLightCount; ++i )
	{
		pCurRoot->GetLightMng()->DeleteLight( pCurRoot->GetLightMng()->GetLight( 0 ) );
	}
	// 머지할 맵의 라이트 추가
	nLightCount = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetLightCount();
	for( int i=0; i<nLightCount; ++i )
	{
		CsGBLight* pLight = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetLight( i );
		switch( pLight->GetType() )
		{
		case CsGBLight::DIRECTION:
			{
				CsGBLight_Dir* pDirLight = (CsGBLight_Dir*)pLight;
				pCurRoot->GetLightMng()->AddDirectionLight( pDirLight->GetBaseInfo(), pDirLight->GetInfo(), true );
			}
			break;
		case CsGBLight::POINT:
			{
				CsGBLight_Point* pPointLight = (CsGBLight_Point*)pLight;
				pCurRoot->GetLightMng()->AddPointLight( pPointLight->GetBaseInfo(), pPointLight->GetInfo(), true );
			}
			break;
		}
	}
	pCurRoot->GetLightMng()->GetInfo()->s_nLightCount = nLightCount;

	//======================================================================================================
	//
	// 헤이트 필드 정보를 가져 온다
	//
	//======================================================================================================
	float fSrcWidth = nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth();
	float fSrcHeight = nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight();
	float fDestWidth = pCurRoot->GetMapSizeWidth();
	float fDestHeight = pCurRoot->GetMapSizeHeight();
	float fRateWidht = fSrcWidth /fDestWidth;
	float fRateHeight = fSrcHeight /fDestHeight;	

	for( int i=0; i<nTotalLeaf; ++i )
	{
		CsGBTerrainLeaf* pLeaf = pCurRoot->GetNode( i );
		CsGBTerrainLeaf::sNiTriINFO* pNiTriInfo = pLeaf->GetNiTriInfo();		
		NiGeometryData* pGeomData = pLeaf->GetNiTriStrip()->GetModelData();
		NiPoint3* pPos = pGeomData->GetVertices();
		USHORT nVertCount = pGeomData->GetActiveVertexCount();
		float fHeight;
		for( USHORT nVert=0; nVert<nVertCount; ++nVert )
		{
			NiPoint3 CheckPos = pPos[ nVert ] + NiPoint3( pNiTriInfo->s_ptOffsetPos.x, pNiTriInfo->s_ptOffsetPos.y, 0.0f );
			NiPoint3 ResultPos = NiPoint3( CsMin( fSrcWidth, CheckPos.x*fRateWidht ), CsMin( fSrcHeight, CheckPos.y*fRateHeight ), 0.0f );
			assert_cs( g_TempRoot.IsInTerrain( ResultPos.x, ResultPos.y ) == true );
			fHeight = g_TempRoot.GetHeight_NoneTBP( ResultPos.x, ResultPos.y, g_TempRoot.GetOrgHeightField() );
			pPos[ nVert ].z = fHeight;
			pNiTriInfo->s_pfHeight[ nVert ] = fHeight;
		}
		pGeomData->MarkAsChanged( NiGeometryData::VERTEX_MASK );
		pGeomData->GetBound().ComputeFromData( nVertCount, pPos );
	}
	pCurRoot->MakeHeightField();
	//pCurRoot->CalculateNormal();

	//======================================================================================================
	//
	// 오브젝트 머지
	//
	//======================================================================================================
	float fDestRateWidth = fDestWidth / fSrcWidth;
	float fDestRateHeight = fDestHeight / fSrcHeight;

	g_TempRoot.ObjectResetCallReady();
	nTotalLeaf = g_TempRoot.GetInfo()->s_nTotalLeafCount;	
	for( int i=0; i<nTotalLeaf; ++i )
	{
		CsGBTerrainLeaf* pLeaf = g_TempRoot.GetNode( i );
		MAP_GBTERRAIN_OBJECT* pMap = pLeaf->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		CsGBObject::sINFO ObjInfo;
		for( ; it!=pMap->end(); ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;

			ObjInfo = *it->second->GetInfo();

			/*NiPoint3 ptObjPos = ObjInfo.s_trObject.m_Translate;
			NiPoint3 ResultPos = NiPoint3( CsMin( fDestWidth, ptObjPos.x*fDestRateWidth ), CsMin( fDestHeight, ptObjPos.y*fDestRateHeight ), ptObjPos.z );
			ObjInfo.s_trObject.m_Translate = ResultPos;
			assert_cs( pCurRoot->IsInTerrain( ObjInfo.s_trObject.m_Translate.x, ObjInfo.s_trObject.m_Translate.y ) == true );
			*/

			if( pCurRoot->IsInTerrain( ObjInfo.s_trObject.m_Translate.x, ObjInfo.s_trObject.m_Translate.y ) == false )
			{
				continue;
			}			

			ObjInfo.s_PathInfo.Reset();
			CsGBObject* pCsObject = pCurRoot->ResistObject( &ObjInfo );
			if( pCsObject->UD_IsNoFog() == false )
			{
				pCsObject->GetNiNode()->RemoveProperty( NiFogProperty::GetType() );
				pCsObject->GetNiNode()->AttachProperty( pCurRoot->GetFogProperty() );
				pCsObject->GetNiNode()->UpdateProperties();
			}
			if( pCsObject->UD_IsNoLight() == false )
			{
				int nLightCount = g_TempRoot.GetLightMng()->GetLightCount();
				for( int l=0; l<nLightCount; ++l )
				{
					if( g_TempRoot.GetLightMng()->GetLight( l )->GetBaseInfo()->s_bEnableLight == true )
					{
						if( g_TempRoot.GetLightMng()->GetLight( l )->GetBaseInfo()->s_bApplyObject == true )
						{
							pCsObject->GetNiNode()->DetachEffect( g_TempRoot.GetLightMng()->GetLight( l )->GetNiLight() );
						}
					}
				}

				pCurRoot->GetLightMng()->ApplyObject( pCsObject->GetNiNode() );
				pCsObject->GetNiNode()->UpdateEffects();
			}	
		}
	}

	//======================================================================================================
	//
	// 셀 오브젝트 머지 - 기존거 다 제거 후 새로운넘으로 대체
	//
	//======================================================================================================
	CsGBSpeedCell* pCurSpeedCell = pCurRoot->GetSpeedCell();
	CsGBSpeedCell* pSpeedCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell();
	int nKind = pSpeedCell->GetBaseObjectKind();
	for( int k=0; k<nKind; ++k )
	{
		pCurSpeedCell->AddBaseObject( k, pSpeedCell->GetBaseObject( k )->s_ObjectInfo.s_cPath );

		// 라이트 현재 걸로 대체
		int nLightCount = g_TempRoot.GetLightMng()->GetLightCount();
		for( int l=0; l<nLightCount; ++l )
		{
			if( g_TempRoot.GetLightMng()->GetLight( l )->GetBaseInfo()->s_bEnableLight == true )
			{
				if( g_TempRoot.GetLightMng()->GetLight( l )->GetBaseInfo()->s_bApplyObject == true )
				{
					pCurSpeedCell->GetBaseObject( k )->s_Node.m_pNiNode->DetachEffect( g_TempRoot.GetLightMng()->GetLight( l )->GetNiLight() );
				}
			}
		}
		pCurRoot->GetLightMng()->ApplyObject( pCurSpeedCell->GetBaseObject( k )->s_Node.m_pNiNode );
		pCurSpeedCell->GetBaseObject( k )->s_Node.m_pNiNode->UpdateEffects();
	}

	int nCellCount = pSpeedCell->GetCellTotalCount();
	for( int c=0; c<nCellCount; ++c )
	{		
		for( int k=0; k<nKind; ++k )
		{
			int nObjectCount = pSpeedCell->GetCell( c )->GetSize( k );
			for( int o=0; o<nObjectCount; ++o )
			{
				CsGBCell::sINFO* pInfo = pSpeedCell->GetCell( c )->GetInfo( k, o );
				if( pCurRoot->IsInTerrain( pInfo->s_trObject.m_Translate.x, pInfo->s_trObject.m_Translate.y ) == false )
					continue;
				pCurSpeedCell->ResistObject( k, &pInfo->s_trParent, &pInfo->s_trObject );
			}
		}		
	}

	//======================================================================================================
	//
	// 패쓰 엔진 머지
	//
	//======================================================================================================
	CsGBTerrainRoot::sTR_PATHINFO* pSrcTrPathInfo = nsCsGBTerrain::g_pCurRoot->GetTrPathInfo();
	CsGBTerrainRoot::sTR_PATHINFO* pDestTrPathInfo = pCurRoot->GetTrPathInfo();
	pDestTrPathInfo->Reset();
	pDestTrPathInfo->s_PathInfo.s_uiPEGroupCount = pSrcTrPathInfo->s_PathInfo.s_uiPEGroupCount;
	pDestTrPathInfo->s_PathInfo.s_uiTotalPECount = pSrcTrPathInfo->s_PathInfo.s_uiTotalPECount;
	pDestTrPathInfo->s_nDataTotalCount = pSrcTrPathInfo->s_nDataTotalCount;
	UINT uiPEGroupCount = pDestTrPathInfo->s_PathInfo.s_uiPEGroupCount;
	
	pDestTrPathInfo->s_pType = csnew char[ uiPEGroupCount ];
	memcpy( pDestTrPathInfo->s_pType, pSrcTrPathInfo->s_pType, sizeof( char )*uiPEGroupCount );

	pDestTrPathInfo->s_PathInfo.s_pptTranse = NiNew NiPoint2[ uiPEGroupCount ];
	memcpy( pDestTrPathInfo->s_PathInfo.s_pptTranse, pSrcTrPathInfo->s_PathInfo.s_pptTranse, sizeof( NiPoint2 )*uiPEGroupCount );

	pDestTrPathInfo->s_PathInfo.s_pData = csnew long[ pDestTrPathInfo->s_nDataTotalCount ];
	memcpy( pDestTrPathInfo->s_PathInfo.s_pData, pSrcTrPathInfo->s_PathInfo.s_pData, sizeof( long )*pDestTrPathInfo->s_nDataTotalCount );

	GET_MV_MAINTAB_DLG->_GetDlgCellObject()->_PostViewReset();
	GET_MV_OBJECTLIST_DLG->_PostViewReset();
	

	//======================================================================================================
	//
	// 종료 처리
	//
	//======================================================================================================
	CsGBTerrainLeaf::m_dwObjectUniqIDConstant = 0;
	g_TempRoot.DeleteTemp();

	nsCsGBTerrain::g_eCreateType = ct;
	CAMERA_ST.LoadCameraState();

	GET_MV_MAINTAB_DLG->_Reset();	
	GET_MV_MAINTAB_DLG->_Resist();
	GET_MV_MAINTAB_DLG->_PostViewReset();
	GET_MV_NODE->_ResistNode();
}




void _DlgMTMain::OnBnClickedBtnReload()
{
	if( nsCsGBTerrain::g_pCurRoot == NULL )
		return;

	TCHAR szRelative[ MAX_PATH ] = {0, };
	M2W( szRelative, nsCsGBTerrain::g_pCurRoot->GetMapFilePath(), MAX_PATH );

	g_RollBack.Delete();
	g_RollBack.Init();

	GET_MV_VIEW->_PreDelete();

	g_MapMng.Delete();
	g_MapMng.Init( g_pRenderer, 8, true, true );

	GET_MV_VIEW->_PreReset();

	//==== Load To File
	
	_DlgNotice dlgNotice( _DlgNotice::NT_LOAD, szRelative );
	if( dlgNotice.DoModal() == IDCANCEL )
	{
		m_list_MapInfo.ResetContent();
		return;
	}	

	GET_MV_MAINTAB_DLG->_Reset();
	GET_MV_MAINTAB_DLG->_Resist();
	GET_MV_VIEW->_PostReset();

	// Node
	GET_MV_NODE->_ResistNode();

	nsCSFILE::GetFilePathOnly( g_Resist.GetMainMenu()->s_szSaveLoad, MAX_PATH, szRelative );

	__ReadMapInfo();
}
