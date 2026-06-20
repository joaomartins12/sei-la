// _DlgMTPath.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTPath.h"
#include "ToolNiNode/UndoManager.h"

#define PATH_OBJECT_HEIGHT		50000.0f
#define TERRAIN_PATH_MIN		71.0f
#define TERRAIN_PATH_MAX		15000.0f

_DlgMTPath*		g_pDlgPath = NULL;
int _DlgMTPath::mLastCUID = 0;
bool _DlgMTPath::MovePathOBjSpin = false;
bool _DlgMTPath::DownPathOBjSpin = false;
float _DlgMTPath::m_fDownDelta = false;
MoveDir _DlgMTPath::eCurrMoveDIR = DIR_X;
// _DlgMTPath 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTPath, CDialog)

_DlgMTPath::_DlgMTPath(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTPath::IDD, pParent),
	m_bCreateSquare(false),
	m_pNiTerrainRoot(NULL)
{

}

_DlgMTPath::~_DlgMTPath()
{
}

void _DlgMTPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_RESIST_PATH, m_chk_ResistPath);
	DDX_Control(pDX, IDC_EDIT_TERRAINPATH_HEIGHT, m_edit_TerrainPathHeight);
	DDX_Control(pDX, IDC_SPIN_TERRAINPATH_HEIGHT, m_spin_TerrainPathHeight);
	DDX_Control(pDX, IDC_EDIT_OBJECTPATH_HEIGHT, m_edit_ObjectPathHeight);
	DDX_Control(pDX, IDC_SPIN_OBJECTPATH_HEIGHT, m_spin_ObjectPathHeight);
	DDX_Control(pDX, IDC_EDIT_MAXPATH_HEIGHT, m_edit_MaxPathHeight);
	DDX_Control(pDX, IDC_SPIN_MAXPATH_HEIGHT, m_spin_MaxPathHeight);
	DDX_Control(pDX, IDC_LIST_TERRAINPATH, m_list_TerrainPath);
	DDX_Control(pDX, IDC_SPIN_ET, m_spin_ET);
	DDX_Control(pDX, IDC_SPIN_ER, m_spin_ER);
	DDX_Control(pDX, IDC_SPIN_EL, m_spin_EL);
	DDX_Control(pDX, IDC_SPIN_EB, m_spin_EB);
	DDX_Control(pDX, IDC_SPIN_EROT, m_spin_ERot);
	DDX_Control(pDX, IDC_SPIN_PX, m_spin_PX);
	DDX_Control(pDX, IDC_SPIN_PY, m_spin_PY);
	DDX_Control(pDX, IDC_RADIO_MESH4, m_rdo_Mesh4);
	DDX_Control(pDX, IDC_RADIO_MESH6, m_rdo_Mesh6);
	DDX_Control(pDX, IDC_RADIO_MESH8, m_rdo_Mesh8);
	DDX_Control(pDX, IDC_PATHCNT, mPathCnt);
}


BEGIN_MESSAGE_MAP(_DlgMTPath, CDialog)
	ON_EN_CHANGE(IDC_EDIT_TERRAINPATH_HEIGHT, &_DlgMTPath::OnEnChangeEditPathobjHeight)
	ON_BN_CLICKED(IDC_BUTTON_RESET_OBJECTPATH, &_DlgMTPath::OnBnClickedButtonResetObjectpath)
	ON_BN_CLICKED(IDC_BUTTON_RESET_MAXPATH, &_DlgMTPath::OnBnClickedButtonResetMaxpath)
	ON_EN_CHANGE(IDC_EDIT_OBJECTPATH_HEIGHT, &_DlgMTPath::OnEnChangeEditObjectpathHeight)
	ON_EN_CHANGE(IDC_EDIT_MAXPATH_HEIGHT, &_DlgMTPath::OnEnChangeEditMaxpathHeight)
	ON_BN_CLICKED(IDC_BUTTON_RESET_TERRAINPATH, &_DlgMTPath::OnBnClickedButtonResetTerrainpath)
	ON_LBN_SELCHANGE(IDC_LIST_TERRAINPATH, &_DlgMTPath::OnLbnSelchangeListTerrainpath)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_DEL_PATH, &_DlgMTPath::OnBnClickedButtonDelPath)
	ON_BN_CLICKED(IDC_RADIO_MESH4, &_DlgMTPath::OnBnClickedRadioMesh4)
	ON_BN_CLICKED(IDC_RADIO_MESH6, &_DlgMTPath::OnBnClickedRadioMesh6)
	ON_BN_CLICKED(IDC_RADIO_MESH8, &_DlgMTPath::OnBnClickedRadioMesh8)
	ON_WM_LBUTTONDOWN()
	ON_STN_CLICKED(IDC_PATHCNT, &_DlgMTPath::OnStnClickedPathcnt)
	ON_BN_CLICKED(IDC_BUTTON_DEL_PATH_ALL, &_DlgMTPath::OnBnClickedButtonDelPathAll)
END_MESSAGE_MAP()


// _DlgMTPath 메시지 처리기입니다.

//====================================================================================================
//
//		virtual
//
//====================================================================================================

void _DlgMTPath::_Delete()
{
	__DeleteTerrainPath();
	__DeleteObjectPath();
	__DeleteMaxPath();

	m_pNiTerrainRoot = 0;
	m_pNiObjectRoot = 0;
	m_pNiMaxRoot = 0;
}

void _DlgMTPath::_PreViewReset()
{
	__DeleteTerrainPath();
	__DeleteObjectPath();
	__DeleteMaxPath();
}

void _DlgMTPath::_PostViewReset()
{
	__InitTerrainPath();	
}

void _DlgMTPath::_PostRenderView()
{
	//NiDrawScene( CAMERA_ST.GetCameraObj(), m_pNiTerrainRoot, *g_pCuller );
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_pNiObjectRoot, *g_pCuller );
	NiDrawScene( CAMERA_ST.GetCameraObj(), m_pNiMaxRoot, *g_pCuller );
}

void _DlgMTPath::_ViewInit()
{
	m_pNiTerrainRoot = NiNew NiNode;
	m_pNiObjectRoot = NiNew NiNode;
	m_pNiMaxRoot = NiNew NiNode;

	m_pNiTerrainRoot->SetTranslate( 0.0f, 0.0f, g_Resist.GetPath()->s_nTerrainPathHeight*100.0f );
	m_pNiTerrainRoot->Update( 0.0f );

	m_pNiObjectRoot->SetTranslate( 0.0f, 0.0f, g_Resist.GetPath()->s_nObjectPathHeight*100.0f );
	m_pNiObjectRoot->Update( 0.0f );

	m_pNiMaxRoot->SetTranslate( 0.0f, 0.0f, g_Resist.GetPath()->s_nMaxPathHeight*100.0f );
	m_pNiMaxRoot->Update( 0.0f );

	m_rdo_Mesh4.SetCheck( BST_CHECKED );
}

void _DlgMTPath::_PreSave( const char* cFile, bool bAutoSave )
{
	__ToTerrainRoot();
}

//====================================================================================================
//
//		Base
//
//====================================================================================================

BOOL _DlgMTPath::OnInitDialog()
{
	_DlgBase::OnInitDialog();
	
	nsCsMfcFunc::Int2Control( &m_edit_TerrainPathHeight, g_Resist.GetPath()->s_nTerrainPathHeight );
	m_spin_TerrainPathHeight.SetCsBuddy( &m_edit_TerrainPathHeight, 1 );
	m_spin_TerrainPathHeight.SetCsDeltaPixel( 5 );

	nsCsMfcFunc::Int2Control( &m_edit_ObjectPathHeight, g_Resist.GetPath()->s_nObjectPathHeight );
	m_spin_ObjectPathHeight.SetCsBuddy( &m_edit_ObjectPathHeight, 1 );
	m_spin_ObjectPathHeight.SetCsDeltaPixel( 5 );

	nsCsMfcFunc::Int2Control( &m_edit_MaxPathHeight, g_Resist.GetPath()->s_nMaxPathHeight );
	m_spin_MaxPathHeight.SetCsBuddy( &m_edit_MaxPathHeight, 1 );
	m_spin_MaxPathHeight.SetCsDeltaPixel( 5 );

	g_pDlgPath = this;
	m_spin_ET.SetCsAccel( 2 );
	m_spin_ET.ResistCsMoveCallBack( (void*)&MoveCallBack_ET );
	m_spin_ET.ResistCsMouseDownCallback((void*)&OnMouseDown);
	m_spin_ER.SetCsAccel( 2 );
	m_spin_ER.ResistCsMoveCallBack( (void*)&MoveCallBack_ER );
	m_spin_ER.ResistCsMouseDownCallback((void*)&OnMouseDown);
	m_spin_EL.SetCsAccel( 2 );
	m_spin_EL.ResistCsMoveCallBack( (void*)&MoveCallBack_EL );
	m_spin_EL.ResistCsMouseDownCallback((void*)&OnMouseDown);
	m_spin_EB.SetCsAccel( 2 );
	m_spin_EB.ResistCsMoveCallBack( (void*)&MoveCallBack_EB );
	m_spin_EB.ResistCsMouseDownCallback((void*)&OnMouseDown);
	m_spin_ERot.SetCsAccel( 0.33f );
	m_spin_ERot.ResistCsMoveCallBack( (void*)&MoveCallBack_ERot );
	m_spin_ERot.ResistCsMouseDownCallback((void*)&OnMouseDown);
	m_spin_PX.SetCsAccel( 2 );
	m_spin_PX.ResistCsMoveCallBack( (void*)&MoveCallBack_PX );
	m_spin_PX.ResistCsMouseDownCallback((void*)&OnMouseDown_PX);
	m_spin_PX.ResistCsMouseUpCallback((void*)&OnMouseUp_PX);
	m_spin_PY.SetCsAccel( 2 );
	m_spin_PY.ResistCsMoveCallBack( (void*)&MoveCallBack_PY );
	m_spin_PY.ResistCsMouseDownCallback((void*)&OnMouseDown_PY);
	m_spin_PY.ResistCsMouseUpCallback((void*)&OnMouseUp_PX);
	m_pCurTerrainPath = NULL;

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgMTPath::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		m_bLButtonDown = false;
		g_ToolNiNodeMng.OnStartObjectEdit(1);
		theApp.SetShowPathObj(true);
	}
	else
	{
		OnBnClickedButtonResetTerrainpath();
	}
	UNDO::Manager::GetSingletonPtr()->Reset();
}
void _DlgMTPath::_UpdateView(void)
{
	CheckingDownPathObjXSpin(0.16f);
}


