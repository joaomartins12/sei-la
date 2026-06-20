// _DlgObjectControl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgObjectControl.h"
#include "ToolNiNode/UndoManager.h"
#define ROOT_STRING				_T("Root")
#define FACTOR_OPEN_TYPE_STR	_T("OpenType")
#define FACTOR_TYPE_STR			_T("Factor")
#define FACTOR_COUNT_STR		_T("Factor Cnt")

inline void MAKE_ITEM_DATA( DWORD& dwData, BYTE type, BYTE first, BYTE second )
{
	dwData = ( type << 16 ) | ( first << 8 ) | second;
}

inline void GET_ITEM_DATA( DWORD dwData, BYTE& type, BYTE& first, BYTE& second )
{
	type	= (dwData & 0x00FF0000) >> 16;
	first	= (dwData & 0x0000FF00) >> 8;
	second	= (dwData & 0x000000FF);
}

inline BYTE GET_ITEM_TYPE( DWORD dwData )
{
	return (dwData & 0x00FF0000) >> 16;
}

inline BYTE GET_ITEM_FIRST( DWORD dwData )
{
	return (dwData & 0x0000FF00) >> 8;
}

inline BYTE GET_ITEM_SECOND( DWORD dwData )
{
	return (dwData & 0x000000FF);
}


// _DlgObjectControl 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgObjectControl, CDialog)


BEGIN_MESSAGE_MAP(_DlgObjectControl, CDialog)
	ON_EN_SETFOCUS(IDC_EDIT_TL_X, &_DlgObjectControl::OnEnSetfocusEditTlX)
	ON_EN_SETFOCUS(IDC_EDIT_TL_Y, &_DlgObjectControl::OnEnSetfocusEditTlY)
	ON_EN_SETFOCUS(IDC_EDIT_TL_Z, &_DlgObjectControl::OnEnSetfocusEditTlZ)
	ON_EN_SETFOCUS(IDC_EDIT_ROT_X, &_DlgObjectControl::OnEnSetfocusEditRotX)
	ON_EN_SETFOCUS(IDC_EDIT_ROT_Y, &_DlgObjectControl::OnEnSetfocusEditRotY)
	ON_EN_SETFOCUS(IDC_EDIT_ROT_Z, &_DlgObjectControl::OnEnSetfocusEditRotZ)	
	ON_EN_KILLFOCUS(IDC_EDIT_TL_X, &_DlgObjectControl::OnEnKillfocusEditTlX)
	ON_EN_KILLFOCUS(IDC_EDIT_TL_Y, &_DlgObjectControl::OnEnKillfocusEditTlX)
	ON_EN_KILLFOCUS(IDC_EDIT_TL_Z, &_DlgObjectControl::OnEnKillfocusEditTlX)
	ON_EN_KILLFOCUS(IDC_EDIT_ROT_X, &_DlgObjectControl::OnEnKillfocusEditTlX)
	ON_EN_KILLFOCUS(IDC_EDIT_ROT_Y, &_DlgObjectControl::OnEnKillfocusEditTlX)
	ON_EN_KILLFOCUS(IDC_EDIT_ROT_Z, &_DlgObjectControl::OnEnKillfocusEditTlX)

	ON_EN_SETFOCUS(IDC_EDIT_OBJECT_FACTOR1, &_DlgObjectControl::OnEnSetfocusEditFactor1)
	ON_EN_SETFOCUS(IDC_EDIT_OBJECT_FACTOR_CNT1, &_DlgObjectControl::OnEnSetfocusEditFactorCnt1)
	ON_NOTIFY(NM_RCLICK, IDC_OBJECT_FATOR_TREE, &_DlgObjectControl::OnNMRclickTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_OBJECT_FATOR_TREE, &_DlgObjectControl::OnTvnSelchangedTreeObject)
	ON_COMMAND(ID_POPMENU_TREEITEM_DEL, &_DlgObjectControl::OnPopmenu_TreeItemDel)

	ON_BN_CLICKED(IDC_OBJECT_FACTOR_ADD, &_DlgObjectControl::OnBnClickedBtnFactorAdd)
	ON_BN_CLICKED(IDC_OBJECT_FACTOR_CHANGE, &_DlgObjectControl::OnBnClickedBtnFactorChange)

	ON_EN_CHANGE(IDC_EDIT_ROT_X, &_DlgObjectControl::OnEnChangeEditRotX)
	ON_EN_CHANGE(IDC_EDIT_ROT_Y, &_DlgObjectControl::OnEnChangeEditRotY)
	ON_EN_CHANGE(IDC_EDIT_ROT_Z, &_DlgObjectControl::OnEnChangeEditRotZ)
	ON_EN_CHANGE(IDC_EDIT_TL_X, &_DlgObjectControl::OnEnChangeEditTlX)
	ON_EN_CHANGE(IDC_EDIT_TL_Y, &_DlgObjectControl::OnEnChangeEditTlY)
	ON_EN_CHANGE(IDC_EDIT_TL_Z, &_DlgObjectControl::OnEnChangeEditTlZ)

	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_EDIT_ORDER_NUMBER, &_DlgObjectControl::OnEnChangeEditOrderNumber)
