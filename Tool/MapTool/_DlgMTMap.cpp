// _DlgMTMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTMap.h"


// _DlgMTMap 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTMap, CDialog)


void _DlgMTMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MAPID, m_combo_MapID);
	DDX_Control(pDX, IDC_EDIT_MAPNAME, m_edit_MapName);
	DDX_Control(pDX, IDC_EDIT_MAP_DESCRIPTION, m_edit_MapDescription);
	DDX_Control(pDX, IDC_COMBO_UNIQ_OBJ, m_combo_UniqObj);
	DDX_Control(pDX, IDC_LIST_UNIQ_OBJ, m_list_UniqObj);
	DDX_Control(pDX, IDC_BUTTON_ADD_UNIQOBJ, m_btn_AddUniqObj);
	DDX_Control(pDX, IDC_EDIT_BGSOUND, m_edit_MapBGSound);
	DDX_Control(pDX, IDC_COMBO_RESURRECTION, m_combo_Resurrection);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_TYPE, m_edit_FG_Type);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_DEBUFF, m_edit_FG_DBuff);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_STIME, m_edit_FG_STime);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_ATIME, m_edit_FG_ATime);
	DDX_Control(pDX, IDC_EDIT_FATIGUE_POINT, m_edit_FG_Point);
	DDX_Control(pDX, IDC_EDIT_CAMERA_DIST_MAX_LV, m_edit_Camera_Max_Level);
	DDX_Control(pDX, IDC_EDIT_MAP_DESCRIPTION_ENG, m_edit_MapDescription_Eng);

	DDX_Control(pDX, IDC_ADD_MAPID_BUTTON, m_btn_AddMapID);
	DDX_Control(pDX, IDC_EDIT_MAPID_ADD, m_edit_AddMapID);
	DDX_Control(pDX, IDC_EDIT_MAPNAME_ADD, m_edit_AddToolMapName);

	DDX_Control(pDX, IDC_MAP_NAME_TXT, m_Static_ToolMapName);
	DDX_Control(pDX, IDC_MAP_ID_TXT, m_Static_AddMapID);
	DDX_Control(pDX, IDC_MAP_ID_ADD_GROUP_TXT, m_Static_AddMapID_Group);

	DDX_Control(pDX, IDC_CHECK_XGCONSUME, m_check_Xgconsume);
	DDX_Control(pDX, IDC_CHECK_BATTLETAG_USE, m_check_BattletagUse);
}


BEGIN_MESSAGE_MAP(_DlgMTMap, CDialog)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_MAPID, &_DlgMTMap::OnCbnSelchangeComboMapid)
	ON_CBN_SELCHANGE(IDC_COMBO_UNIQ_OBJ, &_DlgMTMap::OnCbnSelchangeComboUniqObj)
	ON_BN_CLICKED(IDC_BUTTON_ADD_UNIQOBJ, &_DlgMTMap::OnBnClickedButtonAddUniqobj)
	ON_LBN_SELCHANGE(IDC_LIST_UNIQ_OBJ, &_DlgMTMap::OnLbnSelchangeListUniqObj)
	ON_BN_CLICKED(IDC_BUTTON_DEL_UNIQOBJ, &_DlgMTMap::OnBnClickedButtonDelUniqobj)
	ON_EN_CHANGE(IDC_EDIT_MAP_DESCRIPTION, &_DlgMTMap::OnEnChangeEditMapDescription)
	ON_EN_CHANGE(IDC_EDIT_MAP_DESCRIPTION_ENG, &_DlgMTMap::OnEnChangeEditMapDescriptionEng)
	ON_BN_CLICKED(IDC_BUTTON_LOADBGSOUND, &_DlgMTMap::OnBnClickedButtonLoadbgsound)
	ON_BN_CLICKED(IDC_BUTTON_DEL_BGSOUND, &_DlgMTMap::OnBnClickedButtonDelBgsound)
	ON_CBN_SELCHANGE(IDC_COMBO_RESURRECTION, &_DlgMTMap::OnCbnSelchangeComboResurrection)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_TYPE, &_DlgMTMap::OnEnChangeEditFatigueType)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_DEBUFF, &_DlgMTMap::OnEnChangeEditFatigueDebuff)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_STIME, &_DlgMTMap::OnEnChangeEditFatigueStime)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_ATIME, &_DlgMTMap::OnEnChangeEditFatigueAtime)
	ON_EN_CHANGE(IDC_EDIT_FATIGUE_POINT, &_DlgMTMap::OnEnChangeEditFatiguePoint)
	ON_EN_CHANGE(IDC_EDIT_CAMERA_DIST_MAX_LV, &_DlgMTMap::OnEnChangeEditCameraLevel)

	ON_BN_CLICKED(IDC_ADD_MAPID_BUTTON, &_DlgMTMap::OnBnClickedButtonAddMapID)
	
	ON_BN_CLICKED(IDC_CHECK_XGCONSUME, &_DlgMTMap::OnBnClickedCheckXgconsume)
	ON_BN_CLICKED(IDC_CHECK_BATTLETAG_USE, &_DlgMTMap::OnBnClickedCheckBattletagUse)
END_MESSAGE_MAP()

_DlgMTMap::_DlgMTMap(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTMap::IDD, pParent)
{	
}

_DlgMTMap::~_DlgMTMap()
{
}

// =================================================================================================
//
//		virtual
//
// =================================================================================================

void _DlgMTMap::_PostRenderView()
{
	switch( m_combo_UniqObj.GetCurSel() )
	{
	case UT_NONE:
		break;
	case UT_START:
		m_DlgStart._PostRenderView();
		break;
	case UT_REGION:
		m_DlgRegion._PostRenderView();
		break;
	case UT_RESURRECTION:
		m_DlgResurrection._PostRenderView();
		break;
	case UT_LIMIT_EVO:
		m_DlgLimitEvo._PostRenderView();
		break;	
	case UT_BUFF_REGION:
		m_DlgBuffRegion._PostRenderView();
		break;
	//	m_d
	default:
		assert_cs( false );
	}
}

void _DlgMTMap::_PreSave( const char* cFile, bool bAutoSave )
{
	/*if( g_Config.GetEnvironment()->s_bWriteExcel == false )
		return;*/

	if( m_pCurList == NULL )
	{
		assert_cs( m_StartGroup.GetStartList()->empty() == true );
		assert_cs( m_RegionGroup.GetRegionList()->empty() == true );
		assert_cs( m_ResurrectionGroup.GetResurrectionList()->empty() == true );
		assert_cs( m_LimitEvoGroup.GetList()->empty() == true );
		assert_cs( m_BuffRegion.GetList()->empty() == true );
		return;
	}

	if( bAutoSave == true )
		return;

	assert_csm2( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID == m_pCurList->GetInfo()->s_dwMapID, L"_PreSave :: RootID = %d, FTListID = %d",
								nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID, m_pCurList->GetInfo()->s_dwMapID );
	DWORD dwMapID = m_pCurList->GetInfo()->s_dwMapID;

	char cName[ MAX_FILENAME ];	
	m_pCurList->GetInfo()->s_cMapPath = cFile;
	m_pCurList->GetInfo()->s_cMapName = nsCsGBTerrain::g_pCurRoot->GetMapName( cName, MAX_FILENAME );
	//strcpy_s( m_pCurList->GetInfo()->s_cMapPath, MAX_PATH, cFile );
	//strcpy_s( m_pCurList->GetInfo()->s_cMapName, MAX_FILENAME, nsCsGBTerrain::g_pCurRoot->GetMapName( cName, MAX_FILENAME ) );

	// 저장되어 있던것들을 등록
	nsCsMapTable::g_pMapListMng->Delete();
	nsCsMapTable::g_pMapListMng->Clone( &m_MapListFT );

	nsCsMapTable::g_pMapStartMng->GetGroup( dwMapID )->Delete();
	nsCsMapTable::g_pMapStartMng->GetGroup( dwMapID )->Clone( &m_StartGroup );

	nsCsMapTable::g_pMapRegionMng->GetRegionGroup( dwMapID )->Delete();
	nsCsMapTable::g_pMapRegionMng->GetRegionGroup( dwMapID )->Clone( &m_RegionGroup );

	nsCsMapTable::g_pMapRegionMng->GetLimitEvoGroup( dwMapID )->Delete();
	nsCsMapTable::g_pMapRegionMng->GetLimitEvoGroup( dwMapID )->Clone( &m_LimitEvoGroup );

	nsCsMapTable::g_pMapRegionMng->GetBuffRegionGroup( dwMapID )->Delete();
	nsCsMapTable::g_pMapRegionMng->GetBuffRegionGroup( dwMapID )->Clone( &m_BuffRegion );

	nsCsMapTable::g_pMapResurrectionMng->GetGroup( dwMapID )->Delete();
	nsCsMapTable::g_pMapResurrectionMng->GetGroup( dwMapID )->Clone( &m_ResurrectionGroup );
}

