// _DlgMTWeather.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTWeather.h"


// _DlgMTWeather 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTWeather, CDialog)

_DlgMTWeather::_DlgMTWeather(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTWeather::IDD, pParent)
{
}

_DlgMTWeather::~_DlgMTWeather()
{
}


BOOL _DlgMTWeather::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	DWORD dwExStyle = m_listZone.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_listZone.SetExtendedStyle( dwExStyle );
	m_listZone.InsertColumn( 0, L"Idx", LVCFMT_LEFT, 40 );	
	m_listZone.InsertColumn( 1, L"x", LVCFMT_LEFT, 80 );
	m_listZone.InsertColumn( 2, L"y", LVCFMT_LEFT, 80 );
	m_listZone.InsertColumn( 3, L"EleCount", LVCFMT_LEFT, 100 );	

	dwExStyle = m_listElement.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_listElement.SetExtendedStyle( dwExStyle );
	m_listElement.InsertColumn( 0, L"type", LVCFMT_LEFT, 70 );
	m_listElement.InsertColumn( 1, L"시간", LVCFMT_LEFT, 70 );
	m_listElement.InsertColumn( 2, L"농도", LVCFMT_LEFT, 70 );	
	m_listElement.InsertColumn( 3, L"피로도", LVCFMT_LEFT, 70 );

	m_cmb_Type.InsertString( 0, L"없음" );
	m_cmb_Type.InsertString( 1, L"눈" );
	m_cmb_Type.InsertString( 2, L"비" );
	m_cmb_Type.SetCurSel( 0 );

	TCHAR sz[ 16 ];
	for( int i=1; i<7; ++i )
	{
		_stprintf_s( sz, 16, L"%d", i );
		int nIndex = m_cmb_Level.GetCount();
		m_cmb_Level.InsertString( nIndex, sz );
		m_cmb_Level.SetItemData( nIndex, i );
	}
	m_cmb_Level.SetCurSel( 0 );

	m_sld_Size.SetRange( 1000, 100000 );

#pragma warning(disable: 4312)
	m_pCurSelectZone = (CsWeather*)0xffffffff;
	m_pCurSelectElement = (CsWeather::sELEMENT*)0xffffffff;
#pragma warning(default: 4312)

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgMTWeather::_PostViewReset()
{	
	__ResetZoneList( NULL );

	DWORD nCurID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsMapTable::g_pWeatherMng->IsGroup( nCurID ) )
	{
		CsWeatherGroup* pGroup = nsCsMapTable::g_pWeatherMng->GetGroup( nCurID );
		CsWeather::LIST* pList = pGroup->GetList();
		if( pList->empty() == false )
		{
			CsWeather* pWeather = *pList->begin();
			nsCsMfcFunc::Int2Control( &m_edit_LoopTime, pWeather->GetInfo()->s_nTotalLoopMin );
		}
	}	
}

void _DlgMTWeather::_PostRenderView()
{
	DWORD nCurID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsMapTable::g_pWeatherMng->IsGroup( nCurID ) == false )
		return;

	CsWeatherGroup* pGroup = nsCsMapTable::g_pWeatherMng->GetGroup( nCurID );
	CsWeather::LIST* pList = pGroup->GetList();
	CsWeather::LIST_IT it = pList->begin();
	CsWeather::LIST_IT itEnd = pList->end();
	CsWeather::sINFO* pWInfo = NULL;
	for( ; it!=itEnd; ++it )
	{
		pWInfo = (*it)->GetInfo();
		g_ToolNiNodeMng.GetRegion()->Render( NiPoint2( (float)pWInfo->s_nCenterX, (float)pWInfo->s_nCenterY ),	(float)pWInfo->s_nRadius, (*it) == m_pCurSelectZone );
	}
}

void _DlgMTWeather::_PreSave( const char* cFile, bool bAutoSave )
{
	__CheckTimeZone();
}

void _DlgMTWeather::_OnLButtonDown( CPoint pt )
{
	if( m_pCurSelectZone == NULL )
		return;

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord != NULL )
	{
		NiPoint3 ptIntersect = pRecord->GetIntersection();		
		m_pCurSelectZone->GetInfo()->s_nCenterX = (int)ptIntersect.x;
		m_pCurSelectZone->GetInfo()->s_nCenterY = (int)ptIntersect.y;
		__ResetZoneList( m_pCurSelectZone );
	}
}

void _DlgMTWeather::_KeyDownEscape()
{
	{
		POSITION pos = m_listZone.GetFirstSelectedItemPosition();
		int nItem = m_listZone.GetNextSelectedItem( pos );
		if( nItem > -1 )
		{
			m_listZone.SetItemState( nItem, 0, LVIS_SELECTED );
		}
	}

	{
		POSITION pos = m_listElement.GetFirstSelectedItemPosition();
		int nItem = m_listElement.GetNextSelectedItem( pos );
		if( nItem > -1 )
		{
			m_listElement.SetItemState( nItem, 0, LVIS_SELECTED );
		}
	}

	__SetCurSelectZone( NULL );
}

