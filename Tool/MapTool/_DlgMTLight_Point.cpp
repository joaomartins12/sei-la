// _DlgMTLight_Point.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTLight_Point.h"


// _DlgMTLight_Point 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTLight_Point, CDialog)

_DlgMTLight_Point::_DlgMTLight_Point(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTLight_Point::IDD, pParent)
{
	m_pSelectLight = NULL;	
}

_DlgMTLight_Point::~_DlgMTLight_Point()
{
}

void _DlgMTLight_Point::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SL_D, m_static_Diffuse);
	DDX_Control(pDX, IDC_STATIC_SL_A, m_static_Ambient);
	DDX_Control(pDX, IDC_STATIC_SL_S, m_static_Specular);
	DDX_Control(pDX, IDC_EDIT_TL_X, m_edit_TL[ 0 ]);
	DDX_Control(pDX, IDC_EDIT_TL_Y, m_edit_TL[ 1 ]);
	DDX_Control(pDX, IDC_EDIT_TL_Z, m_edit_TL[ 2 ]);
	DDX_Control(pDX, IDC_SPIN_TL_X, m_spin_TL[ 0 ]);
	DDX_Control(pDX, IDC_SPIN_TL_Y, m_spin_TL[ 1 ]);
	DDX_Control(pDX, IDC_SPIN_TL_Z, m_spin_TL[ 2 ]);
	DDX_Control(pDX, IDC_EDIT_C, m_edit_Constant);
	DDX_Control(pDX, IDC_SPIN_C, m_spin_Constant);
	DDX_Control(pDX, IDC_EDIT_RANGE, m_edit_Range);
	DDX_Control(pDX, IDC_SPIN_RANGE, m_spin_Range);
	DDX_Control(pDX, IDC_EDIT_POWER, m_edit_Power);
	DDX_Control(pDX, IDC_SPIN_POWER, m_spin_Power);
	DDX_Control(pDX, IDC_CHECK_TR, m_chk_TR);
	DDX_Control(pDX, IDC_CHECK_OBJECT, m_chk_Object);
	DDX_Control(pDX, IDC_CHECK_CHAR, m_chk_Char);
	DDX_Control(pDX, IDC_CHECK_OBJECT_SHADOW, m_chk_ObjectShadow);
	DDX_Control(pDX, IDC_CHECK_DYNAMIC_SHADOW, m_chk_DynamicShadow );
}


BEGIN_MESSAGE_MAP(_DlgMTLight_Point, CDialog)
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_STATIC_SL_D, &_DlgMTLight_Point::OnStnClickedStaticSlD)
	ON_STN_CLICKED(IDC_STATIC_SL_A, &_DlgMTLight_Point::OnStnClickedStaticSlA)
	ON_STN_CLICKED(IDC_STATIC_SL_S, &_DlgMTLight_Point::OnStnClickedStaticSlS)
	ON_EN_SETFOCUS(IDC_EDIT_TL_X, &_DlgMTLight_Point::OnEnSetfocusEditTlX)
	ON_EN_SETFOCUS(IDC_EDIT_TL_Y, &_DlgMTLight_Point::OnEnSetfocusEditTlY)
	ON_EN_SETFOCUS(IDC_EDIT_TL_Z, &_DlgMTLight_Point::OnEnSetfocusEditTlZ)
	ON_EN_CHANGE(IDC_EDIT_TL_X, &_DlgMTLight_Point::OnEnChangeEditTlX)
	ON_EN_CHANGE(IDC_EDIT_TL_Y, &_DlgMTLight_Point::OnEnChangeEditTlY)
	ON_EN_CHANGE(IDC_EDIT_TL_Z, &_DlgMTLight_Point::OnEnChangeEditTlZ)
	ON_EN_SETFOCUS(IDC_EDIT_C, &_DlgMTLight_Point::OnEnSetfocusEditC)
	ON_EN_CHANGE(IDC_EDIT_C, &_DlgMTLight_Point::OnEnChangeEditC)
	ON_EN_SETFOCUS(IDC_EDIT_RANGE, &_DlgMTLight_Point::OnEnSetfocusEditRange)
	ON_EN_CHANGE(IDC_EDIT_RANGE, &_DlgMTLight_Point::OnEnChangeEditRange)
	ON_EN_SETFOCUS(IDC_EDIT_POWER, &_DlgMTLight_Point::OnEnSetfocusEditPower)
	ON_EN_CHANGE(IDC_EDIT_POWER, &_DlgMTLight_Point::OnEnChangeEditPower)
	ON_BN_CLICKED(IDC_CHECK_TR, &_DlgMTLight_Point::OnBnClickedCheckTr)
	ON_BN_CLICKED(IDC_CHECK_OBJECT_SHADOW, &_DlgMTLight_Point::OnBnClickedCheckObjectShadow)
	ON_BN_CLICKED(IDC_CHECK_OBJECT, &_DlgMTLight_Point::OnBnClickedCheckObject)
	ON_BN_CLICKED(IDC_CHECK_CHAR, &_DlgMTLight_Point::OnBnClickedCheckChar)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_DYNAMIC_SHADOW, &_DlgMTLight_Point::OnBnClickedCheckDynamicShadow)
