// _DlgMainTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgMainTab.h"


// _DlgMainTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMainTab, CDialog)

_DlgMainTab::_DlgMainTab(CWnd* pParent /*=NULL*/)
: _DlgBase( _DlgMainTab::IDD, pParent )
{

}

_DlgMainTab::~_DlgMainTab()
{
}

void _DlgMainTab::_Delete()
{
	_DlgBase::_Delete();

	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->_Delete();
	}
}

void _DlgMainTab::_ViewInit()
{
	_DlgBase::_ViewInit();
	
	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->_ViewInit();
	}
}

void _DlgMainTab::_PreViewReset()
{
	_DlgBase::_PreViewReset();
	
	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->_PreViewReset();
	}
}

void _DlgMainTab::_PostViewReset()
{
	_DlgBase::_PostViewReset();
	
	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->_PostViewReset();
	}
}

void _DlgMainTab::_UpdateView()
{
	_DlgBase::_UpdateView();

	m_mapDlg[ m_tabMain.GetCurSel() ]->_UpdateView();
}

void _DlgMainTab::_PreRenderView()
{
	_DlgBase::_PreRenderView();

	m_mapDlg[ m_tabMain.GetCurSel() ]->_PreRenderView();
}

void _DlgMainTab::_PostRenderView()
{
	_DlgBase::_PostRenderView();

	m_mapDlg[ m_tabMain.GetCurSel() ]->_PostRenderView();
}

void _DlgMainTab::_PreSave( const char* cFile, bool bAutoSave )
{
	_DlgBase::_PreSave( cFile, bAutoSave );

	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->_PreSave( cFile, bAutoSave );
	}
}

void _DlgMainTab::_OnSlide_SLeft()
{
	_DlgBase::_OnSlide_SLeft();

	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->_OnSlide_SLeft();
	}
}

void _DlgMainTab::_OnSlide_SRight()
{
	_DlgBase::_OnSlide_SRight();

	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->_OnSlide_SRight();
	}
}

void _DlgMainTab::_OnSlide_BLeft()
{
	_DlgBase::_OnSlide_BLeft();

	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->_OnSlide_BLeft();
	}
}

void _DlgMainTab::_OnSlide_BRight()
{
	_DlgBase::_OnSlide_BRight();

	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->_OnSlide_BRight();
	}
}

bool _DlgMainTab::_PreCheckKey( DWORD vKeyCode, bool bDown )
{
	return m_mapDlg[ m_tabMain.GetCurSel() ]->_PreCheckKey( vKeyCode, bDown );
}
void	_DlgMainTab::OnKeyUp(const DWORD msg)
{	
	std::map< DWORD, _DlgBase* >::iterator kIter = m_mapDlg.find(m_tabMain.GetCurSel());
	if(kIter != m_mapDlg.end())
	{
		kIter->second->OnKeyUp(msg);
	}

}
void _DlgMainTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_tabMain);
}

void _DlgMainTab::_Reset()
{
	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->ShowWindow( it->second != &m_DlgMTMain ? SW_HIDE : SW_SHOW );
	}
	m_DlgMTMapping._Reset();
	m_DlgMTObject._Reset();
}

void _DlgMainTab::_Resist()
{
	m_DlgMTMapping._Resist();
	m_DlgMTObject._Resist();
}

void _DlgMainTab::_OnLButtonDown( CPoint pt )
{
	m_mapDlg[ m_tabMain.GetCurSel() ]->_OnLButtonDown( pt );
}

void _DlgMainTab::_OnLButtonDbClick( CPoint pt )
{
	m_mapDlg[ m_tabMain.GetCurSel() ]->_OnLButtonDbClick( pt );
}

void _DlgMainTab::_OnLButtonUp( CPoint pt )
{
	m_mapDlg[ m_tabMain.GetCurSel() ]->_OnLButtonUp( pt );
}

void _DlgMainTab::_OnMouseMove( CPoint pt )
{
	m_mapDlg[ m_tabMain.GetCurSel() ]->_OnMouseMove( pt );
}

void _DlgMainTab::_OnMouseWheel( short zDelta )
{
	m_mapDlg[ m_tabMain.GetCurSel() ]->_OnMouseWheel( zDelta );
}

void _DlgMainTab::_KeyDownEscape()
{
	m_mapDlg[ m_tabMain.GetCurSel() ]->_KeyDownEscape();
}

BEGIN_MESSAGE_MAP(_DlgMainTab, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &_DlgMainTab::OnTcnSelchangeTabMain)
END_MESSAGE_MAP()


// _DlgMainTab 메시지 처리기입니다.

