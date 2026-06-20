// _DlgMTMap_Resurrection.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTMap_Resurrection.h"


// _DlgMTMap_Resurrection 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTMap_Resurrection, CDialog)

_DlgMTMap_Resurrection::_DlgMTMap_Resurrection(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTMap_Resurrection::IDD, pParent)
{
	m_pSelResurrection = NULL;
}

_DlgMTMap_Resurrection::~_DlgMTMap_Resurrection()
{
}

void _DlgMTMap_Resurrection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STARTX, m_edit_StartPosX);
	DDX_Control(pDX, IDC_EDIT_STARTY, m_edit_StartPosY);
	DDX_Control(pDX, IDC_SLIDER_RADIUS, m_sld_Radius);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_edit_Radius);
}


BEGIN_MESSAGE_MAP(_DlgMTMap_Resurrection, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// =================================================================================================
//
//		virtual
//
// =================================================================================================
void _DlgMTMap_Resurrection::_PostRenderView()
{
	CsMapResurrectionGroup* pGroup = GET_MV_MAINTAB_DLG->_GetDlgMap()->_GetResurrectionGroup();
	CsMapResurrection::LIST* plist = pGroup->GetResurrectionList();
	CsMapResurrection::LIST_IT it = plist->begin();
	for( ; it!=plist->end(); ++it )
	{
		bool bSelected = ( m_pSelResurrection == (*it) )? true : false;
		NiPoint2 ptPos = NiPoint2( (float)(*it)->GetInfo()->s_nCenterX, (float)(*it)->GetInfo()->s_nCenterY );
		g_ToolNiNodeMng.GetRegion()->Render( ptPos, (float)(*it)->GetInfo()->s_nRadius, bSelected );
		//g_ToolNiNodeMng.RenderArrow( ptPos, NiPoint3::ZERO, bSelected );
	}
}

// =================================================================================================
//
//		Base
//
// =================================================================================================
BOOL _DlgMTMap_Resurrection::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_sld_Radius.SetRange( 100, 4000 );

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgMTMap_Resurrection::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	__ReleaseControl();
	if( bShow == FALSE )
	{
		m_pSelResurrection = NULL;
	}
}

void _DlgMTMap_Resurrection::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_Radius.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_Radius, m_sld_Radius.GetPos() );
		m_pSelResurrection->GetInfo()->s_nRadius = m_sld_Radius.GetPos();
	}

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}


void _DlgMTMap_Resurrection::_SetResurrection( CsMapResurrection* pSrc )
{
	m_pSelResurrection = pSrc;

	_Int2Control( &m_edit_StartPosX, m_pSelResurrection->GetInfo()->s_nCenterX );
	_Int2Control( &m_edit_StartPosY, m_pSelResurrection->GetInfo()->s_nCenterY );
	m_sld_Radius.EnableWindow( true );
	m_sld_Radius.SetPos( m_pSelResurrection->GetInfo()->s_nRadius );
	_Int2Control( &m_edit_Radius, m_sld_Radius.GetPos() );
}


//=====================================================================================================
//
//		Control
//
//=====================================================================================================

void _DlgMTMap_Resurrection::__ReleaseControl()
{
	m_edit_StartPosX.SetWindowText( L"" );
	m_edit_StartPosY.SetWindowText( L"" );
	m_edit_Radius.SetWindowText( L"" );
	m_sld_Radius.EnableWindow( false );
}


// =================================================================================================
//
//		Input
//
// =================================================================================================
bool _DlgMTMap_Resurrection::_IsLButtonDown( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return false;

	m_pSelResurrection->GetInfo()->s_nCenterX = (int)pRecord->GetIntersection().x;
	m_pSelResurrection->GetInfo()->s_nCenterY = (int)pRecord->GetIntersection().y;
	_SetResurrection( m_pSelResurrection );
	return true;
}