END_MESSAGE_MAP()


// _DlgMTLight_Point 메시지 처리기입니다.




BOOL _DlgMTLight_Point::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	for( int i=0; i<3; ++i )
	{
		m_spin_TL[ i ].SetCsBuddy( &m_edit_TL[ i ], 5, 1 );		
	}
	m_spin_Constant.SetCsBuddy( &m_edit_Constant, -0.02f, 2 );
	m_spin_Range.SetCsBuddy( &m_edit_Range, 1.0f, 1 );
	m_spin_Power.SetCsBuddy( &m_edit_Power, 0.001f, 3 );	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgMTLight_Point::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == FALSE )
	{
		if( m_bCreateMode == true )
		{
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->DeleteLight( &m_CreateLight );			
		}	
	}
}


HBRUSH _DlgMTLight_Point::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = _DlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

	switch( pWnd->GetDlgCtrlID() )
	{
	case IDC_STATIC_SL_D:		hbr = m_Diffuse.s_Brush;	break;
	case IDC_STATIC_SL_A:		hbr = m_Ambient.s_Brush;	break;
	case IDC_STATIC_SL_S:		hbr = m_Specular.s_Brush;	break;
	}
	return hbr;
}


void _DlgMTLight_Point::OnBnClickedCheckTr()
{
	bool bCheck = ( m_chk_TR.GetCheck() == TRUE );
	if( m_bCreateMode == true )
	{

	}
	else
	{
		assert_cs( m_pSelectLight != NULL );
		m_pSelectLight->GetBaseInfo()->s_bTRShadow = bCheck;
	}

	GET_MV_MAINTAB_DLG->_GetDlgLight()->_ReadInfo( true );
}

void _DlgMTLight_Point::OnBnClickedCheckObjectShadow()
{
	bool bCheck = ( m_chk_ObjectShadow.GetCheck() == TRUE );
	if( bCheck == true )
	{
		std::list< CsGBLight* >* pList = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetLightList();
		std::list< CsGBLight* >::iterator it = pList->begin();
		std::list< CsGBLight* >::iterator itEnd = pList->end();
		for( ; it!=itEnd; ++it )
		{
			(*it)->GetBaseInfo()->s_bObjectShadow = false;
		}
	}

	if( m_bCreateMode == true )
	{

	}
	else
	{
		assert_cs( m_pSelectLight != NULL );
		m_pSelectLight->GetBaseInfo()->s_bObjectShadow = bCheck;
	}

	g_pShadow->Tool_ResetObjectShadowLight();
	GET_MV_MAINTAB_DLG->_GetDlgLight()->_ReadInfo( true );
}

void _DlgMTLight_Point::OnBnClickedCheckObject()
{
	bool bCheck = ( m_chk_Object.GetCheck() == TRUE );
	if( m_bCreateMode == true )
	{
		m_CreateLight.GetBaseInfo()->s_bApplyObject = bCheck;
		if( bCheck == true )
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddObject( &m_CreateLight );
		else
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveObject( &m_CreateLight );
	}
	else
	{
		assert_cs( m_pSelectLight != NULL );
		m_pSelectLight->GetBaseInfo()->s_bApplyObject = bCheck;
		if( bCheck == true )
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddObject( m_pSelectLight );
		else
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveObject( m_pSelectLight );
	}

	GET_MV_MAINTAB_DLG->_GetDlgLight()->_ReadInfo( true );
}

void _DlgMTLight_Point::OnBnClickedCheckChar()
{
	bool bCheck = ( m_chk_Char.GetCheck() == TRUE );
	if( m_bCreateMode == true )
	{

	}
	else
	{
		assert_cs( m_pSelectLight != NULL );
		m_pSelectLight->GetBaseInfo()->s_bApplyChar = bCheck;
		if( bCheck == true )
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddChar( m_pSelectLight );
		else
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveChar( m_pSelectLight );
	}

	GET_MV_MAINTAB_DLG->_GetDlgLight()->_ReadInfo( true );
}

