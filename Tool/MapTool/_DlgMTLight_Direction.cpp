// _DlgMTLight_Direction.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTLight_Direction.h"


// _DlgMTLight_Direction 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTLight_Direction, CDialog)

_DlgMTLight_Direction::_DlgMTLight_Direction(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTLight_Direction::IDD, pParent)
{
	m_NiNode = NULL;
	m_NiNodeCenter = NULL;
	m_pSelectLight = NULL;
}

_DlgMTLight_Direction::~_DlgMTLight_Direction()
{
}

void _DlgMTLight_Direction::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_TR, m_chk_TR);
	DDX_Control(pDX, IDC_CHECK_OBJECT, m_chk_Object);
	DDX_Control(pDX, IDC_CHECK_CHAR, m_chk_Char);
	DDX_Control(pDX, IDC_CHECK_OBJECT_SHADOW, m_chk_ObjectShadow);
	DDX_Control(pDX, IDC_CHECK_DYNAMIC_SHADOW, m_chk_DynamicShadow );
	DDX_Control(pDX, IDC_EDIT_TL_X, m_edit_TL[ 0 ]);
	DDX_Control(pDX, IDC_EDIT_TL_Y, m_edit_TL[ 1 ]);
	DDX_Control(pDX, IDC_EDIT_TL_Z, m_edit_TL[ 2 ]);
	DDX_Control(pDX, IDC_EDIT_ROT_X, m_edit_Rot[ 0 ]);
	DDX_Control(pDX, IDC_EDIT_ROT_Y, m_edit_Rot[ 1 ]);
	DDX_Control(pDX, IDC_EDIT_ROT_Z, m_edit_Rot[ 2 ]);
	DDX_Control(pDX, IDC_EDIT_SCALE, m_edit_Scale);
	DDX_Control(pDX, IDC_EDIT_POWER, m_edit_Power);
	DDX_Control(pDX, IDC_SPIN_TL_X, m_spin_TL[ 0 ]);
	DDX_Control(pDX, IDC_SPIN_TL_Y, m_spin_TL[ 1 ]);
	DDX_Control(pDX, IDC_SPIN_TL_Z, m_spin_TL[ 2 ]);
	DDX_Control(pDX, IDC_SPIN_ROT_X, m_spin_Rot[ 0 ]);
	DDX_Control(pDX, IDC_SPIN_ROT_Y, m_spin_Rot[ 1 ]);
	DDX_Control(pDX, IDC_SPIN_ROT_Z, m_spin_Rot[ 2 ]);
	DDX_Control(pDX, IDC_SPIN_SCALE, m_spin_Scale);
	DDX_Control(pDX, IDC_SPIN_POWER, m_spin_Power);
	DDX_Control(pDX, IDC_STATIC_SL_D, m_static_Diffuse);
	DDX_Control(pDX, IDC_STATIC_SL_A, m_static_Ambient);
	DDX_Control(pDX, IDC_STATIC_SL_S, m_static_Specular);

}


