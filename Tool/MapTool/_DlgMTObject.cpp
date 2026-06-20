// _DlgMTObject.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTObject.h"
#include "direct.h"
#include "ToolNiNode/UndoManager.h"
#include "ToolNiNode/GizmoManager.h"
bool g_bCheckToggle = false;

// _DlgMTObject 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTObject, CDialog)

_DlgMTObject::_DlgMTObject(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTObject::IDD, pParent)
	,m_bRenderMonsterRegion(false)
	,m_fUseNoMove(false)
{
	
}

_DlgMTObject::~_DlgMTObject()
{
}


void _DlgMTObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_OBJECT, m_tree_Object);
	DDX_Control(pDX, IDC_CHECK_RENDEROBJECT, m_check_RenderObject);
	DDX_Control(pDX, IDC_EDIT_OBJECT_PATH, m_edit_ObjectPath);
	DDX_Control(pDX, IDC_EDIT_OBJECT_NAME, m_edit_ObjectName);
	DDX_Control(pDX, IDC_LIST_USERDEFINE, m_list_UserDefine);
	DDX_Control(pDX, IDC_COMBO_OBJECT_TYPE, m_combo_ObjType);
	DDX_Control(pDX, IDC_CHECK_USE_SNAP, m_chk_UseSnap);
	DDX_Control(pDX, IDC_EDIT_SNAP_SIZE, m_edit_SnapSize);
	DDX_Control(pDX, IDC_EDIT_SELECT_COUNT, m_edit_SelCount);
	DDX_Control(pDX, IDC_EDIT_SCALE, m_edit_Scale);
	DDX_Control(pDX, IDC_SPIN_SCALE, m_spin_Scale);
	DDX_Control(pDX, IDC_EDIT_OBJID, m_edit_ObjectID);
	DDX_Control(pDX, IDC_CHECK_USE_TOGGLE, m_Chk_Use_Toggle);
	DDX_Control(pDX, IDC_CHECK_SHOW_COLLISIONBOX, m_Chk_Show_CollisionBox);
	DDX_Control(pDX, IDC_CHECK_COPY_NOMOVE, m_Chk_Copy_NoMove);
	DDX_Control(pDX, IDC_SLIDER_GIZMO_SENCE, m_GizmoSensor);
	DDX_Control(pDX, IDC_SLIDER_SELECT_OPACITY, m_SelectedOpacity);
	DDX_Control(pDX, IDC_BASIC_COORD, m_CoordBox);
	DDX_Control(pDX, IDC_CHECK_RENDER_MONSTERREGION, m_check_RenderMonsterRegion);
	DDX_Control(pDX, IDC_CHECK_SHOW_Occluder, m_Chk_Show_Occluder);
}


BEGIN_MESSAGE_MAP(_DlgMTObject, CDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_OBJECT, &_DlgMTObject::OnBnClickedBtnAddObject)
	ON_BN_CLICKED(IDC_BTN_DELETE_ALL, &_DlgMTObject::OnBnClickedBtnDeleteAll)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OBJECT, &_DlgMTObject::OnTvnSelchangedTreeObject)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_OBJECT, &_DlgMTObject::OnTvnKeyDonwTreeObject)	
	ON_BN_CLICKED(IDC_CHECK_RENDEROBJECT, &_DlgMTObject::OnBnClickedCheckRenderobject)
	ON_WM_SHOWWINDOW()	
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT_TYPE, &_DlgMTObject::OnCbnSelchangeComboObjectType)
	ON_EN_SETFOCUS(IDC_EDIT_SCALE, &_DlgMTObject::OnEnSetfocusEditScale)
	ON_EN_CHANGE(IDC_EDIT_SCALE, &_DlgMTObject::OnEnChangeEditScale)
	ON_BN_CLICKED(IDC_CHECK_USE_TOGGLE, &_DlgMTObject::OnBnClickedCheckUseToggle)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_KEY, &_DlgConfig::OnBnClickedButtonDefaultKey)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_GIZMO_SENCE, &_DlgMTObject::OnNMReleasedcaptureSliderGizmoSence)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SELECT_OPACITY, &_DlgMTObject::OnNMReleasedcaptureSliderSelectOpacity)
	ON_CBN_SELCHANGE(IDC_BASIC_COORD, &_DlgMTObject::OnCbnSelchangeBasicCoord)
	ON_BN_CLICKED(IDC_CHECK_RENDER_MONSTERREGION, &_DlgMTObject::OnBnClickedCheckRenderMonsterregion)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_GIZMO_SENCE, &_DlgMTObject::OnNMCustomdrawSliderGizmoSence)
	ON_BN_CLICKED(IDC_CHECK_SHOW_COLLISIONBOX, &_DlgMTObject::OnBnClickedCheckShowCollisionbox)
	ON_BN_CLICKED(IDC_CHECK_COPY_NOMOVE, &_DlgMTObject::OnBnClickedCheckCopyNomove)
	ON_BN_CLICKED(IDC_CHECK_SHOW_Occluder, &_DlgMTObject::OnBnClickedCheckShowOccluder)
