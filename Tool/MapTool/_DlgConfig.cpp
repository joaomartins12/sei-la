// _DlgConfig.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgConfig.h"


// _DlgConfig 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgConfig, CDialog)

_DlgConfig::_DlgConfig(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgConfig::IDD, pParent)
{

}

_DlgConfig::~_DlgConfig()
{
}

void _DlgConfig::__ReadInfo()
{
	__ReadInfo_Key();

	// AutoSave
	if( m_AutoSave.s_bAutoSave == true )
	{
		m_check_UseAutoSave.SetCheck( BST_CHECKED );		
	}
	OnBnClickedCheckUseAutosave();
	_Int2Control( &m_edit_AutoSaveTime, (int)( m_AutoSave.s_fAutoSaveTime/60.0f + 0.5f ) );

	// 환경
	_Int2Control( &m_edit_SleepTick, g_Config.GetEnvironment()->s_nSleepTick );
	g_Config.GetEnvironment()->s_bWriteExcel ? m_chk_WriteExcel.SetCheck( BST_CHECKED ) : m_chk_WriteExcel.SetCheck( BST_UNCHECKED );
	g_Config.GetEnvironment()->s_bWriteList ? m_check_WriteList.SetCheck( BST_CHECKED ) : m_check_WriteList.SetCheck( BST_UNCHECKED );
	g_Config.GetEnvironment()->s_bWritePortal ? m_check_WritePortal.SetCheck( BST_CHECKED ) : m_check_WritePortal.SetCheck( BST_UNCHECKED );
	g_Config.GetEnvironment()->s_bWriteCharLight ? m_check_WriteCharLight.SetCheck( BST_CHECKED ) : m_check_WriteCharLight.SetCheck( BST_UNCHECKED );
}

void _DlgConfig::__SaveInfo()
{
	// Key
	memcpy( g_Config.GetKeySet(), &m_KeySet, sizeof( cConfig::sKEYSET ) );

	// AutoSave
	g_Config.GetAutoSave()->s_bAutoSave = ( m_check_UseAutoSave.GetCheck() == BST_CHECKED );
	float fTime = _Control2Int( &m_edit_AutoSaveTime )*60.0f;
	if( g_Config.GetAutoSave()->s_fAutoSaveTime != fTime )
	{
		g_Config.GetAutoSave()->s_fAutoSaveTime = fTime;
		GET_MV_VIEW->_ResetAutoSave();
	}

	// 환경	
	g_Config.GetEnvironment()->s_nSleepTick = _Control2Int( &m_edit_SleepTick );	
	g_Config.GetEnvironment()->s_bWriteExcel = ( m_chk_WriteExcel.GetCheck() == BST_CHECKED );
	g_Config.GetEnvironment()->s_bWriteList = ( m_check_WriteList.GetCheck() == BST_CHECKED );
	g_Config.GetEnvironment()->s_bWritePortal = ( m_check_WritePortal.GetCheck() == BST_CHECKED );
	g_Config.GetEnvironment()->s_bWriteCharLight = ( m_check_WriteCharLight.GetCheck() == BST_CHECKED );
}

void _DlgConfig::__EndInfo()
{
	GET_MV_VIEW->_EnableConfigKey( true );
}

void _DlgConfig::__ReadInfo_Key()
{
	// Key
	for( int i=0; i<KS_COUNT; ++i )
	{
		m_static_Key[ i ].SetWindowText( __MakeInfo( &m_KeySet.s_KeyInfo[ i ] ) );
	}
}