void _DlgMTMap::_Delete()
{
	m_MapListFT.Delete();
	m_StartGroup.Delete();
	m_RegionGroup.Delete();
	m_LimitEvoGroup.Delete();
	m_ResurrectionGroup.Delete();
	m_BuffRegion.Delete();
}

void _DlgMTMap::_PreViewReset()
{
	_Delete();
}

void _DlgMTMap::_PostViewReset()
{
	char cPath[ MAX_PATH ];
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::KOREA_TRANSLATION, cPath );

	m_MapListFT.Init( cPath, nsCsFileTable::FT_CLONE );
	m_StartGroup.Init();
	m_RegionGroup.Init();
	m_LimitEvoGroup.Init();
	m_ResurrectionGroup.Init();
	//m_BuffRegion.init();

	int nCount = 0;
	m_combo_MapID.Clear();
	m_combo_Resurrection.Clear();

	m_combo_MapID.InsertString( nCount, L"임시 맵" );
	m_combo_MapID.SetItemData( nCount, 0 );
	m_combo_Resurrection.InsertString( nCount, L"임시 맵" );
	m_combo_Resurrection.SetItemData( nCount, 0 );

	nCount++;

	CsMapList::MAP* pMapList = m_MapListFT.GetListMap();
	CsMapList::MAP_IT it = pMapList->begin();
	for(  ; it != pMapList->end(); ++it, ++nCount )
	{
		CsMapList* pMapList = it->second;
		CsMapList::sINFO* pInfo = pMapList->GetInfo();
		TCHAR mapName[1024]={0,};
		swprintf_s( mapName, 1024, _T("%s - %d"), pInfo->s_szMapDiscript.c_str(), pInfo->s_dwMapID );

		m_combo_MapID.InsertString( nCount, mapName );
		m_combo_MapID.SetItemData( nCount, it->first );
		m_combo_Resurrection.InsertString( nCount, mapName );
		m_combo_Resurrection.SetItemData( nCount, 0 );
	}

	__ResetComboMapID( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID );
}

// =================================================================================================
//
//		Base
//
// =================================================================================================

void _DlgMTMap::_ViewInit()
{

}

void _DlgMTMap::OnBnClickedButtonAddMapID()
{
	int nAddMapID = nsCsMfcFunc::Control2Int( &m_edit_AddMapID);
	int nCount = m_combo_MapID.GetCount();
	for( int i=0; i<nCount; ++i )
	{
		if( nAddMapID == (DWORD)m_combo_MapID.GetItemData( i ) )
		{
			CsMessageBox( MB_OK, L"맵 아이디가 이미 존재합니다." );
			return;
		}
	}

	TCHAR newToolMapName[MAX_PATH] = {0,};
	m_edit_AddToolMapName.GetWindowText( newToolMapName, MAX_PATH );

	TCHAR mapName[1024]={0,};
	swprintf_s( mapName, 1024, _T("%s - %d"), newToolMapName, nAddMapID );

	m_combo_MapID.InsertString( nCount, mapName );
	m_combo_MapID.SetItemData( nCount, nAddMapID );

	m_combo_Resurrection.InsertString( nCount, mapName );
	m_combo_Resurrection.SetItemData( nCount, nAddMapID );

	m_edit_AddMapID.SetWindowText( L"" );
	m_edit_AddToolMapName.SetWindowText( L"" );

	CsMessageBox( MB_OK, L"맵 아이디가 추가 되었습니다." );
}	

BOOL _DlgMTMap::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_pCurList = NULL;	
	m_StartGroup.Init();
	m_RegionGroup.Init();
	m_ResurrectionGroup.Init();
	m_LimitEvoGroup.Init();
	//m_BuffRegion.init();
	CRect rc;
	GetWindowRect( &rc );
	m_DlgStart.Create( _DlgMTMap_Start::IDD, this );
	m_DlgStart.SetWindowPos( NULL, 0, 360, rc.Width(), rc.Height() - 234, 0 );	
	
	m_DlgRegion.Create( _DlgMTMap_Region::IDD, this );
	m_DlgRegion.SetWindowPos( NULL, 0, 360, rc.Width(), rc.Height() - 234, 0 );	
	
	m_DlgResurrection.Create( _DlgMTMap_Resurrection::IDD, this );
	m_DlgResurrection.SetWindowPos( NULL, 0, 360, rc.Width(), rc.Height() - 234, 0 );	
	
	m_DlgLimitEvo.Create( _DlgMTMap_LimitEvo::IDD, this );
	m_DlgLimitEvo.SetWindowPos( NULL, 0, 360, rc.Width(), rc.Height() - 234, 0 );	
	
	m_DlgBuffRegion.Create(_DlgMTMap_BuffRegion::IDD, this);
	m_DlgBuffRegion.SetWindowPos( NULL, 0, 360, rc.Width(), rc.Height() - 234, 0 );	


	//__InitMapIDCombo( &m_combo_MapID );
	//__InitMapIDCombo( &m_combo_Resurrection );

	m_edit_MapName.SetLimitText( MAX_FILENAME - 1 );
	m_edit_MapDescription.SetLimitText( MAX_PATH - 1 );
	m_edit_MapDescription_Eng.SetLimitText( MAX_PATH - 1 );

	m_combo_UniqObj.InsertString( UT_NONE, L"선택해 주세요." );
	m_combo_UniqObj.InsertString( UT_START, L"시작 위치" );
	m_combo_UniqObj.InsertString( UT_REGION, L"지역 설정" );
	m_combo_UniqObj.InsertString( UT_RESURRECTION, L"부활 위치" );
	m_combo_UniqObj.InsertString( UT_LIMIT_EVO, L"진화 제한 지역" );
	m_combo_UniqObj.InsertString( UT_BUFF_REGION, L"버프 구역" );
	m_combo_UniqObj.SetCurSel( UT_NONE );
	OnCbnSelchangeComboUniqObj();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// void _DlgMTMap::__InitMapIDCombo( CComboBox* pComboBox )