END_MESSAGE_MAP()

// =================================================================================================
//
//		virtual
//
// =================================================================================================
void _DlgMTObject::_PreSave( const char* cFile, bool bAutoSave )
{
	m_DlgPortal._PreSave( cFile, bAutoSave );
	m_DlgTexType._PreSave( cFile, bAutoSave );
	m_DlgMerge._PreSave( cFile, bAutoSave );
}

void _DlgMTObject::_PostViewReset()
{
	nsCsMfcFunc::Int2Control( &m_edit_SnapSize, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nGridSize );

}
void _DlgMTObject::_PostRenderView()
{
	if(m_bRenderMonsterRegion)
		GET_MV_MAINTAB_DLG->_GetDlgMonster()->__RenderResist(false);
}

void _DlgMTObject::_UpdateView()
{
	m_DlgMerge._UpdateView();
	m_DlgPortal._UpdateView();
	m_DlgAni._UpdateView();
}


// =================================================================================================
//
//		Base
//
// =================================================================================================

BOOL _DlgMTObject::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	CRect rc;
	GetWindowRect( &rc );
	m_DlgPortal.Create( _DlgMTObj_Portal::IDD, this );
	m_DlgPortal.SetWindowPos( NULL, 0, 325, rc.Width(), 600 - ( rc.Height() - 325 ), 0 );
	m_DlgPortal.ShowWindow( SW_HIDE );

	m_DlgTexType.Create( _DlgMTObj_TexType::IDD, this );
	m_DlgTexType.SetWindowPos( NULL, 0, 325, rc.Width(), 600 - ( rc.Height() - 325 ), 0 );
	m_DlgTexType.ShowWindow( SW_HIDE );

	m_DlgMerge.Create( _DlgMTObj_Merge::IDD, this );
	m_DlgMerge.SetWindowPos( NULL, 0, 325, rc.Width(), 600 - ( rc.Height() - 325 ), 0 );
	m_DlgMerge.ShowWindow( SW_HIDE );

	m_DlgAni.Create( _DlgMTObj_Ani::IDD, this );
	m_DlgAni.SetWindowPos( NULL, 0, 325, rc.Width(), 200, 0 );
	m_DlgAni.ShowWindow( SW_HIDE );	

	m_spin_Scale.SetCsBuddy( &m_edit_Scale, 0.005f, 3 );
	m_spin_Scale.EnableWindow( FALSE );
	m_edit_Scale.EnableWindow( FALSE );

	g_ToolResource.LoadObject();

	m_GizmoSensor.SetPos(50);
	g_ToolNiNodeMng.OnChangeGizmoEditSense(50);

	float DefaultOpacity = 50.0f;
	m_SelectedOpacity.SetPos(DefaultOpacity);
	g_ObjectMng.UpdateOpacity(DefaultOpacity / 100.0f);

	m_CoordBox.AddString(L"World");
	m_CoordBox.AddString(L"Model");
	m_CoordBox.AddString(L"Cam");
	m_CoordBox.SetCurSel(0);

	m_check_RenderMonsterRegion.SetCheck(0);
	m_bRenderMonsterRegion = false;
	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgMTObject::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( bShow == FALSE )
	{
		GET_MV_OBJECTLIST_DLG->_UnselectList();
		GET_MV_OBJECTLIST_FRM->ShowWindow( SW_HIDE );

		m_DlgPortal.ShowWindow( SW_HIDE );
		m_DlgTexType.ShowWindow( SW_HIDE );
		m_DlgMerge.ShowWindow( SW_HIDE );
		m_DlgAni.ShowWindow( SW_HIDE );
		m_combo_ObjType.ResetContent();

		g_ObjectMng.DeleteModel();
		g_ToolNiNodeMng.OnEndObjectEdit();
	}
	else
	{
		GET_MV_OBJECTLIST_FRM->ShowWindow( SW_SHOW );

		//m_check_RenderObject.SetCheck( nsCsGBTerrain::g_pCurRoot->GetRenderObject_ByTool() ? BST_CHECKED : BST_UNCHECKED );
		OnBnClickedCheckRenderobject();
		g_ToolNiNodeMng.OnStartObjectEdit();
	}
	UNDO::Manager::GetSingletonPtr()->Reset();
	if( IS_ENABLEFRM( nsMAPTOOL::MV_VIEWER ) == true )
		GET_MV_VIEW->SetFocus();
}