END_MESSAGE_MAP()


_DlgObjectControl::_DlgObjectControl(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgObjectControl::IDD, pParent),
	mLastCUID(-1)
{
}

_DlgObjectControl::~_DlgObjectControl()
{
}

bool _DlgObjectControl::_IsFocus()
{
	CWnd* pWnd = GetFocus();

	if( pWnd->GetSafeHwnd() == m_nOpenType.GetSafeHwnd() ||
		pWnd->GetSafeHwnd() == m_edit_Factor.GetSafeHwnd() || 
		pWnd->GetSafeHwnd() == m_edit_FactorCnt.GetSafeHwnd() )
		return true;

	for( int i=0; i<3; ++i )
	{
		if( ( pWnd->GetSafeHwnd() == m_edit_TL[ i ].GetSafeHwnd() )||
			( pWnd->GetSafeHwnd() == m_edit_Rot[ i ].GetSafeHwnd() ) )
		{
			return true;
		}
	}
	return false;
}

void _DlgObjectControl::_SetControl( bool bPush )
{
	for( int i=0; i<3; ++i )
	{
		m_spin_Rot[ i ].SetCsAccel( bPush ? 1.0f : 0.5f );
		m_spin_TL[ i ].SetCsAccel( bPush ? 10 : 2 );
	}
}
//Dlg (Object Controller)에서 Object 이동할때
void _DlgObjectControl::__SetEditValueApplyToSelectObject()
{	
	NiTransform tr;
	tr.MakeIdentity();
	tr.m_Translate.x = _Control2float( &m_edit_TL[ 0 ] );
	tr.m_Translate.y = _Control2float( &m_edit_TL[ 1 ] );	tr.m_Translate.z = _Control2float( &m_edit_TL[ 2 ] );
	tr.m_Rotate.FromEulerAnglesZXY( CsD2R( _Control2float( &m_edit_Rot[ 2 ] ) ) , CsD2R( _Control2float( &m_edit_Rot[ 0 ] ) ),
									CsD2R( _Control2float( &m_edit_Rot[ 1 ] ) ) );

	switch( m_AxisType )
	{
	case cAxisRotation::AT_OBJECT:
		{
			g_ObjectMng.SelectModel_ApplyTransform( tr, false);
		}
		break;
	case cAxisRotation::AT_NPC:
		{
			g_ModelMng.SelectNpc_ApplyTransform( tr );
		}
		break;
	case cAxisRotation::AT_CELL:
		{
			tr.m_fScale = GET_MV_MAINTAB_DLG->_GetDlgCellObject()->_GetSelectObject()->s_trParent.m_fScale;
			GET_MV_MAINTAB_DLG->_GetDlgCellObject()->_SetSelectObject_Transform( tr );
		}
		break;
	default:
		assert_cs( false );
	}

	g_ToolNiNodeMng.GetAxisRotation()->CalTRNodeTransform();
}

void _DlgObjectControl::_ReleaseValue()
{
	GET_MV_OBJECTCONTROL_FRM->ShowWindow( SW_HIDE );
	m_dwCurrentMapID = 0;
	m_SelectedObjID = 0;
}

void _DlgObjectControl::_InsertValue( cAxisRotation::eAXIS_TYPE at, NiTransform tr )
{
	m_AxisType = at;

	GET_MV_OBJECTCONTROL_FRM->ShowWindow( SW_SHOW );	
	
	_ResetValue( tr );
}

