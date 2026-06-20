// _DlgMTMonster.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTMonster.h"


// _DlgMTMonster 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTMonster, CDialog)

_DlgMTMonster::_DlgMTMonster(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTMonster::IDD, pParent)
{
	m_pSelectResist = NULL;
}

_DlgMTMonster::~_DlgMTMonster()
{
}

void _DlgMTMonster::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MONSTER, m_list_Monster);
	DDX_Control(pDX, IDC_LIST_RESIST_MONSTOR, m_list_ResistMonstor);
	DDX_Control(pDX, IDC_SLIDER_REGIONSIZE, m_sld_RegionSize);
	DDX_Control(pDX, IDC_EDIT_REGIONSIZE, m_edit_RegionSize);
	DDX_Control(pDX, IDC_SLIDER_DEF_REGIONSIZE, m_sld_Def_RegionSize);	
	DDX_Control(pDX, IDC_EDIT_DEF_REGIONSIZE, m_edit_Def_RegionSize);
	DDX_Control(pDX, IDC_SLIDER_MONSCALE, m_sld_MonScale);
	DDX_Control(pDX, IDC_EDIT_MONSCALE, m_edit_MonScale);
	DDX_Control(pDX, IDC_EDIT_MONCOUNT, m_edit_MonCount);
	DDX_Control(pDX, IDC_EDIT_MONRESPONTIME, m_edit_MonResponTime);
	DDX_Control(pDX, IDC_EDIT_DEF_MONCOUNT, m_edit_Def_MonCount);
	DDX_Control(pDX, IDC_EDIT_DEF_MONRESPONTIME, m_edit_Def_MonResponTime);
	DDX_Control(pDX, IDC_EDIT_KILLGEN_ID, m_edit_Killgen_MonID);
	DDX_Control(pDX, IDC_EDIT_KILLGEN_COUNT, m_edit_Killgen_Count);
	DDX_Control(pDX, IDC_EDIT_KILLGEN_VIEWCNT, m_edit_Killgen_ViewCnt);
	DDX_Control(pDX, IDC_EDIT_KILLGEN_DISP, m_edit_Killgen_Disp);
	DDX_Control(pDX, IDC_EDIT_MOVE, m_edit_Move);
	DDX_Control(pDX, IDC_EDIT_DEF_MOVE, m_edit_Def_Move);
	DDX_Control(pDX, IDC_INS_REGEN_TYPE, m_check_InsRegen);
}


BEGIN_MESSAGE_MAP(_DlgMTMonster, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_RESISTDELETE, &_DlgMTMonster::OnBnClickedButtonResistdelete)
	ON_LBN_SELCHANGE(IDC_LIST_RESIST_MONSTOR, &_DlgMTMonster::OnLbnSelchangeListResistMonstor)
	ON_EN_CHANGE(IDC_EDIT_MONCOUNT, &_DlgMTMonster::OnEnChangeEditMoncount)
	ON_EN_CHANGE(IDC_EDIT_MONRESPONTIME, &_DlgMTMonster::OnEnChangeEditMonrespontime)
	ON_EN_CHANGE(IDC_EDIT_DEF_MONCOUNT, &_DlgMTMonster::OnEnChangeEditDefMoncount)
	ON_EN_CHANGE(IDC_EDIT_DEF_MONRESPONTIME, &_DlgMTMonster::OnEnChangeEditDefMonrespontime)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MONSTER, &_DlgMTMonster::OnNMClickListMonster)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_MONSTER, &_DlgMTMonster::OnLvnColumnclickListMonster)
	ON_BN_CLICKED(IDC_BUTTON_RESISTALLDELETE, &_DlgMTMonster::OnBnClickedButtonResistalldelete)
	ON_EN_CHANGE(IDC_EDIT_KILLGEN_ID, &_DlgMTMonster::OnEnChangeEditKillgenId)
	ON_EN_CHANGE(IDC_EDIT_KILLGEN_COUNT, &_DlgMTMonster::OnEnChangeEditKillgenCount)
	ON_EN_CHANGE(IDC_EDIT_KILLGEN_VIEWCNT, &_DlgMTMonster::OnEnChangeEditKillgenViewcnt)
	ON_EN_CHANGE(IDC_EDIT_MOVE, &_DlgMTMonster::OnEnChangeEditMove)
	ON_EN_CHANGE(IDC_EDIT_DEF_MOVE, &_DlgMTMonster::OnEnChangeEditDefMove)	
	ON_BN_CLICKED(IDC_INS_REGEN_TYPE, &_DlgMTMonster::OnBnClickedInsRegenType)
END_MESSAGE_MAP()

