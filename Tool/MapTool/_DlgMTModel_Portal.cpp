// _DlgMTModel_Portal.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTModel_Portal.h"


// _DlgMTModel_Portal 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTModel_Portal, CDialog)

_DlgMTModel_Portal::_DlgMTModel_Portal(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTModel_Portal::IDD, pParent)
{
	m_dwBackupSelectNpcID = 0;
	m_nBackupKindIndex = 0;
}

_DlgMTModel_Portal::~_DlgMTModel_Portal()
{
}

void _DlgMTModel_Portal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MAP, m_combo_DestMap);
	DDX_Control(pDX, IDC_EDIT_DESTX, m_edit_DestX);
	DDX_Control(pDX, IDC_EDIT_DESTY, m_edit_DestY);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_edit_DestRadius);
	DDX_Control(pDX, IDC_SLIDER_RADIUS, m_sld_DestRadius);
	DDX_Control(pDX, IDC_COMBO_KIND, m_combo_Kind);
}


BEGIN_MESSAGE_MAP(_DlgMTModel_Portal, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_GOTO_DESTMAP, &_DlgMTModel_Portal::OnBnClickedButtonGotoDestmap)
	ON_BN_CLICKED(IDC_BUTTON_BACK_ORGTR, &_DlgMTModel_Portal::OnBnClickedButtonBackOrgtr)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_KIND, &_DlgMTModel_Portal::OnCbnSelchangeComboKind)
END_MESSAGE_MAP()


// _DlgMTModel_Portal 메시지 처리기입니다.
// =================================================================================================
//
//		virtual
//
// =================================================================================================

void _DlgMTModel_Portal::_PreSave( const char* cFile, bool bAutoSave )
{	
}

// =================================================================================================
//
//		Base
//
// =================================================================================================

// _DlgMTObj_Portal 메시지 처리기입니다.
BOOL _DlgMTModel_Portal::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_sld_DestRadius.SetRange( 0, 4000 );

	return TRUE;  // return TRUE unless you set the focus to a control
}


void _DlgMTModel_Portal::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
	}
	else
	{
		OnBnClickedButtonBackOrgtr();
		m_pProtalFT = NULL;
	}
}

void _DlgMTModel_Portal::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_DestRadius.GetSafeHwnd() )
	{
		nsCsMfcFunc::Int2Control( &m_edit_DestRadius, m_sld_DestRadius.GetPos() );
		m_pProtalFT->GetInfo()->s_nDestRadius = m_sld_DestRadius.GetPos();
	}

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}


// =================================================================================================
//
//		맵 정보
//
// =================================================================================================
void _DlgMTModel_Portal::__ReadMapInfo()
{
	m_combo_DestMap.ResetContent();

	TCHAR sz[ MAX_PATH ];

	CsMapList::MAP* pmapList = nsCsMapTable::g_pMapListMng->GetListMap();
	CsMapList::MAP_IT it = pmapList->begin();
	for( ; it!=pmapList->end(); ++it )
	{
		if( it->second->GetInfo()->s_cMapPath.empty() )
			continue;

		_stprintf_s( sz, MAX_PATH, it->second->GetInfo()->s_szMapDiscript.c_str() );
		int nComboIdx = m_combo_DestMap.GetCount();
		m_combo_DestMap.InsertString( nComboIdx, sz );
		m_combo_DestMap.SetItemData( nComboIdx, it->second->GetInfo()->s_dwMapID );
	}

	nsCsMfcFunc::ComboSelect_DWORD_Data( &m_combo_DestMap, m_pProtalFT->GetInfo()->s_dwDestMapID );
}

void _DlgMTModel_Portal::OnCbnSelchangeComboKind()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	m_pProtalFT = nsCsMapTable::g_pMapPortalMng->GetNpcPortal_ByObjectID( dwMapID, m_pNpcFT->GetInfo()->s_dwNpcID, m_combo_Kind.GetCurSel() );
	assert_cs( m_pProtalFT != NULL );

	__ReadMapInfo();
	__ReadDestTarget();
}


void _DlgMTModel_Portal::_SetNpc( CsNpc* pNpcFT )
{
	m_pNpcFT= pNpcFT;

	m_combo_Kind.ResetContent();
	TCHAR sz[ MAX_PATH ];
	sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)pNpcFT->GetType();
	assert_cs( pType->s_nPortalCount > 0 );
	for( int i=0; i<pType->s_nPortalCount; ++i )
	{
		_stprintf_s( sz, MAX_PATH, L"%d", i );
		m_combo_Kind.InsertString( i, sz );
	}
	m_combo_Kind.SetCurSel( 0 );

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	m_pProtalFT = nsCsMapTable::g_pMapPortalMng->GetNpcPortal_ByObjectID( dwMapID, m_pNpcFT->GetInfo()->s_dwNpcID, 0 );
	assert_cs( m_pProtalFT != NULL );

	__ReadMapInfo();
	__ReadDestTarget();
}