void _DlgMTObject::OnEnSetfocusEditScale(){	m_edit_Scale.SetSel( 0, -1, TRUE );		}
void _DlgMTObject::OnEnChangeEditScale()
{
	if( _IsFocus() == true )
	{
		if( g_ObjectMng.GetListSelectModel()->size() == 1 )
		{
			cObjectModel* pObject = *( g_ObjectMng.GetListSelectModel()->begin() );
			NiTransform tr = pObject->GetObjectInfo()->s_trObject;
			tr.m_fScale = nsCsMfcFunc::Control2Float( &m_edit_Scale );
			pObject->SetTransform( tr );
		}
		else
		{	//오브젝트 그룹으로 스케일
			if(g_ObjectMng.GetListSelectModel()->empty() == false)
			{
				cObjectModel* pObject = *( g_ObjectMng.GetListSelectModel()->begin() );
				NiTransform tr = pObject->GetObjectInfo()->s_trObject;
				tr.m_fScale = nsCsMfcFunc::Control2Float( &m_edit_Scale );

				LIST_OBJMODEL_IT it = g_ObjectMng.GetListSelectModel()->begin();
				LIST_OBJMODEL_IT itEnd = g_ObjectMng.GetListSelectModel()->end();

				for( ; it != itEnd; ++it )
				{
					(*it)->GetNiNode()->SetScale(tr.m_fScale);
				}
			}
		}
	}
}

bool _DlgMTObject::_IsFocus()
{
	CWnd* pWnd = GetFocus();
	
	if( pWnd->GetSafeHwnd() == m_edit_Scale.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_Scale ) < 0.005f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_Scale, 0.005f, 3 );
		}
		return true;
	}
	return false;
}

void _DlgMTObject::_Reset()
{
	//m_check_RenderObject.SetCheck( BST_CHECKED );
	//if( nsCsGBTerrain::g_pCurRoot != NULL )
	//	nsCsGBTerrain::g_pCurRoot->SetRenderObject_ByTool( true );
}

void _DlgMTObject::_Resist()
{
	_Reset();

	if( nsCsGBTerrain::g_pCurRoot == NULL )
		return;

	TCHAR szFullName[ MAX_PATH ];
	TCHAR szPath[ MAX_PATH ];
	LPCTSTR szFile;

	CsGBTerrainRoot* pRoot = nsCsGBTerrain::g_pCurRoot;

	{
		MAP_GBTERRAIN_OBJECT* pMap = pRoot->GetMap_No_Pick();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			M2W( szFullName, it->second->GetInfo()->s_cObjectPath, OBJECT_PATH_LEN );
			nsCSFILE::GetFilePathOnly( szPath, MAX_PATH, szFullName, true );
			szFile = nsCSFILE::GetFileName( szFullName );
			__InsertTree( szPath, szFile );
		}
	}

	MAP_GBTERRAIN_OBJECT* pObjectMap;
	MAP_GBTERRAIN_OBJECT_IT itObject;
	for( int i=0; i<pRoot->GetInfo()->s_nTotalLeafCount; ++i )
	{
		pObjectMap = pRoot->GetNode( i )->GetObjectMap();
		itObject = pObjectMap->begin();
		for( ; itObject != pObjectMap->end(); ++itObject )
		{
			M2W( szFullName, itObject->second->GetInfo()->s_cObjectPath, OBJECT_PATH_LEN );
			nsCSFILE::GetFilePathOnly( szPath, MAX_PATH, szFullName, true );
			szFile = nsCSFILE::GetFileName( szFullName );
			__InsertTree( szPath, szFile );
		}
	}
	g_ToolResource.SaveObject();
}

// =================================================================================================
//
//		Uniq Object
//
// =================================================================================================
void _DlgMTObject::_SetUniqObject( CsGBObject* pCsGBObject )
{
	if( g_TempRoot.IsLoad() == true )
	{
		m_pUniqCsGBObject = NULL;
		return;
	}
	
	// 셀렉트 오브젝트 갯수 체크
	nsCsMfcFunc::Int2Control( &m_edit_SelCount, (int)g_ObjectMng.GetListSelectModel()->size() );

	m_pUniqCsGBObject = pCsGBObject;

	__SetUniqObject_Path();
	__SetUniqObject_UserDefine();
	__SetUniqObject_ObjectType();
	__SetUniqObject_Scale();
	__SetUniqObject_ObjectID();
}