BOOL _DlgMTMonster::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_pCtrlTT->RelayEvent((LPMSG)pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

//==================================================================================================
//
//		virtual
//
//==================================================================================================
void _DlgMTMonster::_Delete()
{
	__ReleasePreview();
	__ReleaseSelectResist();
	__DeleteResist();
	delete m_pCtrlTT;
}

void _DlgMTMonster::_ViewInit()
{
	__WriteList_MonTable();
}

void _DlgMTMonster::_UpdateView()
{
	__UpdateResist();
	__UpdatePreview();
}

void _DlgMTMonster::_PostRenderView()
{
	__RenderResist();
	__RenderPreview();
}

void _DlgMTMonster::_PostViewReset()
{	
}

void _DlgMTMonster::_PreSave( const char* cFile, bool bAutoSave )
{
	if( g_Config.GetEnvironment()->s_bWriteExcel == false )
		return;

	if( bAutoSave == true )
		return;

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	if( dwMapID == 0 )
	{
		assert_cs( m_listMonInfo.size() == 0 );
		return;
	}

	nsCsMapTable::g_pMapMonsterMng->GetGroup( dwMapID )->Delete();
	LIST_MONINFO_IT it = m_listMonInfo.begin();
	for( ; it!=m_listMonInfo.end(); ++it )
	{
		assert_cs( (*it)->s_TableInfo.s_dwMapID == dwMapID );
		nsCsMapTable::g_pMapMonsterMng->AddMonster( &(*it)->s_TableInfo );
	}
	_ResetMapID();
}

//==================================================================================================
//
//		Base
//
//==================================================================================================

BOOL _DlgMTMonster::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_sld_RegionSize.SetRange( 100, 15000 );
	m_sld_Def_RegionSize.SetRange( 100, 15000 );
	m_sld_Def_RegionSize.SetPos( g_Resist.GetMonster()->s_nDefRadius );
	nsCsMfcFunc::Int2Control( &m_edit_Def_RegionSize, m_sld_Def_RegionSize.GetPos() );
	m_sld_MonScale.SetRange( 100, 2000 );
	m_sld_MonScale.SetPos( (int)(g_Resist.GetMonster()->s_fViewMonScale*100) );
	nsCsMfcFunc::Float2Control( &m_edit_MonScale, g_Resist.GetMonster()->s_fViewMonScale );

	nsCsMfcFunc::Int2Control( &m_edit_Def_MonCount, g_Resist.GetMonster()->s_nDefMonCount );
	nsCsMfcFunc::Int2Control( &m_edit_Def_MonResponTime, g_Resist.GetMonster()->s_nDefMonResfonTime );
	nsCsMfcFunc::Int2Control( &m_edit_Def_Move, g_Resist.GetMonster()->s_nDefMove );
	m_check_InsRegen.SetCheck( g_Resist.GetMonster()->s_bInsRegen );	
	DWORD dwExStyle = m_list_Monster.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Monster.SetExtendedStyle( dwExStyle );
	m_list_Monster.InsertColumn( 0, L"ID", LVCFMT_LEFT, 60 );	
	m_list_Monster.InsertColumn( 1, L"이름", LVCFMT_LEFT, 120 );
	m_list_Monster.InsertColumn( 2, L"AI", LVCFMT_LEFT, 40 );
	m_list_Monster.InsertColumn( 3, L"레벨", LVCFMT_LEFT, 40 );
	m_list_Monster.InsertColumn( 4, L"공격력", LVCFMT_LEFT, 60 );
	m_list_Monster.InsertColumn( 5, L"방어력", LVCFMT_LEFT, 60 );
	m_pCtrlTT = new CToolTipCtrl;

	m_pCtrlTT->Create(this, WS_POPUP|WS_EX_TOOLWINDOW);  // 툴팁 스타일 지정, 지정하지 않으면 기본값
	m_pCtrlTT->SetMaxTipWidth(200);                  // 폭 (이값을 넘어가면 개행)

	m_pCtrlTT->AddTool( GetDlgItem(IDC_EDIT_DEF_MOVE), L"0:영역없음 1:영역(기본값) 2:이동 3:고정 4:오브젝트형");
	m_pCtrlTT->AddTool( GetDlgItem(IDC_EDIT_MOVE), L"0:영역없음 1:영역(기본값) 2:이동 3:고정 4:오브젝트형");

	__InitResist();	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgMTMonster::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{		
	}
	else
	{
	}
}

void _DlgMTMonster::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_RegionSize.GetSafeHwnd() )
	{
		_Int2Control( &m_edit_RegionSize, m_sld_RegionSize.GetPos() );

		// 셀렉트 몬스터면 셀렉트 몬스터에게
		sMON_INFO* pMonInfo = m_pSelectResist;
		// 아니라면 기본 정보의
		if( pMonInfo == NULL )
			pMonInfo = &m_PreView;

		pMonInfo->s_TableInfo.s_nRadius = m_sld_RegionSize.GetPos();
		__ResetResistMonstorList();
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_Def_RegionSize.GetSafeHwnd() )
	{
		nsCsMfcFunc::Int2Control( &m_edit_Def_RegionSize, m_sld_Def_RegionSize.GetPos() );
		g_Resist.GetMonster()->s_nDefRadius = m_sld_Def_RegionSize.GetPos();
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_MonScale.GetSafeHwnd() )
	{
		g_Resist.GetMonster()->s_fViewMonScale = m_sld_MonScale.GetPos()*0.01f;
		nsCsMfcFunc::Float2Control( &m_edit_MonScale, g_Resist.GetMonster()->s_fViewMonScale );

		// 스케일값 조정
		LIST_MONINFO_IT it = m_listMonInfo.begin();
		for( ; it!=m_listMonInfo.end(); ++it )
		{
			( *it )->s_pModelRes->GetNiNode()->SetScale( g_Resist.GetMonster()->s_fViewMonScale );
		}
		if( m_PreView.s_pModelRes != NULL )
			m_PreView.s_pModelRes->GetNiNode()->SetScale( g_Resist.GetMonster()->s_fViewMonScale );
	}

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

