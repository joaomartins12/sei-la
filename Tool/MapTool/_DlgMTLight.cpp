// _DlgMTLight.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTLight.h"


// _DlgMTLight 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTLight, CDialog)

_DlgMTLight::_DlgMTLight(CWnd* pParent /*=NULL*/)	
	: _DlgBase(_DlgMTLight::IDD, pParent)
{
	m_bListLightLock = false;
}

_DlgMTLight::~_DlgMTLight()
{
}

void _DlgMTLight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LIGHT, m_list_Light);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_combo_CMType);
	DDX_Control(pDX, IDC_CREATEMODE, m_btn_CreateMode);
	DDX_Control(pDX, IDC_CM_OK, m_btn_CMOk);
	DDX_Control(pDX, IDC_CM_CANCEL, m_btn_CMCancel);	
	DDX_Control(pDX, IDC_CHECK_SETPOS, m_chk_SetPos);
}


BEGIN_MESSAGE_MAP(_DlgMTLight, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CM_CANCEL, &_DlgMTLight::OnBnClickedCmCancel)
	ON_BN_CLICKED(IDC_CREATEMODE, &_DlgMTLight::OnBnClickedCreatemode)
	ON_BN_CLICKED(IDC_CM_OK, &_DlgMTLight::OnBnClickedCmOk)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &_DlgMTLight::OnCbnSelchangeComboType)	
	ON_NOTIFY(NM_CLICK, IDC_LIST_LIGHT, &_DlgMTLight::OnNMClickListLight)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LIGHT, &_DlgMTLight::OnLvnItemchangedListLight)
	ON_BN_CLICKED(IDC_BUTTON_LISTDEL, &_DlgMTLight::OnBnClickedButtonListdel)	
	ON_BN_CLICKED(IDC_BUTTON_LIST_COPY, &_DlgMTLight::OnBnClickedButtonListCopy)
END_MESSAGE_MAP()


// _DlgMTLight 메시지 처리기입니다.

BOOL _DlgMTLight::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	CRect rc, rcChild;
	GetWindowRect( &rc );	
	m_DlgDirection.Create( _DlgMTLight_Direction::IDD, this );
	m_DlgDirection.GetWindowRect( &rcChild );
	m_DlgDirection.SetWindowPos( NULL, 0, 190, rc.Width(), rcChild.Height(), 0 );
	m_DlgDirection.ShowWindow( SW_HIDE );

	m_DlgPoint.Create( _DlgMTLight_Point::IDD, this );
	m_DlgPoint.SetWindowPos( NULL, 0, 190, rc.Width(), rcChild.Height(), 0 );
	m_DlgPoint.ShowWindow( SW_HIDE );


	DWORD dwExStyle = m_list_Light.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES;
	m_list_Light.SetExtendedStyle( dwExStyle );

	m_list_Light.InsertColumn( 0, L"타입", LVCFMT_RIGHT, 120 );	
	m_list_Light.InsertColumn( 1, L"지형", LVCFMT_CENTER, 50 );
	m_list_Light.InsertColumn( 2, L"오브젝트 그림자", LVCFMT_CENTER, 80 );
	m_list_Light.InsertColumn( 3, L"오브젝트", LVCFMT_CENTER, 80 );
	m_list_Light.InsertColumn( 4, L"케릭터", LVCFMT_CENTER, 60 );
	m_list_Light.InsertColumn( 5, L"동적그림자", LVCFMT_CENTER, 60 );

	m_combo_CMType.SetCurSel( 0 );

	__SetNormalMode();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgMTLight::OnBnClickedButtonListdel()
{
	POSITION pos = m_list_Light.GetFirstSelectedItemPosition();
	int nItem = m_list_Light.GetNextSelectedItem( pos );

	if( nItem >= 0 )
	{
		CsGBLight* pLight = (CsGBLight*)m_list_Light.GetItemData( nItem );
		m_list_Light.DeleteItem( nItem );
		
		if( pLight->GetBaseInfo()->s_bApplyChar )
		{
			g_ModelMng.RemoveLight( pLight->GetNiLight() );			
		}
		nsCsGBTerrain::g_pCurRoot->GetLightMng()->DeleteLight( pLight );
		m_DlgDirection._SetSelectLight( NULL );
		m_DlgPoint._SetSelectLight( NULL );
	}
}