// {
// 	int nComboIdx = 0;
// 	pComboBox->InsertString( nComboIdx, L"임시 맵" );
// 	pComboBox->SetItemData( nComboIdx, MI_NONE );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"GM ROOM");
// 	pComboBox->SetItemData( nComboIdx, MI_GM_ROOM );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"닷츠 센터" );
// 	pComboBox->SetItemData( nComboIdx, MI_DOTS );
// 	pComboBox->InsertString( ++nComboIdx, L"D-터미널" );
// 	pComboBox->SetItemData( nComboIdx, MI_DTERMINAL );
// 	pComboBox->InsertString( ++nComboIdx, L"닷츠 리뉴얼" );
// 	pComboBox->SetItemData( nComboIdx, MI_DATSTEST );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"닷트 본부 지하수도" );
// 	pComboBox->SetItemData( nComboIdx, MI_DATSUNDERGROUND );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"콜로세움" );
// 	pComboBox->SetItemData( nComboIdx, MI_COLOSSEUM );
// 	
// 	pComboBox->InsertString( ++nComboIdx, L"구 D-터미널 1F" );
// 	pComboBox->SetItemData( nComboIdx, MI_DTERMINAL_MAZE_1F );
// 	pComboBox->InsertString( ++nComboIdx, L"구 D-터미널 2F" );
// 	pComboBox->SetItemData( nComboIdx, MI_DTERMINAL_MAZE_2F );
// 	pComboBox->InsertString( ++nComboIdx, L"구 D-터미널 3F" );
// 	pComboBox->SetItemData( nComboIdx, MI_DTERMINAL_MAZE_3F );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"신비한 GM의 방" );
// 	pComboBox->SetItemData( nComboIdx, MI_GM_ROOM2 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"요코하마" );
// 	pComboBox->SetItemData( nComboIdx, MI_YOKOHAMA );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"리얼타운1 - 작은숲공원" );
// 	pComboBox->SetItemData( nComboIdx, MI_RT1_FIELD1_1 );
// 	pComboBox->InsertString( ++nComboIdx, L"리얼타운1 - 작은숲산책로" );
// 	pComboBox->SetItemData( nComboIdx, MI_RT1_FIELD1_2 );
// 	pComboBox->InsertString( ++nComboIdx, L"리얼타운1 - 나무벤치공원" );
// 	pComboBox->SetItemData( nComboIdx, MI_RT1_FIELD1_3 );
// 	pComboBox->InsertString( ++nComboIdx, L"리얼타운1 - 나무벤치산책로" );
// 	pComboBox->SetItemData( nComboIdx, MI_RT1_FIELD1_4 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"요코하마pt2" );
// 	pComboBox->SetItemData( nComboIdx, MI_YOKOHAMA_PT2 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"요코하마 동쪽" );
// 	pComboBox->SetItemData( nComboIdx, MI_YOKOHAMA_EAST );
// 	pComboBox->InsertString( ++nComboIdx, L"리얼타운1 - 정유공장1" );
// 	pComboBox->SetItemData( nComboIdx, MI_RT1_FIELD2_1 );
// 	pComboBox->InsertString( ++nComboIdx, L"리얼타운1 - 정유공장2" );
// 	pComboBox->SetItemData( nComboIdx, MI_RT1_FIELD2_2 );
// 	pComboBox->InsertString( ++nComboIdx, L"리얼타운1 - 정유공장3" );
// 	pComboBox->SetItemData( nComboIdx, MI_RT1_FIELD2_3 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"캠프장" );
// 	pComboBox->SetItemData( nComboIdx, MI_CAMPGROUND );
// 	pComboBox->InsertString( ++nComboIdx, L"빛의 언덕 " );
// 	pComboBox->SetItemData( nComboIdx, MI_HIKARIGAOKA );
// 	pComboBox->InsertString( ++nComboIdx, L"시바우라 " );
// 	pComboBox->SetItemData( nComboIdx, MI_SHIBAURA );
// 	pComboBox->InsertString( ++nComboIdx, L"오다이바 " );
// 	pComboBox->SetItemData( nComboIdx, MI_ODAIBA );
// 	pComboBox->InsertString( ++nComboIdx, L"무너진 레인보우 브릿지" );
// 	pComboBox->SetItemData( nComboIdx, MI_RAINBOW_BRIDGE );
// 	pComboBox->InsertString( ++nComboIdx, L"무너진 레인보우 브릿지 (영웅)" );
// 	pComboBox->SetItemData( nComboIdx, MI_RAINBOW_BRIDGE_HERO );
// 	
// 
// 
// 	pComboBox->InsertString( ++nComboIdx, L"서부마을1" );
// 	pComboBox->SetItemData( nComboIdx, MI_WESTTOWN1 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"황야지역" );
// 	pComboBox->SetItemData( nComboIdx, MI_WEST_BATTLE1 );
// 	pComboBox->InsertString( ++nComboIdx, L"디지몬목장" );
// 	pComboBox->SetItemData( nComboIdx, MI_DIGIMON_FORM1 );
// 	pComboBox->InsertString( ++nComboIdx, L"바람계곡" );
// 	pComboBox->SetItemData( nComboIdx, MI_WEST_BATTLE2 );
// 	pComboBox->InsertString( ++nComboIdx, L"황폐한 유적지" );
// 	pComboBox->SetItemData( nComboIdx, MI_WEST_BATTLE3 );
// 	pComboBox->InsertString( ++nComboIdx, L"서부마을 동쪽" );
// 	pComboBox->SetItemData( nComboIdx, MI_WESTTOWN_EAST );
// 	pComboBox->InsertString( ++nComboIdx, L"서부마을 외곽" );
// 	pComboBox->SetItemData( nComboIdx, MI_WESTTOWN_OUT );
// 	pComboBox->InsertString( ++nComboIdx, L"서부마을 서쪽" );
// 	pComboBox->SetItemData( nComboIdx, MI_WESTTOWN_WEST );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"디지몬 미궁 필드" );
// 	pComboBox->SetItemData( nComboIdx, MI_DIGIMON_MAZE );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"다크타워 황무지" );
// 	pComboBox->SetItemData( nComboIdx, MI_WEST_DARKTOWER );	
// 
// 	pComboBox->InsertString( ++nComboIdx, L"카이저의 실험실(일반)" );
// 	pComboBox->SetItemData( nComboIdx, MI_KAISER_LABORATORY_NORMAL );	
// 	pComboBox->InsertString( ++nComboIdx, L"카이저의 실험실(영웅)" );
// 	pComboBox->SetItemData( nComboIdx, MI_KAISER_LABORATORY_HERO );	
// 	
// 	pComboBox->InsertString( ++nComboIdx, L"디지몬 미궁 B1" );
// 	pComboBox->SetItemData( nComboIdx, MI_DIGIMON_MAZE_B1 );
// 	pComboBox->InsertString( ++nComboIdx, L"디지몬 미궁 B2" );
// 	pComboBox->SetItemData( nComboIdx, MI_DIGIMON_MAZE_B2 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"디지몬 미궁 F1" );
// 	pComboBox->SetItemData( nComboIdx, MI_DIGIMON_MAZE_F1 );
// 	pComboBox->InsertString( ++nComboIdx, L"디지몬 미궁 F2" );
// 	pComboBox->SetItemData( nComboIdx, MI_DIGIMON_MAZE_F2 );
// 	pComboBox->InsertString( ++nComboIdx, L"디지몬 미궁 F3" );
// 	pComboBox->SetItemData( nComboIdx, MI_DIGIMON_MAZE_F3 );
// 	pComboBox->InsertString( ++nComboIdx, L"디지몬 미궁 F4" );
// 	pComboBox->SetItemData( nComboIdx, MI_DIGIMON_MAZE_F4 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"시작의 마을" );
// 	pComboBox->SetItemData( nComboIdx, FL_ISLAND_STARTVILLAGE );
// 	pComboBox->InsertString( ++nComboIdx, L"은빛 호수" );
// 	pComboBox->SetItemData( nComboIdx, FL_SILVER_LAKE );
// 	pComboBox->InsertString( ++nComboIdx, L"고요한 숲" );
// 	pComboBox->SetItemData( nComboIdx, FL_SLIENT_FOREST );
// 	pComboBox->InsertString( ++nComboIdx, L"잃어버린 유적지" );
// 	pComboBox->SetItemData( nComboIdx, FL_LOST_TEMPLE );
// 	pComboBox->InsertString( ++nComboIdx, L"유적지 인던" );
// 	pComboBox->SetItemData( nComboIdx, FL_IDG_LOST_TEMPLE );
// 	pComboBox->InsertString( ++nComboIdx, L"파일섬 해안가" );
// 	pComboBox->SetItemData( nComboIdx, FL_FILE_ISLAND );
// 	pComboBox->InsertString( ++nComboIdx, L"무한 산" );
// 	pComboBox->SetItemData( nComboIdx, FL_MT_INFINITY );
// 	pComboBox->InsertString( ++nComboIdx, L"무한 산 던전" );
// 	pComboBox->SetItemData( nComboIdx, FL_DG_INFINITY_ );
// 	pComboBox->InsertString( ++nComboIdx, L"무한 산 인던" );
// 	pComboBox->SetItemData( nComboIdx, FL_IDG_INFINITY_ );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"서버 대륙" );
// 	pComboBox->SetItemData( nComboIdx, CW_SERVER_PANGAEA );
// 	pComboBox->InsertString( ++nComboIdx, L"서버 대륙 협곡" );
// 	pComboBox->SetItemData( nComboIdx, CW_SERVER_CANYON );
// 	pComboBox->InsertString( ++nComboIdx, L"서버 대륙 피라미드" );
// 	pComboBox->SetItemData( nComboIdx, CW_SERVER_PYRAMID );
// 	pComboBox->InsertString( ++nComboIdx, L"데이터몬 미궁(일반)" );
// 	pComboBox->SetItemData( nComboIdx, CW_SERVER_PYRAMID_NORMAL );
// 	pComboBox->InsertString( ++nComboIdx, L"데이터몬 미궁(영웅)" );
// 	pComboBox->SetItemData( nComboIdx, CW_SERVER_PYRAMID_HERO );
// 	pComboBox->InsertString( ++nComboIdx, L"데이터몬 미궁(이벤트)" );
// 	pComboBox->SetItemData( nComboIdx, CW_SERVER_PYRAMID_EVENT );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"그린존" );
// 	pComboBox->SetItemData( nComboIdx, CW_GREEN_ZONE );
// 	pComboBox->InsertString( ++nComboIdx, L"그린존 미소마을" );
// 	pComboBox->SetItemData( nComboIdx, CW_GREEN_ZONE_TOWN );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"현무몬 던전" );
// 	pComboBox->SetItemData( nComboIdx, XUANWUMON_DUNGEON );
// 	pComboBox->InsertString( ++nComboIdx, L"주작몬 인던" );
// 	pComboBox->SetItemData( nComboIdx, Zhuqiaomon_DUNGEON );
// 	pComboBox->InsertString( ++nComboIdx, L"백호몬 인던" );
// 	pComboBox->SetItemData( nComboIdx, Baihumon_DUNGEON );
// 	pComboBox->InsertString( ++nComboIdx, L"청룡몬 인던" );
// 	pComboBox->SetItemData( nComboIdx, Qinglongmon_DUNGEON );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"눈보라마을" );
// 	pComboBox->SetItemData( nComboIdx, MI_ICE_TOWN );
// 	pComboBox->InsertString( ++nComboIdx, L"얼어붙은대지" );
// 	pComboBox->SetItemData( nComboIdx, MI_ICE_1 );
// 	pComboBox->InsertString( ++nComboIdx, L"눈사람마을" );
// 	pComboBox->SetItemData( nComboIdx, MI_ICE_2 );
// 	pComboBox->InsertString( ++nComboIdx, L"뒤틀린데이터마을" );
// 	pComboBox->SetItemData( nComboIdx, MI_ICE_3 );
// 	pComboBox->InsertString( ++nComboIdx, L"무한빙벽" );
// 	pComboBox->SetItemData( nComboIdx, MI_ICE_4 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"배틀 개인전 1" );
// 	pComboBox->SetItemData( nComboIdx, MI_BATTLE_S_1 );
// 	pComboBox->InsertString( ++nComboIdx, L"배틀 개인전 2" );
// 	pComboBox->SetItemData( nComboIdx, MI_BATTLE_S_2 );
// 	pComboBox->InsertString( ++nComboIdx, L"배틀 개인전 3" );
// 	pComboBox->SetItemData( nComboIdx, MI_BATTLE_S_3 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"배틀 단체전(8vs8)" );
// 	pComboBox->SetItemData( nComboIdx, BATTLE_ZONE_A08 );
// 	pComboBox->InsertString( ++nComboIdx, L"배틀 단체전(16vs16)" );
// 	pComboBox->SetItemData( nComboIdx, BATTLE_ZONE_A16 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"배틀 단체전 1" );
// 	pComboBox->SetItemData( nComboIdx, MI_BATTLE_G_1 );
// 	pComboBox->InsertString( ++nComboIdx, L"배틀 단체전 2" );
// 	pComboBox->SetItemData( nComboIdx, MI_BATTLE_G_2 );
// 	pComboBox->InsertString( ++nComboIdx, L"배틀 단체전 3" );
// 	pComboBox->SetItemData( nComboIdx, MI_BATTLE_G_3 );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"홀리 디펜스" );
// 	pComboBox->SetItemData( nComboIdx, HOLY_POINT );
// 
// 	pComboBox->InsertString( ++nComboIdx, L"테스트맵 1" );
// 	pComboBox->SetItemData( nComboIdx, MI_TESTMAP_1 );
// 	pComboBox->InsertString( ++nComboIdx, L"테스트맵 2" );
// 	pComboBox->SetItemData( nComboIdx, MI_TESTMAP_2 );
//}