void _DlgMTWeather::OnEnChangeEditZoneLooptime()
{
	DWORD nCurID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsMapTable::g_pWeatherMng->IsGroup( nCurID ) == false )
		return;

	int nMin = nsCsMfcFunc::Control2Int( &m_edit_LoopTime );
	CsWeatherGroup* pGroup = nsCsMapTable::g_pWeatherMng->GetGroup( nCurID );
	CsWeather::LIST* pList = pGroup->GetList();
	CsWeather::LIST_IT it = pList->begin();
	CsWeather::LIST_IT itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->GetInfo()->s_nTotalLoopMin = nMin;
	}
}

void _DlgMTWeather::__ResetZoneList( CsWeather* pSelect )
{
	__SetCurSelectZone( pSelect );

	m_listZone.DeleteAllItems();

	DWORD nCurID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsMapTable::g_pWeatherMng->IsGroup( nCurID ) == false )
		return;

	TCHAR sz[ 128 ];
	int nIDX = nCurID*100;
	CsWeatherGroup* pGroup = nsCsMapTable::g_pWeatherMng->GetGroup( nCurID );
	CsWeather::LIST* pList = pGroup->GetList();
	CsWeather::LIST_IT it = pList->begin();
	CsWeather::LIST_IT itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		++nIDX;
		int nListIndex = m_listZone.GetItemCount();
		CsWeather::sINFO* pWInfo = (*it)->GetInfo();

		_stprintf_s( sz, 128, L"%d", nIDX );
		m_listZone.InsertItem( nListIndex, sz );
		_stprintf_s( sz, 128, L"%d", (int)pWInfo->s_nCenterX );
		m_listZone.SetItem( nListIndex, 1, LVIF_TEXT, sz, 0, 0, 0, 0 );
		_stprintf_s( sz, 128, L"%d", (int)pWInfo->s_nCenterY );
		m_listZone.SetItem( nListIndex, 2, LVIF_TEXT, sz, 0, 0, 0, 0 );

		_stprintf_s( sz, 128, L"%d", (int)pWInfo->s_nElementCount );
		m_listZone.SetItem( nListIndex, 3, LVIF_TEXT, sz, 0, 0, 0, 0 );

		m_listZone.SetItemData( nListIndex, (DWORD_PTR)(*it) );
	}

	if( pSelect )
	{
		int nCnt = m_listZone.GetItemCount();
		for( int i=0; i<nCnt; ++i )
		{
			if( m_listZone.GetItemData( i ) == (DWORD_PTR)pSelect )
			{
				m_listZone.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
				break;
			}
		}
	}
}

void _DlgMTWeather::__SetCurSelectZone( CsWeather* pSelect )
{
	m_pCurSelectZone = pSelect;
	if( m_pCurSelectZone != NULL )
	{
		m_sld_Size.SetPos( m_pCurSelectZone->GetInfo()->s_nRadius );
	}

	__ResetElementList( NULL );
}

void _DlgMTWeather::__CheckTimeZone()
{
	// 총 토탈 길이에 맞춰서 검사
	DWORD nCurID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsMapTable::g_pWeatherMng->IsGroup( nCurID ) == false )
		return;

	CsWeatherGroup* pGroup = nsCsMapTable::g_pWeatherMng->GetGroup( nCurID );
	CsWeather::LIST* pList = pGroup->GetList();
	CsWeather::LIST_IT it = pList->begin();
	CsWeather::LIST_IT itEnd = pList->end();
	CsWeather::sINFO* pWInfo = NULL;
	for( ;it!=itEnd; ++it )
	{
		__CheckTimeElement( *it );
	}

	__ResetZoneList( NULL );
	_KeyDownEscape();
}

void _DlgMTWeather::OnNMClickListZone(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_listZone.GetFirstSelectedItemPosition();
	int nItem = m_listZone.GetNextSelectedItem( pos );
	if( nItem < 0 )
	{
		__SetCurSelectZone( NULL );		
		return;
	}

	__SetCurSelectZone( (CsWeather*)m_listZone.GetItemData( nItem ) );
	*pResult = 0;
}

// _DlgMTWeather 메시지 처리기입니다.
void _DlgMTWeather::OnBnClickedButtonAddZone()
{
	CsWeather::sINFO info;
	info.s_dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	info.s_nTotalLoopMin = nsCsMfcFunc::Control2Int( &m_edit_LoopTime );
	CsWeather* pWeather = nsCsMapTable::g_pWeatherMng->GetGroup( info.s_dwMapID )->Add( &info );
	__ResetZoneList( pWeather );
}