void _DlgObjectControl::_InsertValue_Act( cObjectModel* pObjectModel )
{
	m_dwCurrentMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	__EmptyValue_Act( false );

	bool bMulti = ( (int)g_ObjectMng.GetListSelectModel()->size() == 1 ) ? false : true;
	if( pObjectModel == NULL || bMulti )
		return;

	m_SelectedObjID = pObjectModel->GetObjectInfo()->s_dwUniqID;
	_Int2Control( &m_ObjectUniqID, m_SelectedObjID );		
	
	if( !nsCsMapTable::g_pActObjectMng->IsGroup( m_dwCurrentMapID, m_SelectedObjID ) )
	{
		if( ( pObjectModel->GetObjectInfo()->s_dwUserDefinePlag & sUSER_DEFINE::UD_ACTORMANAGER ) == 0 )
			return;
	}
	__EmptyValue_Act( true );
	_ResetValue_Act( m_dwCurrentMapID, m_SelectedObjID );
}

void _DlgObjectControl::_ResetValue_Act( DWORD const& dwMapID, DWORD const& dwObjID )
{
	CsMapObjectGroup::LIST_OBJECT const* pFactorList = nsCsMapTable::g_pActObjectMng->GetFactorList( dwMapID, dwObjID );
	if( NULL == pFactorList )
		return;

	HTREEITEM hRoot = m_FactorTree.GetRootItem();

	CsMapObjectGroup::LIST_OBJECT_CIT it = pFactorList->begin();
	for( int nFirstLineNum = 0; it != pFactorList->end(); ++it, ++nFirstLineNum )
	{
		HTREEITEM hChild = NULL;

		_Int2Control( &m_edit_OrderNumber, (*it)->GetOrderNumber());

		CsMapObject::FACT_LIST const& pFact = (*it)->GetFactInfoList();
		CsMapObject::FACT_LIST_CIT itFact = pFact.begin();

		for( int nSecondLineNum = 0; itFact != pFact.end(); ++itFact, ++nSecondLineNum )
		{
			if( 0 == (*itFact).s_nOpenType )
				continue;
			CString addItem;
			addItem.Format(_T("%s : %d,	%s : %d, %s : %d"), FACTOR_OPEN_TYPE_STR, (*itFact).s_nOpenType, FACTOR_TYPE_STR, (*itFact).s_nFactor, FACTOR_COUNT_STR, (*itFact).s_nFactorCnt);
			if( nSecondLineNum == 0 )
			{
				DWORD dwItemData = 0;
				MAKE_ITEM_DATA( dwItemData, TT_FACTOR_ROOT, nFirstLineNum, nSecondLineNum );

				hChild = m_FactorTree.InsertItem( addItem.GetBuffer(), hRoot, TVI_LAST );
				m_FactorTree.SetItemData( hChild, dwItemData );
			}
			else
			{
				DWORD dwItemData = 0;
				MAKE_ITEM_DATA( dwItemData, TT_FACTOR_NODE, nFirstLineNum, nSecondLineNum );

				HTREEITEM hChildNode = m_FactorTree.InsertItem( addItem.GetBuffer(), hChild, TVI_LAST );
				m_FactorTree.SetItemData( hChildNode, dwItemData );
			}
		}

		if( hChild )
			m_FactorTree.Expand( hChild, TVE_EXPAND );
	}	
	m_FactorTree.Expand( hRoot, TVE_EXPAND );
}

void _DlgObjectControl::__EmptyValue_Act( bool bEnable )
{	
	m_FactorTree.DeleteAllItems();

	HTREEITEM hRoot = m_FactorTree.InsertItem( ROOT_STRING, TVI_ROOT, TVI_LAST );
	m_FactorTree.SetItemData( hRoot, TT_ROOT );

	m_FactorTree.EnableWindow(bEnable);
	m_FactorAddBtn.EnableWindow(bEnable);
	m_FactorChangeBtn.EnableWindow(bEnable);
	m_nOpenType.EnableWindow( bEnable );	
	m_nOpenType.SetCurSel( 0 );

	m_edit_OrderNumber.EnableWindow( bEnable );
	if( !bEnable )
		_Int2Control( &m_edit_OrderNumber, 0 );

	m_edit_Factor.EnableWindow( bEnable );
	_Int2Control( &m_edit_Factor, 0 );

	m_edit_FactorCnt.EnableWindow( bEnable );
	_Int2Control( &m_edit_FactorCnt, 0 );
}