//====================================================================================================
//
//		Path NiObject Parent
//
//====================================================================================================

void _DlgMTPath::__ToTerrainRoot()
{
	CsGBObject::sPATHINFO* pPathInfo = &nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_PathInfo;
	nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->Reset();

	size_t nCount = m_listTerrainPath.size();
	pPathInfo->s_uiPEGroupCount = (UINT)nCount;
	nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_pType = csnew char[ nCount ];
	pPathInfo->s_pptTranse = NiNew NiPoint2[ nCount ];

	// 패쓰 버퍼 체크
	assert_cs( nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_nDataTotalCount == 0 );
	int nIndex = 0;
	{
		std::list< NiGeometryPtr >::iterator it = m_listTerrainPath.begin();
		for( ; it!=m_listTerrainPath.end(); ++it )
		{
			switch( (*it)->GetVertexCount() )
			{
			case 8:
				nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_nDataTotalCount += 8;
				nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_pType[ nIndex ] = CsGBTerrainRoot::sTR_PATHINFO::MESH4;
				break;
			case 12:
				nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_nDataTotalCount += 12;
				nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_pType[ nIndex ] = CsGBTerrainRoot::sTR_PATHINFO::MESH6;
				break;
			case 16:
				nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_nDataTotalCount += 16;
				nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_pType[ nIndex ] = CsGBTerrainRoot::sTR_PATHINFO::MESH8;
				break;
			default:
				assert_cs( false );
			}
			++nIndex;
		}
	}
	pPathInfo->s_pData = csnew long[ nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_nDataTotalCount ];
	

	nIndex = 0;
	int nOffsetData = 0;
	long x, y;
	std::list< NiGeometryPtr >::iterator it = m_listTerrainPath.begin();
	for( ; it!=m_listTerrainPath.end(); ++it )
	{
		__CalPathData( nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_pType[ nIndex ], (*it), x, y, &pPathInfo->s_pData[ nOffsetData ] );
		pPathInfo->s_pptTranse[ nIndex ].x = (float)x;
		pPathInfo->s_pptTranse[ nIndex ].y = (float)y;		
		++nIndex;

		nOffsetData += (*it)->GetVertexCount();
	}
}
void _DlgMTPath::UpdatePath(void)
{
	CsGBObject::sPATHINFO* pPathInfo = &nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_PathInfo;
	if(pPathInfo->s_pData == NULL)
		return;
	int nIndex = 0;
	int nOffsetData = 0;
	long x, y;
	std::list< NiGeometryPtr >::iterator it = m_listTerrainPath.begin();
	for( ; it!=m_listTerrainPath.end(); ++it )
	{
		__CalPathData( nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_pType[ nIndex ], (*it), x, y, &pPathInfo->s_pData[ nOffsetData ] );
		pPathInfo->s_pptTranse[ nIndex ].x = (float)x;
		pPathInfo->s_pptTranse[ nIndex ].y = (float)y;		
		++nIndex;

		nOffsetData += (*it)->GetVertexCount();
	}


}
//====================================================================================================
//
//		Terrain Path NiObject
//
//====================================================================================================

void _DlgMTPath::OnBnClickedButtonResetTerrainpath()
{
	__ToTerrainRoot();

	GET_MV_MAINTAB_DLG->_GetDlgPlayer()->OnBnClickedButtonApplyCollision();
}

void _DlgMTPath::OnLbnSelchangeListTerrainpath()
{
	int nSel = m_list_TerrainPath.GetCurSel();
	if( nSel < 0 )
		return;

	_ReleaseCurTerrainPath();

	m_pCurTerrainPath = (NiGeometry*)m_list_TerrainPath.GetItemData( nSel );
	nsCSGBFUNC::Set_Emittance( m_pCurTerrainPath, NiColor( 1.0f, 0.0f, 0.0f ) );
	NiPoint3 GizmoPos = m_pCurTerrainPath->GetWorldBound().GetCenter();
	GizmoPos.z = 0.0f;

	PATH::Manager::GetSingletonPtr()->ResetSelected();
	PATH::Manager::GetSingletonPtr()->AddSelected(nSel, m_pCurTerrainPath);


	g_ToolNiNodeMng.OnObjectSelected(PATH::Manager::GetSingletonPtr()->GetSelectedCenter(),
		PATH::Manager::GetSingletonPtr());

}

void _DlgMTPath::OnPickPathObject(NiAVObject* pkObj)
{
	int SelCnt = m_list_TerrainPath.GetCount();
	if(SelCnt > 0)
	{
		for(int i =0 ; i < SelCnt; ++i)
		{
			NiAVObject* pkNiGeometry = (NiAVObject*)m_list_TerrainPath.GetItemData( i );
			if(pkNiGeometry == pkObj)
			{
				m_list_TerrainPath.SetCurSel(i);
				OnLbnSelchangeListTerrainpath();
				PATH::Manager::GetSingletonPtr()->ResetSelected();
				PATH::Manager::GetSingletonPtr()->AddSelected(i, pkNiGeometry);


				g_ToolNiNodeMng.OnObjectSelected(PATH::Manager::GetSingletonPtr()->GetSelectedCenter(),
					PATH::Manager::GetSingletonPtr());
				break;
			}
		}

	}

}
void _DlgMTPath::OnBnClickedButtonDelPath()
{
	int nSel = m_list_TerrainPath.GetCurSel();
	if( nSel < 0 )
		return;

	_ReleaseCurTerrainPath();
	std::list< NiGeometryPtr >::iterator it = m_listTerrainPath.begin();
	for( ; it!=m_listTerrainPath.end(); ++it )
	{
		if( nSel == 0 )
		{
			if( PATH::Manager::GetSingletonPtr() ){
				PATH::Manager::GetSingletonPtr()->RemovePathObj((*it));
			}
			m_pNiTerrainRoot->DetachChild( (*it) );
			(*it) = 0;
			m_listTerrainPath.erase( it );
			__ToTerrainRoot();
			//OnBnClickedButtonResetTerrainpath();
			_ResetTerrainPathList();			
			return;
		}
		--nSel;
	}
	assert_cs( false );
}
void _DlgMTPath::RemovePathObject(int Key)
{
	if( PATH::Manager::GetSingletonPtr() ){
		NiGeometry* pkGeo = PATH::Manager::GetSingletonPtr()->GetPathObject(Key);
		if(pkGeo)
		{
			_ReleaseCurTerrainPath();
			std::list< NiGeometryPtr >::iterator kIter = m_listTerrainPath.begin();
			for( ; kIter!=m_listTerrainPath.end(); ++kIter )
			{
				if( (*kIter) == pkGeo )
				{
					PATH::Manager::GetSingletonPtr()->RemovePathObjNoSnapshot((*kIter));
					m_pNiTerrainRoot->DetachChild( (*kIter) );
					(*kIter) = 0;
					m_listTerrainPath.erase( kIter );
					__ToTerrainRoot();
					_ResetTerrainPathList();			
					return;
				}
			}
		}
	}
}
void _DlgMTPath::_ResetTerrainPathList()
{
	int nCurSel = m_list_TerrainPath.GetCurSel();
	m_list_TerrainPath.ResetContent();

	int nIndex = 0;
	long x, y;
	long data[ 16 ];
	TCHAR szMsg[ 512 ];
	std::list< NiGeometryPtr >::iterator it = m_listTerrainPath.begin();
	for( ; it!=m_listTerrainPath.end(); ++it )
	{
		int nType = 0;
		switch( (*it)->GetVertexCount() )
		{
		case 8:			nType = CsGBTerrainRoot::sTR_PATHINFO::MESH4;		break;
		case 12:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH6;		break;
		case 16:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH8;		break;
		default:		assert_cs( false );
		}

		__CalPathData( nType, (*it), x, y, data );

		_stprintf_s( szMsg, 512, L"위치 : %d x %d [%d,%d] [%d,%d] [%d,%d] [%d,%d]", (int)x, (int)y,
						data[ 0 ], data[ 1 ],
						data[ 2 ], data[ 3 ],
						data[ 4 ], data[ 5 ],
						data[ 6 ], data[ 7 ]
						);

		int nIndex = m_list_TerrainPath.GetCount();
		m_list_TerrainPath.InsertString( nIndex, szMsg );
		m_list_TerrainPath.SetItemData( nIndex, (DWORD_PTR)(NiGeometry*)(*it) );

		++nIndex;
	}
	wchar_t text[256];
	wsprintf(text,L"MaxCnt : [%d]", m_list_TerrainPath.GetCount() );

	mPathCnt.SetWindowText(text);
	if( nCurSel < m_list_TerrainPath.GetCount() )
	{
		m_list_TerrainPath.SetCurSel( nCurSel );
	}
}

