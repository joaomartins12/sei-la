// _DlgMTCellObject.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTCellObject.h"
#include "ToolNiNode/UndoManager.h"

// _DlgMTCellObject 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTCellObject, CDialog)

_DlgMTCellObject::_DlgMTCellObject(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTCellObject::IDD, pParent)
{
	__ReleaseSelectObject();
}

_DlgMTCellObject::~_DlgMTCellObject()
{
}

void _DlgMTCellObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CELLOBJECT, m_list_CellObject);
	DDX_Control(pDX, IDC_CHECK_RANDOM_SCALE, m_chk_RandomScale);
	DDX_Control(pDX, IDC_CHECK_RANDOM_XROT, m_chk_RandomXRot);
	DDX_Control(pDX, IDC_CHECK_RANDOM_YROT, m_chk_RandomYRot);
	DDX_Control(pDX, IDC_CHECK_RANDOM_ZROT, m_chk_RandomZRot);
	DDX_Control(pDX, IDC_CHECK_RANDOM_ZTRANS, m_chk_RandomZTrans);
	DDX_Control(pDX, IDC_EDIT_SCALE, m_edit_Scale);
	DDX_Control(pDX, IDC_SPIN_SCALE, m_spin_Scale);
	DDX_Control(pDX, IDC_EDIT_RANDOM_SCALE_MIN, m_edit_RScaleMin);
	DDX_Control(pDX, IDC_SPIN_RANDOM_SCALE_MIN, m_spin_RScaleMin);
	DDX_Control(pDX, IDC_EDIT_RANDOM_SCALE_MAX, m_edit_RScaleMax);
	DDX_Control(pDX, IDC_SPIN_RANDOM_SCALE_MAX, m_spin_RScaleMax);
	DDX_Control(pDX, IDC_EDIT_RANDOM_XROT_MIN, m_edit_XRotMin);
	DDX_Control(pDX, IDC_SPIN_RANDOM_XROT_MIN, m_spin_XRotMin);
	DDX_Control(pDX, IDC_EDIT_RANDOM_XROT_MAX, m_edit_XRotMax);
	DDX_Control(pDX, IDC_SPIN_RANDOM_XROT_MAX, m_spin_XRotMax);
	DDX_Control(pDX, IDC_EDIT_RANDOM_YROT_MIN, m_edit_YRotMin);
	DDX_Control(pDX, IDC_SPIN_RANDOM_YROT_MIN, m_spin_YRotMin);
	DDX_Control(pDX, IDC_EDIT_RANDOM_YROT_MAX, m_edit_YRotMax);
	DDX_Control(pDX, IDC_SPIN_RANDOM_YROT_MAX, m_spin_YRotMax);
	DDX_Control(pDX, IDC_EDIT_RANDOM_ZROT_MIN, m_edit_ZRotMin);
	DDX_Control(pDX, IDC_SPIN_RANDOM_ZROT_MIN, m_spin_ZRotMin);
	DDX_Control(pDX, IDC_EDIT_RANDOM_ZROT_MAX, m_edit_ZRotMax);
	DDX_Control(pDX, IDC_SPIN_RANDOM_ZROT_MAX, m_spin_ZRotMax);
	DDX_Control(pDX, IDC_EDIT_RANDOM_ZTRANS_MIN, m_edit_ZTransMin);
	DDX_Control(pDX, IDC_SPIN_RANDOM_ZTRANS_MIN, m_spin_ZTransMin);
	DDX_Control(pDX, IDC_EDIT_RANDOM_ZTRANS_MAX, m_edit_ZTransMax);
	DDX_Control(pDX, IDC_SPIN_RANDOM_ZTRANS_MAX, m_spin_ZTransMax);
	DDX_Control(pDX, IDC_EDIT_GROUP_COUNT, m_edit_GroupCount);
	DDX_Control(pDX, IDC_CHECK_GROUP, m_chk_Group);
	DDX_Control(pDX, IDC_SLIDER_GROUP_RADIUS, m_sld_GroupRadius);
	DDX_Control(pDX, IDC_EDIT_GROUP_RADIUS, m_edit_GroupRadius);
	DDX_Control(pDX, IDC_CHECK_GROUP_DELETE, m_chk_GroupDelete);
	DDX_Control(pDX, IDC_EDIT_CHECK_TIME_MIN, m_edit_CheckTime_Min);
	DDX_Control(pDX, IDC_SPIN_CHECK_TIME_MIN, m_spin_CheckTime_Min);
	DDX_Control(pDX, IDC_EDIT_CHECK_TIME_MAX, m_edit_CheckTime_Max);
	DDX_Control(pDX, IDC_SPIN_CHECK_TIME_MAX, m_spin_CheckTime_Max);
	DDX_Control(pDX, IDC_EDIT_ANI_SPEED_MIN, m_edit_AniSpeed_Min);
	DDX_Control(pDX, IDC_SPIN_ANI_SPEED_MIN, m_spin_AniSpeed_Min);
	DDX_Control(pDX, IDC_EDIT_ANI_SPEED_MAX, m_edit_AniSpeed_Max);
	DDX_Control(pDX, IDC_SPIN_ANI_SPEED_MAX, m_spin_AniSpeed_Max);
	DDX_Control(pDX, IDC_CHECK_GROUP_COPY, m_chk_GroupCopy);
	DDX_Control(pDX, IDC_CHECK_GROUP_PASTE, m_chk_GroupPaste);
	DDX_Control(pDX, IDC_CHECK_TERRAIN_NORMAL, m_UseTerrainNormal);


}


BEGIN_MESSAGE_MAP(_DlgMTCellObject, CDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_OBJECT, &_DlgMTCellObject::OnBnClickedBtnAddObject)

	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_RANDOM_SCALE, &_DlgMTCellObject::OnBnClickedCheckRandomScale)
	ON_EN_CHANGE(IDC_EDIT_SCALE, &_DlgMTCellObject::OnEnChangeEditScale)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_SCALE_MIN, &_DlgMTCellObject::OnEnChangeEditRandomScaleMin)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_SCALE_MAX, &_DlgMTCellObject::OnEnChangeEditRandomScaleMax)
	ON_BN_CLICKED(IDC_CHECK_RANDOM_XROT, &_DlgMTCellObject::OnBnClickedCheckRandomXrot)
	ON_BN_CLICKED(IDC_CHECK_RANDOM_YROT, &_DlgMTCellObject::OnBnClickedCheckRandomYrot)
	ON_BN_CLICKED(IDC_CHECK_RANDOM_ZROT, &_DlgMTCellObject::OnBnClickedCheckRandomZrot)
	ON_BN_CLICKED(IDC_CHECK_RANDOM_ZTRANS, &_DlgMTCellObject::OnBnClickedCheckRandomZtrans)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_XROT_MIN, &_DlgMTCellObject::OnEnChangeEditRandomXrotMin)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_XROT_MAX, &_DlgMTCellObject::OnEnChangeEditRandomXrotMax)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_YROT_MIN, &_DlgMTCellObject::OnEnChangeEditRandomYrotMin)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_YROT_MAX, &_DlgMTCellObject::OnEnChangeEditRandomYrotMax)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_ZROT_MIN, &_DlgMTCellObject::OnEnChangeEditRandomZrotMin)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_ZROT_MAX, &_DlgMTCellObject::OnEnChangeEditRandomZrotMax)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_ZTRANS_MIN, &_DlgMTCellObject::OnEnChangeEditRandomZtransMin)
	ON_EN_CHANGE(IDC_EDIT_RANDOM_ZTRANS_MAX, &_DlgMTCellObject::OnEnChangeEditRandomZtransMax)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CELLOBJECT, &_DlgMTCellObject::OnNMClickListCellobject)
	ON_BN_CLICKED(IDC_CHECK_GROUP, &_DlgMTCellObject::OnBnClickedCheckGroup)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_DELETE, &_DlgMTCellObject::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_CHECK_GROUP_DELETE, &_DlgMTCellObject::OnBnClickedCheckGroupDelete)
	ON_EN_CHANGE(IDC_EDIT_CHECK_TIME_MIN, &_DlgMTCellObject::OnEnChangeEditCheckTimeMin)
	ON_EN_CHANGE(IDC_EDIT_CHECK_TIME_MAX, &_DlgMTCellObject::OnEnChangeEditCheckTimeMax)
	ON_EN_CHANGE(IDC_EDIT_ANI_SPEED_MIN, &_DlgMTCellObject::OnEnChangeEditAniSpeedMin)
	ON_EN_CHANGE(IDC_EDIT_ANI_SPEED_MAX, &_DlgMTCellObject::OnEnChangeEditAniSpeedMax)
	ON_BN_CLICKED(IDC_CHECK_GROUP_COPY, &_DlgMTCellObject::OnBnClickedCheckGroupCopy)
	ON_BN_CLICKED(IDC_CHECK_GROUP_PASTE, &_DlgMTCellObject::OnBnClickedCheckGroupPaste)
END_MESSAGE_MAP()