void _DlgMTMap::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
	}
}

// =================================================================================================
//
//		Map ID
//
// =================================================================================================
void _DlgMTMap::__ResetComboMapID( DWORD dwMapID )
{
	int nCount = m_combo_MapID.GetCount();
	for( int i=0; i<nCount; ++i )
	{
		if( dwMapID == (DWORD)m_combo_MapID.GetItemData( i ) )
		{
			m_combo_MapID.SetCurSel( i );
			OnCbnSelchangeComboMapid();
			return;
		}
	}
	assert_cs( false );
}

// 맵 아이디 변경시 호출
void _DlgMTMap::OnCbnSelchangeComboMapid()
{
	if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID != (DWORD)m_combo_MapID.GetItemData( m_combo_MapID.GetCurSel() ) )
	{
		if( CsMessageBox( MB_OKCANCEL, L"맵 아이디를 변경 하면 저장되지 않은 Map.xls 설정 정보는 사라 집니다.\n계속하시겠습니까?" ) == IDCANCEL )
		{
			__ResetComboMapID( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID );
			return;
		}
	}

	if( m_combo_MapID.GetCurSel() == 0 )
	{
		m_pCurList = NULL;
		m_StartGroup.Delete();
		m_RegionGroup.Delete();
		m_ResurrectionGroup.Delete();
		m_LimitEvoGroup.Delete();
		m_BuffRegion.Delete();

		m_edit_MapName.SetWindowText( L"" );
		m_edit_MapDescription.SetWindowText( L"" );
		m_edit_MapDescription_Eng.SetWindowText( L"" );
		m_edit_MapName.EnableWindow( false );
		m_edit_MapDescription.EnableWindow( false );
		m_edit_MapDescription_Eng.EnableWindow( false );
		__EnableUniqObj( false );

		nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID = 0;
	}
	else
	{
		DWORD dwMapID = (DWORD)m_combo_MapID.GetItemData( m_combo_MapID.GetCurSel() );
		nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID = dwMapID;

		// 없다면 추가
		if( m_MapListFT.IsList( dwMapID ) == false )
		{
			CsMapList::sINFO info;
			info.Init();
			info.s_dwMapID = dwMapID;			
			m_MapListFT.AddList( &info );
		}
		m_pCurList = m_MapListFT.GetList( dwMapID );
		// 넓이 높이 다시 기입
		m_pCurList->GetInfo()->s_nWidth = (int)nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth();
		m_pCurList->GetInfo()->s_nHeight = (int)nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight();

		m_StartGroup.Delete();
		m_StartGroup.Clone( nsCsMapTable::g_pMapStartMng->GetGroup( dwMapID ) );
		m_RegionGroup.Delete();
		m_RegionGroup.Clone( nsCsMapTable::g_pMapRegionMng->GetRegionGroup( dwMapID ) );
		m_ResurrectionGroup.Delete();
		m_ResurrectionGroup.Clone( nsCsMapTable::g_pMapResurrectionMng->GetGroup( dwMapID ) );
		m_LimitEvoGroup.Delete();
		m_LimitEvoGroup.Clone( nsCsMapTable::g_pMapRegionMng->GetLimitEvoGroup( dwMapID ) );
		m_BuffRegion.Delete();
		m_BuffRegion.Clone(nsCsMapTable::g_pMapRegionMng->GetBuffRegionGroup( dwMapID ) );

		__EnableUniqObj( true );

		m_edit_MapName.EnableWindow( true );
		m_edit_MapDescription.EnableWindow( true );
		m_edit_MapDescription_Eng.EnableWindow( true );

		TCHAR szName[ MAX_FILENAME ];
		M2W( szName, m_pCurList->GetInfo()->s_cMapName.c_str(), MAX_FILENAME );
		m_edit_MapName.SetWindowText( szName );
		m_edit_MapDescription.SetWindowText( m_pCurList->GetInfo()->s_szMapDiscript.c_str() );
		m_edit_MapDescription_Eng.SetWindowText( m_pCurList->GetInfo()->s_szMapDiscript_Eng.c_str() );

		// 부활 맵
		CsMapList::sINFO* pFTMap = m_MapListFT.GetList( dwMapID )->GetInfo();
		nsCsMfcFunc::ComboSelect_DWORD_Data( &m_combo_Resurrection, pFTMap->s_dwResurrectionMapID );

		// 피로도
		nsCsMfcFunc::Int2Control( &m_edit_FG_Type, pFTMap->s_nFatigue_Type );
		nsCsMfcFunc::Int2Control( &m_edit_FG_DBuff, pFTMap->s_nFatigue_DeBuff );
		nsCsMfcFunc::Int2Control( &m_edit_FG_STime, pFTMap->s_nFatigue_StartTime );
		nsCsMfcFunc::Int2Control( &m_edit_FG_ATime, pFTMap->s_nFatigue_AddTime );
		nsCsMfcFunc::Int2Control( &m_edit_FG_Point, pFTMap->s_nFatigue_AddPoint );
		nsCsMfcFunc::Int2Control( &m_edit_Camera_Max_Level, pFTMap->s_nCamera_Max_Level );		
		m_check_Xgconsume.SetCheck( !pFTMap->s_bXgConsumeType );
		m_check_BattletagUse.SetCheck( !pFTMap->s_bBattleTagUse );
	}

	g_ModelMng.ResetResistMapNpc();
	GET_MV_MAINTAB_DLG->_GetDlgMonster()->_ResetMapID();
	
	__ReadBGSound();

	
	m_combo_Resurrection.EnableWindow( ( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID != 0 ) );	
}