void _DlgMTPath::_SetCurTerrainPath( int nIndex )
{
	_ReleaseCurTerrainPath();

	m_pCurTerrainPath = (NiGeometry*)m_list_TerrainPath.GetItemData( nIndex );
	nsCSGBFUNC::Set_Emittance( m_pCurTerrainPath, NiColor( 1.0f, 0.0f, 0.0f ) );

	m_list_TerrainPath.SetCurSel( nIndex );
}

void _DlgMTPath::__CalPathData( int nType, NiGeometry* pSrcGeom, long& x, long&y, long* pData )
{
	NiPoint3* pPos = pSrcGeom->GetVertices();

	switch( nType )
	{
	case CsGBTerrainRoot::sTR_PATHINFO::MESH4:		
		x = (long)( ( pPos[ 0 ].x + pPos[ 2 ].x )*0.5f );
		y = (long)( ( pPos[ 0 ].y + pPos[ 2 ].y )*0.5f );	
		pData[ 0 ] = (long)( pPos[ 0 ].x + 0.5f ) - x;
		pData[ 1 ] = (long)( pPos[ 0 ].y + 0.5f ) - y;
		pData[ 2 ] = (long)( pPos[ 3 ].x + 0.5f ) - x;
		pData[ 3 ] = (long)( pPos[ 3 ].y + 0.5f ) - y;
		pData[ 4 ] = (long)( pPos[ 2 ].x + 0.5f ) - x;
		pData[ 5 ] = (long)( pPos[ 2 ].y + 0.5f ) - y;
		pData[ 6 ] = (long)( pPos[ 1 ].x + 0.5f ) - x;
		pData[ 7 ] = (long)( pPos[ 1 ].y + 0.5f ) - y;
		break;
	case CsGBTerrainRoot::sTR_PATHINFO::MESH6:
		x = (long)( ( pPos[ 0 ].x + pPos[ 3 ].x )*0.5f );
		y = (long)( ( pPos[ 0 ].y + pPos[ 3 ].y )*0.5f );	
		pData[ 0 ] = (long)( pPos[ 0 ].x + 0.5f ) - x;
		pData[ 1 ] = (long)( pPos[ 0 ].y + 0.5f ) - y;
		pData[ 2 ] = (long)( pPos[ 5 ].x + 0.5f ) - x;
		pData[ 3 ] = (long)( pPos[ 5 ].y + 0.5f ) - y;
		pData[ 4 ] = (long)( pPos[ 4 ].x + 0.5f ) - x;
		pData[ 5 ] = (long)( pPos[ 4 ].y + 0.5f ) - y;
		pData[ 6 ] = (long)( pPos[ 3 ].x + 0.5f ) - x;
		pData[ 7 ] = (long)( pPos[ 3 ].y + 0.5f ) - y;
		pData[ 8 ] = (long)( pPos[ 2 ].x + 0.5f ) - x;
		pData[ 9 ] = (long)( pPos[ 2 ].y + 0.5f ) - y;
		pData[ 10 ] = (long)( pPos[ 1 ].x + 0.5f ) - x;
		pData[ 11 ] = (long)( pPos[ 1 ].y + 0.5f ) - y;
		break;
	case CsGBTerrainRoot::sTR_PATHINFO::MESH8:
		x = (long)( ( pPos[ 0 ].x + pPos[ 4 ].x )*0.5f );
		y = (long)( ( pPos[ 0 ].y + pPos[ 4 ].y )*0.5f );	
		pData[ 0 ] = (long)( pPos[ 0 ].x + 0.5f ) - x;
		pData[ 1 ] = (long)( pPos[ 0 ].y + 0.5f ) - y;
		pData[ 2 ] = (long)( pPos[ 7 ].x + 0.5f ) - x;
		pData[ 3 ] = (long)( pPos[ 7 ].y + 0.5f ) - y;
		pData[ 4 ] = (long)( pPos[ 6 ].x + 0.5f ) - x;
		pData[ 5 ] = (long)( pPos[ 6 ].y + 0.5f ) - y;
		pData[ 6 ] = (long)( pPos[ 5 ].x + 0.5f ) - x;
		pData[ 7 ] = (long)( pPos[ 5 ].y + 0.5f ) - y;
		pData[ 8 ] = (long)( pPos[ 4 ].x + 0.5f ) - x;
		pData[ 9 ] = (long)( pPos[ 4 ].y + 0.5f ) - y;
		pData[ 10 ] = (long)( pPos[ 3 ].x + 0.5f ) - x;
		pData[ 11 ] = (long)( pPos[ 3 ].y + 0.5f ) - y;
		pData[ 12 ] = (long)( pPos[ 2 ].x + 0.5f ) - x;
		pData[ 13 ] = (long)( pPos[ 2 ].y + 0.5f ) - y;
		pData[ 14 ] = (long)( pPos[ 1 ].x + 0.5f ) - x;
		pData[ 15 ] = (long)( pPos[ 1 ].y + 0.5f ) - y;
		break;
	default:
		assert_cs( false );
	}	
}

void _DlgMTPath::__DeleteTerrainPath()
{
	_ReleaseCurTerrainPath();

	std::list< NiGeometryPtr >::iterator it = m_listTerrainPath.begin();
	for( ; it!=m_listTerrainPath.end(); ++it )
	{
		m_pNiTerrainRoot->DetachChild( (*it) );
		(*it) = 0;
	}
	m_listTerrainPath.clear();
}