BOOL _DlgMTCellObject::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	DWORD dwExStyle = m_list_CellObject.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES;
	m_list_CellObject.SetExtendedStyle( dwExStyle );
	m_list_CellObject.InsertColumn( 0, L"이름", LVCFMT_RIGHT, 150 );
	m_list_CellObject.InsertColumn( 1, L"경로", LVCFMT_RIGHT, 370 );
	m_list_CellObject.InsertColumn( 2, L"갯수", LVCFMT_CENTER, 70 );

	m_sld_GroupRadius.SetRange( 100, 30000 );

	nsCsMfcFunc::Int2Control( &m_edit_GroupCount, 100 );
	m_sld_GroupRadius.SetPos( 1000 );
	nsCsMfcFunc::Float2Control( &m_edit_GroupRadius, 10.0f );

	m_nSelectBaseIndex = -1;

	m_spin_RScaleMin.SetCsBuddy( &m_edit_RScaleMin, 0.002f, 3 );
	m_spin_RScaleMax.SetCsBuddy( &m_edit_RScaleMax, 0.002f, 3 );
	nsCsMfcFunc::Float2Control( &m_edit_RScaleMin, 0.9f, 3 );
	nsCsMfcFunc::Float2Control( &m_edit_RScaleMax, 1.1f, 3 );

	m_spin_XRotMin.SetCsBuddy( &m_edit_XRotMin, 0.2f, 2 );
	m_spin_XRotMax.SetCsBuddy( &m_edit_XRotMax, 0.2f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_XRotMin, 0, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_XRotMax, 0.1f, 2 );

	m_spin_YRotMin.SetCsBuddy( &m_edit_YRotMin, 0.2f, 2 );
	m_spin_YRotMax.SetCsBuddy( &m_edit_YRotMax, 0.2f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_YRotMin, 0, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_YRotMax, 0.1f, 2 );

	m_spin_ZRotMin.SetCsBuddy( &m_edit_ZRotMin, 0.2f, 2 );
	m_spin_ZRotMax.SetCsBuddy( &m_edit_ZRotMax, 0.2f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_ZRotMin, 0, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_ZRotMax, 360, 2 );

	m_spin_ZTransMin.SetCsBuddy( &m_edit_ZTransMin, 1.0f, 1 );
	m_spin_ZTransMax.SetCsBuddy( &m_edit_ZTransMax, 1.0f, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_ZTransMin, 0, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_ZTransMax, 0.1f, 1 );

	m_spin_Scale.SetCsBuddy( &m_edit_Scale, 0.002f, 3 );
	nsCsMfcFunc::Float2Control( &m_edit_Scale, 1.0f, 3 );

	m_spin_CheckTime_Min.SetCsBuddy( &m_edit_CheckTime_Min, 0.01f, 2 );
	m_spin_CheckTime_Max.SetCsBuddy( &m_edit_CheckTime_Max, 0.01f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_CheckTime_Min, 2.0f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_CheckTime_Max, 5.0f, 2 );

	m_spin_AniSpeed_Min.SetCsBuddy( &m_edit_AniSpeed_Min, 0.01f, 2 );
	m_spin_AniSpeed_Max.SetCsBuddy( &m_edit_AniSpeed_Max, 0.01f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_AniSpeed_Min, 0.2f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_AniSpeed_Max, 1.2f, 2 );

	m_edit_Scale.EnableWindow( FALSE );
	m_spin_Scale.EnableWindow( FALSE );
	m_edit_CheckTime_Min.EnableWindow( FALSE );
	m_edit_CheckTime_Max.EnableWindow( FALSE );
	m_edit_AniSpeed_Min.EnableWindow( FALSE );
	m_edit_AniSpeed_Min.EnableWindow( FALSE );

	m_edit_RScaleMin.EnableWindow( FALSE );
	m_edit_RScaleMax.EnableWindow( FALSE );
	m_spin_RScaleMin.EnableWindow( FALSE );
	m_spin_RScaleMax.EnableWindow( FALSE );

	m_edit_XRotMin.EnableWindow( FALSE );
	m_edit_XRotMax.EnableWindow( FALSE );
	m_spin_XRotMin.EnableWindow( FALSE );
	m_spin_XRotMax.EnableWindow( FALSE );

	m_edit_YRotMin.EnableWindow( FALSE );
	m_edit_YRotMax.EnableWindow( FALSE );
	m_spin_YRotMin.EnableWindow( FALSE );
	m_spin_YRotMax.EnableWindow( FALSE );

	m_edit_ZRotMin.EnableWindow( FALSE );
	m_edit_ZRotMax.EnableWindow( FALSE );
	m_spin_ZRotMin.EnableWindow( FALSE );
	m_spin_ZRotMax.EnableWindow( FALSE );

	m_edit_ZTransMin.EnableWindow( FALSE );
	m_edit_ZTransMax.EnableWindow( FALSE );
	m_spin_ZTransMin.EnableWindow( FALSE );
	m_spin_ZTransMax.EnableWindow( FALSE );



	return TRUE;  // return TRUE unless you set the focus to a control
}


void _DlgMTCellObject::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);	

	if( bShow == FALSE )
	{
		m_chk_Group.SetCheck( BST_UNCHECKED );
		m_chk_Group.SetCheck( BST_UNCHECKED );
		__ReleaseSelectObject();
	}
	else
	{
		if( m_vpPasteInfo.Size() == 0 )
		{
			m_chk_GroupPaste.EnableWindow( FALSE );
		}
		g_ToolNiNodeMng.OnStartObjectEdit(0);
	}
	UNDO::Manager::GetSingletonPtr()->Reset();
}


// _DlgMTCellObject 메시지 처리기입니다.

void _DlgMTCellObject::_PostViewReset()
{
	m_vpPasteInfo.ClearElement();
	m_chk_GroupPaste.EnableWindow( FALSE );

	__ReleaseSelectObject();

	m_nSelectBaseIndex = -1;
	__ResetListInfo();
}

void _DlgMTCellObject::_UpdateView()
{
	if( IsClientRect() == false )
		m_bRenderSelectObject = false;

	if( m_pNiSelectObject != NULL )
		m_pNiSelectObject->Update( 0.0f );
}

void _DlgMTCellObject::_PostRenderView()
{
	if( ( m_chk_Group.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupDelete.GetCheck() == BST_UNCHECKED )&&
		( m_chk_GroupCopy.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupPaste.GetCheck() == BST_UNCHECKED ) )
	{
		if( m_bRenderSelectObject == false )
			return;

		if( m_pNiSelectObject != NULL )
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_pNiSelectObject, *g_pCuller );
	}
	else if( IsClientRect() == true )
	{
		{
			NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), m_ptMouse.x, m_ptMouse.y );		
			if( pRecord != NULL )
			{
				NiPoint2 pt = NiPoint2( pRecord->GetIntersection().x, pRecord->GetIntersection().y );

				NiColor color = NiColor( 1, 0, 0 );
				if( m_chk_GroupDelete.GetCheck() == BST_CHECKED )
				{
					color = NiColor( 1, 0, 0 );
				}
				else if( m_chk_GroupCopy.GetCheck() == BST_CHECKED )
				{
					color = NiColor( 0, 0, 1 );
				}
				else if( m_chk_GroupPaste.GetCheck() == BST_CHECKED )
				{
					color = NiColor( 0, 1, 0 );
				}

				g_ToolNiNodeMng.GetRegion()->Render( pt, (float)m_sld_GroupRadius.GetPos(), color );
			}
		}		
	}
}
void _DlgMTCellObject::OnKeyUp(const DWORD msg)
{
	g_ToolNiNodeMng.OnKeyUp(msg,  CELLOBJ::Manager::GetSingletonPtr());
}
void _DlgMTCellObject::_OnLButtonUp( CPoint pt )
{
	g_ToolNiNodeMng.LButtonUp( pt ,CELLOBJ::Manager::GetSingletonPtr());
}
void _DlgMTCellObject::_OnLButtonDown( CPoint pt )
{

	if( m_chk_Group.GetCheck() == BST_CHECKED )
	{
		__GroupResist( pt );
	}
	else if( m_chk_GroupDelete.GetCheck() == BST_CHECKED )
	{
		__GroupDelete( pt );
	}
	else if( m_chk_GroupCopy.GetCheck() == BST_CHECKED )
	{
		__GroupCopy( pt );
	}
	else if( m_chk_GroupPaste.GetCheck() == BST_CHECKED )
	{
		__GroupPaste( pt );
	}
	else
	{
		if( m_bRenderSelectObject == true )
		{
			__ResistObject();
		}
		else
		{
			if(g_ToolNiNodeMng.LButtonDown( pt , CELLOBJ::Manager::GetSingletonPtr()) == false)
			{
				__PickObject( pt );
			}
		}
	}
}

void _DlgMTCellObject::_OnMouseMove( CPoint pt )
{
	g_ToolNiNodeMng.MouseMove( pt , CELLOBJ::Manager::GetSingletonPtr());
	m_ptMouse = pt;

	if(m_pSelectCellObject)
		GET_MV_OBJECTCONTROL_DLG->_ResetValue( m_pSelectCellObject->s_trParent );
	
	if( m_pNiSelectObject == NULL )
		return;	

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
	{
		m_bRenderSelectObject = false;
		return;
	}

	NiPoint3 pos = pRecord->GetIntersection();
	pos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( pos );
	m_bRenderSelectObject = true;
	m_pNiSelectObject->SetTranslate( pos );
}

void _DlgMTCellObject::_OnMouseWheel( short zDelta )
{
	_OnMouseMove( m_ptMouse );
}

void _DlgMTCellObject::_KeyDownEscape()
{
	if( m_chk_Group.GetCheck() == BST_CHECKED )
	{
		m_chk_Group.SetCheck( BST_UNCHECKED );
	}
	else if( m_chk_GroupDelete.GetCheck() == BST_CHECKED )
	{
		m_chk_GroupDelete.SetCheck( BST_UNCHECKED );
	}
	else if( m_chk_GroupCopy.GetCheck() == BST_CHECKED )
	{
		m_chk_GroupCopy.SetCheck( BST_UNCHECKED );
	}
	else if( m_chk_GroupPaste.GetCheck() == BST_CHECKED )
	{
		m_chk_GroupPaste.SetCheck( BST_UNCHECKED );
	}
	else
	{
		__ReleaseSelectObject();
		CELLOBJ::Manager::GetSingletonPtr()->ResetSelected();
	}
}

