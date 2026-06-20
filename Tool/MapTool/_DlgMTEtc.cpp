// _DlgMTEtc.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTEtc.h"


// _DlgMTEtc 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTEtc, CDialog)

_DlgMTEtc::_DlgMTEtc(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTEtc::IDD, pParent)
{

}

_DlgMTEtc::~_DlgMTEtc()
{
}

//================================================================================================
//
//		전체 관리
//
//================================================================================================
void _DlgMTEtc::_Delete()
{
}


//================================================================================================
//
//		미니 맵
//
//================================================================================================

void _DlgMTEtc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FEMR_DIST, m_edit_FEMRDist);
	DDX_Control(pDX, IDC_EDIT_DEST_TGA, m_desttgaName);
	DDX_Control(pDX, IDC_EDIT_DEST_EMR, m_destemrName);
}


BEGIN_MESSAGE_MAP(_DlgMTEtc, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_MINIMAP, &_DlgMTEtc::OnBnClickedButtonMakeMinimap)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_FEMR, &_DlgMTEtc::OnBnClickedButtonMakeFemr)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DIALOG, &_DlgMTEtc::OnBnClickedButtonOpenDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DIALOG2, &_DlgMTEtc::OnBnClickedButtonOpenDialog2)
	ON_BN_CLICKED(IDC_BUTTON_TGA_TO_EMR, &_DlgMTEtc::OnBnClickedButtonTgaToEmr)
	ON_BN_CLICKED(IDC_BUTTON_EMR_TO_TGA, &_DlgMTEtc::OnBnClickedButtonEmrToTga)
END_MESSAGE_MAP()


// _DlgMTEtc 메시지 처리기입니다.
BOOL _DlgMTEtc::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	_Int2Control( &m_edit_FEMRDist, 30 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

//=============================================================================================
//
//		미니맵
//
//=============================================================================================

void _DlgMTEtc::OnBnClickedButtonMakeMinimap()
{
	/*TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileSave( szOut, MAX_PATH, L"Tga File(*.tga)\0*.tga\0", g_Resist.GetEtc()->s_szMinimapPath ) == false )
		return;*/

	if( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_MAKE )
	{
		MessageBox( L"맵을 세이브 해야만 미니맵 생성이 가능합니다." );
		return;
	}
	
	//==== Save To File
	char cMap[ MAX_PATH ] = {0, };
	char cName[ MAX_PATH ];
	TCHAR szMap[ MAX_PATH ] = {0, };
	sprintf_s( cMap, MAX_PATH, "Data/Interface/Map/%s", nsCsGBTerrain::g_pCurRoot->GetMinimapName( cName, MAX_PATH ) );
	M2W( szMap, cMap, MAX_PATH );
	_DlgNotice dlgNotice( _DlgNotice::NT_SAVE_MINIMAP, szMap );
	dlgNotice.DoModal();
}

//=============================================================================================
//
//		필드 이동 가능 지역
//
//=============================================================================================

void _DlgMTEtc::OnBnClickedButtonMakeFemr()
{
	if( _Control2Int( &m_edit_FEMRDist ) == 0 )
	{
		MessageBox( L"간격 (cm단위) 을 설정하여 주십시오!" );
		return;
	}

	/*TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileSave( szOut, MAX_PATH, L"emr File(*.emr)\0*.emr\0", g_Resist.GetEtc()->s_szEMRPath ) == false )
		return;*/

	if( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_MAKE )
	{
		MessageBox( L"맵을 세이브 해야만 EMR 생성이 가능합니다." );
		return;
	}

	sEMR_INFO ei;
	ei.s_nWidth = ( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nXVertCount - 1 )*nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nGridSize;
	ei.s_nHeight = ( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nYVertCount - 1 )*nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nGridSize;
	ei.s_nGridSize = _Control2Int( &m_edit_FEMRDist );
	ei.s_nXFieldCount = ei.s_nWidth / ei.s_nGridSize + 1;
	
	//==== Save To File
	char cMap[ MAX_PATH ] = {0, };
	TCHAR szMap[ MAX_PATH ] = {0, };
	M2W( szMap, nsCsGBTerrain::g_pCurRoot->GetEMRPath( cMap, MAX_PATH ), MAX_PATH );
	_DlgNotice dlgNotice(	_DlgNotice::NT_SAVE_EMRFILE, szMap, &ei );
	dlgNotice.DoModal();

	//nsCSFILE::GetFilePathOnly( g_Resist.GetEtc()->s_szEMRPath, MAX_PATH, szOut );
}

void _DlgMTEtc::OnBnClickedButtonOpenDialog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog fd( TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST  , L"Tga Files (*.Tga)|*.Tga||" );
	int iResult = fd.DoModal();
	if(iResult == IDOK)
	{
		CString kFolderPath = fd.GetPathName();
		CString kFullPath = fd.GetFileName();

		m_desttgaName.SetWindowText(kFolderPath);
		int a= 10;

	}
	
}

void _DlgMTEtc::OnBnClickedButtonOpenDialog2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog fd( TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST  , L"Emr Files (*.Emr)|*.Emr||" );
	int iResult = fd.DoModal();
	if(iResult == IDOK)
	{
		CString kFolderPath = fd.GetPathName();
		CString kFullPath = fd.GetFileName();

		m_destemrName.SetWindowText(kFolderPath);


		int a= 10;

	}
}

void _DlgMTEtc::OnBnClickedButtonTgaToEmr()
{
	//// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//sCS_TARGADATA* pData = NULL;
	//CString kTargetTgaName = m_destemrName.GetWindowText();

	//CsTarga::GetBuffer(kTargetTgaName, pData);
	//int a=  10;
}

void _DlgMTEtc::OnBnClickedButtonEmrToTga()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