BEGIN_MESSAGE_MAP(_DlgMTLight_Direction, CDialog)
	ON_STN_CLICKED(IDC_STATIC_SL_D, &_DlgMTLight_Direction::OnStnClickedStaticSlD)
	ON_STN_CLICKED(IDC_STATIC_SL_A, &_DlgMTLight_Direction::OnStnClickedStaticSlA)
	ON_STN_CLICKED(IDC_STATIC_SL_S, &_DlgMTLight_Direction::OnStnClickedStaticSlS)
	ON_EN_SETFOCUS(IDC_EDIT_TL_X, &_DlgMTLight_Direction::OnEnSetfocusEditTlX)
	ON_EN_SETFOCUS(IDC_EDIT_TL_Y, &_DlgMTLight_Direction::OnEnSetfocusEditTlY)
	ON_EN_SETFOCUS(IDC_EDIT_TL_Z, &_DlgMTLight_Direction::OnEnSetfocusEditTlZ)
	ON_EN_SETFOCUS(IDC_EDIT_ROT_X, &_DlgMTLight_Direction::OnEnSetfocusEditRotX)
	ON_EN_SETFOCUS(IDC_EDIT_ROT_Y, &_DlgMTLight_Direction::OnEnSetfocusEditRotY)
	ON_EN_SETFOCUS(IDC_EDIT_ROT_Z, &_DlgMTLight_Direction::OnEnSetfocusEditRotZ)
	ON_EN_SETFOCUS(IDC_EDIT_SCALE, &_DlgMTLight_Direction::OnEnSetfocusEditScale)
	ON_EN_SETFOCUS(IDC_EDIT_POWER, &_DlgMTLight_Direction::OnEnSetfocusEditPower)
	ON_EN_CHANGE(IDC_EDIT_ROT_X, &_DlgMTLight_Direction::OnEnChangeEditRotX)
	ON_EN_CHANGE(IDC_EDIT_ROT_Y, &_DlgMTLight_Direction::OnEnChangeEditRotY)
	ON_EN_CHANGE(IDC_EDIT_ROT_Z, &_DlgMTLight_Direction::OnEnChangeEditRotZ)
	ON_EN_CHANGE(IDC_EDIT_TL_X, &_DlgMTLight_Direction::OnEnChangeEditTlX)
	ON_EN_CHANGE(IDC_EDIT_TL_Y, &_DlgMTLight_Direction::OnEnChangeEditTlY)
	ON_EN_CHANGE(IDC_EDIT_TL_Z, &_DlgMTLight_Direction::OnEnChangeEditTlZ)
	ON_EN_CHANGE(IDC_EDIT_SCALE, &_DlgMTLight_Direction::OnEnChangeEditScale)
	ON_EN_CHANGE(IDC_EDIT_POWER, &_DlgMTLight_Direction::OnEnChangeEditPower)
	ON_WM_CTLCOLOR()	
	ON_BN_CLICKED(IDC_CHECK_TR, &_DlgMTLight_Direction::OnBnClickedCheckTr)
	ON_BN_CLICKED(IDC_CHECK_OBJECT_SHADOW, &_DlgMTLight_Direction::OnBnClickedCheckObjectShadow)
	ON_BN_CLICKED(IDC_CHECK_OBJECT, &_DlgMTLight_Direction::OnBnClickedCheckObject)
	ON_BN_CLICKED(IDC_CHECK_CHAR, &_DlgMTLight_Direction::OnBnClickedCheckChar)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_DYNAMIC_SHADOW, &_DlgMTLight_Direction::OnBnClickedCheckDynamicShadow)
END_MESSAGE_MAP()


BOOL _DlgMTLight_Direction::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	m_spin_Scale.SetCsBuddy( &m_edit_Scale, 0.01f, 2 );
	m_spin_Power.SetCsBuddy( &m_edit_Power, 0.001f, 3 );
	for( int i=0; i<3; ++i )
	{
		m_spin_Rot[ i ].SetCsBuddy( &m_edit_Rot[ i ], 0.5f, 2 );		
		m_spin_TL[ i ].SetCsBuddy( &m_edit_TL[ i ], 5, 1 );		
	}

	return TRUE;
}

void _DlgMTLight_Direction::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == FALSE )
	{
		if( m_bCreateMode == true )
		{
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->DeleteLight( &m_CreateLight );			
			g_ModelMng.RemoveLight( m_CreateLight.GetNiLight() );			
		}	
	}
}


HBRUSH _DlgMTLight_Direction::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


void _DlgMTLight_Direction::OnBnClickedCheckTr()
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

void _DlgMTLight_Direction::OnBnClickedCheckObjectShadow()
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

void _DlgMTLight_Direction::OnBnClickedCheckObject()
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