void _DlgMTCellObject::_OnSlide_SLeft()
{
	if( ( m_chk_Group.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupDelete.GetCheck() == BST_UNCHECKED )&&
		( m_chk_GroupCopy.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupPaste.GetCheck() == BST_UNCHECKED ) )
		return;

	int nMin = m_sld_GroupRadius.GetRangeMin();
	int nPos = m_sld_GroupRadius.GetPos() - 10;

	if( nMin > nPos )
		nPos = nMin;

	m_sld_GroupRadius.SetPos( nPos );
	nsCsMfcFunc::Float2Control( &m_edit_GroupRadius, nPos*0.01f );
}

void _DlgMTCellObject::_OnSlide_SRight()
{
	if( ( m_chk_Group.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupDelete.GetCheck() == BST_UNCHECKED )&&
		( m_chk_GroupCopy.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupPaste.GetCheck() == BST_UNCHECKED ) )
		return;

	int nMax = m_sld_GroupRadius.GetRangeMax();
	int nPos = m_sld_GroupRadius.GetPos() + 10;

	if( nMax < nPos )
		nPos = nMax;

	m_sld_GroupRadius.SetPos( nPos );
	nsCsMfcFunc::Float2Control( &m_edit_GroupRadius, nPos*0.01f );
}

void _DlgMTCellObject::_OnSlide_BLeft()
{
	if( ( m_chk_Group.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupDelete.GetCheck() == BST_UNCHECKED )&&
		( m_chk_GroupCopy.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupPaste.GetCheck() == BST_UNCHECKED ) )
		return;

	int nMin = m_sld_GroupRadius.GetRangeMin();
	int nPos = m_sld_GroupRadius.GetPos() - 100;

	if( nMin > nPos )
		nPos = nMin;

	m_sld_GroupRadius.SetPos( nPos );
	nsCsMfcFunc::Float2Control( &m_edit_GroupRadius, nPos*0.01f );
}

void _DlgMTCellObject::_OnSlide_BRight()
{
	if( ( m_chk_Group.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupDelete.GetCheck() == BST_UNCHECKED )&&
		( m_chk_GroupCopy.GetCheck() == BST_UNCHECKED )&&( m_chk_GroupPaste.GetCheck() == BST_UNCHECKED ) )
		return;

	int nMax = m_sld_GroupRadius.GetRangeMax();
	int nPos = m_sld_GroupRadius.GetPos() + 100;

	if( nMax < nPos )
		nPos = nMax;

	m_sld_GroupRadius.SetPos( nPos );
	nsCsMfcFunc::Float2Control( &m_edit_GroupRadius, nPos*0.01f );
}

bool _DlgMTCellObject::_IsFocus()
{
	CWnd* pWnd = GetFocus();

	if( pWnd->GetSafeHwnd() == m_edit_Scale.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_Scale ) < 0.002f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_Scale, 0.002f, 3 );
		}
		return true;
	}

	else if( pWnd->GetSafeHwnd() == m_edit_CheckTime_Min.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Min ) < 0.5f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_CheckTime_Min, 0.5f, 2 );
		}
		if( nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Min ) >= nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Max ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_CheckTime_Min, nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Max )-0.1f, 2 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_CheckTime_Max.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Max ) < 0.5f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_CheckTime_Max, 0.5f, 2 );
		}
		if( nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Max ) <= nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Min ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_CheckTime_Max, nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Min )+0.1f, 2 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_AniSpeed_Min.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Min ) < 0.01f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_AniSpeed_Min, 0.01f, 2 );
		}
		if( nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Min ) >= nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Max ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_AniSpeed_Min, nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Max )-0.1f, 2 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_AniSpeed_Max.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Max ) < 0.02f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_AniSpeed_Max, 0.02f, 2 );
		}
		if( nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Max ) <= nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Min ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_AniSpeed_Max, nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Min )+0.1f, 2 );
		}
		return true;
	}

	else if( pWnd->GetSafeHwnd() == m_edit_RScaleMin.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_RScaleMin ) < 0.002f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_RScaleMin, 0.002f, 3 );
		}
		if( nsCsMfcFunc::Control2Float( &m_edit_RScaleMin ) >= nsCsMfcFunc::Control2Float( &m_edit_RScaleMax ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_RScaleMin, nsCsMfcFunc::Control2Float( &m_edit_RScaleMax )-0.002f, 3 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_RScaleMax.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_RScaleMax ) < 0.002f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_RScaleMax, 0.002f, 3 );
		}
		if( nsCsMfcFunc::Control2Float( &m_edit_RScaleMax ) <= nsCsMfcFunc::Control2Float( &m_edit_RScaleMin ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_RScaleMax, nsCsMfcFunc::Control2Float( &m_edit_RScaleMin )+0.002f, 3 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_XRotMin.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_XRotMin ) > nsCsMfcFunc::Control2Float( &m_edit_XRotMax ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_XRotMin, nsCsMfcFunc::Control2Float( &m_edit_XRotMax )-1, 2 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_XRotMax.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_XRotMax ) < nsCsMfcFunc::Control2Float( &m_edit_XRotMin ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_XRotMax, nsCsMfcFunc::Control2Float( &m_edit_XRotMin )+1, 2 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_YRotMin.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_YRotMin ) > nsCsMfcFunc::Control2Float( &m_edit_YRotMax ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_YRotMin, nsCsMfcFunc::Control2Float( &m_edit_YRotMax )-1, 2 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_YRotMax.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_YRotMax ) < nsCsMfcFunc::Control2Float( &m_edit_YRotMin ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_YRotMax, nsCsMfcFunc::Control2Float( &m_edit_YRotMin )+1, 2 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_ZRotMin.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_ZRotMin ) > nsCsMfcFunc::Control2Float( &m_edit_ZRotMax ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_ZRotMin, nsCsMfcFunc::Control2Float( &m_edit_ZRotMax )-1, 2 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_ZRotMax.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_ZRotMax ) < nsCsMfcFunc::Control2Float( &m_edit_ZRotMin ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_ZRotMax, nsCsMfcFunc::Control2Float( &m_edit_ZRotMin )+1, 2 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_ZTransMin.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_ZTransMin ) > nsCsMfcFunc::Control2Float( &m_edit_ZTransMax ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_ZTransMin, nsCsMfcFunc::Control2Float( &m_edit_ZTransMax )-1, 1 );
		}
		return true;
	}
	else if( pWnd->GetSafeHwnd() == m_edit_ZTransMax.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_ZTransMax ) < nsCsMfcFunc::Control2Float( &m_edit_ZTransMin ) )
		{
			nsCsMfcFunc::Float2Control( &m_edit_ZTransMax, nsCsMfcFunc::Control2Float( &m_edit_ZTransMin )+1, 1 );
		}
		return true;
	}

	return false;
}


// =================================================================================================
//
//		Control
//
// =================================================================================================
void _DlgMTCellObject::OnBnClickedBtnAddObject()
{
	if( g_TempRoot.IsLoad() == true )
		return;
	
#define MAP_OBJECT_PATH		L".\\Data\\Map"

	// 폴더가 존재 하지 않는다면 폴더 생성	
	nsCSFILE::CreateDirectory( MAP_OBJECT_PATH );
	
	if( _taccess_s( g_Resist.GetObject()->s_szObjectPath, 0 ) != 0 )
		g_Resist.GetObject()->Reset();

	TCHAR szOut[ MAX_PATH ];
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Nif File(*.nif)\0*.nif\0", g_Resist.GetObject()->s_szObjectPath ) == false )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"오브젝트는 현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	TCHAR szRootPath[ MAX_PATH ] = {0, };
	TCHAR szRootFullPath[ MAX_PATH ] = {0, };
	nsCSFILE::GetRelativePath( szRootFullPath, MAX_PATH, szOut );
	nsCSFILE::GetFilePathOnly( szRootPath, MAX_PATH, szRootFullPath );

	__InsertCellObject( szRootFullPath );

	_tcscpy_s( g_Resist.GetObject()->s_szObjectPath, MAX_PATH, szRootPath );
}


void _DlgMTCellObject::OnBnClickedBtnDelete()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	// 선택되어 있는 항목 백업
	POSITION pos = m_list_CellObject.GetFirstSelectedItemPosition();
	int nSelectItem = m_list_CellObject.GetNextSelectedItem( pos );

	if( nSelectItem < 0 )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"설치된 해당 오브젝트도 전부 삭제 됩니다. 그래도 제거 하시겠습니까?" ) == IDCANCEL )
		return;

	nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->DeleteBaseObject( nSelectItem );

	__ReleaseSelectObject( true );

	m_list_CellObject.DeleteItem( nSelectItem );
	__ResetListInfo();
}


void _DlgMTCellObject::__InsertCellObject( LPCTSTR szPath )
{
	// 같은게 있는지 찾는다
	int nComboCount = m_list_CellObject.GetItemCount();
	TCHAR szTemp[ OBJECT_PATH_LEN ];
	for( int i=0; i<nComboCount; ++i )
	{
		m_list_CellObject.GetItemText( i, 0, szTemp, OBJECT_PATH_LEN );
		if( _tcsicmp( szTemp, szPath ) == 0 )
			return;
	}

	char cPaht[ OBJECT_PATH_LEN ];
	W2M( cPaht, szPath, OBJECT_PATH_LEN );
	nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->AddBaseObject( nComboCount, cPaht );

	__ResetListInfo();
}