void _DlgObjectControl::_ResetValue( NiTransform tr )
{
	float frx, fry, frz;
	tr.m_Rotate.ToEulerAnglesZXY( frz, frx, fry );

	switch( m_AxisType )
	{
	case cAxisRotation::AT_OBJECT:
		{
			_Float2Control( &m_edit_TL[ 0 ], tr.m_Translate.x );
			_Float2Control( &m_edit_TL[ 1 ], tr.m_Translate.y );
			_Float2Control( &m_edit_TL[ 2 ], tr.m_Translate.z );

			if( g_ObjectMng.GetListSelectModel()->size() == 1 )
			{
				_Float2Control( &m_edit_Rot[ 0 ], CsR2D( frx ) );
				_Float2Control( &m_edit_Rot[ 1 ], CsR2D( fry ) );
				_Float2Control( &m_edit_Rot[ 2 ], CsR2D( frz ) );
			}
			else
			{
				_Float2Control( &m_edit_Rot[ 0 ], CsR2D( frx ) );
				_Float2Control( &m_edit_Rot[ 1 ], CsR2D( fry ) );
				_Float2Control( &m_edit_Rot[ 2 ], CsR2D( frz ) );
			}

			m_edit_Rot[ 0 ].ShowWindow( SW_SHOW );
			m_edit_Rot[ 1 ].ShowWindow( SW_SHOW );
			m_spin_Rot[ 0 ].ShowWindow( SW_SHOW );
			m_spin_Rot[ 1 ].ShowWindow( SW_SHOW );
		}
		break;
	case cAxisRotation::AT_NPC:
		{
			_Float2Control( &m_edit_TL[ 0 ], tr.m_Translate.x );
			_Float2Control( &m_edit_TL[ 1 ], tr.m_Translate.y );
			_Float2Control( &m_edit_TL[ 2 ], tr.m_Translate.z );
			_Float2Control( &m_edit_Rot[ 0 ], CsR2D( frx ) );
			_Float2Control( &m_edit_Rot[ 1 ], CsR2D( fry ) );
			_Float2Control( &m_edit_Rot[ 2 ], CsR2D( frz ) );

			m_edit_Rot[ 0 ].ShowWindow( SW_HIDE );
			m_edit_Rot[ 1 ].ShowWindow( SW_HIDE );
			m_spin_Rot[ 0 ].ShowWindow( SW_HIDE );
			m_spin_Rot[ 1 ].ShowWindow( SW_HIDE );
		}
		break;
	case cAxisRotation::AT_CELL:
		{
			_Float2Control( &m_edit_TL[ 0 ], tr.m_Translate.x );
			_Float2Control( &m_edit_TL[ 1 ], tr.m_Translate.y );
			_Float2Control( &m_edit_TL[ 2 ], tr.m_Translate.z );
			_Float2Control( &m_edit_Rot[ 0 ], CsR2D( frx ) );
			_Float2Control( &m_edit_Rot[ 1 ], CsR2D( fry ) );
			_Float2Control( &m_edit_Rot[ 2 ], CsR2D( frz ) );

			m_edit_Rot[ 0 ].ShowWindow( SW_SHOW );
			m_edit_Rot[ 1 ].ShowWindow( SW_SHOW );
			m_spin_Rot[ 0 ].ShowWindow( SW_SHOW );
			m_spin_Rot[ 1 ].ShowWindow( SW_SHOW );
		}
		break;
	default:
		assert_cs( false );
	}
}

void _DlgObjectControl::__EmptyValue()
{
	_Float2Control( &m_edit_TL[ 0 ], 0.0f );
	_Float2Control( &m_edit_TL[ 1 ], 0.0f );
	_Float2Control( &m_edit_TL[ 2 ], 0.0f );
	_Float2Control( &m_edit_Rot[ 0 ], 0.0f );
	_Float2Control( &m_edit_Rot[ 1 ], 0.0f );
	_Float2Control( &m_edit_Rot[ 2 ], 0.0f );	
}