void _DlgMTObject::__SetUniqObject_Path()
{
	m_edit_ObjectPath.SetWindowText( L"" );
	m_edit_ObjectName.SetWindowText( L"" );

	if( m_pUniqCsGBObject == NULL )
		return;

	TCHAR szPath[ MAX_PATH ];
	M2W( szPath, m_pUniqCsGBObject->GetInfo()->s_cObjectPath, OBJECT_PATH_LEN );
	m_edit_ObjectPath.SetWindowText( szPath );

	m_edit_ObjectName.SetWindowText( nsCSFILE::GetFileName( szPath ) );
}


void _DlgMTObject::__SetUniqObject_UserDefine()
{
	m_list_UserDefine.ResetContent();

	if( m_pUniqCsGBObject == NULL )
		return;
		
	// 플래그별로 전부 추가
	if( m_pUniqCsGBObject->UD_IsNoShadow() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"그림자 없음(ns)" );

	if( m_pUniqCsGBObject->UD_IsAnimation() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"애니메이션(ani)" );

	if( m_pUniqCsGBObject->UD_IsUseTexType() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"텍스쳐 타입(tt)" );

	if( m_pUniqCsGBObject->UD_IsCameraAlpha() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"카메라 알파(ca)" );

	if( m_pUniqCsGBObject->UD_IsCameraAlphaPick() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"카메라 알파 픽킹(cap)" );

	/*if( m_pUniqCsGBObject->UD_IsMove() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"이동" );*/

	/*if( m_pUniqCsGBObject->UD_IsTerrainBase() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"지형 기본" );*/

	if( m_pUniqCsGBObject->UD_IsTerrainBasePick() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"지형 기본 픽킹(tbp)" );

	if( m_pUniqCsGBObject->UD_IsTableObj() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"테이블 오브젝트(table)" );

	if( m_pUniqCsGBObject->UD_IsCharShadow() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"동적 그림자 적용(shadow)" );

	if( m_pUniqCsGBObject->UD_IsNoFog() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"포그 없음(nf)" );

	if( m_pUniqCsGBObject->UD_IsNoLight() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"라이트 없음(nl)" );

	if( m_pUniqCsGBObject->UD_IsPortal() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"포탈(pt)" );

	if( m_pUniqCsGBObject->UD_IsRotation() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"회전(ro)" );

	if( m_pUniqCsGBObject->UD_IsRotationLimit() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"회전 제한(rol)" );

	if( m_pUniqCsGBObject->UD_IsCameraCollision() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"카메라 충돌(cc)" );

	if( m_pUniqCsGBObject->UD_IsMerge() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"오브젝트 합체(mg)" );

	if( m_pUniqCsGBObject->UD_IsDarkMap() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"다크맵 생성(dm)" );

	if( m_pUniqCsGBObject->UD_IsTerrainHidePick() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"지형 숨은메쉬 픽킹(thp)" );

	if( m_pUniqCsGBObject->UD_IsEffectRender() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"이펙트 랜더링(ef)" );

	if( m_pUniqCsGBObject->UD_IsNoClipping() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"클리핑 안함(nc)" );

	if( m_pUniqCsGBObject->UD_IsNoPick() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"픽킹 안되게막음(nopick)" );

	if( m_pUniqCsGBObject->UD_IsAlwaysAnimation() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"항상 애니메이션(awani)" );

	if( m_pUniqCsGBObject->UD_IsAnimationRandom() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"랜덤 프레임 애니(anir)" );

	if( m_pUniqCsGBObject->UD_IsSound() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"사운드(sd)" );

	if( m_pUniqCsGBObject->UD_IsSkin() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"스킨(skin)" );

	if( m_pUniqCsGBObject->UD_IsF1() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"우선순위1 랜더링(f1)" );

	if( m_pUniqCsGBObject->UD_IsQuestOn() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"퀘스트 보이기(queston)" );

	if( m_pUniqCsGBObject->UD_IsQuestOff() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"퀘스트 숨기기(questoff)" );

	if( m_pUniqCsGBObject->UD_IsActorManager() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"KFM File(actor)" );

	if( m_pUniqCsGBObject->UD_IsAct() == true )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"액션 오브젝트(act)" );

	if( m_list_UserDefine.GetCount() == 0 )
		m_list_UserDefine.InsertString( m_list_UserDefine.GetCount(), L"   x   " );
}