LPTSTR _DlgConfig::__MakeInfo( cConfig::sKEYSET::sKEYINFO* pKeyInfo )
{
	static TCHAR sz[ MAX_FILENAME ];
	memset( sz, 0, sizeof( TCHAR )*MAX_FILENAME );
	if( ( pKeyInfo->s_dwPlag & KP_SHIPT ) != 0 )
		_tcscat_s( sz, MAX_FILENAME, L"Shift + " );
	else if( ( pKeyInfo->s_dwPlag & KP_CONTROL ) != 0 )
		_tcscat_s( sz, MAX_FILENAME, L"Ctrl + " );
	else if( ( pKeyInfo->s_dwPlag & KP_ALT ) != 0 )
		_tcscat_s( sz, MAX_FILENAME, L"Alt + " );

	

	if( ( ( pKeyInfo->s_btKey >= 0x30 )&&( pKeyInfo->s_btKey <= 0x39 ) )||
		( ( pKeyInfo->s_btKey >= 0x41 )&&( pKeyInfo->s_btKey <= 0x5A ) )	)
	{
		TCHAR key[ 2 ];
		M2W( key, (char*)&pKeyInfo->s_btKey, 1 );
		key[ 1 ] = NULL;
		_tcscat_s( sz, MAX_FILENAME, key );		
	}
	else
	{
		switch( pKeyInfo->s_btKey )
		{
			// 방향키 위 6개 키
		case VK_PRIOR:			_tcscat_s( sz, MAX_FILENAME, L"PgUp" );		break;
		case VK_NEXT:			_tcscat_s( sz, MAX_FILENAME, L"PgDn" );		break;
		case VK_HOME:			_tcscat_s( sz, MAX_FILENAME, L"Home" );		break;
		case VK_END:			_tcscat_s( sz, MAX_FILENAME, L"End" );		break;
		case VK_INSERT:			_tcscat_s( sz, MAX_FILENAME, L"Ins" );		break;
		case VK_DELETE:			_tcscat_s( sz, MAX_FILENAME, L"Del" );		break;
			// 방향 키
		case VK_LEFT:			_tcscat_s( sz, MAX_FILENAME, L"Left" );		break;
		case VK_UP:				_tcscat_s( sz, MAX_FILENAME, L"Up" );		break;
		case VK_RIGHT:			_tcscat_s( sz, MAX_FILENAME, L"Right" );	break;
		case VK_DOWN:			_tcscat_s( sz, MAX_FILENAME, L"Down" );		break;
			// 숫자패드
		case VK_NUMPAD0:		_tcscat_s( sz, MAX_FILENAME, L"Num0" );		break;
		case VK_NUMPAD1:		_tcscat_s( sz, MAX_FILENAME, L"Num1" );		break;
		case VK_NUMPAD2:		_tcscat_s( sz, MAX_FILENAME, L"Num2" );		break;
		case VK_NUMPAD3:		_tcscat_s( sz, MAX_FILENAME, L"Num3" );		break;
		case VK_NUMPAD4:		_tcscat_s( sz, MAX_FILENAME, L"Num4" );		break;
		case VK_NUMPAD5:		_tcscat_s( sz, MAX_FILENAME, L"Num5" );		break;
		case VK_NUMPAD6:		_tcscat_s( sz, MAX_FILENAME, L"Num6" );		break;
		case VK_NUMPAD7:		_tcscat_s( sz, MAX_FILENAME, L"Num7" );		break;
		case VK_NUMPAD8:		_tcscat_s( sz, MAX_FILENAME, L"Num8" );		break;
		case VK_NUMPAD9:		_tcscat_s( sz, MAX_FILENAME, L"Num9" );		break;
		case VK_MULTIPLY:		_tcscat_s( sz, MAX_FILENAME, L"Num*" );		break;
		case VK_ADD:			_tcscat_s( sz, MAX_FILENAME, L"Num+" );		break;
		case VK_SUBTRACT:		_tcscat_s( sz, MAX_FILENAME, L"Num-" );		break;
		case VK_DECIMAL:		_tcscat_s( sz, MAX_FILENAME, L"Num." );		break;
		case VK_DIVIDE:			_tcscat_s( sz, MAX_FILENAME, L"Num/" );		break;
		case VK_NUMLOCK:		_tcscat_s( sz, MAX_FILENAME, L"NumLk" );	break;
			// F1~F12
		case VK_F1:				_tcscat_s( sz, MAX_FILENAME, L"F1" );		break;
		case VK_F2:				_tcscat_s( sz, MAX_FILENAME, L"F2" );		break;
		case VK_F3:				_tcscat_s( sz, MAX_FILENAME, L"F3" );		break;
		case VK_F4:				_tcscat_s( sz, MAX_FILENAME, L"F4" );		break;
		case VK_F5:				_tcscat_s( sz, MAX_FILENAME, L"F5" );		break;
		case VK_F6:				_tcscat_s( sz, MAX_FILENAME, L"F6" );		break;
		case VK_F7:				_tcscat_s( sz, MAX_FILENAME, L"F7" );		break;
		case VK_F8:				_tcscat_s( sz, MAX_FILENAME, L"F8" );		break;
		case VK_F9:				_tcscat_s( sz, MAX_FILENAME, L"F9" );		break;
		case VK_F10:			_tcscat_s( sz, MAX_FILENAME, L"F10" );		break;
		case VK_F11:			_tcscat_s( sz, MAX_FILENAME, L"F11" );		break;
		case VK_F12:			_tcscat_s( sz, MAX_FILENAME, L"F12" );		break;
			// 특수 문자
		case VK_OEM_NEC_EQUAL:	_tcscat_s( sz, MAX_FILENAME, L"=" );		break;
		case VK_OEM_1:			_tcscat_s( sz, MAX_FILENAME, L";" );		break;
		case VK_OEM_PLUS:		_tcscat_s( sz, MAX_FILENAME, L"+" );		break;
		case VK_OEM_COMMA:		_tcscat_s( sz, MAX_FILENAME, L"," );		break;
		case VK_OEM_MINUS:		_tcscat_s( sz, MAX_FILENAME, L"-" );		break;
		case VK_OEM_PERIOD:		_tcscat_s( sz, MAX_FILENAME, L"." );		break;
		case VK_OEM_2:			_tcscat_s( sz, MAX_FILENAME, L"/" );		break;
		case VK_OEM_3:			_tcscat_s( sz, MAX_FILENAME, L"~" );		break;
		case VK_OEM_4:			_tcscat_s( sz, MAX_FILENAME, L"[" );		break;
		case VK_OEM_5:			_tcscat_s( sz, MAX_FILENAME, L"\\" );		break;
		case VK_OEM_6:			_tcscat_s( sz, MAX_FILENAME, L"]" );		break;
		case VK_OEM_7:			_tcscat_s( sz, MAX_FILENAME, L"\"" );		break;
			// 기타
		case VK_BACK:			_tcscat_s( sz, MAX_FILENAME, L"BS" );		break;
		case VK_TAB:			_tcscat_s( sz, MAX_FILENAME, L"Tab" );		break;
		case VK_SPACE:			_tcscat_s( sz, MAX_FILENAME, L"Space" );	break;
		case VK_CAPITAL:		_tcscat_s( sz, MAX_FILENAME, L"Caps" );		break;
		}
	}

	return sz;
}