void _DlgObjectControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TL_X, m_edit_TL[ 0 ]);
	DDX_Control(pDX, IDC_EDIT_TL_Y, m_edit_TL[ 1 ]);
	DDX_Control(pDX, IDC_EDIT_TL_Z, m_edit_TL[ 2 ]);
	DDX_Control(pDX, IDC_EDIT_ROT_X, m_edit_Rot[ 0 ]);
	DDX_Control(pDX, IDC_EDIT_ROT_Y, m_edit_Rot[ 1 ]);
	DDX_Control(pDX, IDC_EDIT_ROT_Z, m_edit_Rot[ 2 ]);
	DDX_Control(pDX, IDC_SPIN_TL_X, m_spin_TL[ 0 ]);
	DDX_Control(pDX, IDC_SPIN_TL_Y, m_spin_TL[ 1 ]);
	DDX_Control(pDX, IDC_SPIN_TL_Z, m_spin_TL[ 2 ]);
	DDX_Control(pDX, IDC_SPIN_ROT_X, m_spin_Rot[ 0 ]);
	DDX_Control(pDX, IDC_SPIN_ROT_Y, m_spin_Rot[ 1 ]);
	DDX_Control(pDX, IDC_SPIN_ROT_Z, m_spin_Rot[ 2 ]);	
	DDX_Control(pDX, IDC_STATIC_OBJECT_UNIQID, m_ObjectUniqID);

	DDX_Control(pDX, IDC_EDIT_ORDER_NUMBER, m_edit_OrderNumber);
	DDX_Control(pDX, IDC_EDIT_OBJECT_FACTOR1, m_edit_Factor);
	DDX_Control(pDX, IDC_EDIT_OBJECT_FACTOR_CNT1, m_edit_FactorCnt);
	DDX_Control(pDX, IDC_COMBO_OBJECT_OPEN_TYPE1, m_nOpenType);
	DDX_Control(pDX, IDC_OBJECT_FATOR_TREE, m_FactorTree);

	DDX_Control(pDX, IDC_OBJECT_FACTOR_ADD, m_FactorAddBtn);
	DDX_Control(pDX, IDC_OBJECT_FACTOR_CHANGE, m_FactorChangeBtn);	
}