// 맵 설명 변경
void _DlgMTMap::OnEnChangeEditMapDescription()
{
	if( m_pCurList == NULL )
		return;

	TCHAR szMapDiscript[ MAX_PATH ] = {0,};
	m_edit_MapDescription.GetWindowText( szMapDiscript, MAX_PATH );
	m_pCurList->GetInfo()->s_szMapDiscript = szMapDiscript;
}

void _DlgMTMap::OnEnChangeEditMapDescriptionEng()
{
	if( m_pCurList == NULL )
		return;

	TCHAR szMapDiscript[ MAX_PATH ] = {0,};
	m_edit_MapDescription_Eng.GetWindowText( szMapDiscript, MAX_PATH );
	m_pCurList->GetInfo()->s_szMapDiscript_Eng = szMapDiscript;
}

void _DlgMTMap::__ReadBGSound()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( dwMapID == 0 )
	{
		m_edit_MapBGSound.SetWindowText( L"" );
		return;
	}

	assert_cs( m_pCurList != NULL );
	TCHAR sz[ MAX_FILENAME ];
	M2W( sz, m_pCurList->GetInfo()->s_cBGSound.c_str(), MAX_FILENAME );
	m_edit_MapBGSound.SetWindowText( sz );
}

// 배경음 로드 버튼
void _DlgMTMap::OnBnClickedButtonLoadbgsound()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( dwMapID == 0 )
	{
		CsMessageBox( MB_OK, L"맵 ID 가 0인상태에서는 사운드를 넣을수 없습니다." );
		return;
	}

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

 	assert_cs( m_pCurList != NULL );
	char bgSound[MAX_FILENAME] = {0,};
 	W2M( bgSound, nsCSFILE::GetFileName( szOut ), MAX_FILENAME );
	m_pCurList->GetInfo()->s_cBGSound = bgSound;
	_tcscpy_s( g_Resist.GetMap()->s_szBGSound, MAX_PATH, szRootPath );

	__ReadBGSound();
}

// 배경음 제거 버튼
void _DlgMTMap::OnBnClickedButtonDelBgsound()
{
	if( m_pCurList == NULL )
		return;

	m_pCurList->GetInfo()->s_cBGSound.clear();
	__ReadBGSound();
}

// 부활 맵 ID 콤보 박스 변경
void _DlgMTMap::OnCbnSelchangeComboResurrection()
{
	int nSel = m_combo_Resurrection.GetCurSel();
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	m_MapListFT.GetList( dwMapID )->GetInfo()->s_dwResurrectionMapID = (DWORD)m_combo_Resurrection.GetItemData( nSel );
}



// =================================================================================================
//
//		Uniq Object
//
// =================================================================================================
void _DlgMTMap::__EnableUniqObj( bool bEnable )
{
	if( bEnable == false )
	{
		m_combo_UniqObj.SetCurSel( 0 );
		OnCbnSelchangeComboUniqObj();
		m_combo_UniqObj.EnableWindow( bEnable );
	}
	else
	{
		m_combo_UniqObj.EnableWindow( bEnable );
		OnCbnSelchangeComboUniqObj();
	}
}

