// _DlgMTObj_Ani.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTObj_Ani.h"


// _DlgMTObj_Ani 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTObj_Ani, CDialog)

_DlgMTObj_Ani::_DlgMTObj_Ani(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTObj_Ani::IDD, pParent)
{
	m_pObjectModel = NULL;
}

_DlgMTObj_Ani::~_DlgMTObj_Ani()
{
}

void _DlgMTObj_Ani::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_chk_DoAnimation);
	DDX_Control(pDX, IDC_SLIDER_ANISPEED, m_sld_AniSpeed);
	DDX_Control(pDX, IDC_SLIDER_FRAME, m_sld_AniFrame);
	DDX_Control(pDX, IDC_EDIT_ANISPEED, m_edit_AniSpeed);
	DDX_Control(pDX, IDC_EDIT_FRAME, m_edit_AniFrame);
}


BEGIN_MESSAGE_MAP(_DlgMTObj_Ani, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK1, &_DlgMTObj_Ani::OnBnClickedCheck1)
END_MESSAGE_MAP()


// _DlgMTObj_Ani 메시지 처리기입니다.

BOOL _DlgMTObj_Ani::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_sld_AniSpeed.SetRange( 1, 500 );

	return TRUE;
}


void _DlgMTObj_Ani::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		//m_pCsGBObject = GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetUniqCsGBObject();
		assert_cs( g_ObjectMng.GetListSelectModel()->size() == 1 );
		m_pObjectModel = ( *g_ObjectMng.GetListSelectModel()->begin() );
		if( __ReadAniInfo() == false )
		{
			CsMessageBoxA( MB_OK, "%s 오브젝트에는 애니메이션이 존재하지 않습니다.\n프로퍼티 확인해주세요", m_pObjectModel->GetObjectInfo()->s_cObjectPath );
			m_pObjectModel = NULL;
			ShowWindow( SW_HIDE );
		}
	}
	else
	{
		m_pObjectModel = NULL;
	}
}

void _DlgMTObj_Ani::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_AniSpeed.GetSafeHwnd() )
	{
		m_pObjectModel->GetObjectInfo()->s_fAniSpeed = m_sld_AniSpeed.GetPos()*0.01f;
		nsCsMfcFunc::Float2Control( &m_edit_AniSpeed, m_pObjectModel->GetObjectInfo()->s_fAniSpeed, 2 );
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_AniFrame.GetSafeHwnd() )
	{
		if( m_chk_DoAnimation.GetCheck() == BST_UNCHECKED )
		{
			m_pObjectModel->GetObjectInfo()->s_fStopAniTime = m_sld_AniFrame.GetPos()*0.001f;
			nsCsMfcFunc::Float2Control( &m_edit_AniFrame, m_pObjectModel->GetObjectInfo()->s_fStopAniTime, 3 );
		}		
	}

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

void _DlgMTObj_Ani::OnBnClickedCheck1()
{
	m_pObjectModel->GetObjectInfo()->s_fStopAniTime = m_sld_AniFrame.GetPos()*0.001f;
	m_pObjectModel->GetObjectInfo()->s_bCheckDoAnimation = m_chk_DoAnimation.GetCheck() == BST_CHECKED;
}


void _DlgMTObj_Ani::_UpdateView()
{
	if( m_pObjectModel == NULL )
		return;
	
	int nAniTime = (int)( m_pObjectModel->GetAniTime()*1000.0f )%(int)( m_fEndAniTime*1000.0f );
	m_sld_AniFrame.SetPos( nAniTime );
	nsCsMfcFunc::Float2Control( &m_edit_AniFrame, nAniTime*0.001f, 3 );

}

bool _DlgMTObj_Ani::__ReadAniInfo()
{
	CsGBObject::sINFO* pCsObjInfo = m_pObjectModel->GetObjectInfo();

	m_chk_DoAnimation.SetCheck( pCsObjInfo->s_bCheckDoAnimation == true ? BST_CHECKED : BST_UNCHECKED );
	m_sld_AniSpeed.SetPos( (int)(pCsObjInfo->s_fAniSpeed*100.0f ) );
	nsCsMfcFunc::Float2Control( &m_edit_AniSpeed, pCsObjInfo->s_fAniSpeed, 2 );

	nsCSGBFUNC::EndAnimationTime( m_fEndAniTime, m_pObjectModel->GetNiNode() );

	if( m_fEndAniTime == 0.0f )
	{
		return false;
	}
	m_sld_AniFrame.SetRange( 0, (int)( m_fEndAniTime*1000.0f ) );
	int nAniTime = (int)( m_pObjectModel->GetAniTime()*1000.0f )%(int)( m_fEndAniTime*1000.0f );
	m_sld_AniFrame.SetPos( nAniTime );
	nsCsMfcFunc::Float2Control( &m_edit_AniFrame, nAniTime*0.001f, 3 );

	return true;
}