void _DlgObjectControl::OnEnSetfocusEditTlX()
{		
	m_edit_TL[ 0 ].SetSel( 0, -1, TRUE );
	switch( m_AxisType )
	{
	case cAxisRotation::AT_OBJECT:
		{
			mLastCUID = g_ObjectMng.TakeUndoSnapshot(UNDO::TRANSLATE);
		}
		break;
	case cAxisRotation::AT_NPC:
		{
		}
		break;
	case cAxisRotation::AT_CELL:
		{
			mLastCUID = CELLOBJ::Manager::GetSingletonPtr()->TakeUndoSnapshot(UNDO::CELLOBJ_TRANSLATE);
		}
		break;
	//default:
	//	assert_cs( false );
	}
}
void _DlgObjectControl::OnEnKillfocusEditTlX()
{
	switch( m_AxisType )
	{
	case cAxisRotation::AT_OBJECT:
		{
			g_ObjectMng.TakeRedoSnapshot(mLastCUID);
		}
		break;
	case cAxisRotation::AT_NPC:
		{
		}
		break;
	case cAxisRotation::AT_CELL:
		{
			CELLOBJ::Manager::GetSingletonPtr()->TakeRedoSnapshot(mLastCUID);
		}
		break;
	//default:
	//	assert_cs( false );
	}

}
void _DlgObjectControl::OnEnSetfocusEditTlY()
{		
	m_edit_TL[ 1 ].SetSel( 0, -1, TRUE );	
	switch( m_AxisType )
	{
	case cAxisRotation::AT_OBJECT:
		{
			mLastCUID = g_ObjectMng.TakeUndoSnapshot(UNDO::TRANSLATE);
		}
		break;
	case cAxisRotation::AT_NPC:
		{
		}
		break;
	case cAxisRotation::AT_CELL:
		{
			mLastCUID = CELLOBJ::Manager::GetSingletonPtr()->TakeUndoSnapshot(UNDO::CELLOBJ_TRANSLATE);
		}
		break;
	//default:
	//	assert_cs( false );
	}
}
void _DlgObjectControl::OnEnSetfocusEditTlZ()
{		
	m_edit_TL[ 2 ].SetSel( 0, -1, TRUE );	
	switch( m_AxisType )
	{
	case cAxisRotation::AT_OBJECT:
		{
			mLastCUID = g_ObjectMng.TakeUndoSnapshot(UNDO::TRANSLATE);
		}
		break;
	case cAxisRotation::AT_NPC:
		{
		}
		break;
	case cAxisRotation::AT_CELL:
		{
			mLastCUID = CELLOBJ::Manager::GetSingletonPtr()->TakeUndoSnapshot(UNDO::EDIT_TYPE::CELLOBJ_TRANSLATE);
		}
		break;
	//default:
	//	assert_cs( false );
	}
}
void _DlgObjectControl::OnEnSetfocusEditRotX()
{
	m_edit_Rot[ 0 ].SetSel( 0, -1, TRUE );	
	switch( m_AxisType )
	{
	case cAxisRotation::AT_OBJECT:
		{
			mLastCUID = g_ObjectMng.TakeUndoSnapshot(UNDO::ROTATE);
		}
		break;
	case cAxisRotation::AT_NPC:
		{
		}
		break;
	case cAxisRotation::AT_CELL:
		{
			mLastCUID = CELLOBJ::Manager::GetSingletonPtr()->TakeUndoSnapshot(UNDO::CELLOBJ_TRANSLATE);
		}
		break;
	//default:
	//	assert_cs( false );
	}

}
void _DlgObjectControl::OnEnSetfocusEditRotY()
{		
	m_edit_Rot[ 1 ].SetSel( 0, -1, TRUE );	
	switch( m_AxisType )
	{
	case cAxisRotation::AT_OBJECT:
		{
			mLastCUID = g_ObjectMng.TakeUndoSnapshot(UNDO::ROTATE);
		}
		break;
	case cAxisRotation::AT_NPC:
		{
		}
		break;
	case cAxisRotation::AT_CELL:
		{
			mLastCUID = CELLOBJ::Manager::GetSingletonPtr()->TakeUndoSnapshot(UNDO::CELLOBJ_TRANSLATE);
		}
		break;
	//default:
	//	assert_cs( false );
	}
}
void _DlgObjectControl::OnEnSetfocusEditRotZ()
{		
	m_edit_Rot[ 2 ].SetSel( 0, -1, TRUE );	
	switch( m_AxisType )
	{
	case cAxisRotation::AT_OBJECT:
		{
			mLastCUID = g_ObjectMng.TakeUndoSnapshot(UNDO::ROTATE);
		}
		break;
	case cAxisRotation::AT_NPC:
		{
		}
		break;
	case cAxisRotation::AT_CELL:
		{
			mLastCUID = CELLOBJ::Manager::GetSingletonPtr()->TakeUndoSnapshot(UNDO::CELLOBJ_TRANSLATE);
		}
		break;
	//default:
	//	assert_cs( false );
	}
}

void _DlgObjectControl::OnEnSetfocusEditFactor1(){		m_edit_Factor.SetSel( 0, -1, TRUE );	}
void _DlgObjectControl::OnEnSetfocusEditFactorCnt1(){	m_edit_FactorCnt.SetSel( 0, -1, TRUE );	}