// 유닉 오브젝트 콤보 박스 변경
void _DlgMTMap::OnCbnSelchangeComboUniqObj()
{
	m_list_UniqObj.ResetContent();
	m_DlgStart.ShowWindow( SW_HIDE );
	m_DlgRegion.ShowWindow( SW_HIDE );
	m_DlgResurrection.ShowWindow( SW_HIDE );
	m_DlgLimitEvo.ShowWindow( SW_HIDE );
	m_DlgBuffRegion.ShowWindow(SW_HIDE);

	m_edit_AddMapID.ShowWindow(SW_SHOW);
	m_btn_AddMapID.ShowWindow(SW_SHOW);
	m_edit_AddToolMapName.ShowWindow(SW_SHOW);
	m_Static_AddMapID.ShowWindow(SW_SHOW);
	m_Static_ToolMapName.ShowWindow(SW_SHOW);
	m_Static_AddMapID_Group.ShowWindow(SW_SHOW);

	switch( m_combo_UniqObj.GetCurSel() )
	{
	case UT_NONE:
		m_btn_AddUniqObj.EnableWindow( false );
		break;
	case UT_START:
		{
			m_btn_AddUniqObj.EnableWindow( true );
			_ResetUniqObjList_ForStart();
		}
		break;
	case UT_REGION:
		{
			m_btn_AddUniqObj.EnableWindow( true );
			_ResetUniqObjList_ForRegion();
		}
		break;
	case UT_RESURRECTION:
		{
			m_btn_AddUniqObj.EnableWindow( true );
			_ResetUniqObjList_ForResurrection();
		}
		break;
	case UT_LIMIT_EVO:
		{
			m_btn_AddUniqObj.EnableWindow( true );
			_ResetUniqObjList_ForLimitEvo();
		}
		break;
	case UT_BUFF_REGION:
		{
			m_btn_AddUniqObj.EnableWindow( true );
			_ResetUniqObjList_ForBuffRegion();
		}
		break;
	default:
		assert_cs( false );
	}
}

// 유닉 오브젝트 콤보 박스 추가
void _DlgMTMap::OnBnClickedButtonAddUniqobj()
{
	switch( m_combo_UniqObj.GetCurSel() )
	{
	case UT_NONE:
		assert_cs( false );
		break;
	case UT_START:
		{
			DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
			assert_cs( m_pCurList != NULL );			
			assert_cs( m_pCurList->GetInfo()->s_dwMapID == dwMapID );
			
			// 그룹에 스타트 추가.
			CsMapStart::sINFO info;
			info.Init( dwMapID );
			m_StartGroup.AddStart( &info );			

			// 스타트 리스트 읽어 온다.
			_ResetUniqObjList_ForStart();

			// 추가된 스타트 선택 - 가장 마지막
			m_list_UniqObj.SetCurSel( m_list_UniqObj.GetCount() - 1 );
			OnLbnSelchangeListUniqObj();
		}
		break;
	case UT_REGION:
		{
			DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
			assert_cs( m_pCurList != NULL );
			assert_cs( m_pCurList->GetInfo()->s_dwMapID == dwMapID );

			// 그룹에 지역 추가
			CsMapRegion::sINFO info;
			info.Init( dwMapID );
			m_RegionGroup.AddRegion( &info );

			_ResetUniqObjList_ForRegion();

			// 추가된 스타트 선택 - 가장 마지막
			m_list_UniqObj.SetCurSel( m_list_UniqObj.GetCount() - 1 );
			OnLbnSelchangeListUniqObj();
		}
		break;
	case UT_RESURRECTION:
		{
			DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
			assert_cs( m_pCurList != NULL );
			assert_cs( m_pCurList->GetInfo()->s_dwMapID == dwMapID );

			// 그룹에 추가
			CsMapResurrection::sINFO info;
			info.Init( dwMapID );
			m_ResurrectionGroup.AddResurrection( &info );

			_ResetUniqObjList_ForResurrection();

			// 추가된 스타트 선택 - 가장 마지막
			m_list_UniqObj.SetCurSel( m_list_UniqObj.GetCount() - 1 );
			OnLbnSelchangeListUniqObj();
		}
		break;
	case UT_LIMIT_EVO:
		{
			DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
			assert_cs( m_pCurList != NULL );
			assert_cs( m_pCurList->GetInfo()->s_dwMapID == dwMapID );

			// 그룹에 지역 추가
			CsMap_LimitEvo::sINFO info;
			info.Init( dwMapID );
			m_LimitEvoGroup.AddRegion( &info );

			_ResetUniqObjList_ForLimitEvo();

			// 추가된 스타트 선택 - 가장 마지막
			m_list_UniqObj.SetCurSel( m_list_UniqObj.GetCount() - 1 );
			OnLbnSelchangeListUniqObj();
		}
		break;

	case UT_BUFF_REGION:
		{
			DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
			assert_cs( m_pCurList != NULL );
			assert_cs( m_pCurList->GetInfo()->s_dwMapID == dwMapID );

			// 그룹에 지역 추가
			CsMap_BuffRegion::sINFO info;
			info.Init( dwMapID );
			CsMap_BuffRegion* pkBuffRegion =	m_BuffRegion.AddRegionInfo( &info );
			m_DlgBuffRegion.OnAddBuffRegion(pkBuffRegion);
			_ResetUniqObjList_ForBuffRegion();
			m_list_UniqObj.SetCurSel( m_list_UniqObj.GetCount() - 1 );
			OnLbnSelchangeListUniqObj();
		}
		break;

	default:
		assert_cs( false );
	}
}

// 유닉 오브젝트 리스트 변경
void _DlgMTMap::OnLbnSelchangeListUniqObj()
{
	switch( m_combo_UniqObj.GetCurSel() )
	{
	case UT_NONE:
		assert_cs( false );
		break;
	case UT_START:
		{
			if( m_list_UniqObj.GetCurSel() >= 0 )
			{				
				m_edit_AddMapID.ShowWindow(SW_HIDE);
				m_btn_AddMapID.ShowWindow(SW_HIDE);
				m_edit_AddToolMapName.ShowWindow(SW_HIDE);
				m_Static_AddMapID.ShowWindow(SW_HIDE);
				m_Static_ToolMapName.ShowWindow(SW_HIDE);
				m_Static_AddMapID_Group.ShowWindow(SW_HIDE);
				m_DlgStart.ShowWindow( SW_HIDE );
				m_DlgStart.ShowWindow( SW_SHOW );
				m_DlgStart._SetStart( (CsMapStart*)m_list_UniqObj.GetItemData( m_list_UniqObj.GetCurSel() ) );
			}
		}
		break;
	case UT_REGION:
		{
			if( m_list_UniqObj.GetCurSel() >= 0 )
			{ 
				m_edit_AddMapID.ShowWindow(SW_HIDE);
				m_btn_AddMapID.ShowWindow(SW_HIDE);
				m_edit_AddToolMapName.ShowWindow(SW_HIDE);
				m_Static_AddMapID.ShowWindow(SW_HIDE);
				m_Static_ToolMapName.ShowWindow(SW_HIDE);
				m_Static_AddMapID_Group.ShowWindow(SW_HIDE);
				m_DlgRegion.ShowWindow( SW_HIDE );
				m_DlgRegion.ShowWindow( SW_SHOW );
				m_DlgRegion._SetRegion( (CsMapRegion*)m_list_UniqObj.GetItemData( m_list_UniqObj.GetCurSel() ) );
			}
		}
		break;
	case UT_RESURRECTION:
		{
			if( m_list_UniqObj.GetCurSel() >= 0 )
			{ 
				m_edit_AddMapID.ShowWindow(SW_HIDE);
				m_btn_AddMapID.ShowWindow(SW_HIDE);
				m_edit_AddToolMapName.ShowWindow(SW_HIDE);
				m_Static_AddMapID.ShowWindow(SW_HIDE);
				m_Static_ToolMapName.ShowWindow(SW_HIDE);
				m_Static_AddMapID_Group.ShowWindow(SW_HIDE);
				m_DlgResurrection.ShowWindow( SW_HIDE );
				m_DlgResurrection.ShowWindow( SW_SHOW );
				m_DlgResurrection._SetResurrection( (CsMapResurrection*)m_list_UniqObj.GetItemData( m_list_UniqObj.GetCurSel() ) );
			}
		}
		break;
	case UT_LIMIT_EVO:
		{
			if( m_list_UniqObj.GetCurSel() >= 0 )
			{ 
				m_edit_AddMapID.ShowWindow(SW_HIDE);
				m_btn_AddMapID.ShowWindow(SW_HIDE);
				m_edit_AddToolMapName.ShowWindow(SW_HIDE);
				m_Static_AddMapID.ShowWindow(SW_HIDE);
				m_Static_ToolMapName.ShowWindow(SW_HIDE);
				m_Static_AddMapID_Group.ShowWindow(SW_HIDE);
				m_DlgLimitEvo.ShowWindow( SW_HIDE );
				m_DlgLimitEvo.ShowWindow( SW_SHOW );
				m_DlgLimitEvo._SetLimitEvo( (CsMap_LimitEvo*)m_list_UniqObj.GetItemData( m_list_UniqObj.GetCurSel() ) );
			}
		}
		break;
	case UT_BUFF_REGION:
		if( m_list_UniqObj.GetCurSel() >= 0 )
		{ 
			m_edit_AddMapID.ShowWindow(SW_HIDE);
			m_btn_AddMapID.ShowWindow(SW_HIDE);
			m_edit_AddToolMapName.ShowWindow(SW_HIDE);
			m_Static_AddMapID.ShowWindow(SW_HIDE);
			m_Static_ToolMapName.ShowWindow(SW_HIDE);
			m_Static_AddMapID_Group.ShowWindow(SW_HIDE);
			m_DlgBuffRegion.ShowWindow( SW_HIDE );
			m_DlgBuffRegion.ShowWindow( SW_SHOW );

			int currSel = m_list_UniqObj.GetCurSel(); 
			CsMap_BuffRegion* PKBuffRegion = (CsMap_BuffRegion*)m_list_UniqObj.GetItemData( currSel );

			m_DlgBuffRegion.OnSelectBuffRegion( PKBuffRegion );

		}

		break;


	default:
		assert_cs( false );
	}
}

