// _DlgMTMap_Region.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTMap_Region.h"


// _DlgMTMap_Region 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTMap_Region, CDialog)

_DlgMTMap_Region::_DlgMTMap_Region(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTMap_Region::IDD, pParent)
{
	m_pSelRegion = NULL;
}

_DlgMTMap_Region::~_DlgMTMap_Region()
{
}

void _DlgMTMap_Region::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STARTX, m_edit_StartPosX);
	DDX_Control(pDX, IDC_EDIT_STARTY, m_edit_StartPosY);
	DDX_Control(pDX, IDC_SLIDER_RADIUS, m_sld_Radius);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_edit_Radius);
	DDX_Control(pDX, IDC_EDIT_MAP_REGION_DISCRIPT, m_edit_Discript);
	DDX_Control(pDX, IDC_EDIT_REGION_BGSOUND, m_edit_BGSound);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_TYPE, m_edit_FG_Type);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_DEBUFF, m_edit_FG_DBuff);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_STIME, m_edit_FG_STime);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_ATIME, m_edit_FG_ATime);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_POINT, m_edit_FG_Point);
	DDX_Control(pDX, IDC_EDIT_MAP_REGION_DISCRIPT_ENG, m_edit_Discript_Eng);
}


BEGIN_MESSAGE_MAP(_DlgMTMap_Region, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_MAP_REGION_DISCRIPT, &_DlgMTMap_Region::OnEnChangeEditMapRegionDiscript)
	ON_EN_CHANGE(IDC_EDIT_MAP_REGION_DISCRIPT_ENG, &_DlgMTMap_Region::OnEnChangeEditMapRegionDiscript_Eng)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_REGION_BGSOUND, &_DlgMTMap_Region::OnBnClickedButtonLoadRegionBgsound)
	ON_BN_CLICKED(IDC_BUTTON_DEL_REGION_BGSOUND, &_DlgMTMap_Region::OnBnClickedButtonDelRegionBgsound)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_TYPE, &_DlgMTMap_Region::OnEnChangeEditFatigueType)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_DEBUFF, &_DlgMTMap_Region::OnEnChangeEditFatigueDebuff)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_STIME, &_DlgMTMap_Region::OnEnChangeEditFatigueStime)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_ATIME, &_DlgMTMap_Region::OnEnChangeEditFatigueAtime)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_POINT, &_DlgMTMap_Region::OnEnChangeEditFatiguePoint)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_RADIUS, &_DlgMTMap_Region::OnNMCustomdrawSliderRadius)
END_MESSAGE_MAP()








// =================================================================================================
//
//		virtual
//
// =================================================================================================
void _DlgMTMap_Region::_PostRenderView()
{
	CsMapRegionGroup* pGroup = GET_MV_MAINTAB_DLG->_GetDlgMap()->_GetRegionGroup();
	CsMapRegion::LIST* plist = pGroup->GetRegionList();
	CsMapRegion::LIST_IT it = plist->begin();
	for( ; it!=plist->end(); ++it )
	{
		NiPoint2 ptPos = NiPoint2( (float)(*it)->GetInfo()->s_nCenterX, (float)(*it)->GetInfo()->s_nCenterY );
		if( m_pSelRegion == (*it) )
			g_ToolNiNodeMng.GetRegion()->Render( ptPos, (float)(*it)->GetInfo()->s_nRadius, true );
		else
			g_ToolNiNodeMng.GetRegion()->Render( ptPos, (float)(*it)->GetInfo()->s_nRadius, false );
	}
}

// =================================================================================================
//
//		Base
//
// =================================================================================================

BOOL _DlgMTMap_Region::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_sld_Radius.SetRange( 100, 50000 );

	return TRUE;
}

// _DlgMTMap_Region 메시지 처리기입니다.

void _DlgMTMap_Region::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	__ReleaseControl();
	if( bShow == FALSE )
	{
		m_pSelRegion = NULL;
	}
}

void _DlgMTMap_Region::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_Radius.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_Radius, m_sld_Radius.GetPos() );
		m_pSelRegion->GetInfo()->s_nRadius = m_sld_Radius.GetPos();
	}

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}


// =================================================================================================
//
//		Region
//
// =================================================================================================
void _DlgMTMap_Region::_SetRegion( CsMapRegion* pRegion )
{
	m_pSelRegion = pRegion;

	_Int2Control( &m_edit_StartPosX, m_pSelRegion->GetInfo()->s_nCenterX );
	_Int2Control( &m_edit_StartPosY, m_pSelRegion->GetInfo()->s_nCenterY );
	m_sld_Radius.EnableWindow( true );
	m_sld_Radius.SetPos( m_pSelRegion->GetInfo()->s_nRadius );
	_Int2Control( &m_edit_Radius, m_sld_Radius.GetPos() );

	__ReadDiscript();
	__ReadBGSound();

	// 피로도
	nsCsMfcFunc::Int2Control( &m_edit_FG_Type, m_pSelRegion->GetInfo()->s_nFatigue_Type );
	nsCsMfcFunc::Int2Control( &m_edit_FG_DBuff, m_pSelRegion->GetInfo()->s_nFatigue_DeBuff );
	nsCsMfcFunc::Int2Control( &m_edit_FG_STime, m_pSelRegion->GetInfo()->s_nFatigue_StartTime );
	nsCsMfcFunc::Int2Control( &m_edit_FG_ATime, m_pSelRegion->GetInfo()->s_nFatigue_AddTime );
	nsCsMfcFunc::Int2Control( &m_edit_FG_Point, m_pSelRegion->GetInfo()->s_nFatigue_AddPoint );
}