BOOL _DlgObjectControl::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	for( int i=0; i<3; ++i )
	{
		m_spin_Rot[ i ].SetCsBuddy( &m_edit_Rot[ i ], 0.5f, 2 );
		m_spin_TL[ i ].SetCsBuddy( &m_edit_TL[ i ], 0.5f, 2 );
	}

	m_nOpenType.InsertString( 0, L"타입선택" );
	m_nOpenType.SetItemData( 0, AT_NONE );
	
	m_nOpenType.InsertString( 1, L"몬스터킬(1)" );
	m_nOpenType.SetItemData( 1, AT_MONSTER );
	
	m_nOpenType.InsertString( 2, L"퀘스트완료(2)" );
	m_nOpenType.SetItemData( 2, AT_QUEST_COMPLETE );

	m_nOpenType.InsertString( 3, L"서버컨트롤(3)" );
	m_nOpenType.SetItemData( 3, AT_SERVER_CONTROL );
	
	m_nOpenType.SetCurSel( AT_NONE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgObjectControl::OnEnChangeEditRotX()
{
	if( _IsFocus() == true )
		__SetEditValueApplyToSelectObject();
}

void _DlgObjectControl::OnEnChangeEditRotY()
{
	if( _IsFocus() == true )
		__SetEditValueApplyToSelectObject();
}

void _DlgObjectControl::OnEnChangeEditRotZ()
{
	if( _IsFocus() == true )
		__SetEditValueApplyToSelectObject();
}

void _DlgObjectControl::OnEnChangeEditTlX()
{
	if( _IsFocus() == true )
		__SetEditValueApplyToSelectObject();
}

void _DlgObjectControl::OnEnChangeEditTlY()
{
	if( _IsFocus() == true )
		__SetEditValueApplyToSelectObject();
}

void _DlgObjectControl::OnEnChangeEditTlZ()
{
	if( _IsFocus() == true )
		__SetEditValueApplyToSelectObject();
}

// 트리에서 선택 변경
void _DlgObjectControl::OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
	if( hSelItem == NULL )
		return;

	DWORD dwItemData = m_FactorTree.GetItemData( hSelItem );	
	BYTE byType = GET_ITEM_TYPE( dwItemData );
	switch( byType )
	{
	case eTREE_TYPE::TT_ROOT:
		m_FactorChangeBtn.EnableWindow(FALSE);
		return;
	case eTREE_TYPE::TT_FACTOR_ROOT:
		m_FactorAddBtn.EnableWindow(TRUE);
		m_FactorChangeBtn.EnableWindow(TRUE);
		break;
	case eTREE_TYPE::TT_FACTOR_NODE:
		m_FactorAddBtn.EnableWindow(FALSE);
		m_FactorChangeBtn.EnableWindow(TRUE);
		break;
	}

	BYTE byFt1 = GET_ITEM_FIRST( dwItemData );
	BYTE byFt2 = GET_ITEM_SECOND( dwItemData );

	CsMapObject::sFACTOR_INFO const* pFactorInfo = nsCsMapTable::g_pActObjectMng->GetFactorInfo( m_dwCurrentMapID, m_SelectedObjID, byFt1, byFt2 );
	if( pFactorInfo )
	{
		m_nOpenType.SetCurSel( pFactorInfo->s_nOpenType );		
		_Int2Control( &m_edit_Factor, pFactorInfo->s_nFactor );
		_Int2Control( &m_edit_FactorCnt, pFactorInfo->s_nFactorCnt );
	}
	else
	{
		m_nOpenType.SetCurSel( 0 );
		_Int2Control( &m_edit_Factor, 0 );
		_Int2Control( &m_edit_FactorCnt, 0 );
	}
}

void _DlgObjectControl::OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CPoint ptInTree, ptInSrceen;

	GetCursorPos(&ptInSrceen); //R버튼 눌린좌표얻음
	ptInTree = ptInSrceen;

	m_FactorTree.ScreenToClient(&ptInTree);       //트리컨트롤 좌표로변환
	//좌표에 해당하는 아이템을 자져옴
	HTREEITEM hit = m_FactorTree.HitTest(ptInTree);    
	if( NULL == hit )
		return;

	m_FactorTree.SelectItem(hit);    //그 놈 선택

	DWORD dwItemData = m_FactorTree.GetItemData( hit );	
	BYTE byType = GET_ITEM_TYPE( dwItemData );
	if( TT_ROOT == byType )
		return;

	CMenu menu, *pMenu;
	menu.LoadMenu(IDR_MENU1);
	pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, ptInSrceen.x, ptInSrceen.y, this);
}

void _DlgObjectControl::OnPopmenu_TreeItemDel()
{
	HTREEITEM hSelItem = m_FactorTree.GetSelectedItem();
	if( NULL == hSelItem )
		return;

	DWORD dwItemData = m_FactorTree.GetItemData( hSelItem );	
	BYTE byType = GET_ITEM_TYPE( dwItemData );

	// Root노드는 삭제하면 안된다.
	switch( byType )
	{
	case TT_ROOT:
		break;
	case TT_FACTOR_ROOT:
		{
			BYTE byFactor1 = GET_ITEM_FIRST( dwItemData );
			nsCsMapTable::g_pActObjectMng->DeleteActorFactor1( m_dwCurrentMapID, m_SelectedObjID, byFactor1 );
			__EmptyValue_Act( true );
			_ResetValue_Act( m_dwCurrentMapID, m_SelectedObjID );
		}		
		break;
	case TT_FACTOR_NODE:
		{
			BYTE byFactor1 = GET_ITEM_FIRST( dwItemData );
			BYTE byFactor2 = GET_ITEM_SECOND( dwItemData );
			nsCsMapTable::g_pActObjectMng->DeleteActorFactor2( m_dwCurrentMapID, m_SelectedObjID, byFactor1, byFactor2 );
			__EmptyValue_Act( true );
			_ResetValue_Act( m_dwCurrentMapID, m_SelectedObjID );
		}
		break;
	}
}