void _DlgMTPath::__InitTerrainPath()
{
	if( PATH::Manager::GetSingletonPtr() != NULL)
		PATH::Manager::GetSingletonPtr()->ResetAllPaths();

	CsGeomBox::sINFO info;
	info.Reset();
	info.s_fAlpha = 0.3f;
	info.s_Color = NiColor( 1.0f, 1.0f, 0.0f );
	info.s_bTwoSide = true;

	CsGBObject::sPATHINFO* pPathInfo = &nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_PathInfo;
	NiPoint3 vPos[ 8 ];	
	int nOffset = 0;
	for( UINT i=0; i<pPathInfo->s_uiPEGroupCount; ++i )
	{
		NiPoint3 ptTranse = NiPoint3( pPathInfo->s_pptTranse[ i ].x, pPathInfo->s_pptTranse[ i ].y, 0.0f );

		NiGeometryPtr pGeom;
		switch( nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_pType [ i ] )
		{
		case CsGBTerrainRoot::sTR_PATHINFO::MESH4:
			vPos[ 0 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 0 ], (float)pPathInfo->s_pData[ nOffset + 1 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 1 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 6 ], (float)pPathInfo->s_pData[ nOffset + 7 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 2 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 4 ], (float)pPathInfo->s_pData[ nOffset + 5 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 3 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 2 ], (float)pPathInfo->s_pData[ nOffset + 3 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			nOffset += 8;
			pGeom = CsGeomBox::CreatePolygon( 4, PATH_OBJECT_HEIGHT, vPos, &info );
			break;
		case CsGBTerrainRoot::sTR_PATHINFO::MESH6:
			vPos[ 0 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 0 ], (float)pPathInfo->s_pData[ nOffset + 1 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 1 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 10 ], (float)pPathInfo->s_pData[ nOffset + 11 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 2 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 8 ], (float)pPathInfo->s_pData[ nOffset + 9 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 3 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 6 ], (float)pPathInfo->s_pData[ nOffset + 7 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 4 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 4 ], (float)pPathInfo->s_pData[ nOffset + 5 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 5 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 2 ], (float)pPathInfo->s_pData[ nOffset + 3 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			nOffset += 12;
			pGeom = CsGeomBox::CreatePolygon( 6, PATH_OBJECT_HEIGHT, vPos, &info );
			break;
		case CsGBTerrainRoot::sTR_PATHINFO::MESH8:
			vPos[ 0 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 0 ], (float)pPathInfo->s_pData[ nOffset + 1 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 1 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 14 ], (float)pPathInfo->s_pData[ nOffset + 15 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 2 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 12 ], (float)pPathInfo->s_pData[ nOffset + 13 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 3 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 10 ], (float)pPathInfo->s_pData[ nOffset + 11 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 4 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 8 ], (float)pPathInfo->s_pData[ nOffset + 9 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 5 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 6 ], (float)pPathInfo->s_pData[ nOffset + 7 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 6 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 4 ], (float)pPathInfo->s_pData[ nOffset + 5 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			vPos[ 7 ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset + 2 ], (float)pPathInfo->s_pData[ nOffset + 3 ], -PATH_OBJECT_HEIGHT ) + ptTranse;
			nOffset += 16;
			pGeom = CsGeomBox::CreatePolygon( 8, PATH_OBJECT_HEIGHT, vPos, &info );
			break;
		default:
			assert_cs( false );
		}		
		
		m_listTerrainPath.push_back( pGeom );
		m_pNiTerrainRoot->AttachChild( pGeom );
		if( PATH::Manager::GetSingletonPtr() != NULL)
			PATH::Manager::GetSingletonPtr()->InitAddPaths(pGeom);
	}
	m_pNiTerrainRoot->Update( 0.0f );
	_ResetTerrainPathList();
}

NiGeometry* _DlgMTPath::__AddTerrainPath( int nType, NiPoint3 ptSrcLeftTopMinZ, const UNDO::ModelVtInfo& kPos )
{
	_ReleaseCurTerrainPath();

	NiPoint3 ptLeftTopMinZ = ptSrcLeftTopMinZ;
	ptLeftTopMinZ.z = -PATH_OBJECT_HEIGHT;
	NiPoint3 ptRightBottomMaxZ = ptLeftTopMinZ + NiPoint3( TERRAIN_PATH_MIN, TERRAIN_PATH_MIN, PATH_OBJECT_HEIGHT );

	CsGeomBox::sINFO info;
	info.Reset();
	info.s_fAlpha = 0.3f;
	info.s_Color = NiColor( 1.0f, 1.0f, 0.0f );
	info.s_bTwoSide = true;

	NiGeometryPtr pGeom;

	switch( nType )
	{
	case CsGBTerrainRoot::sTR_PATHINFO::MESH4:
		pGeom = CsGeomBox::CreateBox( ptLeftTopMinZ, ptRightBottomMaxZ, &info );
		break;
	case CsGBTerrainRoot::sTR_PATHINFO::MESH6:
		pGeom = CsGeomBox::CreateMesh6( ptLeftTopMinZ, ptRightBottomMaxZ, &info );
		break;
	case CsGBTerrainRoot::sTR_PATHINFO::MESH8:
		pGeom = CsGeomBox::CreateMesh8( ptLeftTopMinZ, ptRightBottomMaxZ, &info );
		break;
	}
	
	m_listTerrainPath.push_back( pGeom );
	m_pNiTerrainRoot->AttachChild( pGeom );
	m_pNiTerrainRoot->Update( 0.0f );
	PATH::Manager::GetSingletonPtr()->InitAddPaths(pGeom);

	NiPoint3* pPos = pGeom->GetVertices();
	UNDO::ModelVtPos::const_iterator kIter = kPos.mVTPos.begin();
	for(; kIter != kPos.mVTPos.end(); ++kIter)
	{
		pPos[ kIter->first ]= kIter->second;
	}
	pGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pGeom->Update(0.0f);

	return pGeom;
}

NiGeometry* _DlgMTPath::__AddTerrainPath( int nType, NiPoint3 ptSrcLeftTopMinZ )
{
	_ReleaseCurTerrainPath();

	NiPoint3 ptLeftTopMinZ = ptSrcLeftTopMinZ;
	ptLeftTopMinZ.z = -PATH_OBJECT_HEIGHT;
	NiPoint3 ptRightBottomMaxZ = ptLeftTopMinZ + NiPoint3( TERRAIN_PATH_MIN, TERRAIN_PATH_MIN, PATH_OBJECT_HEIGHT );

	CsGeomBox::sINFO info;
	info.Reset();
	info.s_fAlpha = 0.3f;
	info.s_Color = NiColor( 1.0f, 0.0f, 0.0f );
	info.s_bTwoSide = true;

	NiGeometryPtr pGeom;

	switch( nType )
	{
	case CsGBTerrainRoot::sTR_PATHINFO::MESH4:
		pGeom = CsGeomBox::CreateBox( ptLeftTopMinZ, ptRightBottomMaxZ, &info );
		break;
	case CsGBTerrainRoot::sTR_PATHINFO::MESH6:
		pGeom = CsGeomBox::CreateMesh6( ptLeftTopMinZ, ptRightBottomMaxZ, &info );
		break;
	case CsGBTerrainRoot::sTR_PATHINFO::MESH8:
		pGeom = CsGeomBox::CreateMesh8( ptLeftTopMinZ, ptRightBottomMaxZ, &info );
		break;
	}
	
	m_listTerrainPath.push_back( pGeom );
	m_pNiTerrainRoot->AttachChild( pGeom );
	m_pNiTerrainRoot->Update( 0.0f );
	PATH::Manager::GetSingletonPtr()->CreateAddPaths(pGeom);
	return pGeom;
}

void _DlgMTPath::_ReleaseCurTerrainPath()
{
	if( m_pCurTerrainPath != NULL )
	{
		nsCSGBFUNC::Set_Emittance( m_pCurTerrainPath, NiColor( 1.0f, 1.0f, 0.0f ) );
		m_pCurTerrainPath = NULL;
	}
}

// 터레인 오브젝트 헤이트값 에디트 변경시 호출
void _DlgMTPath::OnEnChangeEditPathobjHeight()
{
	g_Resist.GetPath()->s_nTerrainPathHeight = nsCsMfcFunc::Control2Int( &m_edit_TerrainPathHeight );

	if( m_pNiTerrainRoot != NULL )
	{
		m_pNiTerrainRoot->SetTranslate( 0.0f, 0.0f, g_Resist.GetPath()->s_nTerrainPathHeight*100.0f );
		m_pNiTerrainRoot->Update( 0.0f );
	}
}

#define MOVECALLBACK_EDITSIZE( start1, start2, end1, end2, end3, end4 ) {\
	NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();\
	if( pSrcGeom == NULL )\
	return;\
	if( GET_MV_VIEW->_IsControl() == true )\
		fDelta *= 5.0f;\
	NiPoint3* pPos = pSrcGeom->GetVertices();\
	NiPoint3 vStart = NiPoint3( (pPos[ start1 ].x+pPos[ start2 ].x)*0.5f, (pPos[ start1 ].y+pPos[ start2 ].y)*0.5f, 0.0f );\
	NiPoint3 vEnd = NiPoint3( (pPos[ end1 ].x+pPos[ end2 ].x)*0.5f, (pPos[ end1 ].y+pPos[ end2 ].y)*0.5f, 0.0f );\
	NiPoint3 vDir = vEnd - vStart;\
	vDir.Unitize();\
	NiPoint2 vMin = NiPoint2( FLT_MAX, FLT_MAX );\
	NiPoint2 vMax = NiPoint2( -FLT_MAX, -FLT_MAX );\
	for( int i=0; i<4; ++i )\
	{\
		if( vMin.x > pPos[ i ].x )		vMin.x = pPos[ i ].x;\
		if( vMax.x < pPos[ i ].x )		vMax.x = pPos[ i ].x;\
		if( vMin.y > pPos[ i ].y )		vMin.y = pPos[ i ].y;\
		if( vMax.y < pPos[ i ].y )		vMax.y = pPos[ i ].y;\
	}\
	float fLengthX = vMax.x - vMin.x;\
	float fLengthY = vMax.y - vMin.y;\
	float fCos = abs( vDir.Dot( NiPoint3( 1.0f, 0.0f, 0.0f ) ) );\
	float xDelta = fCos*fDelta;\
	if( ( abs( fCos ) > 0.0005f )&&( fLengthX + xDelta < TERRAIN_PATH_MIN ) )\
	{\
		xDelta = TERRAIN_PATH_MIN - fLengthX;\
		fDelta = xDelta/fCos;\
	}\
	else if( ( abs( fCos ) > 0.0005f )&&( fLengthX + xDelta > TERRAIN_PATH_MAX ) )\
	{\
		xDelta = TERRAIN_PATH_MAX - fLengthX;\
		fDelta = xDelta/fCos;\
	}\
	float fSin = NiSin( NiACos( fCos ) );\
	float yDelta = fSin*fDelta;\
	if( ( abs( fSin ) > 0.0005f )&&( fLengthY + yDelta < TERRAIN_PATH_MIN ) )\
	{\
		yDelta = TERRAIN_PATH_MIN - fLengthY;\
		fDelta = yDelta/fSin;\
	}\
	else if( ( abs( fSin ) > 0.0005f )&&( fLengthY + yDelta > TERRAIN_PATH_MAX ) )\
	{\
		yDelta = TERRAIN_PATH_MAX - fLengthY;\
		fDelta = yDelta/fSin;\
	}\
	if( fDelta == 0.0f )\
		return;\
	pPos[ end1 ] += vDir*fDelta;\
	pPos[ end2 ] += vDir*fDelta;\
	pPos[ end3 ] += vDir*fDelta;\
	pPos[ end4 ] += vDir*fDelta;\
	pSrcGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );\
	pSrcGeom->Update( 0.0f );\
	g_pDlgPath->_ResetTerrainPathList();\
}