void _DlgMTLight_Direction::OnBnClickedCheckChar()
{
	bool bCheck = ( m_chk_Char.GetCheck() == TRUE );
	if( m_bCreateMode == true )
	{
		m_CreateLight.GetBaseInfo()->s_bApplyChar = bCheck;

		if( bCheck == true )
		{
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddChar( &m_CreateLight );
			g_ModelMng.ApplyLight( m_CreateLight.GetNiLight() );
		}
		else
		{
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveChar( m_pSelectLight );
			g_ModelMng.RemoveLight( m_CreateLight.GetNiLight() );
		}
	}
	else
	{
		assert_cs( m_pSelectLight != NULL );
		m_pSelectLight->GetBaseInfo()->s_bApplyChar = bCheck;

		if( bCheck == true )
		{
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddChar( m_pSelectLight );
			g_ModelMng.ApplyLight( m_pSelectLight->GetNiLight() );
		}
		else
		{
			nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveChar( m_pSelectLight );
			g_ModelMng.RemoveLight( m_pSelectLight->GetNiLight() );
		}
	}

	GET_MV_MAINTAB_DLG->_GetDlgLight()->_ReadInfo( true );
}

void _DlgMTLight_Direction::OnBnClickedCheckDynamicShadow()
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


// _DlgMTLight_Direction 메시지 처리기입니다.

void _DlgMTLight_Direction::OnStnClickedStaticSlD()
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

void _DlgMTLight_Direction::OnStnClickedStaticSlA()
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

void _DlgMTLight_Direction::OnStnClickedStaticSlS()
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


void _DlgMTLight_Direction::OnEnSetfocusEditTlX(){		m_edit_TL[ 0 ].SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Direction::OnEnSetfocusEditTlY(){		m_edit_TL[ 1 ].SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Direction::OnEnSetfocusEditTlZ(){		m_edit_TL[ 2 ].SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Direction::OnEnSetfocusEditRotX(){		m_edit_Rot[ 0 ].SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Direction::OnEnSetfocusEditRotY(){		m_edit_Rot[ 1 ].SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Direction::OnEnSetfocusEditRotZ(){		m_edit_Rot[ 2 ].SetSel( 0, -1, TRUE );	}
void _DlgMTLight_Direction::OnEnSetfocusEditScale(){	m_edit_Scale.SetSel( 0, -1, TRUE );		}
void _DlgMTLight_Direction::OnEnSetfocusEditPower(){	m_edit_Power.SetSel( 0, -1, TRUE );		}

void _DlgMTLight_Direction::OnEnChangeEditRotX()
{
	if( _IsFocus() == true )
	{
		NiMatrix3 mat;
		mat.FromEulerAnglesZXY(	CsD2R( nsCsMfcFunc::Control2Float( &m_edit_Rot[ 2 ] ) ),
			CsD2R( nsCsMfcFunc::Control2Float( &m_edit_Rot[ 0 ] ) ),
			CsD2R( nsCsMfcFunc::Control2Float( &m_edit_Rot[ 1 ] ) )	);

		if( m_bCreateMode == true )
		{
			m_CreateLight.SetRotation( mat );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );			
			m_pSelectLight->SetRotation( mat );
		}
	}
}

void _DlgMTLight_Direction::OnEnChangeEditRotY()
{
	if( _IsFocus() == true )
	{
		NiMatrix3 mat;
		mat.FromEulerAnglesZXY(	CsD2R( nsCsMfcFunc::Control2Float( &m_edit_Rot[ 2 ] ) ),
			CsD2R( nsCsMfcFunc::Control2Float( &m_edit_Rot[ 0 ] ) ),
			CsD2R( nsCsMfcFunc::Control2Float( &m_edit_Rot[ 1 ] ) )	);

		if( m_bCreateMode == true )
		{
			m_CreateLight.SetRotation( mat );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );			
			m_pSelectLight->SetRotation( mat );
		}
	}
}

void _DlgMTLight_Direction::OnEnChangeEditRotZ()
{
	if( _IsFocus() == true )
	{
		NiMatrix3 mat;
		mat.FromEulerAnglesZXY(	CsD2R( nsCsMfcFunc::Control2Float( &m_edit_Rot[ 2 ] ) ),
			CsD2R( nsCsMfcFunc::Control2Float( &m_edit_Rot[ 0 ] ) ),
			CsD2R( nsCsMfcFunc::Control2Float( &m_edit_Rot[ 1 ] ) )	);

		if( m_bCreateMode == true )
		{
			m_CreateLight.SetRotation( mat );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );			
			m_pSelectLight->SetRotation( mat );
		}
	}
}

void _DlgMTLight_Direction::OnEnChangeEditTlX()
{
	if( _IsFocus() == true )
	{
		if( m_bCreateMode == true )
		{
			m_CreateLight.GetInfo()->s_Pos.x = nsCsMfcFunc::Control2Float( &m_edit_TL[ 0 ] );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );			
			m_pSelectLight->GetInfo()->s_Pos.x = nsCsMfcFunc::Control2Float( &m_edit_TL[ 0 ] );
		}
	}
}

void _DlgMTLight_Direction::OnEnChangeEditTlY()
{
	if( _IsFocus() == true )
	{
		if( m_bCreateMode == true )
		{
			m_CreateLight.GetInfo()->s_Pos.y = nsCsMfcFunc::Control2Float( &m_edit_TL[ 1 ] );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );			
			m_pSelectLight->GetInfo()->s_Pos.y = nsCsMfcFunc::Control2Float( &m_edit_TL[ 1 ] );
		}
	}
}