void _DlgMTWeather::OnBnClickedButtonDelZone()
{
	POSITION pos = m_listZone.GetFirstSelectedItemPosition();
	int nItem = m_listZone.GetNextSelectedItem( pos );
	if( nItem < 0 )
	{
		return;
	}

	nsCsMapTable::g_pWeatherMng->GetGroup( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )->Delete( m_pCurSelectZone );
	__ResetZoneList( NULL );
}













void _DlgMTWeather::__ResetElementList( CsWeather::sELEMENT* pSelect )
{
	__SetCurSelectElement( pSelect );
	m_listElement.DeleteAllItems();

	if( m_pCurSelectZone == NULL )
		return;

	TCHAR sz[ 128 ];
	CsWeather::LISTELE* pList = m_pCurSelectZone->GetElementList();
	CsWeather::LISTELE_IT it = pList->begin();
	CsWeather::LISTELE_IT itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		int nListIndex = m_listElement.GetItemCount();
		CsWeather::sELEMENT* pElement = (*it);

		switch( pElement->s_Type )
		{
		case CsWeather::WT_NONE:
			m_listElement.InsertItem( nListIndex, L"없음" );
			break;
		case CsWeather::WT_SNOW:
			m_listElement.InsertItem( nListIndex, L"눈" );
			break;
		case CsWeather::WT_RAIN:
			m_listElement.InsertItem( nListIndex, L"비" );
			break;
		default:
			assert_cs( false );
		}

		_stprintf_s( sz, 128, L"%d", pElement->s_nTime_Min );
		m_listElement.SetItem( nListIndex, 1, LVIF_TEXT, sz, 0, 0, 0, 0 );

		_stprintf_s( sz, 128, L"%d", pElement->s_nLevel );
		m_listElement.SetItem( nListIndex, 2, LVIF_TEXT, sz, 0, 0, 0, 0 );

		_stprintf_s( sz, 128, L"%d", pElement->s_nFatigue );
		m_listElement.SetItem( nListIndex, 3, LVIF_TEXT, sz, 0, 0, 0, 0 );

		m_listElement.SetItemData( nListIndex, (DWORD_PTR)pElement );
	}

	if( pSelect )
	{
		int nCnt = m_listElement.GetItemCount();
		for( int i=0; i<nCnt; ++i )
		{
			if( m_listElement.GetItemData( i ) == (DWORD_PTR)pSelect )
			{
				m_listElement.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
				break;
			}
		}
	}
}

void _DlgMTWeather::__SetCurSelectElement( CsWeather::sELEMENT* pSelect )
{
	if( m_pCurSelectElement == pSelect )
		return;

	m_pCurSelectElement = pSelect;
	if( m_pCurSelectElement )
	{
		m_cmb_Type.EnableWindow( TRUE );
		m_cmb_Level.EnableWindow( TRUE );
		m_edit_Time.EnableWindow( TRUE );
		m_edit_Fatigue.EnableWindow( TRUE );

		m_cmb_Type.SetCurSel( m_pCurSelectElement->s_Type );
		nsCsMfcFunc::ComboSelect_DWORD_Data( &m_cmb_Level, m_pCurSelectElement->s_nLevel );
		nsCsMfcFunc::Int2Control( &m_edit_Time, m_pCurSelectElement->s_nTime_Min );
		nsCsMfcFunc::Int2Control( &m_edit_Fatigue, m_pCurSelectElement->s_nFatigue );
	}
	else
	{
		m_cmb_Type.EnableWindow( FALSE );
		m_cmb_Level.EnableWindow( FALSE );
		m_edit_Time.EnableWindow( FALSE );
		m_edit_Fatigue.EnableWindow( FALSE );
	}
}

void _DlgMTWeather::__CheckTimeElement( CsWeather* pWeather )
{
	CsWeather::LISTELE* pList = pWeather->GetElementList();
	if( pList->empty() == true )
		return;

	int nTotalTime = pWeather->GetInfo()->s_nTotalLoopMin;

	CsWeather::LISTELE_IT it = pList->begin();
	CsWeather::LISTELE_IT itEnd = pList->end();
	CsWeather::sELEMENT* pElement = NULL;
	while( it != itEnd )
	{
		pElement = *it;
		if( nTotalTime )
		{
			if( nTotalTime < pElement->s_nTime_Min )
			{
				pElement->s_nTime_Min = nTotalTime;
				nTotalTime = 0;
			}
			else
			{
				nTotalTime -= pElement->s_nTime_Min;
			}
			++it;
		}
		// 시간 오바된거는 다 지워준다.
		else
		{
			delete pElement;			
			it = pList->erase( it );
			pWeather->GetInfo()->s_nElementCount = (int)pList->size();
		}
	}

	// 시간이 남아 있다면 가장 마지막거에 남은 시간 추가해주자
	if( nTotalTime )
	{
		pElement->s_nTime_Min += nTotalTime;
	}
}