void _DlgMTObject::__SetUniqObject_ObjectType()
{
	int nComboCount = m_combo_ObjType.GetCount();
	for( int i=0; i<nComboCount; ++i )
	{
		CDialog* pDialog = (CDialog*)m_combo_ObjType.GetItemData( i );
		pDialog->ShowWindow( SW_HIDE );
	}
	m_combo_ObjType.ResetContent();

	if( m_pUniqCsGBObject == NULL )
		return;

	// 플래그별로 전부 추가
	if( m_pUniqCsGBObject->UD_IsPortal() == true )
	{
		int nComboIndex = m_combo_ObjType.GetCount();
		m_combo_ObjType.InsertString( nComboIndex, L"포탈" );
		m_combo_ObjType.SetItemData( nComboIndex, (DWORD_PTR)&m_DlgPortal );
	}
	if( m_pUniqCsGBObject->UD_IsUseTexType() == true )
	{
		int nComboIndex = m_combo_ObjType.GetCount();
		m_combo_ObjType.InsertString( nComboIndex, L"텍스쳐 타입" );
		m_combo_ObjType.SetItemData( nComboIndex, (DWORD_PTR)&m_DlgTexType );
	}
	if( m_pUniqCsGBObject->UD_IsMerge() == true )
	{
		int nComboIndex = m_combo_ObjType.GetCount();
		m_combo_ObjType.InsertString( nComboIndex, L"오브젝트 합체" );
		m_combo_ObjType.SetItemData( nComboIndex, (DWORD_PTR)&m_DlgMerge );		
	}
	if( ( m_pUniqCsGBObject->UDF_IsAnimation() == true )&&( m_pUniqCsGBObject->UD_IsActorManager() == false ) )
	{
		int nComboIndex = m_combo_ObjType.GetCount();
		m_combo_ObjType.InsertString( nComboIndex, L"애니메이션" );
		m_combo_ObjType.SetItemData( nComboIndex, (DWORD_PTR)&m_DlgAni );
	}

	if( m_combo_ObjType.GetCount() != 0 )
	{
		m_combo_ObjType.SetCurSel( 0 );
		OnCbnSelchangeComboObjectType();
	}	
}

void _DlgMTObject::__SetUniqObject_Scale()
{
	if( m_pUniqCsGBObject == NULL )
	{
		//m_edit_Scale.SetWindowText( NULL );
		m_spin_Scale.EnableWindow( TRUE );
		m_edit_Scale.EnableWindow( TRUE );
	}
	else
	{
		m_spin_Scale.EnableWindow( TRUE );
		m_edit_Scale.EnableWindow( TRUE );
		nsCsMfcFunc::Float2Control( &m_edit_Scale, m_pUniqCsGBObject->GetInfo()->s_trObject.m_fScale, 3 );
	}
}

void _DlgMTObject::__SetUniqObject_ObjectID()
{
	m_edit_ObjectID.SetWindowText( L"" );

	if( m_pUniqCsGBObject == NULL )
		return;

	nsCsMfcFunc::Int2Control( &m_edit_ObjectID, m_pUniqCsGBObject->GetInfo()->s_dwUniqID );
}

// 오브젝트 타입 콤보 박스 변경
void _DlgMTObject::OnCbnSelchangeComboObjectType()
{
	int nComboCount = m_combo_ObjType.GetCount();
	for( int i=0; i<nComboCount; ++i )
	{
		CDialog* pDialog = (CDialog*)m_combo_ObjType.GetItemData( i );
		pDialog->ShowWindow( SW_HIDE );
	}

	CDialog* pDialog = (CDialog*)m_combo_ObjType.GetItemData( m_combo_ObjType.GetCurSel() );
	pDialog->ShowWindow( SW_SHOW );
}


// =================================================================================================
//
//		Object Tree View
//
// =================================================================================================