void _DlgMTCellObject::__ResetListInfo()
{
	// 체크되어 있는 항목들 백업
	std::map< int, BOOL > mapCheck;
	int nItemCount = m_list_CellObject.GetItemCount();
	for( int i=0; i<nItemCount; ++i )
	{
		mapCheck[ i ] = m_list_CellObject.GetCheck( i );
	}
	// 선택되어 있는 항목 백업
	POSITION pos = m_list_CellObject.GetFirstSelectedItemPosition();
	int nSelectItem = m_list_CellObject.GetNextSelectedItem( pos );	

	m_list_CellObject.DeleteAllItems();
	int nObjectKind = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObjectKind();
	int nCellCount = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCellTotalCount();
	TCHAR szPath[ OBJECT_PATH_LEN ];
	for( int i=0; i<nObjectKind; ++i )
	{
		M2W( szPath, nsCSFILE::GetFileName(nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( i )->s_ObjectInfo.s_cPath),OBJECT_PATH_LEN );
		m_list_CellObject.InsertItem( i, szPath );
		M2W( szPath, nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( i )->s_ObjectInfo.s_cPath, OBJECT_PATH_LEN );
		m_list_CellObject.SetItem( i, 1, LVIF_TEXT, szPath, 0, 0, 0, 0 );
		//m_list_CellObject.InsertItem( i, szPath );

		int nCount = 0;
		for( int c=0; c<nCellCount; ++c )
		{
			nCount += nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( c )->GetSize( i );
		}
		_stprintf_s( szPath, OBJECT_PATH_LEN, L"%d", nCount );
		m_list_CellObject.SetItem( i, 2, LVIF_TEXT, szPath, 0, 0, 0, 0 );
		m_list_CellObject.SetCheck( i, mapCheck[ i ] );
	}

	m_list_CellObject.SetSelectionMark( nSelectItem );
	m_list_CellObject.SetItemState( nSelectItem, LVIS_SELECTED, LVIS_SELECTED );

	mapCheck.clear();
}

void _DlgMTCellObject::OnNMClickListCellobject(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( g_TempRoot.IsLoad() == true )
		return;

	POSITION pos = m_list_CellObject.GetFirstSelectedItemPosition();
	m_nSelectBaseIndex = m_list_CellObject.GetNextSelectedItem( pos );
	if( m_nSelectBaseIndex < 0 )
	{
		__ReleaseSelectObject();
		g_Preview.GetTObj()->ReleaseModelView();
		return;
	}

	TCHAR szPath[ OBJECT_PATH_LEN ];
	m_list_CellObject.GetItemText( m_nSelectBaseIndex, 1, szPath, OBJECT_PATH_LEN );	
	g_Preview.GetTObj()->SetModelView( NULL, szPath );

	m_pSelectCellObject = NULL;
	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();
	__SetSelectObject( nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex )->s_ObjectInfo.s_cPath );	

	*pResult = 0;
}

// =================================================================================================
//
//		Object
//
// =================================================================================================

void _DlgMTCellObject::__PickObject( CPoint pt )
{
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );

	CsGBPick pick( false, NiPick::NO_SORT, false );
	int nKind = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObjectKind();
	int nCellCount = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCellTotalCount();
	CsGBCell* pCell;
	CsGBCell::sINFO* pCellInfo;
	for( int i=0; i<nKind; ++i )
	{
		NiNode* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( i )->s_Node.m_pNiNode;
		for( int c=0; c<nCellCount; ++c )
		{
			pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( c );
			int nObjectCount = pCell->GetSize( i );
			for( int o=0; o<nObjectCount; ++o )
			{
				pCellInfo = pCell->GetInfo( i, o );
				NiTransform kPrev = pBaseObject->GetWorldTransform();

				pBaseObject->SetLocalTransform( pCellInfo->s_trParent );
				pBaseObject->Update( 0.0f, false );
				if( pick.Pick( pBaseObject, origin, dir, false ) == true )
				{
					m_nSelectBaseIndex = i;
					m_pSelectCellObject = pCellInfo;
					m_edit_Scale.EnableWindow( TRUE );
					m_spin_Scale.EnableWindow( TRUE );
					nsCsMfcFunc::Float2Control( &m_edit_Scale, m_pSelectCellObject->s_trParent.m_fScale, 3 );

					CELLOBJ::Manager::GetSingletonPtr()->ResetSelected();
					CELLOBJ::INFO kInfo;
					kInfo.Kind = i;
					kInfo.ObjectIdx = o;
					kInfo.CellIdx = c;
					CELLOBJ::Manager::GetSingletonPtr()->AddSelected(i, kInfo);		

					g_ToolNiNodeMng.OnObjectSelected(CELLOBJ::Manager::GetSingletonPtr()->GetSelectedCenter(),
						CELLOBJ::Manager::GetSingletonPtr());


					GET_MV_OBJECTCONTROL_DLG->_InsertValue( cAxisRotation::AT_CELL, m_pSelectCellObject->s_trParent );
					return;
				}
				else
				{
					pBaseObject->SetLocalTransform( kPrev );
					pBaseObject->Update( 0.0f, false );

				}
			}
		}
	}
}

void _DlgMTCellObject::__ResistObject()
{
	NiTransform trParent = m_pNiSelectObject->GetWorldTransform();
	NiTransform trOrg = trParent;

	if( m_chk_RandomScale.GetCheck() == BST_CHECKED )
	{
		int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_RScaleMin )*100.0f );
		int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_RScaleMax )*100.0f );
		float fScale = ( ( rand()*rand() )%(nMax-nMin) + nMin )*0.01f;
		trParent.m_fScale = fScale;
	}
	if( m_chk_RandomXRot.GetCheck() == BST_CHECKED )
	{
		int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_XRotMin )*100.0f );
		int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_XRotMax )*100.0f );
		float fRot = CsD2R( ( ( rand()*rand() )%( nMax - nMin ) + nMin )*0.01f );

		float x, y, z;
		trParent.m_Rotate.ToEulerAnglesZXY( z, x, y );
		x = fRot;
		trParent.m_Rotate.FromEulerAnglesZXY( z, x, y );
	}
	if( m_chk_RandomYRot.GetCheck() == BST_CHECKED )
	{
		int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_YRotMin )*100.0f );
		int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_YRotMax )*100.0f );
		float fRot = CsD2R( ( ( rand()*rand() )%( nMax - nMin ) + nMin )*0.01f );

		float x, y, z;
		trParent.m_Rotate.ToEulerAnglesZXY( z, x, y );
		y = fRot;
		trParent.m_Rotate.FromEulerAnglesZXY( z, x, y );
	}
	if( m_chk_RandomZRot.GetCheck() == BST_CHECKED )
	{
		int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_ZRotMin )*100.0f );
		int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_ZRotMax )*100.0f );
		float fRot = CsD2R( ( ( rand()*rand() )%( nMax - nMin ) + nMin )*0.01f );

		float x, y, z;
		trParent.m_Rotate.ToEulerAnglesZXY( z, x, y );
		z = fRot;
		trParent.m_Rotate.FromEulerAnglesZXY( z, x, y );
	}
	if( m_chk_RandomZTrans.GetCheck() == BST_CHECKED )
	{
		int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_ZTransMin )*10.0f );
		int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_ZTransMax )*10.0f );
		float fHeight = ( ( rand()*rand() )%( nMax - nMin ) + nMin )*0.1f;

		trParent.m_Translate.z += fHeight;
	}

	m_pNiSelectObject->SetLocalTransform( trParent );
	m_pNiSelectObject->Update( 0.0f, false );

	NiAVObject* pAvObject = m_pNiSelectObject;
	while( 1 )
	{
		assert_cs( NiIsKindOf( NiNode, pAvObject ) == true )
			pAvObject = ( (NiNode*)pAvObject )->GetAt( 0 );
		if( NiIsKindOf( NiGeometry, pAvObject ) == true )
			break;
	}

	NiTransform trObject = pAvObject->GetWorldTransform();
	m_pSelectCellObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->ResistObject( m_nSelectBaseIndex, &trParent, &trObject );

	g_RollBack.Begin();
	cRollBack::sRT_CELL_RESIST_INFO info;
	info.s_nBaseIndex = m_nSelectBaseIndex;
	info.s_Info.s_trParent = trParent;
	info.s_Info.s_trObject = trObject;
	g_RollBack.Push( &info );
	g_RollBack.End();

	if( CELLOBJ::Manager::GetSingletonPtr() )
	{
		int CUID = CELLOBJ::Manager::GetSingletonPtr()->TakeUndoSnapshot(UNDO::CELLOBJ_CREATE);
		CELLOBJ::Manager::GetSingletonPtr()->TakeRedoSnapshotCellRedo(CUID);
	}
	g_ToolNiNodeMng.UpdateGizmoPos(CELLOBJ::Manager::GetSingletonPtr());


	if( GET_MV_VIEW->_IsShift() == false )
	{
		NiNode* pNode = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex )->s_Node.m_pNiNode;
		pNode->SetLocalTransform( trParent );
		pNode->Update( 0.0f, false );
		g_ToolNiNodeMng.GetAxisRotation()->InsertVisible( pNode, cAxisRotation::AT_CELL );

		__ReleaseSelectObject( false );
	}
	else
	{
		m_pNiSelectObject->SetLocalTransform( trOrg );
		m_pNiSelectObject->Update( 0.0f, false );
		m_pSelectCellObject = NULL;
	}	

	__ResetListInfo();
}