void _DlgMTLight_Point::OnBnClickedCheckDynamicShadow()
{
	bool bCheck = ( m_chk_Char.GetCheck() == TRUE );
	if( m_bCreateMode == true )
	{

	}
	else
	{
		assert_cs( m_pSelectLight != NULL );
		m_pSelectLight->GetBaseInfo()->s_bApplyDynamicShadow = bCheck;
	}	
	GET_MV_MAINTAB_DLG->_GetDlgLight()->_ReadInfo( true );
}


void _DlgMTLight_Point::OnStnClickedStaticSlD()
{
	m_Diffuse.OpenDlg( &m_static_Diffuse );

	if( m_bCreateMode == true )
	{
		m_CreateLight.SetDiffuseColor( m_Diffuse.s_Color );
	}
	else
	{
		assert_cs( m_pSelectLight != NULL );
		m_pSelectLight->SetDiffuseColor( m_Diffuse.s_Color );
	}
}

void _DlgMTLight_Point::OnStnClickedStaticSlA()
{
	m_Ambient.OpenDlg( &m_static_Ambient );

	if( m_bCreateMode == true )
	{
		m_CreateLight.SetAmbientColor( m_Ambient.s_Color );
	}
	else
	{
		assert_cs( m_pSelectLight != NULL );
		m_pSelectLight->SetAmbientColor( m_Ambient.s_Color );
	}
}

void _DlgMTLight_Point::OnStnClickedStaticSlS()
{
	m_Specular.OpenDlg( &m_static_Specular );

	if( m_bCreateMode == true )
	{
		m_CreateLight.SetSpecularColor( m_Specular.s_Color );
	}
	else
	{
		assert_cs( m_pSelectLight != NULL );
		m_pSelectLight->SetSpecularColor( m_Specular.s_Color );
	}
}


void _DlgMTLight_Point::OnEnSetfocusEditTlX(){		m_edit_TL[ 0 ].SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Point::OnEnSetfocusEditTlY(){		m_edit_TL[ 1 ].SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Point::OnEnSetfocusEditTlZ(){		m_edit_TL[ 2 ].SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Point::OnEnSetfocusEditC(){		m_edit_Constant.SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Point::OnEnSetfocusEditRange(){	m_edit_Range.SetSel( 0, -1, TRUE ); }
void _DlgMTLight_Point::OnEnSetfocusEditPower(){	m_edit_Power.SetSel( 0, -1, TRUE ); }

void _DlgMTLight_Point::OnEnChangeEditTlX()
{
	if( _IsFocus() == true )
	{
		NiPoint3 pos = NiPoint3(	nsCsMfcFunc::Control2Float( &m_edit_TL[ 0 ] ),
									nsCsMfcFunc::Control2Float( &m_edit_TL[ 1 ] ),
									nsCsMfcFunc::Control2Float( &m_edit_TL[ 2 ] ) );

		if( m_bCreateMode == true )
		{
			m_CreateLight.SetPosition( pos );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );			
			m_pSelectLight->SetPosition( pos );
		}
	}
}

void _DlgMTLight_Point::OnEnChangeEditTlY()
{
	if( _IsFocus() == true )
	{
		NiPoint3 pos = NiPoint3(	nsCsMfcFunc::Control2Float( &m_edit_TL[ 0 ] ),
									nsCsMfcFunc::Control2Float( &m_edit_TL[ 1 ] ),
									nsCsMfcFunc::Control2Float( &m_edit_TL[ 2 ] ) );
		if( m_bCreateMode == true )
		{
			m_CreateLight.SetPosition( pos );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );
			
			m_pSelectLight->SetPosition( pos );
		}
	}
}

void _DlgMTLight_Point::OnEnChangeEditTlZ()
{
	if( _IsFocus() == true )
	{
		NiPoint3 pos = NiPoint3(	nsCsMfcFunc::Control2Float( &m_edit_TL[ 0 ] ),
									nsCsMfcFunc::Control2Float( &m_edit_TL[ 1 ] ),
									nsCsMfcFunc::Control2Float( &m_edit_TL[ 2 ] ) );

		if( m_bCreateMode == true )
		{
			m_CreateLight.SetPosition( pos );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );			
			m_pSelectLight->SetPosition( pos );
		}
	}
}

void _DlgMTLight_Point::OnEnChangeEditC()
{
	if( _IsFocus() == true )
	{
		float fRange = nsCsMfcFunc::Control2Float( &m_edit_Range );
		float fConstant = nsCsMfcFunc::Control2Float( &m_edit_Constant );
		if( m_bCreateMode == true )
		{
			m_CreateLight.SetRange( fRange, fConstant );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );
			m_pSelectLight->SetRange( fRange, fConstant );			
		}
	}
}