void _DlgMTLight_Direction::OnEnChangeEditTlZ()
{
	if( _IsFocus() == true )
	{
		if( m_bCreateMode == true )
		{
			m_CreateLight.GetInfo()->s_Pos.z = nsCsMfcFunc::Control2Float( &m_edit_TL[ 2 ] );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );			
			m_pSelectLight->GetInfo()->s_Pos.z = nsCsMfcFunc::Control2Float( &m_edit_TL[ 2 ] );
		}
	}
}

void _DlgMTLight_Direction::OnEnChangeEditScale()
{
	if( _IsFocus() == true )
	{
		if( m_bCreateMode == true )
		{
			m_CreateLight.GetInfo()->s_fScale = nsCsMfcFunc::Control2Float( &m_edit_Scale );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );
			m_pSelectLight->GetInfo()->s_fScale = nsCsMfcFunc::Control2Float( &m_edit_Scale );
		}
	}
}

void _DlgMTLight_Direction::OnEnChangeEditPower()
{
	if( _IsFocus() == true )
	{
		if( m_bCreateMode == true )
		{
			m_CreateLight.SetPower( nsCsMfcFunc::Control2Float( &m_edit_Power ) );
		}
		else
		{
			assert_cs( m_pSelectLight != NULL );
			m_pSelectLight->SetPower( nsCsMfcFunc::Control2Float( &m_edit_Power ) );
		}
	}
}

bool _DlgMTLight_Direction::_IsFocus()
{
	CWnd* pWnd = GetFocus();
	for( int i=0; i<3; ++i )
	{
		if( ( pWnd->GetSafeHwnd() == m_edit_TL[ i ].GetSafeHwnd() )||
			( pWnd->GetSafeHwnd() == m_edit_Rot[ i ].GetSafeHwnd() ) )
		{
			return true;
		}
	}

	if( pWnd->GetSafeHwnd() == m_edit_Scale.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_Scale ) < 0.1f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_Scale, 0.1f, 2 );
		}
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

void _DlgMTLight_Direction::_SetCreateMode( bool bCreateMode )
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