void _DlgMTPath::MoveCallBack_ET( float fDelta )
{
	g_pDlgPath->_ReleaseCheckResistPath();
	NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();
	if(pSrcGeom)
	{
		int nType = 0;
		switch( pSrcGeom->GetVertexCount() )
		{
		case 8:			
			{
				MOVECALLBACK_EDITSIZE( 0, 1, 2, 3, 6, 7 );
				break;
			}
		case 12:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH6;		break;
		case 16:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH8;		break;
		default:		assert_cs( false );
		}
	}


}

void _DlgMTPath::MoveCallBack_ER( float fDelta )
{
	g_pDlgPath->_ReleaseCheckResistPath();
	NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();
	if(pSrcGeom)
	{
		int nType = 0;
		switch( pSrcGeom->GetVertexCount() )
		{
		case 8:			
			{
				MOVECALLBACK_EDITSIZE( 0, 3, 1, 2, 5, 6 );
				break;
			}
		case 12:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH6;		break;
		case 16:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH8;		break;
		default:		assert_cs( false );
		}
	}
}

void _DlgMTPath::MoveCallBack_EL( float fDelta )
{
	g_pDlgPath->_ReleaseCheckResistPath();
	NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();
	if(pSrcGeom)
	{
		int nType = 0;
		switch( pSrcGeom->GetVertexCount() )
		{
		case 8:			
			{
				MOVECALLBACK_EDITSIZE( 1, 2, 0, 3, 4, 7 );
				break;
			}
		case 12:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH6;		break;
		case 16:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH8;		break;
		default:		assert_cs( false );
		}
	}
}

void _DlgMTPath::MoveCallBack_EB( float fDelta )
{
	g_pDlgPath->_ReleaseCheckResistPath();
	NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();
	if(pSrcGeom)
	{
		int nType = 0;
		switch( pSrcGeom->GetVertexCount() )
		{
		case 8:			
			{
				MOVECALLBACK_EDITSIZE( 2, 3, 0, 1, 4, 5 );
				break;
			}
		case 12:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH6;		break;
		case 16:		nType = CsGBTerrainRoot::sTR_PATHINFO::MESH8;		break;
		default:		assert_cs( false );
		}
	}
}

void _DlgMTPath::MoveCallBack_ERot( float fDelta, bool bUseControl /*=true*/ )
{
	g_pDlgPath->_ReleaseCheckResistPath();
	NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();
	if( pSrcGeom == NULL )
		return;

	if( bUseControl == true )
	{
		if( GET_MV_VIEW->_IsControl() == true )
			fDelta *= 5.0f;
	}	

	NiPoint3* pPos = pSrcGeom->GetVertices();

	NiMatrix3 mat;
	mat.MakeZRotation( CsD2R( fDelta ) );
	NiPoint3 vOffset = NiPoint3( ( pPos[ 0 ].x + pPos[ pSrcGeom->GetVertexCount()/4 ].x )*0.5f, ( pPos[ 0 ].y + pPos[ pSrcGeom->GetVertexCount()/4 ].y )*0.5f, 0.0f );
	NiPoint3 vPos;
	for( int i=0; i<pSrcGeom->GetVertexCount(); ++i )
	{
		vPos = ( pPos[ i ] - vOffset )*mat;
		pPos[ i ] = vPos + vOffset;
	}
	pSrcGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pSrcGeom->Update( 0.0f );

	MoveCallBack_ET( 0.0f );
	MoveCallBack_ER( 0.0f );
	MoveCallBack_EL( 0.0f );
	MoveCallBack_EB( 0.0f );
}

void _DlgMTPath::Scale( float fDeltaScale )
{
	g_pDlgPath->_ReleaseCheckResistPath();
	NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();
	if( pSrcGeom == NULL )
		return;

	NiTransform tr;
	tr.MakeIdentity();
	tr.m_fScale = fDeltaScale;

	NiPoint3* pPos = pSrcGeom->GetVertices();
	NiPoint3 vOffset = NiPoint3( ( pPos[ 0 ].x + pPos[ pSrcGeom->GetVertexCount()/4 ].x )*0.5f, ( pPos[ 0 ].y + pPos[ pSrcGeom->GetVertexCount()/4 ].y )*0.5f, 0.0f );
	NiPoint3 vPos;
	for( int i=0; i<pSrcGeom->GetVertexCount(); ++i )
	{
		vPos = ( pPos[ i ] - vOffset )*( fDeltaScale + 1.0f );
		pPos[ i ] = vPos + vOffset;
	}
	pSrcGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pSrcGeom->Update( 0.0f );

	MoveCallBack_ET( 0.0f );
	MoveCallBack_ER( 0.0f );
	MoveCallBack_EL( 0.0f );
	MoveCallBack_EB( 0.0f );
}

void _DlgMTPath::MoveCallBack_PX( float fDelta )
{
	g_pDlgPath->_ReleaseCheckResistPath();
	NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();
	if( pSrcGeom == NULL )
		return;
	if( GET_MV_VIEW->_IsControl() == true )
		fDelta *= 5.0f;

	NiPoint3* pPos = pSrcGeom->GetVertices();
	for( int i=0; i<pSrcGeom->GetVertexCount(); ++i )
	{
		pPos[ i ].x += fDelta;
	}
	//switch( pSrcGeom->GetVertexCount() )
	//{
	//case 8:	

	//	break;
	//}
	pSrcGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pSrcGeom->Update( 0.0f );
	g_pDlgPath->_ResetTerrainPathList();
	MovePathOBjSpin = true;
	DownPathOBjSpin = false;
}

void _DlgMTPath::MoveCallBack_PY( float fDelta )
{
	g_pDlgPath->_ReleaseCheckResistPath();
	NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();
	if( pSrcGeom == NULL )
		return;
	if( GET_MV_VIEW->_IsControl() == true )
		fDelta *= 5.0f;

	NiPoint3* pPos = pSrcGeom->GetVertices();
	for( int i=0; i<pSrcGeom->GetVertexCount(); ++i )
	{
		pPos[ i ].y += fDelta;
	}
	//switch( pSrcGeom->GetVertexCount() )
	//{
	//case 8:	
	//	if( GET_MV_VIEW->_IsControl() == true )
	//		fDelta *= 5.0f;

	//	NiPoint3* pPos = pSrcGeom->GetVertices();
	//	for( int i=0; i<8; ++i )
	//	{
	//		pPos[ i ].y += fDelta;
	//	}
	//	break;
	//}
	pSrcGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pSrcGeom->Update( 0.0f );
	g_pDlgPath->_ResetTerrainPathList();
	MovePathOBjSpin = true;
	DownPathOBjSpin = false;
}
void _DlgMTPath::OnMouseDown(float fDelta)
{
	CString kLog;
	kLog.Format(_T("OnMouseDown_PX"));
	OutputDebugStringW(kLog);
	mLastCUID = PATH::Manager::GetSingletonPtr()->TakeUndoSnapshot(3);
}

void _DlgMTPath::OnMouseDown_PX(float fDelta)
{
	CString kLog;
	kLog.Format(_T("OnMouseDown_PX"));
	OutputDebugStringW(kLog);
	mLastCUID = PATH::Manager::GetSingletonPtr()->TakeUndoSnapshot(3);
	DownPathOBjSpin= true;
	m_fDownDelta = fDelta;
	eCurrMoveDIR = DIR_X;
}
void _DlgMTPath::OnMouseUp_PX(float fDelta)
{
	CString kLog;
	kLog.Format(_T("OnMouseUp_PX"));
	MovePathOBjSpin = false;
	DownPathOBjSpin = false;
	PATH::Manager::GetSingletonPtr()->TakeRedoSnapshot(mLastCUID);
	OutputDebugStringW(kLog);
}
void _DlgMTPath::OnMouseDown_PY(float fDelta)
{
	CString kLog;
	kLog.Format(_T("OnMouseDown_PY"));
	OutputDebugStringW(kLog);
	mLastCUID = PATH::Manager::GetSingletonPtr()->TakeUndoSnapshot(3);
	DownPathOBjSpin= true;
	m_fDownDelta = fDelta;
	eCurrMoveDIR = DIR_Y;
}
void _DlgMTPath::OnMouseUp_PY(float fDelta)
{
	CString kLog;
	kLog.Format(_T("OnMouseUp_PY"));
	OutputDebugStringW(kLog);
	PATH::Manager::GetSingletonPtr()->TakeRedoSnapshot(mLastCUID);
}