//=====================================================================================================
//
//		Control
//
//=====================================================================================================

void _DlgMTMap_Region::__ReleaseControl()
{
	m_edit_StartPosX.SetWindowText( L"" );
	m_edit_StartPosY.SetWindowText( L"" );
	m_edit_Radius.SetWindowText( L"" );
	m_sld_Radius.EnableWindow( false );
}

void _DlgMTMap_Region::__ReadBGSound()
{
	TCHAR szSound[ MAX_FILENAME ];
	M2W( szSound, m_pSelRegion->GetInfo()->s_cBGSound, MAX_FILENAME );
	m_edit_BGSound.SetWindowText( szSound );
}

void _DlgMTMap_Region::__ReadDiscript()
{
	m_edit_Discript.SetWindowText( m_pSelRegion->GetInfo()->s_szDiscript );
	m_edit_Discript_Eng.SetWindowText( m_pSelRegion->GetInfo()->s_szDiscript_Eng );
}

// 설명 변경시 호출
void _DlgMTMap_Region::OnEnChangeEditMapRegionDiscript()
{
	if( m_pSelRegion == NULL )
	{
		m_edit_Discript.SetWindowText( L"" );		
		return;
	}

	m_edit_Discript.GetWindowText( m_pSelRegion->GetInfo()->s_szDiscript, FT_MAP_REGION_DISC_LEN );
	GET_MV_MAINTAB_DLG->_GetDlgMap()->_ResetUniqObjList_ForRegion();
}

void _DlgMTMap_Region::OnEnChangeEditMapRegionDiscript_Eng()
{
	if( m_pSelRegion == NULL )
	{
		m_edit_Discript_Eng.SetWindowText( L"" );		
		return;
	}

	m_edit_Discript_Eng.GetWindowText( m_pSelRegion->GetInfo()->s_szDiscript_Eng, FT_MAP_REGION_DISC_LEN );
}

// 백그라운드 음악 로드 버튼
void _DlgMTMap_Region::OnBnClickedButtonLoadRegionBgsound()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	assert_cs( dwMapID != 0 );

	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"BackGround Sound File(*.*)\0*.*\0", g_Resist.GetMap()->s_szBGSound ) == false )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"사운드는 현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	TCHAR szRootPath[ MAX_PATH ] = {0, };
	TCHAR szRootFullPath[ MAX_PATH ] = {0, };
	nsCSFILE::GetRelativePath( szRootFullPath, MAX_PATH, szOut );
	nsCSFILE::GetFilePathOnly( szRootPath, MAX_PATH, szRootFullPath );

	W2M( m_pSelRegion->GetInfo()->s_cBGSound, nsCSFILE::GetFileName( szOut ), MAX_FILENAME );

	__ReadBGSound();
}


// 백그라운드 음악 제거
void _DlgMTMap_Region::OnBnClickedButtonDelRegionBgsound()
{
	memset( m_pSelRegion->GetInfo()->s_cBGSound, 0, sizeof( char )*MAX_FILENAME );
	__ReadBGSound();
}


// =================================================================================================
//
//		Input
//
// =================================================================================================
bool _DlgMTMap_Region::_IsLButtonDown( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return false;

	m_pSelRegion->GetInfo()->s_nCenterX = (int)pRecord->GetIntersection().x;
	m_pSelRegion->GetInfo()->s_nCenterY = (int)pRecord->GetIntersection().y;
	_SetRegion( m_pSelRegion );
	return true;
}






void _DlgMTMap_Region::OnEnChangeEditFatigueType()
{
	if( m_pSelRegion == NULL )
		return;

	m_pSelRegion->GetInfo()->s_nFatigue_Type = nsCsMfcFunc::Control2Int( &m_edit_FG_Type );	
}

void _DlgMTMap_Region::OnEnChangeEditFatigueDebuff()
{
	if( m_pSelRegion == NULL )
		return;

	m_pSelRegion->GetInfo()->s_nFatigue_DeBuff = nsCsMfcFunc::Control2Int( &m_edit_FG_DBuff );
}

void _DlgMTMap_Region::OnEnChangeEditFatigueStime()
{
	if( m_pSelRegion == NULL )
		return;

	m_pSelRegion->GetInfo()->s_nFatigue_StartTime = nsCsMfcFunc::Control2Int( &m_edit_FG_STime );	
}

void _DlgMTMap_Region::OnEnChangeEditFatigueAtime()
{
	if( m_pSelRegion == NULL )
		return;

	m_pSelRegion->GetInfo()->s_nFatigue_AddTime = nsCsMfcFunc::Control2Int( &m_edit_FG_ATime );	
}

void _DlgMTMap_Region::OnEnChangeEditFatiguePoint()
{
	if( m_pSelRegion == NULL )
		return;

	m_pSelRegion->GetInfo()->s_nFatigue_AddPoint = nsCsMfcFunc::Control2Int( &m_edit_FG_Point );	
}

void _DlgMTMap_Region::OnNMCustomdrawSliderRadius(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