BOOL _DlgMainTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_mapDlg[ TT_MAIN ] = &m_DlgMTMain;
	m_mapDlg[ TT_MAP ] = &m_DlgMTMap;
	m_mapDlg[ TT_HEIGHT ] = &m_DlgMTHeight;
	m_mapDlg[ TT_MAPPING ] = &m_DlgMTMapping;
	m_mapDlg[ TT_OBJECT ] = &m_DlgMTObject;
	m_mapDlg[ TT_CELLOBJECT ] = &m_DlgMTCellObject;
	m_mapDlg[ TT_MONSTOR ] = &m_DlgMTMonster;
	m_mapDlg[ TT_MODEL ] = &m_DlgMTModel;
	m_mapDlg[ TT_SHADOW ] = &m_DlgMTShadow;
	m_mapDlg[ TT_LIGHT ] = &m_DlgMTLight;
	m_mapDlg[ TT_PLAYER ] = &m_DlgMTPlayer;
	m_mapDlg[ TT_PATH ] = &m_DlgMTPath;
	m_mapDlg[ TT_WEATHER ] = &m_DlgMTWeather;
	m_mapDlg[ TT_ETC ] = &m_DlgMTEtc;

	CRect rc;
	GetWindowRect( &rc );
	int nHeight = rc.Height() - 50;

	std::map< DWORD, _DlgBase* >::iterator itEnd = m_mapDlg.end();
	std::map< DWORD, _DlgBase* >::iterator it = m_mapDlg.begin();
	for( ;it!=itEnd; ++it )
	{
		it->second->Create( it->second->_GetDlgID(), this );
		it->second->SetWindowPos( NULL, 0, 0, rc.Width(), nHeight, 0 );
		it->second->ShowWindow( it->second != &m_DlgMTMain ? SW_HIDE : SW_SHOW );
	}

	m_tabMain.InsertItem( TT_MAIN, L"메인메뉴" );	
	m_tabMain.InsertItem( TT_MAP, L"맵" );
	m_tabMain.InsertItem( TT_HEIGHT, L"높이맵" );
	m_tabMain.InsertItem( TT_MAPPING, L"맵핑" );	
	m_tabMain.InsertItem( TT_OBJECT, L"오브젝트" );
	m_tabMain.InsertItem( TT_CELLOBJECT, L"셀 오브젝트" );
	m_tabMain.InsertItem( TT_MONSTOR, L"몬스터" );
	m_tabMain.InsertItem( TT_MODEL, L"NPC" );
	m_tabMain.InsertItem( TT_SHADOW, L"그림자" );
	m_tabMain.InsertItem( TT_LIGHT, L"라이트" );
	m_tabMain.InsertItem( TT_PLAYER, L"플레이어" );
	m_tabMain.InsertItem( TT_PATH, L"패쓰엔진" );
	m_tabMain.InsertItem( TT_WEATHER, L"날씨" );
	m_tabMain.InsertItem( TT_ETC, L"기타" );

	m_tabMain.SetCurSel( TT_MAIN );

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgMainTab::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( ( m_tabMain.GetCurSel() != TT_MAIN )&&
		( ( nsCsGBTerrain::g_pCurRoot == NULL )||(nsCsGBTerrain::g_pCurRoot->GetRootNode()->IsExistTerrain() == false) ) )
	{
		m_tabMain.SetCurSel( TT_MAIN );
		MessageBox( L"맵을 생성하거나 로드하여 주십시오.");
		return;
	}

	switch( m_tabMain.GetCurSel() )
	{	
	case TT_MAPPING:
		g_Preview.SetPreview( cPreviewObj::POT_TEXTURE );
		break;

	case TT_OBJECT:
	case TT_CELLOBJECT:
		g_Preview.SetPreview( cPreviewObj::POT_TERRAIN_OBJ );
		break;

	case TT_MONSTOR:
	case TT_MODEL:
	case TT_WEATHER:
		if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID == 0 )
		{
			MessageBox( L"맵 ID 를 설정 하여 주십시오." );
			m_tabMain.SetCurSel( TT_MAP );
			NMHDR nmhdr;
			LRESULT result;
			OnTcnSelchangeTabMain( &nmhdr, &result );
			break;
		}
		g_Preview.SetPreview( cPreviewObj::POT_CHAR );
		break;

	default:
		g_Preview.SetPreview( cPreviewObj::POT_NONE );
	}

	g_ToolNiNodeMng.OnResetGizmo();
	for( int i=0; i<TT_COUNT; ++i )
	{
		m_mapDlg[ i ]->ShowWindow( m_tabMain.GetCurSel() == i ? SW_SHOW : SW_HIDE );
	}
}