//====================================================================================================
//
//		Object Path NiObject
//
//====================================================================================================

bool isClick = false;
// 리셋 버튼 클릭
void _DlgMTPath::OnBnClickedButtonResetObjectpath()
{
	if(isClick == false)
	{
		__DeleteObjectPath();
		__InitObjectPath();
		isClick = !isClick;
	}
	else
	{
		isClick = !isClick;
		DeleteObjectPath();
	}

}

void _DlgMTPath::__DeleteObjectPath()
{
	std::list< NiGeometryPtr >::iterator it = m_listObjectPath.begin();
	for( ; it!=m_listObjectPath.end(); ++it )
	{
		m_pNiObjectRoot->DetachChild( (*it) );
		(*it) = 0;
	}
	m_listObjectPath.clear();
}

void _DlgMTPath::DeleteObjectPath(void)
{
	int nTotalLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	CsGBObject::sPATHINFO* pPathInfo = NULL;
	for( int i=0; i<nTotalLeafCount; ++i )
	{
		MAP_GBTERRAIN_OBJECT* pMapObject = nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMapObject->begin();
		for( ; it!=pMapObject->end(); ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;

			pPathInfo = &it->second->GetInfo()->s_PathInfo;
			pPathInfo->Reset();
		}
	}
}

void _DlgMTPath::__InitObjectPath()
{
	assert_cs( m_listObjectPath.empty() == true );

	GET_MV_MAINTAB_DLG->_GetDlgPlayer()->OnBnClickedButtonApplyCollision();
	
	int nTotalLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	CsGBObject::sPATHINFO* pPathInfo = NULL;

	NiPoint3 ptPos[ 100 ];
	for( int i=0; i<nTotalLeafCount; ++i )
	{
		MAP_GBTERRAIN_OBJECT* pMapObject = nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMapObject->begin();
		for( ; it!=pMapObject->end(); ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;

			pPathInfo = &it->second->GetInfo()->s_PathInfo;

			if( pPathInfo->s_uiPEGroupCount == 0 )
				continue;

			int nOffset = 0;			
			for( UINT i=0; i<pPathInfo->s_uiPEGroupCount; ++i )
			{
				NiPoint3 ptTranse = NiPoint3( pPathInfo->s_pptTranse[ i ].x, pPathInfo->s_pptTranse[ i ].y, 0.0f );
				for( USHORT usVert=0; usVert<pPathInfo->s_pPECount[ i ]; ++usVert )
				{
					ptPos[ usVert ] = NiPoint3( (float)pPathInfo->s_pData[ nOffset ],
												(float)pPathInfo->s_pData[ nOffset + 1 ],
												-PATH_OBJECT_HEIGHT ) + ptTranse;
					nOffset += 2;									  
				}

				__AddObjectPath( pPathInfo->s_pPECount[ i ], ptPos );

			}
		}
	}

	m_pNiObjectRoot->Update( 0.0f );
}

void _DlgMTPath::__AddObjectPath( UINT uiBottomVertCount, NiPoint3* pPos )
{
	CsGeomBox::sINFO info;
	info.Reset();
	info.s_fAlpha = 0.3f;
	info.s_Color = NiColor( 0.0f, 0.0f, 1.0f );
	info.s_bTwoSide = true;

	NiGeometryPtr pGeom = CsGeomBox::CreatePolygon( uiBottomVertCount, PATH_OBJECT_HEIGHT, pPos, &info );
	m_listObjectPath.push_back( pGeom );

	m_pNiObjectRoot->AttachChild( pGeom );
}


// 터레인 오브젝트 헤이트값 에디트 변경시 호출
void _DlgMTPath::OnEnChangeEditObjectpathHeight()
{
	g_Resist.GetPath()->s_nObjectPathHeight = nsCsMfcFunc::Control2Int( &m_edit_ObjectPathHeight );

	if( m_pNiObjectRoot != NULL )
	{
		m_pNiObjectRoot->SetTranslate( 0.0f, 0.0f, g_Resist.GetPath()->s_nObjectPathHeight*100.0f );
		m_pNiObjectRoot->Update( 0.0f );
	}
}

//====================================================================================================
//
//		Max Path NiObject
//
//====================================================================================================

// 리셋 버튼 클릭
void _DlgMTPath::OnBnClickedButtonResetMaxpath()
{
	__DeleteMaxPath();
	__InitMaxPath();
}

void _DlgMTPath::__DeleteMaxPath()
{
	std::list< NiGeometryPtr >::iterator it = m_listMaxPath.begin();
	for( ; it!=m_listMaxPath.end(); ++it )
	{
		m_pNiMaxRoot->DetachChild( (*it) );
		(*it) = 0;		
	}
	m_listMaxPath.clear();	
}

void _DlgMTPath::__InitMaxPath()
{
	assert_cs( m_listMaxPath.empty() == true );

	// xml 파일이 있는지 체크
	char cStr[ MAX_PATH ];
	strcpy_s( cStr, MAX_PATH, nsCsGBTerrain::g_pCurRoot->GetMapFilePath() );
	size_t nLen = strlen( cStr );
	sprintf_s( &cStr[ nLen - 3 ], MAX_PATH - (nLen-3), "xml" );
	if( _access_s( cStr, 0 ) != 0 )
		return;

	size_t ulSize;	 
	char* buffer = LoadBinary_AllocatedBuffer( cStr, ulSize );
	char cData[ 2048 ];
	char cInt[ 128 ];
	char* cOutVert;

	NiPoint3 ptPos[ 100 ];

	nsCSFILE::sTOKEN_c token( buffer, "<baseObstacles>", "</baseObstacles>" );
	while( token.Next( cData, 2048, "obstacle position", "\"", 2 ) )
	{
		nsCSFILE::sTOKEN_c tokenTr( cData );
		float fTrx = (float)atoi( tokenTr.Next( cInt, 128 ) );
		float fTry = (float)atoi( tokenTr.Next( cInt, 128 ) );

		token.Next( cData, 2048, "vertices", "\"" );
		nsCSFILE::sTOKEN_c tokenVert( cData );
		int nptPosIndex = 0;
		while( cOutVert = tokenVert.Next( cInt, 128 ) )
		{
			ptPos[ nptPosIndex ].x = (float)atoi( cOutVert ) + (float)fTrx;
			cOutVert = tokenVert.Next( cInt, 128 );
			assert_cs( cOutVert != NULL );
			ptPos[ nptPosIndex ].y = (float)atoi( cOutVert ) + (float)fTry;
			ptPos[ nptPosIndex ].z = -PATH_OBJECT_HEIGHT;
			++nptPosIndex;
		}
		__AddMaxPath( nptPosIndex, ptPos );
	}

	SAFE_DELETE_ARRAY( buffer );

	m_pNiMaxRoot->Update( 0.0f );
}

void _DlgMTPath::__AddMaxPath( UINT uiBottomVertCount, NiPoint3* pPos )
{
	CsGeomBox::sINFO info;
	info.Reset();
	info.s_fAlpha = 0.3f;
	info.s_Color = NiColor( 1.0f, 0.0f, 1.0f );
	info.s_bTwoSide = true;

	NiGeometryPtr pGeom = CsGeomBox::CreatePolygon( uiBottomVertCount, PATH_OBJECT_HEIGHT, pPos, &info );
	m_listMaxPath.push_back( pGeom );

	m_pNiMaxRoot->AttachChild( pGeom );
}

// 맥스 오브젝트 헤이트값 에디트 변경시 호출
void _DlgMTPath::OnEnChangeEditMaxpathHeight()
{
	g_Resist.GetPath()->s_nMaxPathHeight = nsCsMfcFunc::Control2Int( &m_edit_MaxPathHeight );

	if( m_pNiMaxRoot != NULL )
	{
		m_pNiMaxRoot->SetTranslate( 0.0f, 0.0f, g_Resist.GetPath()->s_nMaxPathHeight*100.0f );
		m_pNiMaxRoot->Update( 0.0f );
	}
}


//====================================================================================================
//
//		Control
//
//====================================================================================================



//====================================================================================================
//
//		Input
//
//====================================================================================================

void _DlgMTPath::_KeyDownEscape()
{
	_ReleaseCheckResistPath();
	_ReleaseCurTerrainPath();
	m_list_TerrainPath.SetCurSel( -1 );
	
	PATH::Manager::GetSingletonPtr()->ResetSelected();

	UpdatePath();
	//g_ToolNiNodeMng.
}

