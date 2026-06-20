// _DlgMTMap_Start.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTMap_Start.h"


// _DlgMTMap_Start 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTMap_Start, CDialog)

_DlgMTMap_Start::_DlgMTMap_Start(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTMap_Start::IDD, pParent)
{
	m_pSelStart = NULL;
}

_DlgMTMap_Start::~_DlgMTMap_Start()
{
}

void _DlgMTMap_Start::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STARTX, m_edit_StartPosX);
	DDX_Control(pDX, IDC_EDIT_STARTY, m_edit_StartPosY);
	DDX_Control(pDX, IDC_SLIDER_RADIUS, m_sld_Radius);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_edit_Radius);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_combo_Type);
}


BEGIN_MESSAGE_MAP(_DlgMTMap_Start, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &_DlgMTMap_Start::OnCbnSelchangeComboType)
END_MESSAGE_MAP()


// =================================================================================================
//
//		virtual
//
// =================================================================================================
void _DlgMTMap_Start::_PostRenderView()
{
	CsMapStartGroup* pGroup = GET_MV_MAINTAB_DLG->_GetDlgMap()->_GetStartGroup();
	CsMapStart::LIST* plistStart = pGroup->GetStartList();
	CsMapStart::LIST_IT it = plistStart->begin();
	for( ; it!=plistStart->end(); ++it )
	{
		bool bSelected = ( m_pSelStart == (*it) )? true : false;
		NiPoint2 ptPos = NiPoint2( (float)(*it)->GetInfo()->s_nCenterX, (float)(*it)->GetInfo()->s_nCenterY );
		g_ToolNiNodeMng.GetRegion()->Render( ptPos, (float)(*it)->GetInfo()->s_nRadius, bSelected );
	//	g_ToolNiNodeMng.RenderArrow( ptPos, NiPoint3::ZERO, bSelected );
	}
}

// =================================================================================================
//
//		Base
//
// =================================================================================================
BOOL _DlgMTMap_Start::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_sld_Radius.SetRange( 100, 4000 );

	m_combo_Type.InsertString( 0, L"이동" );
	m_combo_Type.InsertString( 1, L"케릭터 생성" );
	m_combo_Type.SetCurSel( 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgMTMap_Start::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	__ReleaseControl();
	if( bShow == FALSE )
	{
		m_pSelStart = NULL;
	}
}

void _DlgMTMap_Start::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_Radius.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_Radius, m_sld_Radius.GetPos() );
		m_pSelStart->GetInfo()->s_nRadius = m_sld_Radius.GetPos();
	}

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}


void _DlgMTMap_Start::_SetStart( CsMapStart* pStart )
{
	m_pSelStart = pStart;

	_Int2Control( &m_edit_StartPosX, m_pSelStart->GetInfo()->s_nCenterX );
	_Int2Control( &m_edit_StartPosY, m_pSelStart->GetInfo()->s_nCenterY );
	m_sld_Radius.EnableWindow( true );
	m_sld_Radius.SetPos( m_pSelStart->GetInfo()->s_nRadius );
	_Int2Control( &m_edit_Radius, m_sld_Radius.GetPos() );

	m_combo_Type.SetCurSel( m_pSelStart->GetInfo()->s_nType );
}

//=====================================================================================================
//
//		Control
//
//=====================================================================================================

void _DlgMTMap_Start::__ReleaseControl()
{
	m_edit_StartPosX.SetWindowText( L"" );
	m_edit_StartPosY.SetWindowText( L"" );
	m_edit_Radius.SetWindowText( L"" );
	m_sld_Radius.EnableWindow( false );
	m_combo_Type.SetCurSel( 0 );
}


// =================================================================================================
//
//		Input
//
// =================================================================================================
bool _DlgMTMap_Start::_IsLButtonDown( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return false;

	m_pSelStart->GetInfo()->s_nCenterX = (int)pRecord->GetIntersection().x;
	m_pSelStart->GetInfo()->s_nCenterY = (int)pRecord->GetIntersection().y;
	_SetStart( m_pSelStart );
	return true;
}

void _DlgMTMap_Start::OnCbnSelchangeComboType()
{
	assert_cs( m_pSelStart != NULL );
	m_pSelStart->GetInfo()->s_nType = m_combo_Type.GetCurSel();
}