// 유닉 오브젝트 리스트에서 제거
void _DlgMTMap::OnBnClickedButtonDelUniqobj()
{
	switch( m_combo_UniqObj.GetCurSel() )
	{
	case UT_NONE:
		break;
	case UT_START:
		{
			if( m_list_UniqObj.GetCurSel() >= 0 )
			{
				m_DlgStart.ShowWindow( SW_HIDE );
				m_edit_AddMapID.ShowWindow(SW_SHOW);
				m_btn_AddMapID.ShowWindow(SW_SHOW);
				m_edit_AddToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID.ShowWindow(SW_SHOW);
				m_Static_ToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID_Group.ShowWindow(SW_SHOW);
				m_StartGroup.DeleteStart( (CsMapStart*)m_list_UniqObj.GetItemData( m_list_UniqObj.GetCurSel() ) );
				_ResetUniqObjList_ForStart();
			}
		}		
		break;
	case UT_REGION:
		{
			if( m_list_UniqObj.GetCurSel() >= 0 )
			{
				m_DlgRegion.ShowWindow( SW_HIDE );
				m_edit_AddMapID.ShowWindow(SW_SHOW);
				m_btn_AddMapID.ShowWindow(SW_SHOW);
				m_edit_AddToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID.ShowWindow(SW_SHOW);
				m_Static_ToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID_Group.ShowWindow(SW_SHOW);
				m_RegionGroup.DeleteRegion( (CsMapRegion*)m_list_UniqObj.GetItemData( m_list_UniqObj.GetCurSel() ) );
				_ResetUniqObjList_ForRegion();
			}
		}
		break;
	case UT_RESURRECTION:
		{
			if( m_list_UniqObj.GetCurSel() >= 0 )
			{
				m_DlgResurrection.ShowWindow( SW_HIDE );
				m_edit_AddMapID.ShowWindow(SW_SHOW);
				m_btn_AddMapID.ShowWindow(SW_SHOW);
				m_edit_AddToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID.ShowWindow(SW_SHOW);
				m_Static_ToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID_Group.ShowWindow(SW_SHOW);
				m_ResurrectionGroup.DeleteResurrection( (CsMapResurrection*)m_list_UniqObj.GetItemData( m_list_UniqObj.GetCurSel() ) );
				_ResetUniqObjList_ForResurrection();
			}
		}
		break;
	case UT_LIMIT_EVO:
		{
			if( m_list_UniqObj.GetCurSel() >= 0 )
			{
				m_DlgLimitEvo.ShowWindow( SW_HIDE );
				m_edit_AddMapID.ShowWindow(SW_SHOW);
				m_btn_AddMapID.ShowWindow(SW_SHOW);
				m_edit_AddToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID.ShowWindow(SW_SHOW);
				m_Static_ToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID_Group.ShowWindow(SW_SHOW);
				m_LimitEvoGroup.DeleteRegion( (CsMap_LimitEvo*)m_list_UniqObj.GetItemData( m_list_UniqObj.GetCurSel() ) );
				_ResetUniqObjList_ForLimitEvo();
			}
		}
		break;

	case UT_BUFF_REGION:
		{
			if( m_list_UniqObj.GetCurSel() >= 0 )
			{
				m_DlgBuffRegion.ShowWindow( SW_HIDE );
				m_edit_AddMapID.ShowWindow(SW_SHOW);
				m_btn_AddMapID.ShowWindow(SW_SHOW);
				m_edit_AddToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID.ShowWindow(SW_SHOW);
				m_Static_ToolMapName.ShowWindow(SW_SHOW);
				m_Static_AddMapID_Group.ShowWindow(SW_SHOW);
				m_BuffRegion.DeleteRegion( (CsMap_BuffRegion*)m_list_UniqObj.GetItemData( m_list_UniqObj.GetCurSel() ) );
				_ResetUniqObjList_ForBuffRegion();
			}

		}
		break;
	default:
		assert_cs( false );
	}
}



// =================================================================================================
//
//		Uniq Object		Start
//
// =================================================================================================
void _DlgMTMap::_ResetUniqObjList_ForStart()
{
	assert_cs( m_pCurList != NULL );
	m_list_UniqObj.ResetContent();

	// 리스트에 해당 항복 받는다
	TCHAR szMsg[ MAX_PATH ];

	int nSelStartIndex = -1;
	CsMapStart::LIST* plistStart = m_StartGroup.GetStartList();
	CsMapStart::LIST_IT it = plistStart->begin();
	for( ; it!=plistStart->end(); ++it )
	{
		_stprintf_s( szMsg, MAX_PATH, L"Start Pos, %d x %d", (*it)->GetInfo()->s_nCenterX, (*it)->GetInfo()->s_nCenterY );
		int nSel = m_list_UniqObj.GetCount();
		m_list_UniqObj.InsertString( nSel, szMsg );
		m_list_UniqObj.SetItemData( nSel, (DWORD_PTR)(*it) );

		if( (*it) == m_DlgStart._GetStart() )
		{
			nSelStartIndex = nSel;
		}
	}

	if( nSelStartIndex != -1 )
		m_list_UniqObj.SetCurSel( nSelStartIndex );
}

void _DlgMTMap::_ResetUniqObjList_ForRegion()
{
	assert_cs( m_pCurList != NULL );
	m_list_UniqObj.ResetContent();

	// 리스트에 해당 항복 받는다
	TCHAR szMsg[ MAX_PATH ];

	int nSelStartIndex = -1;
	CsMapRegion::LIST* plist = m_RegionGroup.GetRegionList();
	CsMapRegion::LIST_IT it = plist->begin();
	for( ; it!=plist->end(); ++it )
	{
		_stprintf_s( szMsg, MAX_PATH, L"%s, %d x %d", (*it)->GetInfo()->s_szDiscript, (*it)->GetInfo()->s_nCenterX, (*it)->GetInfo()->s_nCenterY );
		int nSel = m_list_UniqObj.GetCount();
		m_list_UniqObj.InsertString( nSel, szMsg );
		m_list_UniqObj.SetItemData( nSel, (DWORD_PTR)(*it) );

		if( (*it) == m_DlgRegion._GetRegion() )
		{
			nSelStartIndex = nSel;
		}
	}

	if( nSelStartIndex != -1 )
		m_list_UniqObj.SetCurSel( nSelStartIndex );
}

