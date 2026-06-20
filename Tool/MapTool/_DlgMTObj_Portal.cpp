// _DlgMTObj_Portal.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTObj_Portal.h"


// _DlgMTObj_Portal 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTObj_Portal, CDialog)

_DlgMTObj_Portal::_DlgMTObj_Portal(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTObj_Portal::IDD, pParent)
{
	m_pBackupSelectObjectInfo = NULL;
	m_pPortalObj = NULL;
}

_DlgMTObj_Portal::~_DlgMTObj_Portal()
{
}

void _DlgMTObj_Portal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MAP, m_combo_DestMap);
	DDX_Control(pDX, IDC_EDIT_DESTX, m_edit_DestX);
	DDX_Control(pDX, IDC_EDIT_DESTY, m_edit_DestY);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_edit_DestRadius);
	DDX_Control(pDX, IDC_SLIDER_RADIUS, m_sld_DestRadius);
	DDX_Control(pDX, IDC_EDIT_TOTALCOUNT, m_edit_TotalCount);
	DDX_Control(pDX, IDC_EDIT_PORTAL_ID, m_edit_PortalID);
	DDX_Control(pDX, IDC_COMBO_PORTAL_TYPE, m_combo_Portal_Type);
}


BEGIN_MESSAGE_MAP(_DlgMTObj_Portal, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_GOTO_DESTMAP, &_DlgMTObj_Portal::OnBnClickedButtonGotoDestmap)
	ON_BN_CLICKED(IDC_BUTTON_BACK_ORGTR, &_DlgMTObj_Portal::OnBnClickedButtonBackOrgtr)
	ON_CBN_SELCHANGE(IDC_COMBO_PORTAL_TYPE, &_DlgMTObj_Portal::OnCbnSelchangeComboPortalType)	
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

// =================================================================================================
//
//		virtual
//
// =================================================================================================
void _DlgMTObj_Portal::_PreSave( const char* cFile, bool bAutoSave )
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( dwMapID == 0 )
		return;

	// 포탈 ID 컨버팅
	nsCsGBTerrain::g_pCurRoot->ConvertPortalID();

	if( bAutoSave == true )
		return;

	// 기존 값들을 새값으로 변경
	nsCsMapTable::g_pMapPortalMng->DeleteGroup_PortalType( dwMapID, CsMapPortal::OBJECT );
	CsMapPortalGroup* pGroup = nsCsMapTable::g_pMapPortalMng->GetGroup( dwMapID );
	CsMapPortal::sINFO info;

	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	for( int i=0; i<nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{			
			MAP_GBTERRAIN_OBJECT* pPortalMap = pLeaf->GetPortalObjMap();
			MAP_GBTERRAIN_OBJECT_IT itPortal = pPortalMap->begin();
			for( ;itPortal != pPortalMap->end(); ++itPortal )
			{
				CsGBObj_Portal* pPortalObj = (CsGBObj_Portal*)itPortal->second;
				assert_cs( pPortalObj->GetType() == CsGBObject::OT_PORTAL );
				assert_cs( pPortalObj->GetPortalID() != 0 );

				if( pPortalObj->IsCallReady() == true )
				{
					info.Init();
					info.s_dwPortalID = pPortalObj->GetPortalID();
					info.s_dwPortalType = pPortalObj->GetPortalType();
					info.s_dwSrcMapID = dwMapID;
					info.s_nSrcTargetX = (int)pPortalObj->GetSrcTarget()->s_ptCenterPos.x;
					info.s_nSrcTargetY = (int)pPortalObj->GetSrcTarget()->s_ptCenterPos.y;
					info.s_nSrcRadius = (int)pPortalObj->GetSrcTarget()->s_fRadius;
					info.s_dwDestMapID = pPortalObj->GetTargetMapID();
					info.s_nDestTargetX = (int)pPortalObj->GetDestTarget()->s_ptCenterPos.x;
					info.s_nDestTargetY = (int)pPortalObj->GetDestTarget()->s_ptCenterPos.y;
					info.s_nDestRadius = (int)pPortalObj->GetDestTarget()->s_fRadius;
					info.s_dwUniqObjectID = pPortalObj->GetInfo()->s_dwUniqID;
					info.s_ePortalType = CsMapPortal::OBJECT;
					info.s_nViewTargetX	= (int)pPortalObj->GetViewTarget().x;
					info.s_nViewTargetY	= (int)pPortalObj->GetViewTarget().y;
					info.s_nPortalKindIndex = 0;

					nsCsMapTable::g_pMapPortalMng->AddPortal( &info, NULL );
				}
			}
		}
	}
}

// =================================================================================================
//
//		Base
//
// =================================================================================================

// _DlgMTObj_Portal 메시지 처리기입니다.
BOOL _DlgMTObj_Portal::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_sld_DestRadius.SetRange( 0, 4000 );

	return TRUE;  // return TRUE unless you set the focus to a control
}


void _DlgMTObj_Portal::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		if( g_TempRoot.IsLoad() == false )
		{
			m_pPortalObj = (CsGBObj_Portal*)GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetUniqCsGBObject();

			__ReadMapInfo();
			__ReadDestTarget();
		}		
	}
	else
	{
		OnBnClickedButtonBackOrgtr();
	}
}