// =================================================================================================
//
//		지형간 이동
//
// =================================================================================================
void _DlgMTModel_Portal::__ReadDestTarget()
{
	_Int2Control( &m_edit_DestX, m_pProtalFT->GetInfo()->s_nDestTargetX );
	_Int2Control( &m_edit_DestY, m_pProtalFT->GetInfo()->s_nDestTargetY );
	_Int2Control( &m_edit_DestRadius, m_pProtalFT->GetInfo()->s_nDestRadius );
	m_sld_DestRadius.SetPos( m_pProtalFT->GetInfo()->s_nDestRadius );
}

void _DlgMTModel_Portal::_SetDestTargetPos( int x, int y )
{
	assert_cs( m_pProtalFT != NULL );
	m_pProtalFT->GetInfo()->s_nDestTargetX = x;
	m_pProtalFT->GetInfo()->s_nDestTargetY = y;
	__ReadDestTarget();
}

void _DlgMTModel_Portal::_SetViewTargetPos( int x, int y )
{
	assert_cs( m_pProtalFT != NULL );
	m_pProtalFT->GetInfo()->s_nViewTargetX = x;
	m_pProtalFT->GetInfo()->s_nViewTargetY = y;

	g_TempRoot.SetViewTarget( m_pProtalFT->GetInfo()->s_nViewTargetX, m_pProtalFT->GetInfo()->s_nViewTargetY );
}

void _DlgMTModel_Portal::OnBnClickedButtonGotoDestmap()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	g_TempRoot.DeleteTemp();

	int nComboIdx = m_combo_DestMap.GetCurSel();
	DWORD dwMapID = (DWORD)m_combo_DestMap.GetItemData( nComboIdx );

	m_pProtalFT->GetInfo()->s_dwDestMapID = dwMapID;

	// 현재 셀렉트 오브젝트 보관	
	if( m_dwBackupSelectNpcID == 0 )
	{
		m_dwBackupSelectNpcID = m_pNpcFT->GetInfo()->s_dwNpcID;
		m_nBackupKindIndex = m_combo_Kind.GetCurSel();
		CAMERA_ST.SaveCameraState();

		m_combo_Kind.EnableWindow( FALSE );
		m_combo_DestMap.EnableWindow( FALSE );
	}

	g_ObjectMng.DeleteModel();
	CsGBTerrainLeaf::m_dwObjectUniqIDConstant = 2000000000;
	g_TempRoot.LoadTemp( nsCsMapTable::g_pMapListMng->GetList( dwMapID )->GetInfo()->s_cMapPath.c_str(), cTempRoot::TT_NPC_PORTAL );
	g_TempRoot.SetPortalTarget( &m_pProtalFT->GetInfo()->s_nDestTargetX, &m_pProtalFT->GetInfo()->s_nDestTargetY, &m_pProtalFT->GetInfo()->s_nDestRadius );
	g_TempRoot.SetViewTarget( m_pProtalFT->GetInfo()->s_nViewTargetX, m_pProtalFT->GetInfo()->s_nViewTargetY );
	GET_MV_VIEW->_ResetCamera();

	GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetDlgPortal()->ShowWindow( SW_SHOW );
	m_combo_DestMap.SetCurSel( nComboIdx );
}

void _DlgMTModel_Portal::OnBnClickedButtonBackOrgtr()
{
	CsGBTerrainLeaf::m_dwObjectUniqIDConstant = 0;

	if( g_TempRoot.IsLoad() == false )
		return;

	g_TempRoot.DeleteTemp();
	CAMERA_ST.LoadCameraState();

	m_combo_Kind.EnableWindow( TRUE );
	m_combo_DestMap.EnableWindow( TRUE );

	GET_MV_MAINTAB_DLG->_GetDlgModel()->_SetMapNpcList( m_dwBackupSelectNpcID );
	g_ModelMng._SetSelectNpc( m_dwBackupSelectNpcID%90000 );
	
	ShowWindow( SW_SHOW );
	_SetNpc( nsCsFileTable::g_pNpcMng->GetNpc( m_dwBackupSelectNpcID ) );

	m_dwBackupSelectNpcID = 0;

	m_combo_Kind.SetCurSel( m_nBackupKindIndex );
	OnCbnSelchangeComboKind();
}