// Factor Add Button Click
void _DlgObjectControl::OnBnClickedBtnFactorAdd()
{
	HTREEITEM hSelItem = m_FactorTree.GetSelectedItem();
	if( NULL == hSelItem )
		return;

	DWORD dwData = m_FactorTree.GetItemData( hSelItem );
	BYTE byType = GET_ITEM_TYPE( dwData );
	if( TT_FACTOR_NODE == byType )
		return;

	CsMapObject::sFACTOR_INFO addInfo;
	addInfo.s_nOpenType = m_nOpenType.GetCurSel();
	if( eACTOBJ_TYPE::AT_NONE == addInfo.s_nOpenType )
		return;

	addInfo.s_nFactor = nsCsMfcFunc::Control2Int( &m_edit_Factor );
// 	if( 0 == addInfo.s_nFactor )
// 		return;

	addInfo.s_nFactorCnt = nsCsMfcFunc::Control2Int( &m_edit_FactorCnt );
// 	if( 0 == addInfo.s_nFactorCnt )
// 		return;

	if( TT_ROOT == byType )
		nsCsMapTable::g_pActObjectMng->AddActorOrFactor( m_dwCurrentMapID, m_SelectedObjID, addInfo );
	else if( TT_FACTOR_ROOT == byType )
	{
		BYTE byFactor1 = GET_ITEM_FIRST( dwData );
		nsCsMapTable::g_pActObjectMng->AddActorAndFactor( m_dwCurrentMapID, m_SelectedObjID, byFactor1, addInfo );
	}

	__EmptyValue_Act( true );
	_ResetValue_Act( m_dwCurrentMapID, m_SelectedObjID );
}

// Factor Change Button Click
void _DlgObjectControl::OnBnClickedBtnFactorChange()
{
	HTREEITEM hSelItem = m_FactorTree.GetSelectedItem();
	if( NULL == hSelItem )
		return;

	DWORD dwData = m_FactorTree.GetItemData( hSelItem );
	BYTE byType = GET_ITEM_TYPE( dwData );

	// Root 노드는 데이터 변경이 안된다.
	if( TT_ROOT == byType )
		return;

	CsMapObject::sFACTOR_INFO addInfo;
	addInfo.s_nOpenType = m_nOpenType.GetCurSel();
	if( eACTOBJ_TYPE::AT_NONE == addInfo.s_nOpenType )
		return;

	addInfo.s_nFactor = nsCsMfcFunc::Control2Int( &m_edit_Factor );
	if( 0 == addInfo.s_nFactor )
		return;

	addInfo.s_nFactorCnt = nsCsMfcFunc::Control2Int( &m_edit_FactorCnt );
	if( 0 == addInfo.s_nFactorCnt )
		return;

	BYTE byFactor1 = GET_ITEM_FIRST( dwData );
	if( TT_FACTOR_ROOT == byType )
		nsCsMapTable::g_pActObjectMng->ChangeActorOrFactor( m_dwCurrentMapID, m_SelectedObjID, byFactor1, addInfo );
	else if( TT_FACTOR_NODE == byType )
	{
		BYTE byFactor2 = GET_ITEM_SECOND( dwData );
		nsCsMapTable::g_pActObjectMng->ChangeActorAndFactor( m_dwCurrentMapID, m_SelectedObjID, byFactor1, byFactor2, addInfo );
	}

	__EmptyValue_Act( true );
	_ResetValue_Act( m_dwCurrentMapID, m_SelectedObjID );
}

void _DlgObjectControl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		_SetControl( false );
	}
}

void _DlgObjectControl::OnEnChangeEditOrderNumber()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// _DlgBase::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( 0 == m_dwCurrentMapID || m_SelectedObjID == 0 )
		return;

	int nOrderNum = nsCsMfcFunc::Control2Int( &m_edit_OrderNumber );
	nsCsMapTable::g_pActObjectMng->ChangeOrderNumber( m_dwCurrentMapID, m_SelectedObjID, nOrderNum);
}