void _DlgMTLight_Point::OnEnChangeEditRange()
{
	if( _IsFocus() == true )
	{
		float fRange = nsCsMfcFunc::Control2Float( &m_edit_Range );
		float fConstant = nsCsMfcFunc::Control2Float( &m_edit_Constant );
		if( m_bCreateMode == true )
		{
			m_CreateLight.SetRange( fRange, fConstant );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );
			m_pSelectLight->SetRange( fRange, fConstant );
		}
	}
}

void _DlgMTLight_Point::OnEnChangeEditPower()
{
	if( _IsFocus() == true )
	{
		float fPower = nsCsMfcFunc::Control2Float( &m_edit_Power );
		if( m_bCreateMode == true )
		{
			m_CreateLight.SetPower( fPower );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );
			m_pSelectLight->SetPower( fPower );
		}
	}
}

bool _DlgMTLight_Point::_IsFocus()
{
	CWnd* pWnd = GetFocus();
	for( int i=0; i<3; ++i )
	{
		if( pWnd->GetSafeHwnd() == m_edit_TL[ i ].GetSafeHwnd() )
			
		{
			return true;
		}
	}

	if( ( pWnd->GetSafeHwnd() == m_edit_Range.GetSafeHwnd() )||
		( pWnd->GetSafeHwnd() == m_edit_Constant.GetSafeHwnd() ) )
	{
		return true;
	}

	if( pWnd->GetSafeHwnd() == m_edit_Power.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_Power ) < 0.0f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_Power, 0.0f, 3 );
		}
		return true;
	}

	return false;
}

void _DlgMTLight_Point::_SetCreateMode( bool bCreateMode )
{
	m_bCreateMode = bCreateMode;
	if( m_bCreateMode == true )
	{
		__ResetCreateLight();
		if( ( m_CreateLight.GetNiLight() != NULL )&&( m_CreateLight.GetBaseInfo()->s_bApplyObject ) )
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddObject( &m_CreateLight );
	}
	else
	{
		if( ( m_CreateLight.GetNiLight() != NULL )&&( m_CreateLight.GetBaseInfo()->s_bApplyObject ) )
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveObject( &m_CreateLight );
	}
}

void _DlgMTLight_Point::_SetSelectLight( CsGBLight* pLight )
{
	m_pSelectLight = (CsGBLight_Point*)pLight;	

	if( m_pSelectLight )
	{
		__ReadLightInfo( m_pSelectLight );
		ShowWindow( SW_SHOW );
	}
	else
	{
		ShowWindow( SW_HIDE );
	}
}

void _DlgMTLight_Point::_PickSetPos( NiPoint3 pos )
{
	if( m_bCreateMode == true )
	{
		m_CreateLight.SetPosition( pos );
	}
	else if( m_pSelectLight != NULL )
	{
		m_pSelectLight->SetPosition( pos );
	}

	nsCsMfcFunc::Float2Control( &m_edit_TL[ 0 ], pos.x, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_TL[ 1 ], pos.y, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_TL[ 2 ], pos.z, 1 );
}

void _DlgMTLight_Point::_Delete()
{
	m_CreateLight.Delete();
	m_Diffuse.Delete();
	m_Ambient.Delete();
	m_Specular.Delete();
	m_NiNodeFar = 0;
	m_NiNodeNear = 0;
	m_NiNodeCenter = 0;
}