// =================================================================================================
//
//		Monster Table
//
// =================================================================================================
void _DlgMTMonster::_ResetMapID()
{
	__ReleasePreview();
	__ReleaseSelectResist();
	__DeleteResist();
	
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( dwMapID == 0 )
		return;

	__FileTableToResist();
	__ResetResistMonstorList();
}

void _DlgMTMonster::__FileTableToResist()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	CsMapMonster::LIST* pMapMonster = nsCsMapTable::g_pMapMonsterMng->GetGroup( dwMapID )->GetMonsterList();
	CsMapMonster::LIST_IT it = pMapMonster->begin();
	for( ; it!=pMapMonster->end(); ++it )
	{
		if( nsCsFileTable::g_pMonsterMng->IsMonster( (*it)->GetInfo()->s_dwMonsterTableID ) == false )
		{
			static bool bMsg = false;
			if( bMsg == false )
			{
				CsMessageBox( MB_OK, L"맵에 배치된 몬스터가 몬스터테이블( ID:%d )에 존재하지 않습니다.", (*it)->GetInfo()->s_dwMonsterTableID  );
				bMsg = true;
			}
			continue;
		}
		__AddResist( (*it)->GetInfo() );
	}
}

void _DlgMTMonster::__WriteList_MonTable()
{
	m_list_Monster.DeleteAllItems();

	CsMonster::MAP* pMapMonster = nsCsFileTable::g_pMonsterMng->GetMonsterMap();
	CsMonster::MAP_IT it = pMapMonster->begin();
	for( ; it!=pMapMonster->end(); ++it )
	{
		__InsertMonTable( it->second );
	}
}