void _DlgMTLight_Direction::_SetSelectLight( CsGBLight* pLight )
{
	m_pSelectLight = (CsGBLight_Dir*)pLight;	

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

void _DlgMTLight_Direction::_PickSetPos( NiPoint3 pos )
{
	if( m_bCreateMode == true )
	{
		m_CreateLight.GetInfo()->s_Pos = pos;
	}
	else if( m_pSelectLight != NULL )
	{
		m_pSelectLight->GetInfo()->s_Pos = pos;
	}

	nsCsMfcFunc::Float2Control( &m_edit_TL[ 0 ], pos.x, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_TL[ 1 ], pos.y, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_TL[ 2 ], pos.z, 1 );
}

void _DlgMTLight_Direction::_Delete()
{
	m_Diffuse.Delete();
	m_Ambient.Delete();
	m_Specular.Delete();
	m_NiNode = 0;
	m_NiNodeCenter = 0;
}

void _DlgMTLight_Direction::_ViewInit()
{
	#define MESH_NAME			".\\ToolData\\DirectionLight.nif"
	#define MESH_NAME_CENTER	".\\ToolData\\DirectionLight2.nif"

	{
		NiStream stream;
		bool bLoad = stream.Load( MESH_NAME );
		if( bLoad == false )
		{
			CsMessageBoxA( MB_OK, ".\\ToolData\\%s 가 존재 하지 않습니다. 비정상적으로 종료 될수 있습니다.", MESH_NAME );
			return;
		}

		m_NiNode = (NiNode*)stream.GetObjectAt( 0 );
		m_NiNode->UpdateProperties();
		m_NiNode->Update( 0.0f );
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

void _DlgMTLight_Direction::_PreViewReset()
{
	m_pSelectLight = NULL;
}

void _DlgMTLight_Direction::_Render( CsGBLight_Dir* pLight )
{
	assert_cs( m_NiNode != NULL );

	CsGBLight::sBASE_INFO* pBaseInfo = pLight->GetBaseInfo();
	CsGBLight_Dir::sINFO* pInfo = pLight->GetInfo();

	

	if( m_bCreateMode == false )
	{
		if( m_pSelectLight == pLight )
		{
			m_NiNodeCenter->SetRotate( pInfo->s_Rotation );
			m_NiNodeCenter->SetTranslate( pInfo->s_Pos );			
			__SetCenterColor( NiColor( 1.0f, 0.0f, 0.0f ) );
			m_NiNodeCenter->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeCenter, *g_pCuller );

			m_NiNode->SetRotate( pInfo->s_Rotation );
			m_NiNode->SetTranslate( pInfo->s_Pos );
			m_NiNode->SetScale( pInfo->s_fScale );
			m_NiNode->Update( 0.0f );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNode, *g_pCuller );
		}
		else
		{
			m_NiNodeCenter->SetRotate( pInfo->s_Rotation );
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
			m_NiNodeCenter->SetRotate( pInfo->s_Rotation );
			m_NiNodeCenter->SetTranslate( pInfo->s_Pos );			
			__SetCenterColor( NiColor( 0.0f, 0.0f, 1.0f ) );
			m_NiNodeCenter->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeCenter, *g_pCuller );

			m_NiNode->SetRotate( pInfo->s_Rotation );
			m_NiNode->SetTranslate( pInfo->s_Pos );
			m_NiNode->SetScale( pInfo->s_fScale );
			m_NiNode->Update( 0.0f );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNode, *g_pCuller );
		}
		else
		{
			m_NiNodeCenter->SetRotate( pInfo->s_Rotation );
			m_NiNodeCenter->SetTranslate( pInfo->s_Pos );			
			__SetCenterColor( NiColor( 0.5f, 0.5f, 0.5f ) );
			m_NiNodeCenter->Update( 0.0f, false );
			NiDrawScene( CAMERA_ST.GetCameraObj(), m_NiNodeCenter, *g_pCuller );
		}
	}	
}

void _DlgMTLight_Direction::_PostRenderView()
{
	if( m_bCreateMode == false )
		return;

	_Render( &m_CreateLight );
}