void _DlgMTLight::OnBnClickedButtonListCopy()
{
	POSITION pos = m_list_Light.GetFirstSelectedItemPosition();
	int nItem = m_list_Light.GetNextSelectedItem( pos );

	if( nItem >= 0 )
	{
		CsGBLight* pLight = (CsGBLight*)m_list_Light.GetItemData( nItem );
		switch( pLight->GetType() )
		{
		case CsGBLight::DIRECTION:
			{
				CsGBLight_Dir* pDir = (CsGBLight_Dir*)pLight;

				CsGBLight::sBASE_INFO baseInfo;
				memcpy( &baseInfo, pDir->GetBaseInfo(), sizeof( CsGBLight::sBASE_INFO ) );
				if( g_pShadow->GetShadowLight() != NULL )
					baseInfo.s_bObjectShadow = false;

				CsGBLight_Dir::sINFO info;
				memcpy( &info, pDir->GetInfo(), sizeof( CsGBLight_Dir::sINFO ) );
				info.s_Pos = NiPoint3( 0, 0, 8000.0f );

				CsGBLight* pCreateLight = nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddDirectionLight( &baseInfo, &info, false );
				if( baseInfo.s_bEnableLight )
				{
					if( baseInfo.s_bApplyObject == true)
					{
						nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddObject( pCreateLight );
					}
					if( baseInfo.s_bApplyChar == true )
					{
						g_ModelMng.ApplyLight( pCreateLight->GetNiLight() );
					}
				}				
			}
			break;
		case CsGBLight::POINT:
			{
				CsGBLight_Point* pPoint = (CsGBLight_Point*)pLight;

				CsGBLight::sBASE_INFO baseInfo;
				memcpy( &baseInfo, pPoint->GetBaseInfo(), sizeof( CsGBLight::sBASE_INFO ) );
				if( g_pShadow->GetShadowLight() != NULL )
					baseInfo.s_bObjectShadow = false;

				CsGBLight_Point::sINFO info;
				memcpy( &info, pPoint->GetInfo(), sizeof( CsGBLight_Point::sINFO ) );
				info.s_Pos = NiPoint3::ZERO;

				CsGBLight* pCreateLight = nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddPointLight( &baseInfo, &info, false );
				if( baseInfo.s_bEnableLight )
				{
					if( baseInfo.s_bApplyObject == true)
					{
						nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddObject( pCreateLight );
					}
					if( baseInfo.s_bApplyChar == true )
					{
						g_ModelMng.ApplyLight( pCreateLight->GetNiLight() );
					}
				}				
			}
			break;
		}

		_ReadInfo( true );
	}
}


void _DlgMTLight::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);
	__SetNormalMode();
}

void _DlgMTLight::OnCbnSelchangeComboType()
{
	switch( m_combo_CMType.GetCurSel() )
	{
	case LT_DIRECTION:
		m_DlgDirection.ShowWindow( SW_SHOW );		
		m_DlgPoint.ShowWindow( SW_HIDE );
		break;
	case LT_POINT:
		m_DlgDirection.ShowWindow( SW_HIDE );
		m_DlgPoint.ShowWindow( SW_SHOW );
		break;
	default:
		assert_cs( false );
	}
}


void _DlgMTLight::OnBnClickedCmOk()
{
	switch( m_combo_CMType.GetCurSel() )
	{
	case LT_DIRECTION:
		{
			m_DlgDirection._AddLight();
		}
		break;
	case LT_POINT:
		{
			m_DlgPoint._AddLight();
		}
		break;
	default:
		assert_cs( false );
	}

	_ReadInfo( false );
	__SetNormalMode();
}