void _DlgMTMonster::__InsertMonTable( CsMonster* pMonster )
{
	CsMonster::sINFO* pMonInfo = pMonster->GetInfo();

	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_Monster.GetItemCount();

	// ID
	_stprintf_s( szMsg, 256, L"%d", pMonInfo->s_dwMonsterID );
	m_list_Monster.InsertItem( nListIndex, szMsg );
	// 이름
	_stprintf_s( szMsg, 256, L"%s %s", pMonInfo->s_szName, pMonInfo->s_szPenName );
	m_list_Monster.SetItem( nListIndex, 1, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	// AI
	_stprintf_s( szMsg, 256, L"%d", pMonInfo->s_nBattle );
	m_list_Monster.SetItem( nListIndex, 2, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	// 레벨
	_stprintf_s( szMsg, 256, L"%d", pMonInfo->s_nLevel );
	m_list_Monster.SetItem( nListIndex, 3, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	// 공격력
	_stprintf_s( szMsg, 256, L"%d", pMonInfo->s_nAttPower );
	m_list_Monster.SetItem( nListIndex, 4, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	// 방어력
	_stprintf_s( szMsg, 256, L"%d", pMonInfo->s_nDefPower );
	m_list_Monster.SetItem( nListIndex, 5, LVIF_TEXT, szMsg, 0, 0, 0, 0 );

	m_list_Monster.SetItemData( nListIndex, pMonInfo->s_dwMonsterID );
}

// 몬스터 리스트 선택 변경
void _DlgMTMonster::OnNMClickListMonster(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_Monster.GetFirstSelectedItemPosition();
	int nItem = m_list_Monster.GetNextSelectedItem( pos );
	if( nItem > -1 )
	{
		DWORD dwMonstorTableID = (DWORD)m_list_Monster.GetItemData( nItem );
		DWORD dwModelKey = nsCsFileTable::g_pMonsterMng->GetMonster( dwMonstorTableID )->GetInfo()->s_dwModelID;
		if( g_Preview.GetChar()->SetModelView( dwModelKey ) == true )
		{
			__SetPreview( dwMonstorTableID );			
			return;
		}
		else
		{
			__ReleasePreview();
		}
	}

	m_PreView.Delete();	
	g_Preview.GetChar()->ReleaseModelView();

	*pResult = 0;
}

void _DlgMTMonster::__Sort_MonTable( int nColume, bool bIncrease )
{
	m_list_Monster.DeleteAllItems();

	std::list< CsMonster* > listSort;
	CsMonster::MAP* pMap = nsCsFileTable::g_pMonsterMng->GetMonsterMap();	
	CsMonster::MAP_IT it = pMap->begin();
	for( ; it!=pMap->end(); ++it )
	{
		listSort.push_back( it->second );
	}
	
	while( listSort.size() != 0 )
	{
		std::list< CsMonster* >::iterator itInput = listSort.begin();
		std::list< CsMonster* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( nColume )
			{
				// ID
			case 0:
				if( ( (*it)->GetInfo()->s_dwMonsterID < (*itInput)->GetInfo()->s_dwMonsterID ) == bIncrease )
					itInput = it;
				break;
				// 이름
			case 1:
				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
					itInput = it;
				break;
				// AI
			case 2:
				if( ( (*it)->GetInfo()->s_nBattle < (*itInput)->GetInfo()->s_nBattle ) == bIncrease )
					itInput = it;
				break;
				// 레벨
			case 3:
				if( ( (*it)->GetInfo()->s_nLevel < (*itInput)->GetInfo()->s_nLevel ) == bIncrease )
					itInput = it;
				break;
				// 공격력
			case 4:
				if( ( (*it)->GetInfo()->s_nAttPower < (*itInput)->GetInfo()->s_nAttPower ) == bIncrease )
					itInput = it;
				break;
				// 방어력
			case 5:
				if( ( (*it)->GetInfo()->s_nDefPower < (*itInput)->GetInfo()->s_nDefPower ) == bIncrease )
					itInput = it;
				break;
			default:
				assert_cs( false );
			}
		}

		__InsertMonTable( (*itInput ) );
		listSort.erase( itInput );
	}
}

// 몬스터 리스트 컬럼 클릭
void _DlgMTMonster::OnLvnColumnclickListMonster(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	static bool bIncrease = true;
	bIncrease = !bIncrease;

	// 선택된 것이 있는가
	POSITION pos = m_list_Monster.GetFirstSelectedItemPosition();
	int nIndex = m_list_Monster.GetNextSelectedItem( pos );
	DWORD dwSelID = 0;
	if( nIndex >= 0 )
		dwSelID = (DWORD)m_list_Monster.GetItemData( nIndex );

	__Sort_MonTable( pNMLV->iSubItem, bIncrease );

	// 선택된 넘은 소팅후에 다시 선택 해준다.
	if( dwSelID != 0 )
	{
		int nCount = m_list_Monster.GetItemCount();
		for( int c=0; c<nCount; ++c )
		{
			if( dwSelID == (DWORD)m_list_Monster.GetItemData( c ) )
			{
				m_list_Monster.SetItemState( c, LVIS_SELECTED, LVIS_SELECTED );
				return;
			}
		}
	}


	*pResult = 0;
}


// =================================================================================================
//
//		Monster Info
//
// =================================================================================================
void _DlgMTMonster::sMON_INFO::Init()
{
	Delete();
	s_TableInfo.Init();
}

void _DlgMTMonster::sMON_INFO::Delete()
{
	if( s_pModelRes != NULL )
	{
		s_pModelRes->Delete();
		g_Preview.GetChar()->m_mempoolModelRes.Push( s_pModelRes );
		s_pModelRes = NULL;
	}
}

_DlgMTMonster::sMON_INFO* _DlgMTMonster::sMON_INFO::Clone()
{
	sMON_INFO* pMonInfo = GET_MV_MAINTAB_DLG->_GetDlgMonster()->m_mempoolMonInfo.Pop();
	pMonInfo->Init();

	memcpy( &pMonInfo->s_TableInfo, &s_TableInfo, sizeof( CsMapMonster::sINFO ) );

	DWORD dwModelID = s_TableInfo.GetModelID();
	pMonInfo->s_pModelRes = g_Preview.GetChar()->m_mempoolModelRes.Pop();
	pMonInfo->s_pModelRes->Init( g_Preview.GetChar()->GetModelRes( dwModelID )->Clone(), dwModelID );
	pMonInfo->s_pModelRes->GetNiNode()->SetLocalTransform( s_pModelRes->GetNiNode()->GetWorldTransform() );
	pMonInfo->s_pModelRes->GetNiNode()->SetScale( g_Resist.GetMonster()->s_fViewMonScale );

	return pMonInfo;
}

void _DlgMTMonster::__SetMonInfo( sMON_INFO* pMonInfo )
{
	m_sld_RegionSize.SetPos( pMonInfo->s_TableInfo.s_nRadius );
	_Int2Control( &m_edit_RegionSize, pMonInfo->s_TableInfo.s_nRadius );

	_Int2Control( &m_edit_MonCount, pMonInfo->s_TableInfo.s_nCount );
	_Int2Control( &m_edit_MonResponTime, pMonInfo->s_TableInfo.s_nResponTime );

	_Int2Control( &m_edit_Killgen_MonID, pMonInfo->s_TableInfo.s_nKillgen_MonFTID );
	_Int2Control( &m_edit_Killgen_Count, pMonInfo->s_TableInfo.s_nKillgen_Count );
	_Int2Control( &m_edit_Killgen_ViewCnt, pMonInfo->s_TableInfo.s_nKillgen_ViewCnt );
	_Int2Control( &m_edit_Move, pMonInfo->s_TableInfo.s_nMoveType );

	m_check_InsRegen.SetCheck( pMonInfo->s_TableInfo.s_bInsRespon );		
}

//===================================================================================================
//
//		Preview
//
//===================================================================================================
void _DlgMTMonster::__SetPreview( DWORD dwMonstorTableID )
{
	m_PreView.Init();
	m_PreView.s_TableInfo.s_nCount = g_Resist.GetMonster()->s_nDefMonCount;
	m_PreView.s_TableInfo.s_nResponTime = g_Resist.GetMonster()->s_nDefMonResfonTime;
	m_PreView.s_TableInfo.s_nRadius = g_Resist.GetMonster()->s_nDefRadius;

	m_PreView.s_TableInfo.s_dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	m_PreView.s_TableInfo.s_dwMonsterTableID = dwMonstorTableID;
	m_PreView.s_TableInfo.s_nKillgen_MonFTID = 0;
	m_PreView.s_TableInfo.s_nKillgen_Count = 0;
	m_PreView.s_TableInfo.s_nKillgen_ViewCnt = 0;
	m_PreView.s_TableInfo.s_bInsRespon = g_Resist.GetMonster()->s_bInsRegen;
	m_PreView.s_TableInfo.s_nMoveType = g_Resist.GetMonster()->s_nDefMove;
	DWORD dwModelID = m_PreView.s_TableInfo.GetModelID();
	m_PreView.s_pModelRes = g_Preview.GetChar()->m_mempoolModelRes.Pop();
	m_PreView.s_pModelRes->Init( g_Preview.GetChar()->GetModelRes( dwModelID )->Clone(), dwModelID );

	m_PreView.s_pModelRes->GetNiNode()->SetScale( g_Resist.GetMonster()->s_fViewMonScale );
	nsCSGBFUNC::Set_Emittance( m_PreView.s_pModelRes->GetNiNode(), NiColor( 0.75f, 0.25f, 0.25f ) );

	m_bRenderPreview = false;
	__ReleaseSelectResist();

	__SetMonInfo( &m_PreView );
}

void _DlgMTMonster::__ReleasePreview()
{
	m_bRenderPreview = false;
	m_PreView.Delete();

	POSITION pos = m_list_Monster.GetFirstSelectedItemPosition();
	int nSel = m_list_Monster.GetNextSelectedItem( pos );
	if( nSel != -1 )
	{
		m_list_Monster.SetItemState( nSel, 0, LVIS_SELECTED );
	}
}

void _DlgMTMonster::__UpdatePreview()
{
	if( IsClientRect() == false )
		m_bRenderPreview = false;

	if( __IsPreview() == false )
		return;

	m_PreView.s_pModelRes->Update( g_fElapsedTime, true );
}

void _DlgMTMonster::__RenderPreview()
{
	if( __IsPreview() == false )
		return;

	NiDrawScene( CAMERA_ST.GetCameraObj(), m_PreView.s_pModelRes->GetNiNode(), *g_pCuller );
	g_ToolNiNodeMng.GetRegion()->Render(	NiPoint2( (float)m_PreView.s_TableInfo.s_nCenterX, (float)m_PreView.s_TableInfo.s_nCenterY ),
											(float)m_PreView.s_TableInfo.s_nRadius,
											true	);
}

void _DlgMTMonster::__PreviewToResist()
{
	sMON_INFO* pNewMonInfo = m_PreView.Clone();
	m_listMonInfo.push_back( pNewMonInfo );	

	__ResetResistMonstorList();
}

//===================================================================================================
//
//		Resist
//
//===================================================================================================

void _DlgMTMonster::__DeleteResist()
{
	LIST_MONINFO_IT it = m_listMonInfo.begin();
	for( ; it!=m_listMonInfo.end(); ++it )
	{
		(*it)->Delete();
		m_mempoolMonInfo.Push( *it );
	}
	m_listMonInfo.clear();
}

void _DlgMTMonster::__InitResist()
{
	assert_cs( m_listMonInfo.empty() == true );
	__ReleaseSelectResist();
}

void _DlgMTMonster::__UpdateResist()
{
	LIST_MONINFO_IT it = m_listMonInfo.begin();
	for( ; it!=m_listMonInfo.end(); ++it )
	{
		(*it)->s_pModelRes->Update( g_fElapsedTime, false );
	}
}

void _DlgMTMonster::__RenderResist(bool RenderMonster)
{
	LIST_MONINFO_IT it = m_listMonInfo.begin();
	for( ; it!=m_listMonInfo.end(); ++it )
	{
		bool bSelect = false;
		if( (*it) == m_pSelectResist )
			bSelect = true;

		if(RenderMonster )
			NiDrawScene( CAMERA_ST.GetCameraObj(), (*it)->s_pModelRes->GetNiNode(), *g_pCuller );
	
		g_ToolNiNodeMng.GetRegion()->Render( NiPoint2( (float)(*it)->s_TableInfo.s_nCenterX, (float)(*it)->s_TableInfo.s_nCenterY ),
											 (float)(*it)->s_TableInfo.s_nRadius,
											 bSelect );
	}
}

bool _DlgMTMonster::__AddResist( CsMapMonster::sINFO* pInfo )
{	
	sMON_INFO* pMonInfo = m_mempoolMonInfo.Pop();
	pMonInfo->Init();
	memcpy( &pMonInfo->s_TableInfo, pInfo, sizeof( CsMapMonster::sINFO) );

	DWORD dwModelID = pMonInfo->s_TableInfo.GetModelID();	
	cModel_Res* pRes = g_Preview.GetChar()->GetModelRes( dwModelID );
	if( pRes == NULL )
	{
		static bool bMsg = false;
		if( bMsg == false )
		{			
			CsMessageBox( MB_OK, L"몬스터의 모델 데이터가 존재하지 않습니다. MonsterTableID = %d, ModelID = %d", pMonInfo->s_TableInfo.s_dwMonsterTableID, dwModelID );
			bMsg = true;
		}
		pMonInfo->Delete();
		m_mempoolMonInfo.Push( pMonInfo );
		return false;
	}
	NiPoint2 ptCenter( (float)pMonInfo->s_TableInfo.s_nCenterX, (float)pMonInfo->s_TableInfo.s_nCenterY );
	if( nsCsGBTerrain::g_pCurRoot->IsInTerrain( ptCenter.x, ptCenter.y ) == false )
	{
		static bool bMsg = false;
		if( bMsg == false )
		{			
			CsMessageBox( MB_OK, L"지형의 크기를 벋어난 몬스터가 존재합니다. MonsterTableID = %d", pMonInfo->s_TableInfo.s_dwMonsterTableID );
			bMsg = true;
		}
		pMonInfo->Delete();
		m_mempoolMonInfo.Push( pMonInfo );
		return false;
	}

	NiPoint3 ptTrans = NiPoint3( ptCenter.x, ptCenter.y, nsCsGBTerrain::g_pCurRoot->GetHeight( ptCenter ) );

	pMonInfo->s_pModelRes = g_Preview.GetChar()->m_mempoolModelRes.Pop();
	pMonInfo->s_pModelRes->Init( pRes->Clone(), dwModelID );	
	pMonInfo->s_pModelRes->GetNiNode()->SetTranslate( ptTrans );
	pMonInfo->s_pModelRes->GetNiNode()->SetScale( g_Resist.GetMonster()->s_fViewMonScale );
	pMonInfo->s_pModelRes->Update( 0.0f, false );
	m_listMonInfo.push_back( pMonInfo );

	return true;
}

void _DlgMTMonster::__ReleaseSelectResist()
{
	if( m_pSelectResist != NULL )
	{
		nsCSGBFUNC::Set_Emittance( m_pSelectResist->s_pModelRes->GetNiNode(), NiColor( 1.0f, 1.0f, 1.0f ) );
		m_pSelectResist = NULL;

		m_list_ResistMonstor.SetCurSel( -1 );
	}
}

_DlgMTMonster::sMON_INFO* _DlgMTMonster::__SelectResist( DWORD dwIndex )
{
	LIST_MONINFO_IT it = m_listMonInfo.begin();
	for( ; it!=m_listMonInfo.end(); ++it )
	{
		if( dwIndex == 0 )
		{
			__SelectResist( (*it) );
			return m_pSelectResist;
		}
		--dwIndex;
	}

	assert_cs( false );
	return NULL;
}

void _DlgMTMonster::__SelectResist( sMON_INFO* pSelectMon )
{
	if( m_pSelectResist != NULL )
	{
		nsCSGBFUNC::Set_Emittance( m_pSelectResist->s_pModelRes->GetNiNode(), NiColor( 1.0f, 1.0f, 1.0f ) );
	}
	m_pSelectResist = pSelectMon;
	nsCSGBFUNC::Set_Emittance( m_pSelectResist->s_pModelRes->GetNiNode(), NiColor( 0.75f, 0.25f, 0.25f ) );
}

int _DlgMTMonster::__GetSelectResistIndex()
{
	if( m_pSelectResist == NULL )
		return -1;

	int nReturnIndex = 0;
	LIST_MONINFO_IT it = m_listMonInfo.begin();
	for( ; it!=m_listMonInfo.end(); ++it )
	{
		if( (*it) == m_pSelectResist )
		{
			return nReturnIndex;
		}
		++nReturnIndex;
	}
	assert_cs( false );
	return -1;
}

void _DlgMTMonster::__DeleteSelectResist()
{
	if( m_pSelectResist == NULL )
	{
		return;
	}

	int nSelIndex = 0;
	LIST_MONINFO_IT it = m_listMonInfo.begin();
	for( ; it!=m_listMonInfo.end(); ++it )
	{
		if( (*it) == m_pSelectResist )
		{
			__ReleaseSelectResist();
			(*it)->Delete();
			m_mempoolMonInfo.Push( (*it) );
			m_listMonInfo.erase( it );			
			return;
		}
		++nSelIndex;
	}
	assert_cs( false );	
}


void _DlgMTMonster::__ResetResistMonstorList()
{
	m_list_ResistMonstor.ResetContent();

	TCHAR szStr[ 1024 ];
	CsMonster* pMonster;
	LIST_MONINFO_IT it = m_listMonInfo.begin();	
	for( ; it!=m_listMonInfo.end(); ++it )
	{
		pMonster = nsCsFileTable::g_pMonsterMng->GetMonster( (*it)->s_TableInfo.s_dwMonsterTableID );
		_stprintf_s(	szStr, 1024, L"%d - %s (%d), %dx%dsec",
						(*it)->s_TableInfo.s_dwMonsterTableID,
						pMonster->GetInfo()->s_szName,
						pMonster->GetInfo()->s_nLevel,
						(*it)->s_TableInfo.s_nCount,
						(*it)->s_TableInfo.s_nResponTime );
		m_list_ResistMonstor.InsertString( m_list_ResistMonstor.GetCount(), szStr );
	}

	int nSel = __GetSelectResistIndex();
	if( nSel == -1 )
		return;
	m_list_ResistMonstor.SetCurSel( nSel );
}



// 몬스터 갯수 에디트 변경
void _DlgMTMonster::OnEnChangeEditMoncount()
{
	// 셀렉트 몬스터면 셀렉트 몬스터에게
	sMON_INFO* pMonInfo = m_pSelectResist;
	// 아니라면 기본 정보의
	if( pMonInfo == NULL )
		pMonInfo = &m_PreView;
	pMonInfo->s_TableInfo.s_nCount = _Control2Int( &m_edit_MonCount );

	__ResetResistMonstorList();
}

// 몬스터 리스폰 타임 에디트 변경
void _DlgMTMonster::OnEnChangeEditMonrespontime()
{
	// 셀렉트 몬스터면 셀렉트 몬스터에게
	sMON_INFO* pMonInfo = m_pSelectResist;
	// 아니라면 기본 정보의
	if( pMonInfo == NULL )
		pMonInfo = &m_PreView;

	pMonInfo->s_TableInfo.s_nResponTime = _Control2Int( &m_edit_MonResponTime );	

	__ResetResistMonstorList();
}

// 디폴트 몬스터 갯수 에디트 변경
void _DlgMTMonster::OnEnChangeEditDefMoncount()
{
	g_Resist.GetMonster()->s_nDefMonCount = nsCsMfcFunc::Control2Int( &m_edit_Def_MonCount );
}

// 디폴트 몬스터 리스폰 타임 에디트 변경
void _DlgMTMonster::OnEnChangeEditDefMonrespontime()
{
	g_Resist.GetMonster()->s_nDefMonResfonTime = nsCsMfcFunc::Control2Int( &m_edit_Def_MonResponTime );
}

void _DlgMTMonster::OnEnChangeEditMove()
{
	// 셀렉트 몬스터면 셀렉트 몬스터에게
	sMON_INFO* pMonInfo = m_pSelectResist;
	// 아니라면 기본 정보의
	if( pMonInfo == NULL )
		pMonInfo = &m_PreView;

	pMonInfo->s_TableInfo.s_nMoveType = _Control2Int( &m_edit_Move );

	__ResetResistMonstorList();
}

void _DlgMTMonster::OnEnChangeEditDefMove()
{
	g_Resist.GetMonster()->s_nDefMove = nsCsMfcFunc::Control2Int( &m_edit_Def_Move );
}

void _DlgMTMonster::OnBnClickedInsRegenType()
{
	if( NULL == m_pSelectResist )
		return;

	// 셀렉트 몬스터면 셀렉트 몬스터에게
	sMON_INFO* pMonInfo = m_pSelectResist;
	// 아니라면 기본 정보의
	if( pMonInfo == NULL )
		pMonInfo = &m_PreView;
 
	//g_Resist.GetMonster()->s_bInsRegen = m_check_InsRegen.GetCheck();
	pMonInfo->s_TableInfo.s_bInsRespon = m_check_InsRegen.GetCheck();
}

void _DlgMTMonster::OnEnChangeEditKillgenId()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_Killgen_MonID.GetSafeHwnd() )
		return;

	// 셀렉트 몬스터면 셀렉트 몬스터에게
	sMON_INFO* pMonInfo = m_pSelectResist;
	// 아니라면 기본 정보의
	if( pMonInfo == NULL )
		pMonInfo = &m_PreView;

	if( ( nsCsFileTable::g_pMonsterMng->IsMonster( pMonInfo->s_TableInfo.s_dwMonsterTableID ) == false )||
		(	( nsCsFileTable::g_pMonsterMng->GetMonster( pMonInfo->s_TableInfo.s_dwMonsterTableID )->GetInfo()->s_nClass != 4 )&&
			( nsCsFileTable::g_pMonsterMng->GetMonster( pMonInfo->s_TableInfo.s_dwMonsterTableID )->GetInfo()->s_nClass != 5 ) ) )
	{
		SetFocus();
		m_edit_Killgen_MonID.SetWindowText( L"" );
		m_edit_Killgen_Disp.SetWindowText( L"" );
		return;
	}

	pMonInfo->s_TableInfo.s_nKillgen_MonFTID = nsCsMfcFunc::Control2Int( &m_edit_Killgen_MonID );
	if( nsCsFileTable::g_pMonsterMng->IsMonster( pMonInfo->s_TableInfo.s_nKillgen_MonFTID ) == false )
	{
		m_edit_Killgen_Disp.SetWindowText( L"None" );
	}
	else
	{
		m_edit_Killgen_Disp.SetWindowText( nsCsFileTable::g_pMonsterMng->GetMonster( pMonInfo->s_TableInfo.s_nKillgen_MonFTID )->GetInfo()->s_szName );
	}
}

void _DlgMTMonster::OnEnChangeEditKillgenCount()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_Killgen_Count.GetSafeHwnd() )
		return;

	// 셀렉트 몬스터면 셀렉트 몬스터에게
	sMON_INFO* pMonInfo = m_pSelectResist;
	// 아니라면 기본 정보의
	if( pMonInfo == NULL )
		pMonInfo = &m_PreView;

	if( ( nsCsFileTable::g_pMonsterMng->IsMonster( pMonInfo->s_TableInfo.s_dwMonsterTableID ) == false )||
		(	( nsCsFileTable::g_pMonsterMng->GetMonster( pMonInfo->s_TableInfo.s_dwMonsterTableID )->GetInfo()->s_nClass != 4 )&&
			( nsCsFileTable::g_pMonsterMng->GetMonster( pMonInfo->s_TableInfo.s_dwMonsterTableID )->GetInfo()->s_nClass != 5 ) ) )
	{
		SetFocus();
		m_edit_Killgen_Count.SetWindowText( L"" );
		return;
	}

	pMonInfo->s_TableInfo.s_nKillgen_Count = nsCsMfcFunc::Control2Int( &m_edit_Killgen_Count );
}