void _DlgMTCellObject::__ResistObject( int nBaseIndex, NiTransform tr, bool bUseRandom , NiPoint3 kNormal)
{
	CsNodeObj* pNodeObj = &nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( nBaseIndex )->s_Node;
	pNodeObj->m_pNiNode->SetLocalTransform( tr );
	pNodeObj->m_pNiNode->Update( 0.0f, false );

	NiTransform trParent = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( nBaseIndex )->s_Node.m_pNiNode->GetWorldTransform();
	NiTransform trOrg = trParent;

	if( bUseRandom == true )
	{
		if( m_UseTerrainNormal.GetCheck() == BST_CHECKED )
		{
			LookAt(pNodeObj->m_pNiNode, kNormal);
			if( m_chk_RandomScale.GetCheck() == BST_CHECKED )
			{
				int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_RScaleMin )*100.0f );
				int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_RScaleMax )*100.0f );
				float fScale = ( ( rand()*rand() )%(nMax-nMin) + nMin )*0.01f;
				NiTransform tr, trDest, trResult;
				tr.MakeIdentity();
				tr.m_fScale = fScale;

				trDest = pNodeObj->m_pNiNode->GetWorldTransform();
				trDest.m_Translate -= pNodeObj->m_pNiNode->GetTranslate();
				trResult = tr*trDest;
				trResult.m_Translate += pNodeObj->m_pNiNode->GetTranslate();
				pNodeObj->m_pNiNode->SetLocalTransform(trResult);
				pNodeObj->m_pNiNode->Update(0.0f);
			}

			if( m_chk_RandomXRot.GetCheck() == BST_CHECKED )
			{
				float fMinDeg =  ( nsCsMfcFunc::Control2Float( &m_edit_XRotMin ) );
				float fMaxDeg = ( nsCsMfcFunc::Control2Float( &m_edit_XRotMax ) );

				float fMinRad = (fMinDeg * NI_PI ) / 180.0f;
				float fMaxRad = (fMaxDeg * NI_PI ) / 180.0f;

				float fRand = NiSymmetricRandom();
				float FinalRad = fMinRad + (fMaxRad - fMinRad) * fRand;

				NiPoint3 kRight = GetWorldDirection(pNodeObj->m_pNiNode);
				NiMatrix3 kRot;
				kRot.MakeRotation(FinalRad,  kRight);

				NiTransform tr, trDest, trResult;
				tr.MakeIdentity();
				tr.m_Rotate = kRot;
				
				trDest = pNodeObj->m_pNiNode->GetWorldTransform();
				trDest.m_Translate -= pNodeObj->m_pNiNode->GetTranslate();
				trResult = tr*trDest;
				trResult.m_Translate += pNodeObj->m_pNiNode->GetTranslate();
				pNodeObj->m_pNiNode->SetLocalTransform(trResult);
				pNodeObj->m_pNiNode->Update(0.0f);
			}
			if( m_chk_RandomYRot.GetCheck() == BST_CHECKED )
			{
				float fMinDeg =  ( nsCsMfcFunc::Control2Float( &m_edit_YRotMin ) );
				float fMaxDeg = ( nsCsMfcFunc::Control2Float( &m_edit_YRotMax ) );

				float fMinRad = (fMinDeg * NI_PI ) / 180.0f;
				float fMaxRad = (fMaxDeg * NI_PI ) / 180.0f;

				float fRand = NiSymmetricRandom();
				float FinalRad = fMinRad + (fMaxRad - fMinRad) * fRand;

				NiMatrix3 kRot;
				NiPoint3 kModel = GetWorldUpVector(pNodeObj->m_pNiNode);
				kRot.MakeRotation(FinalRad, kModel );

				NiTransform tr, trDest, trResult;
				tr.MakeIdentity();
				tr.m_Rotate = kRot;

				trDest = pNodeObj->m_pNiNode->GetWorldTransform();
				trDest.m_Translate -= pNodeObj->m_pNiNode->GetTranslate();
				trResult = tr*trDest;
				trResult.m_Translate += pNodeObj->m_pNiNode->GetTranslate();
				pNodeObj->m_pNiNode->SetLocalTransform(trResult);
				pNodeObj->m_pNiNode->Update(0.0f);
			}
			if( m_chk_RandomZRot.GetCheck() == BST_CHECKED )
			{
				float fMinDeg =  ( nsCsMfcFunc::Control2Float( &m_edit_ZRotMin ) );
				float fMaxDeg = ( nsCsMfcFunc::Control2Float( &m_edit_ZRotMax ) );

				float fMinRad = (fMinDeg * NI_PI ) / 180.0f;
				float fMaxRad = (fMaxDeg * NI_PI ) / 180.0f;

				float fRand = NiSymmetricRandom();
				float FinalRad = fMinRad + (fMaxRad - fMinRad) * fRand;

				NiMatrix3 kRot;
				NiPoint3 kModel = GetWorldRightVector(pNodeObj->m_pNiNode);
				kRot.MakeRotation(FinalRad, kModel );

				NiTransform tr, trDest, trResult;
				tr.MakeIdentity();
				tr.m_Rotate = kRot;

				trDest = pNodeObj->m_pNiNode->GetWorldTransform();
				trDest.m_Translate -= pNodeObj->m_pNiNode->GetTranslate();
				trResult = tr*trDest;
				trResult.m_Translate += pNodeObj->m_pNiNode->GetTranslate();
				pNodeObj->m_pNiNode->SetLocalTransform(trResult);
				pNodeObj->m_pNiNode->Update(0.0f);
			}

			//LookAt(pNodeObj->m_pNiNode, kFinalNormal);
						
			NiAVObject* pAvObject = pNodeObj->m_pNiNode;
			NiTransform trParent = pAvObject->GetLocalTransform();
			NiTransform trObject = pNodeObj->m_vpGeom[ 0 ]->m_pGeometry->GetWorldTransform();
			nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->ResistObject( nBaseIndex, &trParent, &trObject );

			cRollBack::sRT_CELL_RESIST_INFO info;
			info.s_nBaseIndex = nBaseIndex;
			info.s_Info.s_trParent = trParent;
			info.s_Info.s_trObject = trObject;
			g_RollBack.Push( &info );
			return;
		}
		else
		{

			if( m_chk_RandomScale.GetCheck() == BST_CHECKED )
			{
				int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_RScaleMin )*100.0f );
				int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_RScaleMax )*100.0f );
				float fScale = ( ( rand()*rand() )%(nMax-nMin) + nMin )*0.01f;
				trParent.m_fScale = fScale;
			}
			if( m_chk_RandomXRot.GetCheck() == BST_CHECKED )
			{
				int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_XRotMin )*100.0f );
				int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_XRotMax )*100.0f );
				int Value = ( nMax - nMin ) + nMin ;
				float fRot =0.0f;
				if(Value != 0 )
					fRot = CsD2R( ( ( rand()*rand() )%Value )*0.01f );

				float x, y, z;
				trParent.m_Rotate.ToEulerAnglesZXY( z, x, y );
				x = fRot;
				trParent.m_Rotate.FromEulerAnglesZXY( z, x, y );
			}
			if( m_chk_RandomYRot.GetCheck() == BST_CHECKED )
			{
				int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_YRotMin )*100.0f );
				int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_YRotMax )*100.0f );
				int Value = ( nMax - nMin ) + nMin ;
				float fRot =0.0f;
				if(Value != 0 )
					fRot = CsD2R( ( ( rand()*rand() )%Value )*0.01f );

				float x, y, z;
				trParent.m_Rotate.ToEulerAnglesZXY( z, x, y );
				y = fRot;
				trParent.m_Rotate.FromEulerAnglesZXY( z, x, y );
			}
			if( m_chk_RandomZRot.GetCheck() == BST_CHECKED )
			{
				int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_ZRotMin )*100.0f );
				int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_ZRotMax )*100.0f );
				int Value = ( nMax - nMin ) + nMin ;
				float fRot  = 0.0f;
				if(Value != 0 )
					fRot = CsD2R( ( ( rand()*rand() )% Value )*0.01f );
				float x, y, z;
				trParent.m_Rotate.ToEulerAnglesZXY( z, x, y );
				z = fRot;
				trParent.m_Rotate.FromEulerAnglesZXY( z, x, y );
			}
			if( m_chk_RandomZTrans.GetCheck() == BST_CHECKED )
			{
				int nMin = (int)( nsCsMfcFunc::Control2Float( &m_edit_ZTransMin )*10.0f );
				int nMax = (int)( nsCsMfcFunc::Control2Float( &m_edit_ZTransMax )*10.0f );
				float fHeight = ( ( rand()*rand() )%( nMax - nMin ) + nMin )*0.1f;

				trParent.m_Translate.z += fHeight;
			}
		}
	}	
	pNodeObj->m_pNiNode->SetLocalTransform( trParent );
	pNodeObj->m_pNiNode->Update( 0.0f, false );

	NiAVObject* pAvObject = pNodeObj->m_vpGeom[ 0 ]->m_pGeometry;	
	NiTransform trObject = pAvObject->GetWorldTransform();
	nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->ResistObject( nBaseIndex, &trParent, &trObject );

	cRollBack::sRT_CELL_RESIST_INFO info;
	info.s_nBaseIndex = nBaseIndex;
	info.s_Info.s_trParent = trParent;
	info.s_Info.s_trObject = trObject;
	g_RollBack.Push( &info );
}

void _DlgMTCellObject::__SetSelectObject( char* cPath )
{
	NiStream stream;
	stream.Load( cPath );
	m_pNiSelectObject = (NiNode*)stream.GetObjectAt( 0 );

	nsCSGBFUNC::SetMaterial_Alpha_Emittance( m_pNiSelectObject, 0.5f );
	m_bRenderSelectObject = false;

	m_edit_Scale.EnableWindow( TRUE );
	m_spin_Scale.EnableWindow( TRUE );
	if( m_pSelectCellObject != NULL )
	{		
		nsCsMfcFunc::Float2Control( &m_edit_Scale, m_pSelectCellObject->s_trParent.m_fScale, 3 );
	}
	else
	{
		nsCsMfcFunc::Float2Control( &m_edit_Scale, m_pNiSelectObject->GetWorldTransform().m_fScale, 3 );
	}

	m_edit_CheckTime_Min.EnableWindow( TRUE );
	m_spin_CheckTime_Min.EnableWindow( TRUE );
	m_edit_CheckTime_Max.EnableWindow( TRUE );
	m_spin_CheckTime_Max.EnableWindow( TRUE );
	m_edit_AniSpeed_Min.EnableWindow( TRUE );
	m_spin_AniSpeed_Min.EnableWindow( TRUE );
	m_edit_AniSpeed_Max.EnableWindow( TRUE );
	m_spin_AniSpeed_Max.EnableWindow( TRUE );
	CsGBSpeedCell::sOBJECT* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex );
	nsCsMfcFunc::Float2Control( &m_edit_CheckTime_Min, pBaseObject->s_ObjectInfo.s_fCheckTimeMin );
	nsCsMfcFunc::Float2Control( &m_edit_CheckTime_Max, pBaseObject->s_ObjectInfo.s_fCheckTimeMax );
	nsCsMfcFunc::Float2Control( &m_edit_AniSpeed_Min, pBaseObject->s_ObjectInfo.s_fAniSpeedMin );
	nsCsMfcFunc::Float2Control( &m_edit_AniSpeed_Max, pBaseObject->s_ObjectInfo.s_fAniSpeedMax );
}