void _DlgMTPath::_OnLButtonDown( CPoint pt )
{
	if( m_chk_ResistPath.GetCheck() == BST_UNCHECKED )
	{
		//NiPoint3 origin, dir;
		//CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );
		//CsGBPick pick( false );
		//pick.Pick(m_pNiTerrainRoot, origin, dir, true );
		//NiPick::Record* pkRecord = pick.GetRecord();
		//if( pkRecord == NULL )
		//	return;
		if( g_ToolNiNodeMng.LButtonDown( pt, PATH::Manager::GetSingletonPtr() ) == true )
			return;

		NiPick::Record* pRecordNode = PATH::Manager::GetSingletonPtr()->SnapPickingObj(pt);//pkRecord->GetAVObject();///**/nsCSGBFUNC::GetRootNiNode( pkRecord->GetAVObject() );
		if(pRecordNode && pRecordNode->GetAVObject() != NULL){
			OnPickPathObject(pRecordNode->GetAVObject());
		}
		return;
	}

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return;

	m_ptOldIntersect = pRecord->GetIntersection();

	int nType;
	if( m_rdo_Mesh4.GetCheck() == BST_CHECKED )
	{
		nType = 0;
	}
	else if( m_rdo_Mesh6.GetCheck() == BST_CHECKED )
	{
		nType = 1;
	}
	else
	{
		assert_cs( m_rdo_Mesh8.GetCheck() == BST_CHECKED );
		nType = 2;
	}

	m_pCurTerrainPath = __AddTerrainPath( nType, m_ptOldIntersect );

	m_bLButtonDown = true;

}

void _DlgMTPath::_OnLButtonUp( CPoint pt )
{
	g_ToolNiNodeMng.LButtonUp( pt , PATH::Manager::GetSingletonPtr());

	if( m_list_TerrainPath.GetCount() != m_listTerrainPath.size() )
	{
		_ResetTerrainPathList();
		m_list_TerrainPath.SetCurSel( m_list_TerrainPath.GetCount() - 1 );

		NiAVObject* pkNiGeometry = (NiAVObject*)m_list_TerrainPath.GetItemData( m_list_TerrainPath.GetCount() - 1 );
		if(pkNiGeometry != NULL)
		{
			OnLbnSelchangeListTerrainpath();
			PATH::Manager::GetSingletonPtr()->ResetSelected();
			PATH::Manager::GetSingletonPtr()->AddSelected(m_list_TerrainPath.GetCount() - 1, pkNiGeometry);

			g_ToolNiNodeMng.OnObjectSelected(PATH::Manager::GetSingletonPtr()->GetSelectedCenter(),
				PATH::Manager::GetSingletonPtr());
		}
	}
	_ReleaseCheckResistPath();
	ResetSpinControllInfo();
	m_bLButtonDown = false;
}

void _DlgMTPath::_OnMouseMove( CPoint pt )
{
	g_ToolNiNodeMng.MouseMove( pt , PATH::Manager::GetSingletonPtr());
	if( m_bLButtonDown == false )
		return;

	assert_cs( m_pCurTerrainPath != NULL );

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return;

	NiPoint3 ptCurIntersect = pRecord->GetIntersection();
	NiPoint2 ptlt, ptrb;

	if( abs( m_ptOldIntersect.x - ptCurIntersect.x ) > TERRAIN_PATH_MAX )
	{
		if( m_ptOldIntersect.x > ptCurIntersect.x )
			ptCurIntersect.x = m_ptOldIntersect.x - TERRAIN_PATH_MAX;
		else
			ptCurIntersect.x = m_ptOldIntersect.x + TERRAIN_PATH_MAX;
	}
	if( abs( m_ptOldIntersect.y - ptCurIntersect.y ) > TERRAIN_PATH_MAX )
	{
		if( m_ptOldIntersect.y > ptCurIntersect.y )
			ptCurIntersect.y = m_ptOldIntersect.y - TERRAIN_PATH_MAX;
		else
			ptCurIntersect.y = m_ptOldIntersect.y + TERRAIN_PATH_MAX;
	}
	if( abs( m_ptOldIntersect.x - ptCurIntersect.x ) < TERRAIN_PATH_MIN )
	{
		if( m_ptOldIntersect.x > ptCurIntersect.x )
			ptCurIntersect.x = m_ptOldIntersect.x - TERRAIN_PATH_MIN;
		else
			ptCurIntersect.x = m_ptOldIntersect.x + TERRAIN_PATH_MIN;
	}
	if( abs( m_ptOldIntersect.y - ptCurIntersect.y ) < TERRAIN_PATH_MIN )
	{
		if( m_ptOldIntersect.y > ptCurIntersect.y )
			ptCurIntersect.y = m_ptOldIntersect.y - TERRAIN_PATH_MIN;
		else
			ptCurIntersect.y = m_ptOldIntersect.y + TERRAIN_PATH_MIN;
	}

	if( ( m_ptOldIntersect.x - ptCurIntersect.x ) > 0.0f )
	{
		ptlt.x = ptCurIntersect.x;
		ptrb.x = m_ptOldIntersect.x;
	}
	else
	{
		ptlt.x = m_ptOldIntersect.x;
		ptrb.x = ptCurIntersect.x;
	}
	if( ( m_ptOldIntersect.y - ptCurIntersect.y ) > 0.0f )
	{
		if(m_bCreateSquare)
			ptlt.y = m_ptOldIntersect.y + (ptCurIntersect.x - m_ptOldIntersect.x) ;
		else
			ptlt.y = ptCurIntersect.y ;
		ptrb.y = m_ptOldIntersect.y;
	}
	else
	{
		ptlt.y = m_ptOldIntersect.y;
		if(m_bCreateSquare)
			ptrb.y = m_ptOldIntersect.y + (ptCurIntersect.x - m_ptOldIntersect.x) ;
		else
			ptrb.y = ptCurIntersect.y ;
	}
	
	if(m_bCreateSquare)
	{
		ResetVertexPos( m_pCurTerrainPath, ptlt, ptrb );
	}
	else
		CsGeomBox::ResetVertexPos( m_pCurTerrainPath, ptlt, ptrb );
}



void _DlgMTPath::OnBnClickedRadioMesh4()
{

}

void _DlgMTPath::OnBnClickedRadioMesh6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgMTPath::OnBnClickedRadioMesh8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

bool _DlgMTPath::_PreCheckKey( DWORD vKeyCode, bool bDown )
{
	if( vKeyCode == VK_SPACE )
	{
		if( bDown == false )
		{
			m_chk_ResistPath.SetCheck( BST_CHECKED );
			return true;
		}		
	}
	else if( vKeyCode == VK_TAB )
	{
		if( bDown == false )
		{
			if( m_rdo_Mesh4.GetCheck() == BST_CHECKED )
			{
				m_rdo_Mesh4.SetCheck( BST_UNCHECKED );
				m_rdo_Mesh6.SetCheck( BST_CHECKED );				
			}
			else if( m_rdo_Mesh6.GetCheck() == BST_CHECKED )
			{
				m_rdo_Mesh6.SetCheck( BST_UNCHECKED );
				m_rdo_Mesh8.SetCheck( BST_CHECKED );
			}
			else if( m_rdo_Mesh8.GetCheck() == BST_CHECKED )
			{
				m_rdo_Mesh8.SetCheck( BST_UNCHECKED );
				m_rdo_Mesh4.SetCheck( BST_CHECKED );
			}
			return true;
		}		
	}
	

	if( m_pCurTerrainPath == NULL )
		return false;

	if( bDown == true )
	{
		g_ToolNiNodeMng.OnKeyUp(vKeyCode, PATH::Manager::GetSingletonPtr());
		if( vKeyCode == 'W' )
		{
			//CsGeomBox::MoveDeltaPos( m_pCurTerrainPath, NiPoint2( 0, GET_MV_VIEW->_IsControl() == false? 50.0f : 10.0f ) );
			return true;
		}
		else if( vKeyCode == 'D' )
		{
			OnBnClickedButtonDelPath();
			//CsGeomBox::MoveDeltaPos( m_pCurTerrainPath, NiPoint2( GET_MV_VIEW->_IsControl() == false? 50.0f : 10.0f, 0 ) );
			return true;
		}
		else if( vKeyCode == 'S' )
		{
			//CsGeomBox::MoveDeltaPos( m_pCurTerrainPath, NiPoint2( 0, GET_MV_VIEW->_IsControl() == false? -50.0f : -10.0f ) );
			return true;
		}
		else if( vKeyCode == 'A' )
		{
			//CsGeomBox::MoveDeltaPos( m_pCurTerrainPath, NiPoint2( GET_MV_VIEW->_IsControl() == false? -50.0f : -10.0f, 0 ) );
			return true;
		}
		else if( vKeyCode == 'Q' )
		{
			//MoveCallBack_ERot( GET_MV_VIEW->_IsControl() == false ? 45.0f : 10.0f, false );
			return true;
		}
		else if( vKeyCode == 'E' )
		{
			//MoveCallBack_ERot( GET_MV_VIEW->_IsControl() == false ? -45.0f : -10.0f, false );
			return true;
		}
		else if( ( vKeyCode == VK_ADD )||( vKeyCode == '2' ) )
		{
			Scale( GET_MV_VIEW->_IsControl() == false ? 0.01f : 0.03f );
			return true;
		}
		else if( ( vKeyCode == VK_SUBTRACT )||( vKeyCode == '1' ) )
		{
			Scale( GET_MV_VIEW->_IsControl() == false ? -0.01f : -0.03f );
			return true;
		}
		else if( vKeyCode == VK_SHIFT)
		{
			m_bCreateSquare = true;
		}
	}
	else
	{

	}

	return false;
}

