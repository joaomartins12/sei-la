// _DlgMTMap_LimitEvo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTMap_LimitEvo.h"


// _DlgMTMap_LimitEvo 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTMap_LimitEvo, CDialog)

_DlgMTMap_LimitEvo::_DlgMTMap_LimitEvo(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTMap_LimitEvo::IDD, pParent)
{
	m_pSelLimitEvo = NULL;
}

_DlgMTMap_LimitEvo::~_DlgMTMap_LimitEvo()
{
}

void _DlgMTMap_LimitEvo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STARTX, m_edit_StartPosX);
	DDX_Control(pDX, IDC_EDIT_STARTY, m_edit_StartPosY);
	DDX_Control(pDX, IDC_SLIDER_RADIUS, m_sld_Radius);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_edit_Radius);
}


BEGIN_MESSAGE_MAP(_DlgMTMap_LimitEvo, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// _DlgMTMap_LimitEvo 메시지 처리기입니다.



// =================================================================================================
//
//		virtual
//
// =================================================================================================
void _DlgMTMap_LimitEvo::_PostRenderView()
{
	CsMap_LimitEvoGroup* pGroup = GET_MV_MAINTAB_DLG->_GetDlgMap()->_GetLimitEvGoroup();
	CsMap_LimitEvo::LIST* plist = pGroup->GetList();
	CsMap_LimitEvo::LIST_IT it = plist->begin();
	for( ; it!=plist->end(); ++it )
	{
		NiPoint2 ptPos = NiPoint2( (float)(*it)->GetInfo()->s_nCenterX, (float)(*it)->GetInfo()->s_nCenterY );
		if( m_pSelLimitEvo == (*it) )
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
BOOL _DlgMTMap_LimitEvo::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_sld_Radius.SetRange( 100, 50000 );

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgMTMap_LimitEvo::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	__ReleaseControl();
	if( bShow == FALSE )
	{
		m_pSelLimitEvo = NULL;
	}
}

void _DlgMTMap_LimitEvo::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_Radius.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_Radius, m_sld_Radius.GetPos() );
		m_pSelLimitEvo->GetInfo()->s_nRadius = m_sld_Radius.GetPos();
	}

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}


void _DlgMTMap_LimitEvo::_SetLimitEvo( CsMap_LimitEvo* pSrc )
{
	m_pSelLimitEvo = pSrc;

	_Int2Control( &m_edit_StartPosX, m_pSelLimitEvo->GetInfo()->s_nCenterX );
	_Int2Control( &m_edit_StartPosY, m_pSelLimitEvo->GetInfo()->s_nCenterY );
	m_sld_Radius.EnableWindow( true );
	m_sld_Radius.SetPos( m_pSelLimitEvo->GetInfo()->s_nRadius );
	_Int2Control( &m_edit_Radius, m_sld_Radius.GetPos() );
}


//=====================================================================================================
//
//		Control
//
//=====================================================================================================

void _DlgMTMap_LimitEvo::__ReleaseControl()
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
bool _DlgMTMap_LimitEvo::_IsLButtonDown( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return false;

	m_pSelLimitEvo->GetInfo()->s_nCenterX = (int)pRecord->GetIntersection().x;
	m_pSelLimitEvo->GetInfo()->s_nCenterY = (int)pRecord->GetIntersection().y;
	_SetLimitEvo( m_pSelLimitEvo );
	return true;
}