void _DlgMTLight_Point::_ViewInit()
{
#define MESH_NAME_FAR		".\\ToolData\\PointLight.nif"
#define MESH_NAME_NEAR		".\\ToolData\\PointLight2.nif"
#define MESH_NAME_CENTER	".\\ToolData\\PointLight3.nif"

	{
		NiStream stream;
		bool bLoad = stream.Load( MESH_NAME_FAR );
		if( bLoad == false )
		{
			CsMessageBoxA( MB_OK, ".\\ToolData\\%s 가 존재 하지 않습니다. 비정상적으로 종료 될수 있습니다.", MESH_NAME_FAR );
			return;
		}

		m_NiNodeFar = (NiNode*)stream.GetObjectAt( 0 );
		m_NiNodeFar->UpdateProperties();
		m_NiNodeFar->Update( 0.0f );
	}
	{
		NiStream stream;
		bool bLoad = stream.Load( MESH_NAME_NEAR );
		if( bLoad == false )
		{
			CsMessageBoxA( MB_OK, ".\\ToolData\\%s 가 존재 하지 않습니다. 비정상적으로 종료 될수 있습니다.", MESH_NAME_NEAR );
			return;
		}

		m_NiNodeNear = (NiNode*)stream.GetObjectAt( 0 );
		m_NiNodeNear->UpdateProperties();
		m_NiNodeNear->Update( 0.0f );
	}
	{
		NiStream stream;
		bool bLoad = stream.Load( MESH_NAME_CENTER );
		if( bLoad == false )
		{
			CsMessageBoxA( MB_OK, ".\\ToolData\\%s 가 존재 하지 않습니다. 비정상적으로 종료 될수 있습니다.", MESH_NAME_CENTER );
			return;
		}

		m_NiNodeCenter = (NiNode*)stream.GetObjectAt( 0 );
		m_NiNodeCenter->UpdateProperties();
		m_NiNodeCenter->Update( 0.0f );
	}

	__ResetCreateLight();
}

void _DlgMTLight_Point::_PreViewReset()
{
	m_pSelectLight = NULL;
}

void _DlgMTLight_Point::_Render( CsGBLight_Point* pLight )
{
	CsGBLight::sBASE_INFO* pBaseInfo = pLight->GetBaseInfo();
	CsGBLight_Point::sINFO* pInfo = pLight->GetInfo();

	if( m_bCreateMode == false )
	{
		if( m_pSelectLight == pLight )
		{
			m_NiNodeCenter->SetTranslate( pInfo->s_Pos );
			__SetCenterColor( NiColor::WHITE );
			m_NiNodeCenter->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeCenter, *g_pCuller );

			m_NiNodeNear->SetTranslate( pInfo->s_Pos );
			m_NiNodeNear->SetScale( CsMax( 0.0f, ( ( 1.0f - pInfo->s_C )/pInfo->s_L )*0.01f ) );
			m_NiNodeNear->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeNear, *g_pCuller );

			m_NiNodeFar->SetTranslate( pInfo->s_Pos );
			m_NiNodeFar->SetScale( pInfo->s_Range*0.01f );
			m_NiNodeFar->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeFar, *g_pCuller );
		}
		else
		{
			m_NiNodeCenter->SetTranslate( pInfo->s_Pos );
			__SetCenterColor( NiColor( 0.5f, 0.5f, 0.5f ) );
			m_NiNodeCenter->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeCenter, *g_pCuller );
		}
	}
	else
	{
		if( &m_CreateLight == pLight )
		{
			m_NiNodeCenter->SetTranslate( pInfo->s_Pos );
			__SetCenterColor( NiColor( 1.0f, 1.0f, 0.0f ) );
			m_NiNodeCenter->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeCenter, *g_pCuller );

			m_NiNodeNear->SetTranslate( pInfo->s_Pos );
			m_NiNodeNear->SetScale( CsMax( 0.0f, ( ( 1.0f - pInfo->s_C )/pInfo->s_L )*0.01f ) );
			m_NiNodeNear->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeNear, *g_pCuller );

			m_NiNodeFar->SetTranslate( pInfo->s_Pos );
			m_NiNodeFar->SetScale( pInfo->s_Range*0.01f );
			m_NiNodeFar->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeFar, *g_pCuller );
		}
		else
		{
			m_NiNodeCenter->SetTranslate( pInfo->s_Pos );
			__SetCenterColor( NiColor( 0.5f, 0.5f, 0.5f ) );
			m_NiNodeCenter->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeCenter, *g_pCuller );
		}
	}
}

void _DlgMTLight_Point::_PostRenderView()
{
	if( m_bCreateMode == false )
		return;
	
	_Render( &m_CreateLight );
}