void _DlgMTLight_Direction::_AddLight()
{
	assert_cs( m_bCreateMode == true );

	CsGBLight::sBASE_INFO baseInfo;
	CsGBLight_Dir::sINFO Info;
	baseInfo.s_eType = CsGBLight::DIRECTION;
	baseInfo.s_bTRShadow = ( m_chk_TR.GetCheck() == BST_CHECKED );
	baseInfo.s_bObjectShadow = ( m_chk_ObjectShadow.GetCheck() == BST_CHECKED );
	baseInfo.s_bApplyChar = ( m_chk_Char.GetCheck() == BST_CHECKED );
	baseInfo.s_bApplyObject = ( m_chk_Object.GetCheck() == BST_CHECKED );
	baseInfo.s_bApplyDynamicShadow = (m_chk_DynamicShadow.GetCheck() == BST_CHECKED );

	baseInfo.s_Diffuse = m_Diffuse.s_Color;
	baseInfo.s_Ambient = m_Ambient.s_Color;
	baseInfo.s_Specular = m_Specular.s_Color;
	baseInfo.s_fPower = nsCsMfcFunc::Control2Float( &m_edit_Power );

	Info.s_fScale = m_NiNode->GetScale();
	Info.s_Pos = m_NiNode->GetTranslate();
	Info.s_Rotation = m_NiNode->GetRotate();

	CsGBLight* pLight = nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddDirectionLight( &baseInfo, &Info, false );
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

void _DlgMTLight_Direction::__ResetCreateLight()
{
	CsGBLight::sBASE_INFO baseInfo;
	baseInfo.s_eType = CsGBLight::DIRECTION;
	CsGBLight_Dir::sINFO Info;

	if( g_pShadow->GetShadowLight() == NULL )
		baseInfo.s_bObjectShadow = true;

	if( m_CreateLight.GetNiLight() == NULL )
		m_CreateLight.Create( &baseInfo, &Info );
	else
		m_CreateLight.Reset( &baseInfo, &Info );

	__ReadLightInfo( &baseInfo, &Info );
}

void _DlgMTLight_Direction::__ReadLightInfo( CsGBLight* pLight )
{
	__ReadLightInfo( pLight->GetBaseInfo(), ( (CsGBLight_Dir*)pLight )->GetInfo() );

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

void _DlgMTLight_Direction::__ReadLightInfo( CsGBLight::sBASE_INFO* pBaseInfo, CsGBLight_Dir::sINFO* pInfo )
{
	m_chk_TR.SetCheck( pBaseInfo->s_bTRShadow );
	m_chk_Object.SetCheck( pBaseInfo->s_bApplyObject );
	m_chk_Char.SetCheck( pBaseInfo->s_bApplyChar );
	m_chk_ObjectShadow.SetCheck( pBaseInfo->s_bObjectShadow );
	m_chk_DynamicShadow.SetCheck( pBaseInfo->s_bApplyDynamicShadow );

	nsCsMfcFunc::Float2Control( &m_edit_Scale, pInfo->s_fScale, 2 );	

	nsCsMfcFunc::Float2Control( &m_edit_TL[ 0 ], pInfo->s_Pos.x, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_TL[ 1 ], pInfo->s_Pos.y, 1 );
	nsCsMfcFunc::Float2Control( &m_edit_TL[ 2 ], pInfo->s_Pos.z, 1 );

	float x, y, z;
	pInfo->s_Rotation.ToEulerAnglesZXY( z, x, y );
	nsCsMfcFunc::Float2Control( &m_edit_Rot[ 0 ], CsR2D( x ), 2 );
	nsCsMfcFunc::Float2Control( &m_edit_Rot[ 1 ], CsR2D( y ), 2 );
	nsCsMfcFunc::Float2Control( &m_edit_Rot[ 2 ], CsR2D( z ), 2 );

	m_Diffuse.Create( &m_static_Diffuse, pBaseInfo->s_Diffuse );
	m_Ambient.Create( &m_static_Ambient, pBaseInfo->s_Ambient );
	m_Specular.Create( &m_static_Specular, pBaseInfo->s_Specular );

	nsCsMfcFunc::Float2Control( &m_edit_Power, pBaseInfo->s_fPower, 3 );	
}

void _DlgMTLight_Direction::__SetCenterColor( NiColor color )
{
	( ( NiMaterialProperty* )( m_NiNodeCenter->GetAt( 0 ) )->GetProperty( NiMaterialProperty::GetType() ) )->SetEmittance( color );
}