// 오브젝트 추가 버튼 클릭
void _DlgMTObject::OnBnClickedBtnAddObject()
{
	if( g_TempRoot.IsLoad() == true )
		return;

#define MAP_OBJECT_PATH		L"Data\\Map"

	// 폴더가 존재 하지 않는다면 폴더 생성	
	nsCSFILE::CreateDirectory( MAP_OBJECT_PATH );

	CFileDialog fd( TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER, L"Nif Files (*.Nif)|*.Nif||" );
	//OFN_PATHMUSTEXIST = 오직 유효한 경로나 파일명만을 입력( 아님 경고메시지 출력)
	//OFN_NOCHANGEDIR = 디렉토리 변경 허용 x
	//OFN_FILEMUSTEXIST = 기존에 존재하는 파일 이름만 입력할 수 있도록 함
	//OFN_ALLOWMULTISELECT = 여러개 파일 오픈하기
	//OFN_EXPLORER = 다중 파일 선택
	
	// 접근이 되지 않는다면 기본 폴더로
	if( _taccess_s( g_Resist.GetObject()->s_szObjectPath, 0 ) != 0 )
		g_Resist.GetObject()->Reset();
	fd.m_ofn.lpstrInitialDir = g_Resist.GetObject()->s_szObjectPath;

	if( fd.DoModal() == IDCANCEL )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( fd.GetPathName() ) == false )
	{
		MessageBox( L"오브젝트는 현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	TCHAR szRootPath[ MAX_PATH ] = {0, };
	TCHAR szRootFullPath[ MAX_PATH ] = {0, };
	nsCSFILE::GetRelativePath( szRootFullPath, MAX_PATH, fd.GetPathName() );
	nsCSFILE::GetFilePathOnly( szRootPath, MAX_PATH, szRootFullPath );

	for( POSITION pos = fd.GetStartPosition(); pos != NULL; )
	{
		CString strObject = fd.GetNextPathName( pos );
		LPCTSTR szFile = nsCSFILE::GetFileName( strObject );

		__InsertTree( szRootPath, szFile, true );
	}

	_tcscpy_s( g_Resist.GetObject()->s_szObjectPath, MAX_PATH, szRootPath );
	g_ToolResource.SaveObject();
}

//오브젝트 경로 변경이나 없을 때 다른걸로 대체하는 함수
void _DlgMTObject::OnBnClickedBtnAddObject( TCHAR* Name )
{
	if( g_TempRoot.IsLoad() == true )
		return;

#define MAP_OBJECT_PATH		L"Data\\Map"

	// 폴더가 존재 하지 않는다면 폴더 생성	
	nsCSFILE::CreateDirectory( MAP_OBJECT_PATH );

	CFileDialog fd( TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER, L"Nif Files (*.Nif)|*.Nif||" );
	//OFN_PATHMUSTEXIST = 오직 유효한 경로나 파일명만을 입력( 아님 경고메시지 출력)
	//OFN_NOCHANGEDIR = 디렉토리 변경 허용 x
	//OFN_FILEMUSTEXIST = 기존에 존재하는 파일 이름만 입력할 수 있도록 함
	//OFN_ALLOWMULTISELECT = 여러개 파일 오픈하기
	//OFN_EXPLORER = 다중 파일 선택

	// 접근이 되지 않는다면 기본 폴더로
	if( _taccess_s( g_Resist.GetObject()->s_szObjectPath, 0 ) != 0 )
		g_Resist.GetObject()->Reset();
	fd.m_ofn.lpstrInitialDir = g_Resist.GetObject()->s_szObjectPath;

	if( fd.DoModal() == IDCANCEL )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( fd.GetPathName() ) == false )
	{
		MessageBox( L"오브젝트는 현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	TCHAR szRootPath[ MAX_PATH ] = {0, };
	TCHAR szRootFullPath[ MAX_PATH ] = {0, };
	nsCSFILE::GetRelativePath( szRootFullPath, MAX_PATH, fd.GetPathName() );
	nsCSFILE::GetFilePathOnly( szRootPath, MAX_PATH, szRootFullPath );

	//Name = szRootFullPath;
	wcscpy(Name, szRootFullPath);
	wcscpy(m_RouteChangePath, szRootFullPath);

	for( POSITION pos = fd.GetStartPosition(); pos != NULL; )
	{
		CString strObject = fd.GetNextPathName( pos );
		LPCTSTR szFile = nsCSFILE::GetFileName( strObject );

		__InsertTree( szRootPath, szFile );
	}

	_tcscpy_s( g_Resist.GetObject()->s_szObjectPath, MAX_PATH, szRootPath );
	g_ToolResource.SaveObject();

}

void _DlgMTObject::OnObjectRouteChange(TCHAR* Name)
{
	wcscpy(Name, m_RouteChangePath);
}

// 오브젝트 제거 버튼 클릭
void _DlgMTObject::OnBnClickedBtnDeleteAll()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	if( m_tree_Object.GetCount() == NULL )
		return;

	if( MessageBox( L"오브젝트 창을 초기화 합니다. 계속 하시겠습니까?", NULL, MB_OKCANCEL ) == IDCANCEL )
		return;

	__DeleteTree();
}

// 트리에서 선택 변경
void _DlgMTObject::OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( g_TempRoot.IsLoad() == true )
		return;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if( nsCsGBTerrain::g_pCurRoot != NULL )
	{
		if(nsCsGBTerrain::g_pCurRoot->GetRenderObject_ByTool() == false)
			return;
	}
	//if( m_check_RenderObject.GetCheck() == BST_UNCHECKED )
	//	return;

	HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
	if( hSelItem == NULL )
	{
		g_Preview.GetTObj()->ReleaseModelView();
		return;
	}

	g_ObjectMng.DeleteModel();
	if( m_tree_Object.GetItemData( hSelItem ) == TT_ROOT )
	{
		g_Preview.GetTObj()->ReleaseModelView();
		return;
	}
	
	assert_cs( m_tree_Object.GetItemData( hSelItem ) == TT_NODE );
	
	HTREEITEM hParent = m_tree_Object.GetParentItem( hSelItem );
	CString strParent = L"Data\\" + m_tree_Object.GetItemText( hParent );
	CString strSel = m_tree_Object.GetItemText( hSelItem );
	g_ObjectMng.InsertModel( strParent.GetBuffer(), strSel.GetBuffer() );
	g_Preview.GetTObj()->SetModelView( strParent.GetBuffer(), strSel.GetBuffer() );

	*pResult = 0;
}

