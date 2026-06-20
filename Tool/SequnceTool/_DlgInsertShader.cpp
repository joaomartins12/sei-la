// _DlgInsertShader.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SequnceTool.h"
#include "_DlgInsertShader.h"


// _DlgInsertShader 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgInsertShader, CDialog)

_DlgInsertShader::_DlgInsertShader( CsC_AvObject* pTarget, CsModelData::sSHADER* pTextKeyShader, CWnd* pParent /*=NULL*/)
	: CDialog(_DlgInsertShader::IDD, pParent)
{
	m_pTextKeyShader = pTextKeyShader;
	m_pTarget = pTarget;
}

_DlgInsertShader::~_DlgInsertShader()
{
}

void _DlgInsertShader::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_INT_VAL1, m_static_Int1);
	DDX_Control(pDX, IDC_STATIC_FLT_VAL1, m_static_float1);
	DDX_Control(pDX, IDC_STATIC_FLT_VAL2, m_static_float2);
	DDX_Control(pDX, IDC_COMBO_SHADER, m_cmb_Type);
	DDX_Control(pDX, IDC_STATIC_FLT_VAL3, m_static_float3);
	DDX_Control(pDX, IDC_STATIC_FLT_VAL4, m_static_float4);
	DDX_Control(pDX, IDC_STATIC_STR_VAL1, m_static_Str1);
	DDX_Control(pDX, IDC_EDIT_INT_1, m_edit_Int[ 0 ]);
	DDX_Control(pDX, IDC_EDIT_FLOAT_1, m_edit_Float[ 0 ]);
	DDX_Control(pDX, IDC_EDIT_FLOAT_2, m_edit_Float[ 1 ]);
	DDX_Control(pDX, IDC_EDIT_FLOAT_3, m_edit_Float[ 2 ]);
	DDX_Control(pDX, IDC_EDIT_FLOAT_4, m_edit_Float[ 3 ]);
	DDX_Control(pDX, IDC_EDIT_STR_1, m_edit_Str[ 0 ]);
	DDX_Control(pDX, IDC_BUTTON_STR_1, m_btn_Str[ 0 ]);
	DDX_Control(pDX, IDC_LIST_OBJECT, m_list_Object);
}


BEGIN_MESSAGE_MAP(_DlgInsertShader, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SHADER, &_DlgInsertShader::OnCbnSelchangeComboShader)
	ON_BN_CLICKED(IDC_BUTTON_OK, &_DlgInsertShader::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_STR_1, &_DlgInsertShader::OnBnClickedButtonStr1)
END_MESSAGE_MAP()


// _DlgInsertShader 메시지 처리기입니다.

BOOL _DlgInsertShader::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwExStyle = m_list_Object.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Object.SetExtendedStyle( dwExStyle );
	m_list_Object.InsertColumn( 0, L"Name", LVCFMT_LEFT, 130 );
	m_list_Object.InsertColumn( 1, L"Parent", LVCFMT_LEFT, 130 );
	m_list_Object.InsertColumn( 2, L"Skin", LVCFMT_CENTER, 40 );
	__CheckListObject();	

	m_cmb_Type.InsertString( 0, L"알파텍스쳐" );
	m_cmb_Type.InsertString( 1, L"블러" );

	if( m_pTextKeyShader->s_eShaderType == CsC_LoadSequence::sTEXTKEY_SHADER::ST_NONE )
		m_cmb_Type.SetCurSel( 0 );
	else
		m_cmb_Type.SetCurSel( m_pTextKeyShader->s_eShaderType - 1 );

	OnCbnSelchangeComboShader();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgInsertShader::__CheckListObject()
{
	m_list_Object.DeleteAllItems();

	TCHAR sz[ MAX_PATH ];
	NiFixedString str;

	CsNodeObj* pNode = m_pTarget->GetCsNode();
	int nCnt = pNode->m_vpGeom.Size();
	for( int i=0; i<nCnt; ++i )
	{
		CGeometry* pGeom = pNode->m_vpGeom[ i ];

		str = pGeom->m_pGeometry->GetName();
		M2W( sz, str, MAX_PATH );
		m_list_Object.InsertItem( i, sz );

		if( pGeom->m_pGeometry->GetParent() )
		{
			str = pGeom->m_pGeometry->GetParent()->GetName();
			M2W( sz, str, MAX_PATH );
			m_list_Object.SetItemText( i, 1, sz );			
		}
		else
		{
			m_list_Object.SetItemText( i, 1, L"부모 없음" );
		}

		NiSkinInstance* pSkin =	pGeom->m_pGeometry->GetSkinInstance();
		if( pSkin && pSkin->GetSkinPartition() )
		{
			m_list_Object.SetItemText( i, 2, L"O" );
		}
		else
		{
			m_list_Object.SetItemText( i, 2, L"X" );
		}

		if( strcmp( pGeom->m_pGeometry->GetName(), m_pTextKeyShader->s_cApplyObjectName ) == 0 )
			m_list_Object.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
	}
}