NiNode* _DlgMTCellObject::_GetSelectBaseObject()
{
	assert_cs( m_nSelectBaseIndex != -1 );
	return nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex )->s_Node.m_pNiNode;	
}

NiAVObject* _DlgMTCellObject::_GetSelectLeafObject()
{
	NiAVObject* pAvObject = _GetSelectBaseObject();
	while( 1 )
	{
		assert_cs( NiIsKindOf( NiNode, pAvObject ) == true )
			pAvObject = ( (NiNode*)pAvObject )->GetAt( 0 );
		if( NiIsKindOf( NiGeometry, pAvObject ) == true )
			break;
	}	
	return pAvObject;
}

void _DlgMTCellObject::_SetSelectObject_Transform( NiTransform tr )
{
	assert_cs( m_nSelectBaseIndex != -1 );
	NiNode* pNode = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex )->s_Node.m_pNiNode;
	pNode->SetLocalTransform( tr );
	pNode->Update( 0.0f, false );

	NiAVObject* pAvObject = pNode;
	while( 1 )
	{
		assert_cs( NiIsKindOf( NiNode, pAvObject ) == true )
			pAvObject = ( (NiNode*)pAvObject )->GetAt( 0 );
		if( NiIsKindOf( NiGeometry, pAvObject ) == true )
			break;
	}

	m_pSelectCellObject->s_trParent = tr;
	m_pSelectCellObject->s_trObject = pAvObject->GetWorldTransform();
	g_ToolNiNodeMng.UpdateGizmoPos(CELLOBJ::Manager::GetSingletonPtr());
}

void _DlgMTCellObject::__ReleaseSelectObject( bool bReleaseObjectControl /*=true*/ )
{
	if( bReleaseObjectControl == true )
	{
		m_pSelectCellObject = NULL;
		m_nSelectBaseIndex = -1;
		g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();

		if( m_edit_Scale.GetSafeHwnd() != NULL )
		{
			m_edit_Scale.EnableWindow( FALSE );
			m_spin_Scale.EnableWindow( FALSE );

			m_edit_CheckTime_Min.EnableWindow( FALSE );
			m_spin_CheckTime_Min.EnableWindow( FALSE );
			m_edit_CheckTime_Max.EnableWindow( FALSE );
			m_spin_CheckTime_Max.EnableWindow( FALSE );
			m_edit_AniSpeed_Min.EnableWindow( FALSE );
			m_spin_AniSpeed_Min.EnableWindow( FALSE );
			m_edit_AniSpeed_Max.EnableWindow( FALSE );
			m_spin_AniSpeed_Max.EnableWindow( FALSE );
		}		
	}

	m_pNiSelectObject = 0;
	m_bRenderSelectObject = false;
	g_Preview.GetTObj()->ReleaseModelView();	
	
	if( m_list_CellObject.GetSafeHwnd() != NULL)
	{		
		m_list_CellObject.SetSelectionMark( -1 );
		POSITION pos = m_list_CellObject.GetFirstSelectedItemPosition();
		int nSelectItem = m_list_CellObject.GetNextSelectedItem( pos );
		if( nSelectItem >= 0 )
		{
			m_list_CellObject.SetItemState( nSelectItem, 0, LVIS_SELECTED );
		}
	}
}

void _DlgMTCellObject::_Copy()
{
	if( m_pSelectCellObject == NULL )
		return;

	TCHAR szPath[ OBJECT_PATH_LEN ];
	m_list_CellObject.GetItemText( m_nSelectBaseIndex, 1, szPath, OBJECT_PATH_LEN );
	g_Preview.GetTObj()->SetModelView( NULL, szPath );
	__SetSelectObject( nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex )->s_ObjectInfo.s_cPath );
	m_pNiSelectObject->SetLocalTransform( m_pSelectCellObject->s_trParent );
	m_pNiSelectObject->Update( 0.0f );

	m_pSelectCellObject = NULL;
	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();	
}

void _DlgMTCellObject::_Cut()
{
	if( m_pSelectCellObject == NULL )
		return;

	TCHAR szPath[ OBJECT_PATH_LEN ];
	m_list_CellObject.GetItemText( m_nSelectBaseIndex, 1, szPath, OBJECT_PATH_LEN );
	g_Preview.GetTObj()->SetModelView( NULL, szPath );
	__SetSelectObject( nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex )->s_ObjectInfo.s_cPath );
	m_pNiSelectObject->SetLocalTransform( m_pSelectCellObject->s_trParent );
	m_pNiSelectObject->Update( 0.0f );

	int nKind = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObjectKind();
	int nCellCount = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCellTotalCount();
	CsGBCell* pCell;
	for( int c=0; c<nCellCount; ++c )
	{
		pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( c );
		int nObjectCount = pCell->GetSize( m_nSelectBaseIndex );
		for( int o=0; o<nObjectCount; ++o )
		{
			if( m_pSelectCellObject == pCell->GetInfo( m_nSelectBaseIndex, o ) )
			{
				CsGBCell::sINFO* pCellInfo = pCell->GetInfo( m_nSelectBaseIndex, o );

				g_RollBack.Begin();
				cRollBack::sRT_CELL_DELETE_INFO info;
				info.s_nBaseIndex = m_nSelectBaseIndex;
				info.s_Info.s_trParent = pCellInfo->s_trParent;
				info.s_Info.s_trObject = pCellInfo->s_trObject;
				g_RollBack.Push( &info );
				g_RollBack.End();

				pCell->DeleteObject( m_nSelectBaseIndex, o );

				m_pSelectCellObject = NULL;
				g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();
				__ResetListInfo();
				return;
			}
		}
	}

	assert_cs( false );
}

void _DlgMTCellObject::_Del()
{
	if( m_pSelectCellObject == NULL )
		return;

	int nKind = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObjectKind();
	int nCellCount = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCellTotalCount();
	CsGBCell* pCell;

	for( int c=0; c<nCellCount; ++c )
	{
		pCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetCell( c );
		int nObjectCount = pCell->GetSize( m_nSelectBaseIndex );
		for( int o=0; o<nObjectCount; ++o )
		{
			if( m_pSelectCellObject == pCell->GetInfo( m_nSelectBaseIndex, o ) )
			{
				CsGBCell::sINFO* pCellInfo = pCell->GetInfo( m_nSelectBaseIndex, o );

				g_RollBack.Begin();
				cRollBack::sRT_CELL_DELETE_INFO info;
				info.s_nBaseIndex = m_nSelectBaseIndex;
				info.s_Info.s_trParent = pCellInfo->s_trParent;
				info.s_Info.s_trObject = pCellInfo->s_trObject;
				g_RollBack.Push( &info );
				g_RollBack.End();

				pCell->DeleteObject( m_nSelectBaseIndex, o );
				__ReleaseSelectObject();
				__ResetListInfo();
				return;
			}
		}
	}

	assert_cs( false );
}

void _DlgMTCellObject::_Rot( float fRot )
{
	if( m_pSelectCellObject != NULL )
	{
		float x, y, z;
		m_pSelectCellObject->s_trParent.m_Rotate.ToEulerAnglesZXY( z, x, y );
		z += CsD2R( fRot );
		m_pSelectCellObject->s_trParent.m_Rotate.FromEulerAnglesZXY( z, x, y );

		NiTransform trResult = m_pSelectCellObject->s_trParent;
		_SetSelectObject_Transform( trResult );
		GET_MV_OBJECTCONTROL_DLG->_ResetValue( trResult );
	}
	else if( m_pNiSelectObject != NULL )
	{
		NiTransform tr;
		tr.MakeIdentity();
		tr.m_Rotate.MakeZRotation( CsD2R( fRot ) );

		float x, y, z;
		m_pNiSelectObject->GetWorldRotate().ToEulerAnglesZXY( z, x, y );
		z += CsD2R( fRot );
		NiMatrix3 rot;
		rot.FromEulerAnglesZXY( z, x, y );
		m_pNiSelectObject->SetRotate( rot );
	}
}

// =================================================================================================
//
//		스케일
//
// =================================================================================================

void _DlgMTCellObject::OnEnChangeEditScale()
{
	if( _IsFocus() == true )
	{
		float fScale = nsCsMfcFunc::Control2Float( &m_edit_Scale );
		if( m_pSelectCellObject != NULL )
		{
			m_pSelectCellObject->s_trParent.m_fScale = fScale;
			_SetSelectObject_Transform( m_pSelectCellObject->s_trParent );
		}
		else if( m_pNiSelectObject != NULL )
		{
			m_pNiSelectObject->SetScale( fScale );
		}
	}	
}

// =================================================================================================
//
//		랜덤 스케일
//
// =================================================================================================
void _DlgMTCellObject::OnBnClickedCheckRandomScale()
{
	bool bEnableWindow = FALSE;
	if( m_chk_RandomScale.GetCheck() == BST_CHECKED )
	{
		bEnableWindow = TRUE;
	}

	m_edit_RScaleMin.EnableWindow( bEnableWindow );
	m_edit_RScaleMax.EnableWindow( bEnableWindow );
	m_spin_RScaleMin.EnableWindow( bEnableWindow );
	m_spin_RScaleMax.EnableWindow( bEnableWindow );
}



void _DlgMTCellObject::OnEnChangeEditRandomScaleMin()
{
	if( _IsFocus() == true )
	{

	}
}