void _DlgMTObject::OnTvnKeyDonwTreeObject( NMHDR* pNMHDR, LRESULT* pResult )
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

	if (pTVKeyDown->wVKey == VK_DELETE )
	{
		__OneDelete();
		// add code here
		//MessageBox(L"SCORE!");
	}

	*pResult = 0;
}

void _DlgMTObject::__DeleteTree()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	g_ObjectMng.DeleteModel();
	m_tree_Object.DeleteAllItems();
	g_ToolResource.SaveObject();
}

void _DlgMTObject::__OneDelete()
{
	HTREEITEM hTreeItem = m_tree_Object.GetSelectedItem();
	
	m_tree_Object.DeleteItem(hTreeItem); 
	g_ToolResource.SaveObject();	
}

void _DlgMTObject::__InsertTree( LPCTSTR szRootPath, LPCTSTR szFile, bool bLastSelection )
{
	if( g_TempRoot.IsLoad() == true )
		return;

	LPCTSTR szSub = nsCSFILE::GetSubPath( szRootPath, L"Data\\" );

	// 이미 같은 루트가 존재 하는지 검색해 본다
	HTREEITEM hRoot = NULL;
	HTREEITEM hRootFind = m_tree_Object.GetRootItem();
	while( hRootFind != NULL )
	{
		if( _tcscmp( szSub, m_tree_Object.GetItemText( hRootFind ) ) == 0 )
		{
			hRoot = hRootFind;
			break;
		}
		hRootFind = m_tree_Object.GetNextSiblingItem( hRootFind );
	}

	// 같은 루트가 존재 하지 않는다면 새로 생성
	if( hRoot == NULL )
	{
		hRoot = m_tree_Object.InsertItem( szSub, NULL, TVI_SORT );
		m_tree_Object.SetItemData( hRoot, TT_ROOT );
	}

	// 같은 레벨상에 같은이름이 존재 하는지 검사
	HTREEITEM hChildFind = m_tree_Object.GetChildItem( hRoot );
	while( hChildFind != NULL )
	{
		if( _tcscmp( szFile, m_tree_Object.GetItemText( hChildFind ) ) == 0 )
		{
			return;
		}
		hChildFind = m_tree_Object.GetNextSiblingItem( hChildFind );
	}
	HTREEITEM hChild = m_tree_Object.InsertItem( szFile, hRoot, TVI_SORT );
	m_tree_Object.SetItemData( hChild, TT_NODE );
	if( bLastSelection )
		m_tree_Object.SelectItem( hChild );
}

// =================================================================================================
//
//		Toggle Is Render
//
// =================================================================================================
void _DlgMTObject::_ToggleRenderObject()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	if(nsCsGBTerrain::g_pCurRoot)
	{
		if( nsCsGBTerrain::g_pCurRoot->GetRenderObject_ByTool() == true)
			nsCsGBTerrain::g_pCurRoot->SetRenderObject_ByTool(false);
		else
			nsCsGBTerrain::g_pCurRoot->SetRenderObject_ByTool(true);

	}
	//m_check_RenderObject.SetCheck( m_check_RenderObject.GetCheck() == BST_UNCHECKED ? BST_CHECKED : BST_UNCHECKED );
	//																	//조건문    ?  참인 경우  : 거짓인 경우
	//OnBnClickedCheckRenderobject();
}