void _DlgMTMonster::OnEnChangeEditKillgenViewcnt()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_Killgen_ViewCnt.GetSafeHwnd() )
		return;

	// 셀렉트 몬스터면 셀렉트 몬스터에게
	sMON_INFO* pMonInfo = m_pSelectResist;
	// 아니라면 기본 정보의
	if( pMonInfo == NULL )
		pMonInfo = &m_PreView;

	if( ( nsCsFileTable::g_pMonsterMng->IsMonster( pMonInfo->s_TableInfo.s_dwMonsterTableID ) == false )||
		(	( nsCsFileTable::g_pMonsterMng->GetMonster( pMonInfo->s_TableInfo.s_dwMonsterTableID )->GetInfo()->s_nClass != 4 )&&
			( nsCsFileTable::g_pMonsterMng->GetMonster( pMonInfo->s_TableInfo.s_dwMonsterTableID )->GetInfo()->s_nClass != 5 ) ) )
	{
		SetFocus();
		m_edit_Killgen_ViewCnt.SetWindowText( L"" );
		return;
	}

	pMonInfo->s_TableInfo.s_nKillgen_ViewCnt = nsCsMfcFunc::Control2Int( &m_edit_Killgen_ViewCnt );
}


// 레지스트 리스트에서 선택 변경
void _DlgMTMonster::OnLbnSelchangeListResistMonstor()
{
	int nSel = m_list_ResistMonstor.GetCurSel();
	int nSelMon = __GetSelectResistIndex();
	if( nSel == nSelMon )
		return;

	if( nSel >= 0 )
	{
		__ReleasePreview();
		__SelectResist( nSel );
		__SetMonInfo( m_pSelectResist );
		g_Preview.GetChar()->SetModelView( m_pSelectResist->s_TableInfo.GetModelID() );
		if( IS_ENABLEFRM( nsMAPTOOL::MV_OBJECT_CONTROL ) == true )
		{		
			GET_MV_OBJECTCONTROL_DLG->_InsertValue( cAxisRotation::AT_OBJECT, m_pSelectResist->s_pModelRes->GetNiNode()->GetWorldTransform() );
		}
	}
	else
	{
		g_Preview.GetChar()->ReleaseModelView();
	}
}