void _DlgMTCellObject::OnEnChangeEditRandomScaleMax()
{
	if( _IsFocus() == true )
	{

	}
}

void _DlgMTCellObject::OnBnClickedCheckRandomXrot()
{
	bool bEnableWindow = FALSE;
	if( m_chk_RandomXRot.GetCheck() == BST_CHECKED )
	{
		bEnableWindow = TRUE;
	}

	m_edit_XRotMin.EnableWindow( bEnableWindow );
	m_edit_XRotMax.EnableWindow( bEnableWindow );
	m_spin_XRotMin.EnableWindow( bEnableWindow );
	m_spin_XRotMax.EnableWindow( bEnableWindow );
}

void _DlgMTCellObject::OnEnChangeEditRandomXrotMin()
{
	if( _IsFocus() == true )
	{

	}
}

void _DlgMTCellObject::OnEnChangeEditRandomXrotMax()
{
	if( _IsFocus() == true )
	{

	}
}

void _DlgMTCellObject::OnBnClickedCheckRandomYrot()
{
	bool bEnableWindow = FALSE;
	if( m_chk_RandomYRot.GetCheck() == BST_CHECKED )
	{
		bEnableWindow = TRUE;
	}

	m_edit_YRotMin.EnableWindow( bEnableWindow );
	m_edit_YRotMax.EnableWindow( bEnableWindow );
	m_spin_YRotMin.EnableWindow( bEnableWindow );
	m_spin_YRotMax.EnableWindow( bEnableWindow );
}


void _DlgMTCellObject::OnEnChangeEditRandomYrotMin()
{
	if( _IsFocus() == true )
	{

	}
}

void _DlgMTCellObject::OnEnChangeEditRandomYrotMax()
{
	if( _IsFocus() == true )
	{

	}
}


void _DlgMTCellObject::OnBnClickedCheckRandomZrot()
{
	bool bEnableWindow = FALSE;
	if( m_chk_RandomZRot.GetCheck() == BST_CHECKED )
	{
		bEnableWindow = TRUE;
	}

	m_edit_ZRotMin.EnableWindow( bEnableWindow );
	m_edit_ZRotMax.EnableWindow( bEnableWindow );
	m_spin_ZRotMin.EnableWindow( bEnableWindow );
	m_spin_ZRotMax.EnableWindow( bEnableWindow );
}

void _DlgMTCellObject::OnEnChangeEditRandomZrotMin()
{
	if( _IsFocus() == true )
	{

	}
}

void _DlgMTCellObject::OnEnChangeEditRandomZrotMax()
{
	if( _IsFocus() == true )
	{

	}
}

void _DlgMTCellObject::OnBnClickedCheckRandomZtrans()
{
	bool bEnableWindow = FALSE;
	if( m_chk_RandomZTrans.GetCheck() == BST_CHECKED )
	{
		bEnableWindow = TRUE;
	}

	m_edit_ZTransMin.EnableWindow( bEnableWindow );
	m_edit_ZTransMax.EnableWindow( bEnableWindow );
	m_spin_ZTransMin.EnableWindow( bEnableWindow );
	m_spin_ZTransMax.EnableWindow( bEnableWindow );
}

void _DlgMTCellObject::OnBnClickedUseTerraInNormal()
{


}

void _DlgMTCellObject::OnEnChangeEditRandomZtransMin()
{
	if( _IsFocus() == true )
	{

	}
}

void _DlgMTCellObject::OnEnChangeEditRandomZtransMax()
{
	if( _IsFocus() == true )
	{

	}
}


// =================================================================================================
//
//		그룹 배치
//
// =================================================================================================

void _DlgMTCellObject::__GroupResist( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );		
	if( pRecord == NULL )
		return;

	// 선택 오브젝트 종류
	std::list< int > listSelectObject;
	int nTotalObjectCount = m_list_CellObject.GetItemCount();
	for( int i=0; i<nTotalObjectCount; ++i )
	{
		if( m_list_CellObject.GetCheck( i ) == TRUE )
			listSelectObject.push_back( i );
	}

	if( listSelectObject.empty() == true )
	{
		CsMessageBox( MB_OK, L"선택된 오브젝트가 없습니다. 위 오브젝트 리스트에서 체크박스를 활성화 시켜 주십시오." );
		return;
	}

	int nResistCount = nsCsMfcFunc::Control2Int( &m_edit_GroupCount );
	if( nResistCount <= 0 )
	{
		CsMessageBox( MB_OK, L"배치할 오브젝트의 갯수는 0보다 커야 합니다." );
		return;
	}

	g_RollBack.Begin();	

	NiTransform tr;
	tr.MakeIdentity();

	float fRange = (float)m_sld_GroupRadius.GetPos();
	NiPoint3 ptOrg = pRecord->GetIntersection();
	ptOrg.z = 0;
	NiPoint3 ptOrgStart = NiPoint3::ZERO;
	ptOrgStart.x = CsMax( 0, ptOrg.x - fRange );
	ptOrgStart.y = CsMax( 0, ptOrg.y - fRange );
	NiPoint3 ptOrgEnd = NiPoint3::ZERO;
	ptOrgEnd.x = CsMin( nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth(), ptOrg.x+fRange );
	ptOrgEnd.y = CsMin( nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight(), ptOrg.y+fRange );

	for( int i=0; i<nResistCount; ++i )
	{
		int nTryIsIn = 100;
		while( nTryIsIn > 0 )
		{
			NiPoint3 ptResult;
			ptResult.x = ( rand()*rand() )%(int)( (ptOrgEnd.x-ptOrgStart.x)*100.0f )*0.01f + ptOrgStart.x;
			ptResult.y = ( rand()*rand() )%(int)( (ptOrgEnd.y-ptOrgStart.y)*100.0f )*0.01f + ptOrgStart.y;
			ptResult.z = 0;

			if( ( ptResult - ptOrg ).Length() > fRange )
			{
				--nTryIsIn;
				continue;
			}

			// 목적점
			tr.m_Translate = ptResult;
			break;			
		}
		if( nTryIsIn == 0 )
		{
			CsMessageBox( MB_OK, L"지형 밖으로 자꾸 벗어 나므로 캔슬합니다." );
			break;;
		}
		tr.m_Translate.z = nsCsGBTerrain::g_pCurRoot->GetHeight( tr.m_Translate );
		NiPoint3 kNormal =  NiPoint3::ZERO;
		nsCsGBTerrain::g_pCurRoot->GetHeight_Normal(tr.m_Translate.x,tr.m_Translate.y, kNormal );

		// 랜덤 오브젝트
		int nCount = (int)listSelectObject.size();
		int nRandomIndex = ( rand()*rand() )%nCount;
		std::list< int >::iterator itEnd = listSelectObject.end();
		int nResultIndex = 0;
		for( std::list< int >::iterator it = listSelectObject.begin(); it!=itEnd; ++it )
		{
			if( nRandomIndex == 0 )
			{
				nResultIndex = (*it);
				break;
			}
			--nRandomIndex;
		}
		__ResistObject( nResultIndex, tr, true, kNormal);
	}
	
	g_RollBack.End();
	if( CELLOBJ::Manager::GetSingletonPtr() )
	{
		int CUID = CELLOBJ::Manager::GetSingletonPtr()->TakeUndoSnapshot(UNDO::CELLOBJ_CREATE);

		CELLOBJ::Manager::GetSingletonPtr()->TakeRedoSnapshotCellRedo(CUID);
	}
	g_ToolNiNodeMng.UpdateGizmoPos(CELLOBJ::Manager::GetSingletonPtr());

	__ResetListInfo();
}

void _DlgMTCellObject::__GroupDelete( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );		
	if( pRecord == NULL )
		return;

	// 선택 오브젝트 종류
	std::map< int, bool > mapSelectObject;
	int nTotalObjectCount = m_list_CellObject.GetItemCount();
	for( int i=0; i<nTotalObjectCount; ++i )
	{
		if( m_list_CellObject.GetCheck( i ) == TRUE )
			mapSelectObject[ i ] = true;
	}

	// 선택 오브젝트가 없다면 전부 다
	if( mapSelectObject.empty() == true )
	{
		int nTotalObjectCount = m_list_CellObject.GetItemCount();
		for( int i=0; i<nTotalObjectCount; ++i )
		{
			mapSelectObject[ i ] = true;
		}
	}

	g_RollBack.Begin();

	NiPoint3 ptOrg = pRecord->GetIntersection();
	float fRange = (float)m_sld_GroupRadius.GetPos();
	std::map< int, bool >::iterator it = mapSelectObject.begin();
	std::map< int, bool >::iterator itEnd = mapSelectObject.end();
	CsGBSpeedCell* pSpeedCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell();
	int nCellCount = pSpeedCell->GetCellTotalCount();
	for( ; it!=itEnd; ++it )
	{
		int nBaseIndex = it->first;
		for( int c=0; c<nCellCount; ++c )
		{
			CsGBCell* pCell= pSpeedCell->GetCell( c );
			int nObjectCount = pCell->GetSize( nBaseIndex );
			for( int o=nObjectCount-1; o>=0; --o )
			{
				CsGBCell::sINFO* pInfo = pCell->GetInfo( nBaseIndex, o );
				if( ( pInfo->s_trParent.m_Translate - ptOrg ).Length() < fRange )
				{
					pCell->DeleteObject( nBaseIndex, o );
					cRollBack::sRT_CELL_DELETE_INFO info;
					info.s_nBaseIndex = nBaseIndex;
					info.s_Info = *pInfo;
					g_RollBack.Push( &info );
				}
			}
		}
	}

	g_RollBack.End();

	__ResetListInfo();
}