void _DlgMTLight::OnBnClickedCmCancel()
{
	__SetNormalMode();
}

void _DlgMTLight::OnBnClickedCreatemode()
{
	__SetCreateMode();
}

void _DlgMTLight::OnLvnItemchangedListLight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if( m_bListLightLock == false )
	{
		// 체크 박스 정보 다시 읽어 온다
		int nCount = m_list_Light.GetItemCount();
		CsGBLight* pLight;
		for( int i=0; i<nCount; ++i )
		{
			pLight = (CsGBLight*)m_list_Light.GetItemData( i );
			if( pLight )
			{
				bool bCheck = ( m_list_Light.GetCheck( i ) == BST_CHECKED );
				if( bCheck != pLight->GetBaseInfo()->s_bEnableLight )
				{
					pLight->GetBaseInfo()->s_bEnableLight = bCheck;

					if( bCheck == true )
					{
						if( pLight->GetBaseInfo()->s_bApplyObject )
							nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddObject( pLight );
						else
							nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveObject( pLight );

						if( pLight->GetBaseInfo()->s_bApplyChar )
						{
							nsCsGBTerrain::g_pCurRoot->GetLightMng()->AddChar( pLight );
							g_ModelMng.ApplyLight( pLight->GetNiLight() );
						}
						else
						{
							nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveChar( pLight );
							g_ModelMng.RemoveLight( pLight->GetNiLight() );
						}
					}
					else
					{
						nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveObject( pLight );
						nsCsGBTerrain::g_pCurRoot->GetLightMng()->RemoveChar( pLight );
						g_ModelMng.RemoveLight( pLight->GetNiLight() );
					}					
				}
				
			}
		}
	}	

	*pResult = 0;
}


void _DlgMTLight::OnNMClickListLight(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_Light.GetFirstSelectedItemPosition();
	int nItem = m_list_Light.GetNextSelectedItem( pos );
	
	if( nItem >= 0 )
	{
		CsGBLight* pLight = (CsGBLight*)m_list_Light.GetItemData( nItem );
		switch( pLight->GetType() )
		{
		case CsGBLight::DIRECTION:
			m_DlgDirection._SetSelectLight( pLight );
			m_DlgPoint._SetSelectLight( NULL );
			break;
		case CsGBLight::POINT:
			m_DlgDirection._SetSelectLight( NULL );
			m_DlgPoint._SetSelectLight( pLight );
			break;
		default:
			assert_cs( false );
		}

		m_chk_SetPos.ShowWindow( SW_SHOW );
	}
	else
	{
		m_DlgDirection._SetSelectLight( NULL );
		m_DlgPoint._SetSelectLight( NULL );
		m_chk_SetPos.ShowWindow( SW_HIDE );
	}
	
	*pResult = 0;
}





void _DlgMTLight::_Delete()
{
	m_DlgDirection._Delete();
	m_DlgPoint._Delete();
}

void _DlgMTLight::_ViewInit()
{
	m_DlgDirection._ViewInit();
	m_DlgPoint._ViewInit();
}