void _DlgMTPath::_PickSelectPath( CPoint pt )
{
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );

	CsGBPick pick( false );

	std::list< NiGeometryPtr >::iterator it = m_listTerrainPath.begin();
	for( ; it!=m_listTerrainPath.end(); ++it )
	{
		pick.Pick( m_pNiObjectRoot, origin, dir, true );
	}

	int nSel = m_list_TerrainPath.GetCurSel();
	if( nSel < 0 )
		return;

	_ReleaseCurTerrainPath();

	m_pCurTerrainPath = (NiGeometry*)m_list_TerrainPath.GetItemData( nSel );
	nsCSGBFUNC::Set_Emittance( m_pCurTerrainPath, NiColor( 1.0f, 0.0f, 0.0f ) );
}


void _DlgMTPath::OnKeyUp(const DWORD msg)
{	
	if( msg == VK_SHIFT)
	{
		m_bCreateSquare = false;
	}
	g_ToolNiNodeMng.OnKeyUp(msg, PATH::Manager::GetSingletonPtr());
}

void _DlgMTPath::ResetVertexPos( NiGeometry* pGeom, NiPoint2 ptLeftTop, NiPoint2 ptRightBottom )
{
	assert_cs( pGeom != NULL );

	NiPoint3* pPos = pGeom->GetVertices();
	switch( pGeom->GetVertexCount() )
	{
	case 8:
		{
			pPos[ 0 ].x = ptLeftTop.x;
			pPos[ 0 ].y = ptLeftTop.y;

			pPos[ 1 ].x = ptRightBottom.x;
			pPos[ 1 ].y = ptLeftTop.y;

			pPos[ 2 ].x = ptRightBottom.x;
			pPos[ 2 ].y = ptRightBottom.y;

			pPos[ 3 ].x = ptLeftTop.x;
			pPos[ 3 ].y = ptRightBottom.y;	

			pPos[ 4 ].x = ptLeftTop.x;
			pPos[ 4 ].y = ptLeftTop.y;

			pPos[ 5 ].x = ptRightBottom.x;
			pPos[ 5 ].y = ptLeftTop.y;

			pPos[ 6 ].x = ptRightBottom.x;
			pPos[ 6 ].y = ptRightBottom.y;

			pPos[ 7 ].x = ptLeftTop.x;
			pPos[ 7 ].y = ptRightBottom.y;
		}
		break;
	case 12:
		{			
			float dx = ( ptRightBottom.x - ptLeftTop.x )*0.5f;
			float dy = ( ptRightBottom.y - ptLeftTop.y )*0.5f;
			NiPoint3 kCenter = NiPoint3((ptRightBottom.x + ptLeftTop.x)*0.5f , 
				(ptRightBottom.y + ptLeftTop.y)*0.5f , 0.0f);
			NiMatrix3 kMat;
			kMat.MakeZRotation(2.0f * NI_PI / 6.0f);

			NiPoint3 kRadius = NiPoint3(dx, dy, 0.0f);

			for(int i = 0; i < 6; ++i)
			{
				kMat.MakeZRotation((2.0f * NI_PI / 6.0f) * i);
				NiPoint3 kPos = kRadius * kMat;
				pPos[i].x = kPos.x + kCenter.x;
				pPos[i].y = kPos.y + kCenter.y;
			}

			pPos[ 6 ].x = pPos[ 0 ].x;			pPos[ 6 ].y = pPos[ 0 ].y;
			pPos[ 7 ].x = pPos[ 1 ].x;			pPos[ 7 ].y = pPos[ 1 ].y;
			pPos[ 8 ].x = pPos[ 2 ].x;			pPos[ 8 ].y = pPos[ 2 ].y;
			pPos[ 9 ].x = pPos[ 3 ].x;			pPos[ 9 ].y = pPos[ 3 ].y;
			pPos[ 10 ].x = pPos[ 4 ].x;			pPos[ 10 ].y = pPos[ 4 ].y;
			pPos[ 11 ].x = pPos[ 5 ].x;			pPos[ 11 ].y = pPos[ 5 ].y;
		}
		break;
	case 16:
		{			
			float dx = ( ptRightBottom.x - ptLeftTop.x )*0.5f;
			float dy = ( ptRightBottom.y - ptLeftTop.y )*0.5f;
			float dy2 = ( ptRightBottom.y - ptLeftTop.y ) - dy*2.0f;

			NiPoint3 kCenter = NiPoint3((ptRightBottom.x + ptLeftTop.x)*0.5f , 
				(ptRightBottom.y + ptLeftTop.y)*0.5f , 0.0f);
			NiMatrix3 kMat;
			kMat.MakeZRotation(2.0f * NI_PI / 8.0f);

			NiPoint3 kRadius = NiPoint3(dx, dy, 0.0f);

			for(int i = 0; i < 8; ++i)
			{
				kMat.MakeZRotation((2.0f * NI_PI / 8.0f) * i);
				NiPoint3 kPos = kRadius * kMat;
				pPos[i].x = kPos.x + kCenter.x;
				pPos[i].y = kPos.y + kCenter.y;
			}

			pPos[ 8 ].x = pPos[ 0 ].x;			pPos[ 8 ].y = pPos[ 0 ].y;
			pPos[ 9 ].x = pPos[ 1 ].x;			pPos[ 9 ].y = pPos[ 1 ].y;
			pPos[ 10 ].x = pPos[ 2 ].x;			pPos[ 10 ].y = pPos[ 2 ].y;
			pPos[ 11 ].x = pPos[ 3 ].x;			pPos[ 11 ].y = pPos[ 3 ].y;
			pPos[ 12 ].x = pPos[ 4 ].x;			pPos[ 12 ].y = pPos[ 4 ].y;
			pPos[ 13 ].x = pPos[ 5 ].x;			pPos[ 13 ].y = pPos[ 5 ].y;
			pPos[ 14 ].x = pPos[ 6 ].x;			pPos[ 14 ].y = pPos[ 6 ].y;
			pPos[ 15 ].x = pPos[ 7 ].x;			pPos[ 15 ].y = pPos[ 7 ].y;
		}
		break;
	default:
		assert_cs( false );
	}		

	pGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pGeom->Update( 0.0f );
}
void _DlgMTPath::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	_DlgBase::OnLButtonDown(nFlags, point);
}

void _DlgMTPath::CheckingDownPathObjXSpin(float fDeltaTime)
{
	if(MovePathOBjSpin == true)
	{
		fDownAccumTime =0.0f;
		m_fDownDelta = 0.0f;
		return;
	}
	if(DownPathOBjSpin)
	{
		fDownAccumTime += fDeltaTime;
		if(fDownAccumTime > 0.5)
		{
			NiGeometry* pSrcGeom = g_pDlgPath->_GetCurPathGeom();
			if( pSrcGeom == NULL )
				return;
			NiPoint3* pPos = pSrcGeom->GetVertices();
			for( int i=0; i<pSrcGeom->GetVertexCount(); ++i )
			{
				if(eCurrMoveDIR == DIR_X)
					pPos[ i ].x += m_fDownDelta;
				else if( eCurrMoveDIR == DIR_Y)
					pPos[ i ].y += m_fDownDelta;

			}
			pSrcGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
			pSrcGeom->Update( 0.0f );
		}
	}
}


void _DlgMTPath::ResetSpinControllInfo(void)
{
	m_fDownDelta = 0.0f;
	MovePathOBjSpin = false;
	DownPathOBjSpin = false;
	eCurrMoveDIR = DIR_END;
	fDownAccumTime = 0.0f;
}

void _DlgMTPath::OnStnClickedPathcnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgMTPath::OnBnClickedButtonDelPathAll()
{
	_ReleaseCurTerrainPath();
	std::list< NiGeometryPtr >::iterator it = m_listTerrainPath.begin();
	for( ; it!=m_listTerrainPath.end(); )
	{
		{
			if( PATH::Manager::GetSingletonPtr() ){
				PATH::Manager::GetSingletonPtr()->RemovePathObj((*it));
			}
			m_pNiTerrainRoot->DetachChild( (*it) );
			(*it) = 0;
			it = m_listTerrainPath.erase( it );
		}
	}
	__ToTerrainRoot();
	_ResetTerrainPathList();			
	return;
}