void _DlgMTMap::_ResetUniqObjList_ForBuffRegion()
{
	assert_cs( m_pCurList != NULL );
	m_list_UniqObj.ResetContent();

	// 리스트에 해당 항복 받는다
	TCHAR szMsg[ MAX_PATH ];

	int nSelStartIndex = -1;
	CsMap_BuffRegion::LIST* plist = m_BuffRegion.GetList();
	CsMap_BuffRegion::LIST_IT it = plist->begin();
	for( ; it!=plist->end(); ++it )
	{
		_stprintf_s( szMsg, MAX_PATH, L"%d x %d", (*it)->GetInfo()->centerx, (*it)->GetInfo()->centery);
		int nSel = m_list_UniqObj.GetCount();
		m_list_UniqObj.InsertString( nSel, szMsg );
		m_list_UniqObj.SetItemData( nSel, (DWORD_PTR)(*it) );

		if( (*it) == m_DlgBuffRegion.GetBuffRegion() )
		{
			nSelStartIndex = nSel;
		}
	}

	if( nSelStartIndex != -1 )
		m_list_UniqObj.SetCurSel( nSelStartIndex );
}

void _DlgMTMap::_ResetUniqObjList_ForLimitEvo()
{
	assert_cs( m_pCurList != NULL );
	m_list_UniqObj.ResetContent();

	// 리스트에 해당 항복 받는다
	TCHAR szMsg[ MAX_PATH ];

	int nSelStartIndex = -1;
	CsMap_LimitEvo::LIST* plist = m_LimitEvoGroup.GetList();
	CsMap_LimitEvo::LIST_IT it = plist->begin();
	for( ; it!=plist->end(); ++it )
	{
		_stprintf_s( szMsg, MAX_PATH, L"%d x %d", (*it)->GetInfo()->s_nCenterX, (*it)->GetInfo()->s_nCenterY );
		int nSel = m_list_UniqObj.GetCount();
		m_list_UniqObj.InsertString( nSel, szMsg );
		m_list_UniqObj.SetItemData( nSel, (DWORD_PTR)(*it) );

		if( (*it) == m_DlgLimitEvo._GetLimitEvo() )
		{
			nSelStartIndex = nSel;
		}
	}

	if( nSelStartIndex != -1 )
		m_list_UniqObj.SetCurSel( nSelStartIndex );
}



// =================================================================================================
//
//		Uniq Object		Resurrection
//
// =================================================================================================
void _DlgMTMap::_ResetUniqObjList_ForResurrection()
{
	assert_cs( m_pCurList != NULL );
	m_list_UniqObj.ResetContent();

	// 리스트에 해당 항목 받는다
	TCHAR szMsg[ MAX_PATH ];

	int nSelIndex = -1;
	CsMapResurrection::LIST* plist = m_ResurrectionGroup.GetResurrectionList();
	CsMapResurrection::LIST_IT it = plist->begin();
	for( ; it!=plist->end(); ++it )
	{
		_stprintf_s( szMsg, MAX_PATH, L"Rsrt Pos, %d x %d", (*it)->GetInfo()->s_nCenterX, (*it)->GetInfo()->s_nCenterY );
		int nSel = m_list_UniqObj.GetCount();
		m_list_UniqObj.InsertString( nSel, szMsg );
		m_list_UniqObj.SetItemData( nSel, (DWORD_PTR)(*it) );

		if( (*it) == m_DlgResurrection._GetResurrection() )
		{
			nSelIndex = nSel;
		}
	}

	if( nSelIndex != -1 )
		m_list_UniqObj.SetCurSel( nSelIndex );
}

// =================================================================================================
//
//		Input
//
// =================================================================================================
void _DlgMTMap::_OnLButtonDown( CPoint pt )
{
	switch( m_combo_UniqObj.GetCurSel() )
	{
	case UT_NONE:
		break;
	case UT_START:
		{
			if( m_DlgStart._GetStart() != NULL )
			{
				if( m_DlgStart._IsLButtonDown( pt ) == true )
					_ResetUniqObjList_ForStart();
			}
		}
		break;
	case UT_REGION:
		{
			if( m_DlgRegion._GetRegion() != NULL )
			{
				if( m_DlgRegion._IsLButtonDown( pt ) == true )
					_ResetUniqObjList_ForRegion();
			}
		}
		break;
	case UT_RESURRECTION:
		{
			if( m_DlgResurrection._GetResurrection() != NULL )
			{
				if( m_DlgResurrection._IsLButtonDown( pt ) == true )
					_ResetUniqObjList_ForResurrection();
			}
		}
		break;
	case UT_LIMIT_EVO:
		{
			if( m_DlgLimitEvo._GetLimitEvo() != NULL )
			{
				if( m_DlgLimitEvo._IsLButtonDown( pt ) == true )
					_ResetUniqObjList_ForLimitEvo();
			}
		}
		break;
	case UT_BUFF_REGION:
		{
			if( m_DlgBuffRegion.GetBuffRegion() != NULL )
			{
				if( m_DlgBuffRegion._IsLButtonDown( pt ) == true )
					_ResetUniqObjList_ForBuffRegion();
			}
		}
		break;
	default:
		assert_cs( false );
	}	
}

void _DlgMTMap::OnEnChangeEditFatigueType()
{
	if( m_pCurList == NULL )
		return;

	m_pCurList->GetInfo()->s_nFatigue_Type = nsCsMfcFunc::Control2Int( &m_edit_FG_Type );	
}

void _DlgMTMap::OnEnChangeEditFatigueDebuff()
{
	if( m_pCurList == NULL )
		return;

	m_pCurList->GetInfo()->s_nFatigue_DeBuff = nsCsMfcFunc::Control2Int( &m_edit_FG_DBuff );
}

void _DlgMTMap::OnEnChangeEditFatigueStime()
{
	if( m_pCurList == NULL )
		return;

	m_pCurList->GetInfo()->s_nFatigue_StartTime = nsCsMfcFunc::Control2Int( &m_edit_FG_STime );	
}

void _DlgMTMap::OnEnChangeEditFatigueAtime()
{
	if( m_pCurList == NULL )
		return;

	m_pCurList->GetInfo()->s_nFatigue_AddTime = nsCsMfcFunc::Control2Int( &m_edit_FG_ATime );	
}

void _DlgMTMap::OnEnChangeEditFatiguePoint()
{
	if( m_pCurList == NULL )
		return;

	m_pCurList->GetInfo()->s_nFatigue_AddPoint = nsCsMfcFunc::Control2Int( &m_edit_FG_Point );	
}

void _DlgMTMap::OnEnChangeEditCameraLevel()
{
	if( m_pCurList == NULL )
		return;

	m_pCurList->GetInfo()->s_nCamera_Max_Level = nsCsMfcFunc::Control2Int( &m_edit_Camera_Max_Level );	
}

void _DlgMTMap::OnBnClickedCheckXgconsume()
{
	if( m_pCurList == NULL )
		return;

	m_pCurList->GetInfo()->s_bXgConsumeType = !m_check_Xgconsume.GetCheck();
}

void _DlgMTMap::OnBnClickedCheckBattletagUse()
{
	if( m_pCurList == NULL )
		return;

	m_pCurList->GetInfo()->s_bBattleTagUse = !m_check_BattletagUse.GetCheck();
}