void _DlgMTLight::_PreSave( const char* cFile, bool bAutoSave )
{
	if( g_Config.GetEnvironment()->s_bWriteCharLight == false )
		return;

	if( bAutoSave == true )
		return;

	assert_cs( CsGBLight::DIRECTION == 0 );
	assert_cs( CsGBLight::POINT == 1 );

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	// 현재 맵ID 라이트들 제거
	nsCsMapTable::g_pMapCharLightMng->DeleteGroup( dwMapID );

	// 케릭터 전용 라이트만 추가
	CsGBLightMng* pLightMng = nsCsGBTerrain::g_pCurRoot->GetLightMng();
	int nLight = pLightMng->GetLightCount();

	CsMapCharLight::sINFO_DIR infoDir;
	CsMapCharLight::sINFO_POINT infoPoint;
	infoDir.s_dwMapID = dwMapID;
	infoPoint.s_dwMapID = dwMapID;
	for( int i=0; i<nLight; ++i )
	{
		CsGBLight* pLight = pLightMng->GetLight( i );

		if( pLightMng->IsOnlyCharLight( pLight ) == false )
			continue;

		if( pLight->GetBaseInfo()->s_eType == CsGBLight::DIRECTION )
		{
			CsGBLight_Dir* pLightDir = (CsGBLight_Dir*)pLight;
			infoDir.s_bEnableLight = pLightDir->GetBaseInfo()->s_bEnableLight;
			infoDir.s_bChar = pLightDir->GetBaseInfo()->s_bApplyChar;
			infoDir.s_bShadow = pLightDir->GetBaseInfo()->s_bApplyDynamicShadow;
			infoDir.s_nType = pLightDir->GetBaseInfo()->s_eType;
			infoDir.s_fDiffuse[ 0 ] = pLightDir->GetBaseInfo()->s_Diffuse.r;
			infoDir.s_fDiffuse[ 1 ] = pLightDir->GetBaseInfo()->s_Diffuse.g;
			infoDir.s_fDiffuse[ 2 ] = pLightDir->GetBaseInfo()->s_Diffuse.b;
			infoDir.s_fAmbient[ 0 ] = pLightDir->GetBaseInfo()->s_Ambient.r;
			infoDir.s_fAmbient[ 1 ] = pLightDir->GetBaseInfo()->s_Ambient.g;
			infoDir.s_fAmbient[ 2 ] = pLightDir->GetBaseInfo()->s_Ambient.b;
			infoDir.s_fSpecular[ 0 ] = pLightDir->GetBaseInfo()->s_Specular.r;
			infoDir.s_fSpecular[ 1 ] = pLightDir->GetBaseInfo()->s_Specular.g;
			infoDir.s_fSpecular[ 2 ] = pLightDir->GetBaseInfo()->s_Specular.b;
			infoDir.s_fPower = pLightDir->GetBaseInfo()->s_fPower;
			infoDir.s_fRot[ 0 ][ 0 ] = pLightDir->GetInfo()->s_Rotation.GetEntry( 0, 0 );
			infoDir.s_fRot[ 0 ][ 1 ] = pLightDir->GetInfo()->s_Rotation.GetEntry( 0, 1 );
			infoDir.s_fRot[ 0 ][ 2 ] = pLightDir->GetInfo()->s_Rotation.GetEntry( 0, 2 );
			infoDir.s_fRot[ 1 ][ 0 ] = pLightDir->GetInfo()->s_Rotation.GetEntry( 1, 0 );
			infoDir.s_fRot[ 1 ][ 1 ] = pLightDir->GetInfo()->s_Rotation.GetEntry( 1, 1 );
			infoDir.s_fRot[ 1 ][ 2 ] = pLightDir->GetInfo()->s_Rotation.GetEntry( 1, 2 );
			infoDir.s_fRot[ 2 ][ 0 ] = pLightDir->GetInfo()->s_Rotation.GetEntry( 2, 0 );
			infoDir.s_fRot[ 2 ][ 1 ] = pLightDir->GetInfo()->s_Rotation.GetEntry( 2, 1 );
			infoDir.s_fRot[ 2 ][ 2 ] = pLightDir->GetInfo()->s_Rotation.GetEntry( 2, 2 );
			infoDir.s_fPos[ 0 ] = pLightDir->GetInfo()->s_Pos.x;
			infoDir.s_fPos[ 1 ] = pLightDir->GetInfo()->s_Pos.y;
			infoDir.s_fPos[ 2 ] = pLightDir->GetInfo()->s_Pos.z;
			infoDir.s_fScale = pLightDir->GetInfo()->s_fScale;

			nsCsMapTable::g_pMapCharLightMng->GetGroup( dwMapID )->Add( &infoDir );
		}
		else
		{
			assert_cs( pLight->GetBaseInfo()->s_eType == CsGBLight::POINT );
			CsGBLight_Point* pLightPoint = (CsGBLight_Point*)pLight;
			infoPoint.s_bEnableLight = pLightPoint->GetBaseInfo()->s_bEnableLight;
			infoPoint.s_bChar = pLightPoint->GetBaseInfo()->s_bApplyChar;
			infoPoint.s_bShadow = pLightPoint->GetBaseInfo()->s_bApplyDynamicShadow;
			infoPoint.s_nType = pLightPoint->GetBaseInfo()->s_eType;
			infoPoint.s_fDiffuse[ 0 ] = pLightPoint->GetBaseInfo()->s_Diffuse.r;
			infoPoint.s_fDiffuse[ 1 ] = pLightPoint->GetBaseInfo()->s_Diffuse.g;
			infoPoint.s_fDiffuse[ 2 ] = pLightPoint->GetBaseInfo()->s_Diffuse.b;
			infoPoint.s_fAmbient[ 0 ] = pLightPoint->GetBaseInfo()->s_Ambient.r;
			infoPoint.s_fAmbient[ 1 ] = pLightPoint->GetBaseInfo()->s_Ambient.g;
			infoPoint.s_fAmbient[ 2 ] = pLightPoint->GetBaseInfo()->s_Ambient.b;
			infoPoint.s_fSpecular[ 0 ] = pLightPoint->GetBaseInfo()->s_Specular.r;
			infoPoint.s_fSpecular[ 1 ] = pLightPoint->GetBaseInfo()->s_Specular.g;
			infoPoint.s_fSpecular[ 2 ] = pLightPoint->GetBaseInfo()->s_Specular.b;
			infoPoint.s_fPower = pLightPoint->GetBaseInfo()->s_fPower;
			infoPoint.s_Pos[ 0 ] = pLightPoint->GetInfo()->s_Pos.x;
			infoPoint.s_Pos[ 1 ] = pLightPoint->GetInfo()->s_Pos.y;
			infoPoint.s_Pos[ 2 ] = pLightPoint->GetInfo()->s_Pos.z;
			infoPoint.s_C = pLightPoint->GetInfo()->s_C;
			infoPoint.s_L = pLightPoint->GetInfo()->s_L;
			infoPoint.s_Q = pLightPoint->GetInfo()->s_Q;
			infoPoint.s_Range = pLightPoint->GetInfo()->s_Range;

			nsCsMapTable::g_pMapCharLightMng->GetGroup( dwMapID )->Add( &infoPoint );
		}
	}
}