void _DlgMTLight_Point::_AddLight()
{
	assert_cs( m_bCreateMode == true );

	CsGBLight::sBASE_INFO baseInfo;
	CsGBLight_Point::sINFO Info;
	baseInfo.s_eType = CsGBLight::POINT;
	baseInfo.s_bTRShadow = ( m_chk_TR.GetCheck() == BST_CHECKED );
	baseInfo.s_bObjectShadow = ( m_chk_ObjectShadow.GetCheck() == BST_CHECKED );
	baseInfo.s_bApplyChar = ( m_chk_Char.GetCheck() == BST_CHECKED );
	baseInfo.s_bApplyObject = ( m_chk_Object.GetCheck() == BST_CHECKED );
	baseInfo.s_bApplyDynamicShadow = (m_chk_DynamicShadow.GetCheck() == BST_CHECKED );
	baseInfo.s_Diffuse = m_Diffuse.s_Color;
	baseInfo.s_Ambient = m_Ambient.s_Color;
	baseInfo.s_Ambient = m_Specular.s_Color;
	baseInfo.s_fPower = nsCsMfcFunc::Control2Float( &m_edit_Power );

	Info.s_Pos = m_NiNodeFar->GetTranslate();
	Info.s_Range = m_NiNodeFar->GetScale()*100.0f;
	Info.CalConstant( Info.s_Range, nsCsMfcFunc::Control2Float( &m_edit_Constant ) );

	CsGBLight* pLight = nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddPointLight( &baseInfo, &Info, false );
	if( baseInfo.s_bEnableLight )
	{
		if( baseInfo.s_bApplyObject == true)
		{
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddObject( pLight );
		}
		if( baseInfo.s_bApplyChar == true )
		{
			g_ModelMng.ApplyLight( pLight->GetNiLight() );		
		}
	}	
}

void _DlgMTLight_Point::__ResetCreateLight()
{
	CsGBLight::sBASE_INFO baseInfo;
	baseInfo.s_eType = CsGBLight::POINT;
	CsGBLight_Point::sINFO Info;

	if( g_pShadow->GetShadowLight() == NULL )
		baseInfo.s_bObjectShadow = true;

	if( m_CreateLight.GetNiLight() == NULL )
		m_CreateLight.Create( &baseInfo, &Info );
	else
		m_CreateLight.Reset( &baseInfo, &Info );

	__ReadLightInfo( &baseInfo, &Info );
}

void _DlgMTLight_Point::__ReadLightInfo( CsGBLight* pLight )
{
	__ReadLightInfo( pLight->GetBaseInfo(), ( (CsGBLight_Point*)pLight )->GetInfo() );

	/*int nLightCount = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetLightCount();
	for( int i=0; i<nLightCount; ++i )
	{
		CsGBLight* pLightDest = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetLight( i );
		if( pLightDest->GetBaseInfo()->s_bObjectShadow == true )
		{
			if( pLightDest != pLight )
			{
				m_chk_ObjectShadow.EnableWindow( FALSE );
				return;
			}
		}
	}
	m_chk_ObjectShadow.EnableWindow( TRUE );*/
}

void _DlgMTLight_Point::__ReadLightInfo( CsGBLight::sBASE_INFO* pBaseInfo, CsGBLight_Point::sINFO* pInfo )
{
	m_chk_TR.SetCheck( pBaseInfo->s_bTRShadow );
	m_chk_Object.SetCheck( pBaseInfo->s_bApplyObject );
	m_chk_Char.SetCheck( pBaseInfo->s_bApplyChar );
	m_chk_ObjectShadow.SetCheck( pBaseInfo->s_bObjectShadow );
	m_chk_DynamicShadow.SetCheck( pBaseInfo->s_bApplyDynamicShadow );

	nsCsMfcFunc::Float2Control( &m_edit_TL[ 0 ], pInfo->s_Pos.x, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_TL[ 1 ], pInfo->s_Pos.y, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_TL[ 2 ], pInfo->s_Pos.z, 1 );

	nsCsMfcFunc::Float2Control( &m_edit_Constant, pInfo->s_C, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_Range, pInfo->s_Range, 1 );

	m_Diffuse.Create( &m_static_Diffuse, pBaseInfo->s_Diffuse );
	m_Ambient.Create( &m_static_Ambient, pBaseInfo->s_Ambient );
	m_Specular.Create( &m_static_Specular, pBaseInfo->s_Specular );

	nsCsMfcFunc::Float2Control( &m_edit_Power, pBaseInfo->s_fPower, 3 );
}

void _DlgMTLight_Point::__SetCenterColor( NiColor color )
{
	( ( NiMaterialProperty* )( (NiNode*)( m_NiNodeCenter->GetAt( 0 ) ) )->GetAt( 0 )->GetProperty( NiMaterialProperty::GetType() ) )->SetEmittance( color );
}