int _DlgInsertShader::__GetSelectListObject()
{
	int nCnt = m_list_Object.GetItemCount();
	for( int i=0; i<nCnt; ++i )
	{
		if( m_list_Object.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

void _DlgInsertShader::OnCbnSelchangeComboShader()
{
	m_list_Object.ShowWindow( SW_HIDE );
	for( int i=0; i<VAL_INT_CNT; ++i )
		m_edit_Int[ i ].ShowWindow( SW_HIDE );
	for( int i=0; i<VAL_FLOAT_CNT; ++i )
		m_edit_Float[ i ].ShowWindow( SW_HIDE );
	for( int i=0; i<VAL_STR_CNT; ++i )
	{
		m_edit_Str[ i ].ShowWindow( SW_HIDE );
		m_btn_Str[ i ].ShowWindow( SW_HIDE );
	}

	int nSel = m_cmb_Type.GetCurSel();
	switch( nSel )
	{
	case 0:
		m_static_Int1.SetWindowText( L"사용안함" );
		m_static_float1.SetWindowText( L"사용안함" );
		m_static_float2.SetWindowText( L"사용안함" );
		m_static_float3.SetWindowText( L"사용안함" );
		m_static_float4.SetWindowText( L"사용안함" );
		m_static_Str1.SetWindowText( L"사용안함" );
		break;

	case 1:
		{
			m_static_Int1.SetWindowText( L"라이프프레임" );
			m_static_float1.SetWindowText( L"색비율" );
			m_static_float2.SetWindowText( L"시작알파" );
			m_static_float3.SetWindowText( L"종료알파" );
			m_static_float4.SetWindowText( L"사용안함" );
			m_static_Str1.SetWindowText( L"사용안함" );

			m_list_Object.ShowWindow( SW_SHOW );
			m_edit_Int[ 0 ].ShowWindow( SW_SHOW );
			m_edit_Float[ 0 ].ShowWindow( SW_SHOW );
			m_edit_Float[ 1 ].ShowWindow( SW_SHOW );
			m_edit_Float[ 2 ].ShowWindow( SW_SHOW );			

			if( m_pTextKeyShader->s_nValue1 == 0 )
			{
				m_pTextKeyShader->s_nValue1 = 5;
				m_pTextKeyShader->s_fValue1 = 1.0f;
				m_pTextKeyShader->s_fValue2 = 0.7f;
				m_pTextKeyShader->s_fValue3 = 0.0f;			
			}

			nsCsMfcFunc::Int2Control( &m_edit_Int[ 0 ], m_pTextKeyShader->s_nValue1 );
			nsCsMfcFunc::Float2Control( &m_edit_Float[ 0 ], m_pTextKeyShader->s_fValue1, 1 );
			nsCsMfcFunc::Float2Control( &m_edit_Float[ 1 ], m_pTextKeyShader->s_fValue2, 1 );
			nsCsMfcFunc::Float2Control( &m_edit_Float[ 2 ], m_pTextKeyShader->s_fValue3, 1 );
		}		
		break;

	default:
		assert_cs( false );
	}
}

void _DlgInsertShader::OnBnClickedButtonStr1()
{
#define SHADER_STR_INIT_PATH		L"Data\\Digimon\\"

	//TCHAR szOut[ MAX_PATH ] = {0, };
	//if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Texture File(tga,dds)\0*.tga;*.dds\0", SHADER_STR_INIT_PATH ) == false )
	//	return;

	//// 하위폴더 여야만 한다.
	//if( nsCSFILE::IsSubDirectory( szOut ) == false )
	//{
	//	MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
	//	return;
	//}

	//if( szOut[ 0 ] == NULL )
	//	return;

	//TCHAR szRelativePath[ MAX_PATH ];
	//nsCSFILE::GetRelativePath( szRelativePath, MAX_PATH, szOut );

	//size_t nLen = _tcslen( SHADER_STR_INIT_PATH );
	//m_edit_Str[ 0 ].SetWindowText( &szRelativePath[ nLen ] );
}

void _DlgInsertShader::OnCancel()
{
	CDialog::OnCancel();
}

void _DlgInsertShader::OnBnClickedButtonOk()
{
	int nSelObject = __GetSelectListObject();

	// 값 체크
	int nSel = m_cmb_Type.GetCurSel();
	switch( nSel )
	{
	case 0:
		break;

	case 1:
		{
			if( nsCsMfcFunc::Control2Int( &m_edit_Int[ 0 ] ) <= 0 )
			{
				CsMessageBox( MB_OK, L"라이프프레임 값 오류" );
				return;
			}
			if( nsCsMfcFunc::Control2Float( &m_edit_Float[ 0 ] ) <= 0.0f )
			{
				CsMessageBox( MB_OK, L"색비율 값 오류" );
				return;
			}

			if( nSelObject == -1 )
			{
				CsMessageBox( MB_OK, L"적용 오브젝트를 선택하여 주십시요." );
				return;
			}
			if( _tcscmp( m_list_Object.GetItemText( nSelObject, 2 ), L"O" ) == 0 )
			{
				CsMessageBox( MB_OK, L"스킨 오브젝트에는 적용 불가능 합니다." );
				return;
			}
			if( _tcslen( m_list_Object.GetItemText( nSelObject, 0 ) ) >= 32 )
			{
				CsMessageBox( MB_OK, L"메쉬 이름이 너무 깁니다. 프로그래머 에게 문의해 주세요" );
				return;
			}
		}
		break;		

	default:
		assert_cs( false );
		return;
	}	

	m_pTextKeyShader->Reset();
	// 값 입력
	switch( nSel )
	{
	case 0:
		m_pTextKeyShader->s_eShaderType = CsC_LoadSequence::sTEXTKEY_SHADER::ST_ALPHATEX;
		break;

	case 1:
		{
			m_pTextKeyShader->s_eShaderType = CsC_LoadSequence::sTEXTKEY_SHADER::ST_BLUR;
			m_pTextKeyShader->s_nValue1 = nsCsMfcFunc::Control2Int( &m_edit_Int[ 0 ] );
			m_pTextKeyShader->s_fValue1 = nsCsMfcFunc::Control2Float( &m_edit_Float[ 0 ] );
			m_pTextKeyShader->s_fValue2 = nsCsMfcFunc::Control2Float( &m_edit_Float[ 1 ] );
			m_pTextKeyShader->s_fValue3 = nsCsMfcFunc::Control2Float( &m_edit_Float[ 2 ] );
			W2M( m_pTextKeyShader->s_cApplyObjectName, m_list_Object.GetItemText( nSelObject, 0 ), 32 );
		}		
		break;

	default:
		assert_cs( false );
	}

	CDialog::OnOK();
}