void _DlgMTLight::_PreViewReset()
{
	m_DlgDirection._PreViewReset();
	m_DlgPoint._PreViewReset();
}

void _DlgMTLight::_PostViewReset()
{
	_ReadInfo( false );
}

void _DlgMTLight::_PostRenderView()
{
	int nCount = m_list_Light.GetItemCount();
	CsGBLight* pLight;
	for( int i=0; i<nCount; ++i )
	{
		pLight = (CsGBLight*)m_list_Light.GetItemData( i );

		if( pLight->GetBaseInfo()->s_bEnableLight == false )
			continue;

		switch( pLight->GetType() )
		{
		case CsGBLight::DIRECTION:
			m_DlgDirection._Render( (CsGBLight_Dir*)pLight );
			break;
		case CsGBLight::POINT:
			m_DlgPoint._Render( (CsGBLight_Point*)pLight );
			break;
		default:
			assert_cs( false );
		}
	}

	switch( m_combo_CMType.GetCurSel() )
	{
	case LT_DIRECTION:
		m_DlgDirection._PostRenderView();
		break;
	case LT_POINT:
		m_DlgPoint._PostRenderView();
		break;
	default:
		assert_cs( false );
	}	
}

void _DlgMTLight::_OnLButtonDown( CPoint pt )
{
	if( m_chk_SetPos.GetCheck() == BST_CHECKED )
	{
		NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
		if( pRecord == NULL )
			return;

		m_chk_SetPos.SetCheck( BST_UNCHECKED );
		NiPoint3 pos = pRecord->GetIntersection();
		m_DlgDirection._PickSetPos( pos );
		m_DlgPoint._PickSetPos( pos );
	}
}