void _DlgMTWeather::OnNMClickListElement(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_listElement.GetFirstSelectedItemPosition();
	int nItem = m_listElement.GetNextSelectedItem( pos );
	if( nItem < 0 )
	{
		__SetCurSelectElement( NULL );		
		return;
	}

	__SetCurSelectElement( (CsWeather::sELEMENT*)m_listElement.GetItemData( nItem ) );
	*pResult = 0;
}


void _DlgMTWeather::OnBnClickedButtonAddElement()
{
	if( m_pCurSelectZone == NULL )
		return;

	CsWeather::sELEMENT ele;
	CsWeather::sELEMENT* pElement = m_pCurSelectZone->AddElement( &ele );
	__ResetZoneList( m_pCurSelectZone );
	__ResetElementList( pElement );
}

void _DlgMTWeather::OnBnClickedButtonDelElement()
{
	if( m_pCurSelectZone == NULL )
		return;

	POSITION pos = m_listElement.GetFirstSelectedItemPosition();
	int nItem = m_listElement.GetNextSelectedItem( pos );
	if( nItem < 0 )
	{
		return;
	}

	m_pCurSelectZone->DelElement( m_pCurSelectElement );
	__ResetElementList( NULL );
}

void _DlgMTWeather::OnCbnSelchangeComboEleType()
{
	if( m_pCurSelectElement == NULL )
		return;

	m_pCurSelectElement->s_Type = m_cmb_Type.GetCurSel();
	__ResetElementList( m_pCurSelectElement );
}

void _DlgMTWeather::OnEnChangeEditEleTime()
{
	if( m_pCurSelectElement == NULL )
		return;

	m_pCurSelectElement->s_nTime_Min = nsCsMfcFunc::Control2Int( &m_edit_Time );
	__ResetElementList( m_pCurSelectElement );
}

void _DlgMTWeather::OnCbnSelchangeComboEleLevel()
{
	if( m_pCurSelectElement == NULL )
		return;

	m_pCurSelectElement->s_nLevel = (int)m_cmb_Level.GetItemData( m_cmb_Level.GetCurSel() );
	__ResetElementList( m_pCurSelectElement );
}

void _DlgMTWeather::OnEnChangeEditFatigue()
{
	if( m_pCurSelectElement == NULL )
		return;

	m_pCurSelectElement->s_nFatigue = nsCsMfcFunc::Control2Int( &m_edit_Fatigue );
	__ResetElementList( m_pCurSelectElement );
}


void _DlgMTWeather::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_Size.GetSafeHwnd() )
	{
		if( m_pCurSelectZone )
		{
			m_pCurSelectZone->GetInfo()->s_nRadius = m_sld_Size.GetPos();
		}
	}	

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}









void _DlgMTWeather::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ZONE, m_listZone);
	DDX_Control(pDX, IDC_LIST_ELEMENT, m_listElement);
	DDX_Control(pDX, IDC_SLD_SIZE, m_sld_Size);
	DDX_Control(pDX, IDC_COMBO_ELE_TYPE, m_cmb_Type);
	DDX_Control(pDX, IDC_EDIT_ELE_TIME, m_edit_Time);
	DDX_Control(pDX, IDC_COMBO_ELE_LEVEL, m_cmb_Level);
	DDX_Control(pDX, IDC_EDIT_FATIGUE, m_edit_Fatigue);
	DDX_Control(pDX, IDC_EDIT_ZONE_LOOPTIME, m_edit_LoopTime);
}


BEGIN_MESSAGE_MAP(_DlgMTWeather, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD_ZONE, &_DlgMTWeather::OnBnClickedButtonAddZone)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ZONE, &_DlgMTWeather::OnBnClickedButtonDelZone)
	ON_BN_CLICKED(IDC_BUTTON_ADD_ELEMENT, &_DlgMTWeather::OnBnClickedButtonAddElement)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ELEMENT, &_DlgMTWeather::OnBnClickedButtonDelElement)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ZONE, &_DlgMTWeather::OnNMClickListZone)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_ZONE_LOOPTIME, &_DlgMTWeather::OnEnChangeEditZoneLooptime)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ELEMENT, &_DlgMTWeather::OnNMClickListElement)
	ON_CBN_SELCHANGE(IDC_COMBO_ELE_TYPE, &_DlgMTWeather::OnCbnSelchangeComboEleType)
	ON_EN_CHANGE(IDC_EDIT_ELE_TIME, &_DlgMTWeather::OnEnChangeEditEleTime)
	ON_CBN_SELCHANGE(IDC_COMBO_ELE_LEVEL, &_DlgMTWeather::OnCbnSelchangeComboEleLevel)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE, &_DlgMTWeather::OnEnChangeEditFatigue)
END_MESSAGE_MAP()