// 오브젝트 랜더링 할건지 -  체크 박스 클릭(렌더링 체크박스)
void _DlgMTObject::OnBnClickedCheckRenderobject()
{
	//if( g_TempRoot.IsLoad() == true )
	//	return;

	//if( m_check_RenderObject.GetCheck() == BST_CHECKED )
	//{
	//	if(nsCsGBTerrain::g_pCurRoot)
	//		nsCsGBTerrain::g_pCurRoot->SetRenderObject_ByTool( true );
	//}
	//else
	//{
	//	if(nsCsGBTerrain::g_pCurRoot)
	//		nsCsGBTerrain::g_pCurRoot->SetRenderObject_ByTool( false );
	//	m_tree_Object.SelectItem( NULL );		
	//}
}

// =================================================================================================
//
//		Input
//
// =================================================================================================
void _DlgMTObject::_KeyDownEscape()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	_ReleaseObjectTree();
	g_ObjectMng.DeleteModel();
}
void _DlgMTObject::OnKeyUp(const DWORD msg)
{
	g_ToolNiNodeMng.OnKeyUp(msg, &g_ObjectMng);
}
void _DlgMTObject::_OnLButtonDown( CPoint pt )
{
	if( g_ToolNiNodeMng.LButtonDown( pt, &g_ObjectMng ) == true )
		return;
	g_ObjectMng.LButtonDown( pt );
}

void _DlgMTObject::_OnLButtonDbClick( CPoint pt )
{
	g_ObjectMng.LButtonDbClick( pt );
	}

void _DlgMTObject::_OnLButtonUp( CPoint pt )
{
	g_ToolNiNodeMng.LButtonUp( pt , &g_ObjectMng);
	g_ObjectMng.LButtonUp( pt );
}

void _DlgMTObject::_OnMouseMove( CPoint pt )
{
	g_ToolNiNodeMng.MouseMove( pt , &g_ObjectMng);
	g_ObjectMng.MouseMove( pt);
}

void _DlgMTObject::_OnMouseWheel( short zDelta )
{
	g_ObjectMng.MouseWheel();
}


void _DlgMTObject::OnBnClickedCheckUseToggle()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	if(m_Chk_Use_Toggle.GetCheck() == TRUE)
		g_bCheckToggle = true;
	else
		g_bCheckToggle = false;
}

void _DlgMTObject::OnBnClickedUseObjectSnap(void)
{
}
void _DlgMTObject::OnNMReleasedcaptureSliderGizmoSence(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int CurrPos = m_GizmoSensor.GetPos();
	g_ToolNiNodeMng.OnChangeGizmoEditSense(CurrPos);
}
void _DlgMTObject::UpdateViewer(const NiTransform& kTr)
{
	float fScale = kTr.m_fScale;
	nsCsMfcFunc::Float2Control( &m_edit_Scale, fScale, 3 );
}

void _DlgMTObject::OnNMReleasedcaptureSliderSelectOpacity(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	float CurrPos = (float)m_SelectedOpacity.GetPos();

	g_ObjectMng.UpdateOpacity(CurrPos / 100.0f);
}

void _DlgMTObject::OnCbnSelchangeBasicCoord()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int CurSel = m_CoordBox.GetCurSel();
	g_ToolNiNodeMng.OnChangeGizmoCoord(CurSel, &g_ObjectMng);

	GetDlgItem(IDC_BASIC_COORD)->SendMessage(WM_KILLFOCUS, NULL); 
}

void _DlgMTObject::OnBnClickedCheckRenderMonsterregion()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int isChecked = m_check_RenderMonsterRegion.GetCheck();
	if(isChecked == 0)
	{
		m_bRenderMonsterRegion = false;
	}
	else
		m_bRenderMonsterRegion= true;
}

void _DlgMTObject::OnNMCustomdrawSliderGizmoSence(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void _DlgMTObject::OnBnClickedCheckShowCollisionbox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_Chk_Show_CollisionBox.GetCheck() == TRUE)
		g_ObjectMng.ShowCollisionBox(true);
	else
		g_ObjectMng.ShowCollisionBox(false);
}

void _DlgMTObject::OnBnClickedCheckCopyNomove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_Chk_Copy_NoMove.GetCheck() == TRUE)
		g_ObjectMng.CopyNomove(true);
	else
		g_ObjectMng.CopyNomove(false);
}

void _DlgMTObject::OnBnClickedCheckShowOccluder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( CAMERA_STPTR )
		CAMERA_STPTR->SetShowOccluder( m_Chk_Show_Occluder.GetCheck() );
}