void _DlgMTObj_Portal::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_DestRadius.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_DestRadius, m_sld_DestRadius.GetPos() );
		m_pPortalObj->GetDestTarget()->s_fRadius = (float)m_sld_DestRadius.GetPos();
	}

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}


// =================================================================================================
//
//		맵 정보
//
// =================================================================================================
void _DlgMTObj_Portal::__ReadMapInfo()
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
	m_combo_DestMap.SetCurSel( 0 );

	_Int2Control( &m_edit_TotalCount, nsCsGBTerrain::g_pCurRoot->GetPortalTotalCount() + 1 );

	nsCsMfcFunc::Int2Control( &m_edit_PortalID, m_pPortalObj->GetPortalID() );
	m_combo_Portal_Type.ResetContent();

	m_combo_Portal_Type.InsertString(0,_T("노말포탈"));
	m_combo_Portal_Type.SetItemData(0,0);
	m_combo_Portal_Type.InsertString(1,_T("인던포탈"));
	m_combo_Portal_Type.SetItemData(1,1);
	m_combo_Portal_Type.InsertString(2,_T("존내포탈"));
	m_combo_Portal_Type.SetItemData(2,2);

	m_combo_Portal_Type.SetCurSel(m_pPortalObj->GetPortalType());
}


// =================================================================================================
//
//		지형간 이동
//
// =================================================================================================
void _DlgMTObj_Portal::__ReadDestTarget()
{
	int nComboCount = m_combo_DestMap.GetCount();
	for( int i=0; i<nComboCount; ++i )
	{
		if( m_pPortalObj->GetTargetMapID() == (DWORD)m_combo_DestMap.GetItemData( i ) )
		{
			m_combo_DestMap.SetCurSel( i );
			break;
		}
	}

	assert_cs( m_pPortalObj != NULL );
	_Int2Control( &m_edit_DestX, (int)m_pPortalObj->GetDestTarget()->s_ptCenterPos.x );
	_Int2Control( &m_edit_DestY, (int)m_pPortalObj->GetDestTarget()->s_ptCenterPos.y );
	_Int2Control( &m_edit_DestRadius, (int)m_pPortalObj->GetDestTarget()->s_fRadius );
	m_sld_DestRadius.SetPos( (int)m_pPortalObj->GetDestTarget()->s_fRadius );
}

void _DlgMTObj_Portal::_SetDestTargetPos( NiPoint2 pt )
{
	assert_cs( m_pPortalObj != NULL );
	m_pPortalObj->GetDestTarget()->s_ptCenterPos = pt;
	__ReadDestTarget();
}

void _DlgMTObj_Portal::_SetViewTargetPos( NiPoint2 pt )
{
	assert_cs( m_pPortalObj != NULL );
	m_pPortalObj->SetViewTarget( pt );
	g_TempRoot.SetViewTarget( (int)m_pPortalObj->GetViewTarget().x, (int)m_pPortalObj->GetViewTarget().y );
}

void _DlgMTObj_Portal::OnBnClickedButtonGotoDestmap()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	g_TempRoot.DeleteTemp();

	int nComboIdx = m_combo_DestMap.GetCurSel();
	DWORD dwMapID = (DWORD)m_combo_DestMap.GetItemData( nComboIdx );

	m_pPortalObj->SetTargetMapID( dwMapID );

	// 현재 셀렉트 오브젝트 보관
	if( m_pBackupSelectObjectInfo == NULL )
	{
		m_pBackupSelectObjectInfo = ( *g_ObjectMng.GetListSelectModel()->begin() )->GetObjectInfo();
		CAMERA_ST.SaveCameraState();
	}

	g_ObjectMng.DeleteModel();
	CsGBTerrainLeaf::m_dwObjectUniqIDConstant = 2000000000;
	g_TempRoot.LoadTemp( nsCsMapTable::g_pMapListMng->GetList( dwMapID )->GetInfo()->s_cMapPath.c_str(), cTempRoot::TT_PORTALOBJ );
	g_TempRoot.SetPortalTarget( m_pPortalObj->GetDestTarget() );
	g_TempRoot.SetViewTarget( (int)m_pPortalObj->GetViewTarget().x, (int)m_pPortalObj->GetViewTarget().y );
	GET_MV_VIEW->_ResetCamera();
	
	GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetDlgPortal()->ShowWindow( SW_SHOW );
	m_combo_DestMap.SetCurSel( nComboIdx );
}

void _DlgMTObj_Portal::OnBnClickedButtonBackOrgtr()
{
	CsGBTerrainLeaf::m_dwObjectUniqIDConstant = 0;

	if( g_TempRoot.IsLoad() == false )
		return;

	g_TempRoot.DeleteTemp();
	CAMERA_ST.LoadCameraState();

	g_ObjectMng.TerrainObjectToSelectModel( m_pBackupSelectObjectInfo->s_dwUniqID );
	m_pBackupSelectObjectInfo = NULL;	
}
void _DlgMTObj_Portal::OnCbnSelchangeComboPortalType()
{
	m_pPortalObj->SetPortalType( (DWORD)m_combo_Portal_Type.GetItemData(m_combo_Portal_Type.GetCurSel()) );
}