#define	DoData( num )	DDX_Control(pDX, IDC_STATIC_KEY##num, m_static_Key[ num ] )
#define	BEMAP( num )	ON_STN_CLICKED(IDC_STATIC_KEY##num, &_DlgConfig::OnStnClickedStaticKey##num)
#define	CLICK( num )	void _DlgConfig::OnStnClickedStaticKey##num(){\
	BYTE btKey = m_KeySet.s_KeyInfo[ num ].s_btKey;\
	DWORD dwPlag = m_KeySet.s_KeyInfo[ num ].s_dwPlag;\
	_DlgInputKey di( &m_KeySet.s_KeyInfo[ num ] );\
	if( di.DoModal() == IDCANCEL )\
		return;\
	m_static_Key[ num ].SetWindowText( __MakeInfo( &m_KeySet.s_KeyInfo[ num ] ) );\
	for( int i=0; i<KS_COUNT; ++i )\
	{\
		if( i == num )	continue;\
		if( ( m_KeySet.s_KeyInfo[ i ].s_btKey == m_KeySet.s_KeyInfo[ num ].s_btKey )&&\
			( m_KeySet.s_KeyInfo[ i ].s_dwPlag == m_KeySet.s_KeyInfo[ num ].s_dwPlag ) )\
		{\
			if( MessageBox( L"같은 단축키를 사용하는게 있습니다.\n해당 단축키를 비우고 설정하시겠습니까?", NULL, MB_OKCANCEL ) == IDOK )\
			{\
				m_KeySet.s_KeyInfo[ i ].Init();\
				m_static_Key[ i ].SetWindowText( __MakeInfo( &m_KeySet.s_KeyInfo[ i ] ) );\
			}\
			else\
			{\
				m_KeySet.s_KeyInfo[ num ].s_btKey = btKey;\
				m_KeySet.s_KeyInfo[ num ].s_dwPlag = dwPlag;\
				m_static_Key[ num ].SetWindowText( __MakeInfo( &m_KeySet.s_KeyInfo[ num ] ) );\
			}\
			break;\
		}\
	}\
}


void _DlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DoData( 0 );
	DoData( 1 );
	DoData( 2 );
	DoData( 3 );
	DoData( 4 );
	DoData( 5 );
	DoData( 6 );
	DoData( 7 );
	DoData( 8 );
	DoData( 9 );
	DoData( 10 );
	DoData( 11 );
	DoData( 12 );
	DoData( 13 );
	DoData( 14 );
	DoData( 15 );
	DoData( 16 );
	DoData( 17 );
	DoData( 18 );
	DoData( 19 );
	DoData( 20 );
	DoData( 21 );
	DoData( 22 );
	DoData( 23 );
	DoData( 24 );
	DoData( 25 );
	DoData( 26 );
	DoData( 27 );
	DoData( 28 );
	DoData( 29 );
	DoData( 30 );
	DoData( 31 );
	DoData( 32 );
	DoData( 33 );
	DoData( 34 );
	DoData( 35 );
	DoData( 36 );
	DoData( 37 );
	DoData( 38 );
	DoData( 39 );
	DoData( 40 );

	DDX_Control(pDX, IDC_EDIT_AUTOSAVE_MIN, m_edit_AutoSaveTime);
	DDX_Control(pDX, IDC_CHECK_USE_AUTOSAVE, m_check_UseAutoSave);
	DDX_Control(pDX, IDC_EDIT_SLEEPTICK, m_edit_SleepTick);
	DDX_Control(pDX, IDC_CHECK_WRITE_EXCEL, m_chk_WriteExcel);
	DDX_Control(pDX, IDC_CHECK_WRITE_LIST, m_check_WriteList);
	DDX_Control(pDX, IDC_CHECK_WRITE_PORTAL, m_check_WritePortal);
	DDX_Control(pDX, IDC_CHECK_WRITE_CHARLIGHT, m_check_WriteCharLight);
	DDX_Control(pDX, IDC_SLIDER_RATE_CAM_HEIGHT, m_CamAutoMoveHeightRate);

}