// 레지스트 리스트에서 제거
void _DlgMTMonster::OnBnClickedButtonResistdelete()
{
	__DeleteSelectResist();
	__ResetResistMonstorList();
}

void _DlgMTMonster::OnBnClickedButtonResistalldelete()
{
	if( CsMessageBox( MB_OKCANCEL, L"모든 몬스터 제거를 계속 진행하시겠습니까?" ) == IDCANCEL )
		return;

	__DeleteResist();
	__ResetResistMonstorList();
}


//==================================================================================================
//
//		Input
//
//==================================================================================================

void _DlgMTMonster::_KeyDownEscape()
{
	g_Preview.GetChar()->ReleaseModelView();
	__ReleaseSelectResist();
	__ReleasePreview();	
}

void _DlgMTMonster::_OnLButtonDown( CPoint pt )
{
	if( __IsPreview() == true )
	{
		NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
		if( pRecord != NULL )
		{
			NiPoint3 ptIntersect = pRecord->GetIntersection();						
			m_PreView.s_TableInfo.s_nCenterX = (int)ptIntersect.x;
			m_PreView.s_TableInfo.s_nCenterY = (int)ptIntersect.y;
			m_PreView.s_pModelRes->GetNiNode()->SetTranslate( ptIntersect );
			m_PreView.s_pModelRes->Update( 0.0f, false );
			__PreviewToResist();

			if( GET_MV_VIEW->_IsShift() == false )
			{
				__ReleasePreview();
				__SelectResist( (DWORD)m_listMonInfo.size() - 1 );
			}			
		}
	}
	else if( m_pSelectResist )
	{
		NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
		if( pRecord != NULL )
		{
			NiPoint3 ptIntersect = pRecord->GetIntersection();
			m_pSelectResist->s_TableInfo.s_nCenterX = (int)ptIntersect.x;
			m_pSelectResist->s_TableInfo.s_nCenterY = (int)ptIntersect.y;

			m_pSelectResist->s_pModelRes->GetNiNode()->SetTranslate( ptIntersect );
			m_pSelectResist->s_pModelRes->GetNiNode()->Update(0.0f);
			if( IS_ENABLEFRM( nsMAPTOOL::MV_OBJECT_CONTROL ) == true )
			{		
				GET_MV_OBJECTCONTROL_DLG->_InsertValue( cAxisRotation::AT_OBJECT, m_pSelectResist->s_pModelRes->GetNiNode()->GetWorldTransform() );
			}

		}
	}
}

void _DlgMTMonster::_OnMouseMove( CPoint pt )
{
	if( m_PreView.s_pModelRes != NULL )
	{
		NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
		if( pRecord != NULL )
		{
			m_bRenderPreview = true;
			NiPoint3 ptIntersect = pRecord->GetIntersection();
			m_PreView.s_TableInfo.s_nCenterX = (int)ptIntersect.x;
			m_PreView.s_TableInfo.s_nCenterY = (int)ptIntersect.y;
			m_PreView.s_pModelRes->GetNiNode()->SetTranslate( ptIntersect );
			m_PreView.s_pModelRes->Update( 0.0f, false );
		}
		else
		{
			m_bRenderPreview = false;
		}
	}
}