void _DlgMTCellObject::__GroupCopy( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );		
	if( pRecord == NULL )
		return;

	m_vpPasteInfo.ClearElement();

	// 선택 오브젝트 종류
	std::map< int, bool > mapSelectObject;
	int nTotalObjectCount = m_list_CellObject.GetItemCount();
	for( int i=0; i<nTotalObjectCount; ++i )
	{
		if( m_list_CellObject.GetCheck( i ) == TRUE )
			mapSelectObject[ i ] = true;
	}

	// 선택 오브젝트가 없다면 전부 다
	if( mapSelectObject.empty() == true )
	{
		int nTotalObjectCount = m_list_CellObject.GetItemCount();
		for( int i=0; i<nTotalObjectCount; ++i )
		{
			mapSelectObject[ i ] = true;
		}
	}

	NiPoint3 ptOrg = pRecord->GetIntersection();
	float fRange = (float)m_sld_GroupRadius.GetPos();
	std::map< int, bool >::iterator it = mapSelectObject.begin();
	std::map< int, bool >::iterator itEnd = mapSelectObject.end();
	CsGBSpeedCell* pSpeedCell = nsCsGBTerrain::g_pCurRoot->GetSpeedCell();
	int nCellCount = pSpeedCell->GetCellTotalCount();
	for( ; it!=itEnd; ++it )
	{
		int nBaseIndex = it->first;
		for( int c=0; c<nCellCount; ++c )
		{
			CsGBCell* pCell= pSpeedCell->GetCell( c );
			int nObjectCount = pCell->GetSize( nBaseIndex );
			for( int o=nObjectCount-1; o>=0; --o )
			{
				CsGBCell::sINFO* pInfo = pCell->GetInfo( nBaseIndex, o );
				if( ( pInfo->s_trParent.m_Translate - ptOrg ).Length() < fRange )
				{
					sPASTE_INFO paste;
					paste.s_nBaseObjectIndex = nBaseIndex;
					paste.s_fScale = pInfo->s_trParent.m_fScale;
					paste.s_Rotation = pInfo->s_trParent.m_Rotate;
					paste.s_vOffset = pInfo->s_trParent.m_Translate - ptOrg;
					paste.s_fDeltaHeight = pInfo->s_trParent.m_Translate.z - nsCsGBTerrain::g_pCurRoot->GetHeight( pInfo->s_trParent.m_Translate );
					m_vpPasteInfo.PushBack( paste );
				}
			}
		}
	}

	if( m_vpPasteInfo.Size() != 0 )
	{
		m_chk_GroupPaste.EnableWindow( TRUE );
		m_chk_GroupCopy.SetCheck( BST_UNCHECKED );
		m_chk_GroupPaste.SetCheck( BST_CHECKED );
	}	
}

void _DlgMTCellObject::__GroupPaste( CPoint pt )
{
	assert_cs( m_vpPasteInfo.Size() != 0 );

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );		
	if( pRecord == NULL )
		return;

	g_RollBack.Begin();	

	NiTransform tr;
	tr.MakeIdentity();

	NiPoint3 ptOrg = pRecord->GetIntersection();
	ptOrg.z = 0;

	int nPasteSize = m_vpPasteInfo.Size();
	for( int i=0; i<nPasteSize; ++i )
	{
		tr.m_fScale = m_vpPasteInfo[ i ].s_fScale;
		tr.m_Rotate = m_vpPasteInfo[ i ].s_Rotation;
		tr.m_Translate = m_vpPasteInfo[ i ].s_vOffset + ptOrg;
		tr.m_Translate.z = nsCsGBTerrain::g_pCurRoot->GetHeight( tr.m_Translate ) + m_vpPasteInfo[ i ].s_fDeltaHeight;
		__ResistObject( m_vpPasteInfo[ i ].s_nBaseObjectIndex, tr, false, NiPoint3::ZERO );
	}

	g_RollBack.End();

	__ResetListInfo();
}

void _DlgMTCellObject::OnBnClickedCheckGroup()
{
	m_chk_GroupDelete.SetCheck( BST_UNCHECKED );
	m_chk_GroupCopy.SetCheck( BST_UNCHECKED );
	m_chk_GroupPaste.SetCheck( BST_UNCHECKED );
}

void _DlgMTCellObject::OnBnClickedCheckGroupDelete()
{
	m_chk_Group.SetCheck( BST_UNCHECKED );
	m_chk_GroupCopy.SetCheck( BST_UNCHECKED );
	m_chk_GroupPaste.SetCheck( BST_UNCHECKED );
}


void _DlgMTCellObject::OnBnClickedCheckGroupCopy()
{
	m_chk_Group.SetCheck( BST_UNCHECKED );
	m_chk_GroupDelete.SetCheck( BST_UNCHECKED );
	m_chk_GroupPaste.SetCheck( BST_UNCHECKED );
}

void _DlgMTCellObject::OnBnClickedCheckGroupPaste()
{
	m_chk_Group.SetCheck( BST_UNCHECKED );
	m_chk_GroupDelete.SetCheck( BST_UNCHECKED );
	m_chk_GroupCopy.SetCheck( BST_UNCHECKED );
}


void _DlgMTCellObject::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);

	if( pScrollBar->GetSafeHwnd() == m_sld_GroupRadius.GetSafeHwnd() )
	{		
		float fRadius = m_sld_GroupRadius.GetPos()*0.01f;

		nsCsMfcFunc::Float2Control( &m_edit_GroupRadius, fRadius );
	}
}


// =================================================================================================
//
//		랜덤 애니메이션
//
// =================================================================================================

void _DlgMTCellObject::OnEnChangeEditCheckTimeMin()
{
	if( _IsFocus() == true )
	{
		CsGBSpeedCell::sOBJECT* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex );
		pBaseObject->s_ObjectInfo.s_fCheckTimeMin = nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Min );
	}
}

void _DlgMTCellObject::OnEnChangeEditCheckTimeMax()
{
	if( _IsFocus() == true )
	{
		CsGBSpeedCell::sOBJECT* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex );
		pBaseObject->s_ObjectInfo.s_fCheckTimeMax = nsCsMfcFunc::Control2Float( &m_edit_CheckTime_Max );
	}
}

void _DlgMTCellObject::OnEnChangeEditAniSpeedMin()
{
	if( _IsFocus() == true )
	{
		CsGBSpeedCell::sOBJECT* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex );
		pBaseObject->s_ObjectInfo.s_fAniSpeedMin = nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Min );
	}
}

void _DlgMTCellObject::OnEnChangeEditAniSpeedMax()
{
	if( _IsFocus() == true )
	{
		CsGBSpeedCell::sOBJECT* pBaseObject = nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->GetBaseObject( m_nSelectBaseIndex );
		pBaseObject->s_ObjectInfo.s_fAniSpeedMax = nsCsMfcFunc::Control2Float( &m_edit_AniSpeed_Max );
	}
}



void _DlgMTCellObject::LookAt(NiAVObject* pNode, const NiPoint3& kNormal)
{
	NiPoint3 TargetUp = kNormal;
	NiPoint3::UnitizeVector(TargetUp);
	NiPoint3 TargetProjZ = TargetUp;
	TargetProjZ.x = 0.0f;
	NiPoint3::UnitizeVector(TargetProjZ);
	NiMatrix3 kZRot;
	kZRot.MakeXRotation(NI_PI * 0.5f);
	NiPoint3 TargetRight = kZRot * TargetProjZ;
	NiPoint3::UnitizeVector(TargetRight);
	NiPoint3 TargetDir =TargetUp.Cross(TargetRight) * -1.0f;
	NiPoint3::UnitizeVector(TargetDir);
	TargetRight = TargetUp.Cross(TargetDir);
	NiPoint3::UnitizeVector(TargetRight);
	// get parent world transforms
	NiNode* pkParent = pNode->GetParent();
	NiCamera* pkCamera = CAMERA_ST.GetCameraObj();

	NiMatrix3 kFaceMat = NiMatrix3::IDENTITY;
	kFaceMat.SetCol(0, TargetDir);
	kFaceMat.SetCol(1, TargetRight);
	kFaceMat.SetCol(2, TargetUp);
/////////////////////////////////
	NiTransform tr, trDest, trResult;
	tr.MakeIdentity();
	tr.m_Rotate = kFaceMat;
	
	trDest = pNode->GetWorldTransform();
	trDest.m_Rotate = kFaceMat;//pkCamera->GetWorldTransform().m_Rotate;
	trDest.m_Translate -= pNode->GetTranslate();
	trResult = tr;//*trDest;
	trResult.m_Translate += pNode->GetTranslate();
	pNode->SetLocalTransform(trResult);
	pNode->Update(0.0f);
}
//   NiPoint3 kCamD, kCamU, kCamR;

//kCamD = TargetDir;// pkCamera->GetWorldDirection();//
//kCamU = TargetUp;//pkCamera->GetWorldUpVector();//
//kCamR = TargetRight;//pkCamera->GetWorldRightVector();//
NiPoint3 _DlgMTCellObject::GetWorldDirection(NiAVObject* pkAVObject)
{
	NiPoint3 kDVector;
	pkAVObject->GetWorldTransform().m_Rotate.GetCol(0, kDVector);
	return kDVector;
}
//---------------------------------------------------------------------------
NiPoint3 _DlgMTCellObject::GetWorldUpVector(NiAVObject* pkAVObject)
{
	NiPoint3 kUVector;
	pkAVObject->GetWorldTransform().m_Rotate.GetCol(1, kUVector);
	return kUVector;
}
//---------------------------------------------------------------------------
NiPoint3 _DlgMTCellObject::GetWorldRightVector(NiAVObject* pkAVObject)
{
	NiPoint3 kRVector;
	pkAVObject->GetWorldTransform().m_Rotate.GetCol(2, kRVector);
	return kRVector;
}
float _DlgMTCellObject::FindDegree(float x, float y)
{
	float value = (( atan2(x, y) / NI_PI) * 180.f);
	if (value < 0) value += 360.f;
	return value;
}