BEGIN_MESSAGE_MAP(_DlgConfig, CDialog)
	BEMAP( 0 )
	BEMAP( 1 )
	BEMAP( 2 )
	BEMAP( 3 )
	BEMAP( 4 )
	BEMAP( 5 )
	BEMAP( 6 )
	BEMAP( 7 )
	BEMAP( 8 )
	BEMAP( 9 )
	BEMAP( 10 )
	BEMAP( 11 )
	BEMAP( 12 )
	BEMAP( 13 )
	BEMAP( 14 )
	BEMAP( 15 )
	BEMAP( 16 )
	BEMAP( 17 )
	BEMAP( 18 )
	BEMAP( 19 )
	BEMAP( 20 )
	BEMAP( 21 )
	BEMAP( 22 )
	BEMAP( 23 )
	BEMAP( 24 )
	BEMAP( 25 )
	BEMAP( 26 )
	BEMAP( 27 )
	BEMAP( 28 )
	BEMAP( 29 )
	BEMAP( 30 )
	BEMAP( 31 )
	BEMAP( 32 )
	BEMAP( 33 )
	BEMAP( 34 )
	BEMAP( 35 )
	BEMAP( 36 )
	BEMAP( 37 )
	BEMAP( 38 )
	BEMAP( 39 )
	BEMAP( 40 )
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_KEY, &_DlgConfig::OnBnClickedButtonDefaultKey)
	ON_BN_CLICKED(IDC_CHECK_USE_AUTOSAVE, &_DlgConfig::OnBnClickedCheckUseAutosave)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_RATE_CAM_HEIGHT, &_DlgConfig::OnNMReleasedcaptureSliderRateCamHeight)
END_MESSAGE_MAP()

CLICK(0)
CLICK(1)
CLICK(2)
CLICK(3)
CLICK(4)
CLICK(5)
CLICK(6)
CLICK(7)
CLICK(8)
CLICK(9)
CLICK(10)
CLICK(11)
CLICK(12)
CLICK(13)
CLICK(14)
CLICK(15)
CLICK(16)
CLICK(17)
CLICK( 18 )
CLICK( 19 )
CLICK( 20 )
CLICK( 21 )
CLICK( 22 )
CLICK( 23 )
CLICK( 24 )
CLICK( 25 )
CLICK( 26 )
CLICK( 27 )
CLICK( 28 )
CLICK( 29 )
CLICK( 30 )
CLICK( 31 )
CLICK( 32 )
CLICK( 33 )
CLICK( 34 )
CLICK( 35 )
CLICK( 36 )
CLICK( 37 )
CLICK( 38 )
CLICK( 39 )
CLICK( 40 )


// _DlgConfig 메시지 처리기입니다.
BOOL _DlgConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	GET_MV_VIEW->_EnableConfigKey( false );

	memcpy( &m_KeySet, g_Config.GetKeySet(), sizeof( cConfig::sKEYSET ) );
	memcpy( &m_AutoSave, g_Config.GetAutoSave(), sizeof( cConfig::sAUTOSAVE ) );
	
	__ReadInfo();


	float CurrRate = GET_MV_VIEW->GetCurrResetCamHeightRate() * 100.0f;
	m_CamAutoMoveHeightRate.SetPos(CurrRate);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgConfig::OnBnClickedButtonDefaultKey()
{
	m_KeySet.Reset();
	__ReadInfo_Key();
}

void _DlgConfig::OnBnClickedCheckUseAutosave()
{
	if( m_check_UseAutoSave.GetCheck() == BST_CHECKED )
		m_edit_AutoSaveTime.EnableWindow( TRUE );
	else
		m_edit_AutoSaveTime.EnableWindow( FALSE );
}

void _DlgConfig::OnOK()
{
	__SaveInfo();
	__EndInfo();
	
	CDialog::OnOK();
}

void _DlgConfig::OnCancel()
{
	__EndInfo();
	
	CDialog::OnCancel();
}


void _DlgConfig::OnNMReleasedcaptureSliderRateCamHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	float fRate = (float)m_CamAutoMoveHeightRate.GetPos() / 100.0f;
	GET_MV_VIEW->UpdateResetCamHeightRate(fRate);

}