void _DlgMTLight::__SetNormalMode()
{
	m_DlgDirection._SetCreateMode( false );
	m_DlgPoint._SetCreateMode( false );
	
	m_combo_CMType.ShowWindow( SW_HIDE );
	m_btn_CMOk.ShowWindow( SW_HIDE );
	m_btn_CMCancel.ShowWindow( SW_HIDE );
	m_btn_CreateMode.ShowWindow( SW_SHOW );
	m_list_Light.EnableWindow( TRUE );

	m_DlgDirection._ResetSelect();
	m_DlgPoint._ResetSelect();

	if( m_DlgDirection._IsSelect() || m_DlgPoint._IsSelect() )
		m_chk_SetPos.ShowWindow( SW_SHOW );
	else
		m_chk_SetPos.ShowWindow( SW_HIDE );
}

void _DlgMTLight::__SetCreateMode()
{
	m_DlgDirection._SetCreateMode( true );
	m_DlgPoint._SetCreateMode( true );

	m_chk_SetPos.ShowWindow( SW_SHOW );
	m_combo_CMType.ShowWindow( SW_SHOW );
	m_btn_CMOk.ShowWindow( SW_SHOW );
	m_btn_CMCancel.ShowWindow( SW_SHOW );
	m_btn_CreateMode.ShowWindow( SW_HIDE );
	m_list_Light.EnableWindow( FALSE );

	OnCbnSelchangeComboType();
}

void _DlgMTLight::_ReadInfo( bool bSelectKeep )
{
	int nItem;

	if( bSelectKeep == true )
	{
		POSITION pos = m_list_Light.GetFirstSelectedItemPosition();
		nItem = m_list_Light.GetNextSelectedItem( pos );
	}	

	m_list_Light.DeleteAllItems();

	m_bListLightLock = true;

	int nListIndex = 0;
	std::list< CsGBLight* >* pList = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetLightList();
	std::list< CsGBLight* >::iterator it = pList->begin();
	for( ; it!=pList->end(); ++it )
	{
		switch( (*it)->GetType() )
		{
		case CsGBLight::DIRECTION:
			m_list_Light.InsertItem( nListIndex, L"Direction" );
			break;
		case CsGBLight::POINT:
			m_list_Light.InsertItem( nListIndex, L"Point" );
			break;
		}

		m_list_Light.SetItem( nListIndex, 1, LVIF_TEXT, ( (*it)->GetBaseInfo()->s_bTRShadow == true ) ? L"O" : L"x", 0, 0, 0, 0  );
		m_list_Light.SetItem( nListIndex, 2, LVIF_TEXT, ( (*it)->GetBaseInfo()->s_bObjectShadow == true ) ? L"O" : L"x", 0, 0, 0, 0  );
		m_list_Light.SetItem( nListIndex, 3, LVIF_TEXT, ( (*it)->GetBaseInfo()->s_bApplyObject == true ) ? L"O" : L"x", 0, 0, 0, 0  );
		m_list_Light.SetItem( nListIndex, 4, LVIF_TEXT, ( (*it)->GetBaseInfo()->s_bApplyChar == true ) ? L"O" : L"x", 0, 0, 0, 0  );
		m_list_Light.SetItem( nListIndex, 5, LVIF_TEXT, ( (*it)->GetBaseInfo()->s_bApplyDynamicShadow == true ) ? L"O" : L"x", 0, 0, 0, 0  );
		m_list_Light.SetItemData( nListIndex, (DWORD_PTR)( *it ) );
		m_list_Light.SetCheck( nListIndex, (*it)->GetBaseInfo()->s_bEnableLight );
		++nListIndex;
	}

	if( bSelectKeep == true )
	{
		m_list_Light.SetSelectionMark( nItem );
	}

	m_bListLightLock = false;
}



