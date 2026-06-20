#include "stdafx.h"
#include "Global.h"
#include <windows.h>
#include "CIme.h"

/*----------------------------------------------------------------------------*/ 

#define CHT_IMEFILENAME1    "TINTLGNT.IME" // xnew Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // xnew Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

#define LANG_CHT            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANG_CHS            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)

#ifdef DEFAULT_IME	// IME 기본 상태로 동작하도록 함.
	#define _CHT_HKL_DAYI		( (HKL)(INT_PTR)0xE0060404 ) // DaYi
	#define _CHT_HKL_NEW_QUICK	( (HKL)(INT_PTR)0xE00A0404 ) // New Quick
#endif

#define _CHT_HKL            ( (HKL)(INT_PTR)0xE0080404 ) // xnew Phonetic
#define _CHT_HKL2           ( (HKL)(INT_PTR)0xE0090404 ) // xnew Chang Jie
#define _CHS_HKL            ( (HKL)(INT_PTR)0xE00E0804 ) // MSPY
#define MAKEIMEVERSION( major, minor ) \
    ( (DWORD)( ( (BYTE)( major ) << 24 ) | ( (BYTE)( minor ) << 16 ) ) )

#define IMEID_CHT_VER42 ( LANG_CHT | MAKEIMEVERSION( 4, 2 ) )	// New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 ( LANG_CHT | MAKEIMEVERSION( 4, 3 ) )	// New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 ( LANG_CHT | MAKEIMEVERSION( 4, 4 ) )	// xnew ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 ( LANG_CHT | MAKEIMEVERSION( 5, 0 ) )	// New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 ( LANG_CHT | MAKEIMEVERSION( 5, 1 ) )	// New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 ( LANG_CHT | MAKEIMEVERSION( 5, 2 ) )	// New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 ( LANG_CHT | MAKEIMEVERSION( 6, 0 ) )	// New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)

#ifdef DEFAULT_IME	// IME 기본 상태로 동작하도록 함.
	#define IMEID_CHT_VER_VISTA ( LANG_CHT | MAKEIMEVERSION( 7, 0 ) )  // All TSF TIP under Cicero UI-less mode: a hack to make GetImeId() return non-zero value
#endif

#define IMEID_CHS_VER41	( LANG_CHS | MAKEIMEVERSION( 4, 1 ) )	// MSPY1.5	// SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42	( LANG_CHS | MAKEIMEVERSION( 4, 2 ) )	// MSPY2	// Win2k/WinME
#define IMEID_CHS_VER53	( LANG_CHS | MAKEIMEVERSION( 5, 3 ) )	// MSPY3	// WinXP

enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };
enum { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

wchar_t		g_aszIndicator[5][3] =  
{
	_T( "En" ),
	_T( "\x7B80" ),
	_T( "\x7E41" ),
	_T( "\xAC00" ),
	_T( "\x3042" ),
};

static CIMM g_imm;
CIME g_IME;

#ifndef _GIVE
	CsLog* logIME = NULL;  
#endif

/*----------------------------------------------------------------------------*/ /* CIMM */ 
CIMM::CIMM() 
:	m_hDllImm32(NULL) 
{
}

/*----------------------------------------------------------------------------*/ 
CIMM::~CIMM()
{ 
	if(m_hDllImm32) 
		FreeLibrary(m_hDllImm32);
}

/*----------------------------------------------------------------------------*/ 
void	CIMM::Init()
{
    CHAR szPath[MAX_PATH+1];
    
	if( GetSystemDirectoryA( szPath, MAX_PATH+1 ) ) 
	{
		strcat( szPath, "\\imm32.dll" );
		m_hDllImm32 = LoadLibraryA( szPath );
		if( m_hDllImm32 )
		{
			_ImmLockIMC		= (INPUTCONTEXT*(WINAPI *)( HIMC ))
				GetProcAddress( m_hDllImm32, "ImmLockIMC" );
			_ImmUnlockIMC	= (BOOL(WINAPI *)( HIMC ))
				GetProcAddress( m_hDllImm32, "ImmUnlockIMC" );
			_ImmLockIMCC	= (LPVOID(WINAPI *)( HIMCC ))
				GetProcAddress( m_hDllImm32, "ImmLockIMCC" );
			_ImmUnlockIMCC	= (BOOL(WINAPI *)( HIMCC ))
				GetProcAddress( m_hDllImm32, "ImmUnlockIMCC" );
		}
	}
}

/*----------------------------------------------------------------------------*/ 
INPUTCONTEXT*	CIMM::LockIMC(HIMC hImc)
{
	if(_ImmLockIMC == NULL) 
	{
		return NULL;
	} else 
	{
		return _ImmLockIMC(hImc);
	}
}

/*----------------------------------------------------------------------------*/ 
BOOL	CIMM::UnlockIMC(HIMC hImc)
{
	if(_ImmUnlockIMC == NULL) 
	{
		return FALSE;
	} else 
	{
		return _ImmUnlockIMC(hImc);
	}
}

/*----------------------------------------------------------------------------*/ 
LPVOID	CIMM::LockIMCC( HIMCC himcc )
{
	if(_ImmLockIMCC == NULL) 
	{
		return NULL;
	} else 
	{
		return _ImmLockIMCC(himcc);
	}
}

/*----------------------------------------------------------------------------*/ 
BOOL	CIMM::UnlockIMCC( HIMCC himcc )
{
	if(_ImmUnlockIMCC == NULL) 
	{
		return NULL;
	} else 
	{
		return _ImmUnlockIMCC(himcc);
	}
}
/*----------------------------------------------------------------------------*/ 

/*----------------------------------------------------------------------------*/ 
int		ConvertString(UINT codePage, const wchar_t* wText, int wLen, char* text, int len)
{
	if(text == 0) {
		return WideCharToMultiByte(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);
	} else {
		int tLen = WideCharToMultiByte(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);

		if(tLen > len) {
			return 0;
		} else {
			return WideCharToMultiByte(codePage, 0, wText, wLen, text, tLen, NULL, NULL);
		}
	}
}

/*----------------------------------------------------------------------------*/ /* Begin of CIME */ 
CIME::CIME()
:	m_ulStart(0)
,	m_ulEnd(0)
,	m_hDllIme(NULL)
,	m_ImeState(IMEUI_STATE_OFF)
,	m_bImeActive(true)
{
	m_bEdit = false;
	m_bNumberOnly = false;
	m_nLenght = 20;

	m_bLockConidateList = false;
	m_bLockChangeLanguage = false;
#ifndef _GIVE
	logIME = csnew CsLog;
	logIME->Init( L"___IMETest", false, true );
#endif

#ifdef THAI_SPECIALWORD
	m_bKEY_ALTDOWN = false;
	m_bKEY_ALTUP = true;
#endif

#ifdef NOT_ENTER_CREATENAME
	m_bCombination = false;
#endif
}

/*----------------------------------------------------------------------------*/ 
CIME::~CIME()
{
	if( m_hDllIme ) 
		FreeLibrary( m_hDllIme );

#ifndef _GIVE
	SAFE_DELETE( logIME );
#endif
}

/*----------------------------------------------------------------------------*/ 
void	CIME::InitInput()
{
	g_imm.Init();
}

/*----------------------------------------------------------------------------*/ 
void	CIME::OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_hkl = (HKL)lParam;
	m_langId = LOWORD(m_hkl);
	m_codePage = GetCodePageFromLang(m_langId);

#ifndef THAI_WORDTRANSFORM	// 캐릭터 생성 시 IME 문자 변환을 할 경우 입력된 문자가 리셋되는 현상을 막기 위함.
	m_input.resize(0);
#endif

	m_himOrg = ImmGetContext( hWnd );
	if( m_himOrg )
		ImmReleaseContext( hWnd, m_himOrg );

	/* Check Property */ 
	DWORD property = ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);

	m_bUnicodeIME = (property & IME_PROP_UNICODE) ? true : false;

	if( m_bLockChangeLanguage )
		SetImeActive(false);

	/* Update Indicator */ 
	CheckToggleState(hWnd);

	/* Update m_dwId[] */ 
	GetImeId();

	/* Bind Proc */ 
	SetupImeApi(hWnd);
}

/*----------------------------------------------------------------------------*/ 
bool	CIME::OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( m_bNumberOnly == true )
		return true;

	HIMC hImc;
	if( (lParam&GCS_COMPSTR)&&( (int)m_input.size() < m_nLenght ) )
	{
		hImc = ImmGetContext(hWnd);
		if( hImc )
		{
			int tempSize = ImmGetCompositionStringW(hImc, GCS_COMPSTR, NULL, 0);

			wchar_t* temp = (wchar_t*)alloca(tempSize);

			ImmGetCompositionStringW(hImc, GCS_COMPSTR, temp, tempSize);

			m_comp.assign(temp, temp+tempSize/sizeof(wchar_t));

			ImmReleaseContext(hWnd, hImc);

#ifdef NOT_ENTER_CREATENAME
			SetCombination(true);	// 문자 조합중임.
#endif
		}
	}
	else
	{
		OnEndComposition( hWnd, wParam, lParam );
	}
	if( lParam&GCS_RESULTSTR )
	{
		hImc = ImmGetContext(hWnd);
		if(hImc) { 

			int tempSize = ImmGetCompositionStringW(hImc, GCS_RESULTSTR, NULL, 0);

			wchar_t* temp = (wchar_t*)alloca(tempSize);

			ImmGetCompositionStringW(hImc, GCS_RESULTSTR, temp, tempSize);

			int nInputSize = (int)m_input.size();
			if( nInputSize < m_nLenght )
			{
				//m_input.append(temp, temp+ CsMin( m_nLenght - nInputSize, tempSize/sizeof(wchar_t) ) );
				m_input.append( temp, 0, CsMin( m_nLenght - nInputSize, (int)( tempSize/sizeof(wchar_t) ) ) );
			}

			ImmReleaseContext(hWnd, hImc);

#ifdef NOT_ENTER_CREATENAME
			SetCombination(false);	// 문자 조합 완료됨.
#endif
		}
	}
	if( lParam&GCS_COMPATTR )
	{
		hImc = ImmGetContext(hWnd);
		if(hImc) { 

			int tempSize = ImmGetCompositionStringW(hImc, GCS_COMPATTR, NULL, 0);

			BYTE* temp = (BYTE*)alloca(tempSize);

			ImmGetCompositionStringW(hImc, GCS_COMPATTR, temp, tempSize);

			int start = 0;
			int end = 0;

			for(start=0; start<tempSize; ++start) if(temp[start]==ATTR_TARGET_CONVERTED || temp[start]==ATTR_TARGET_NOTCONVERTED) break;
			for(end=start; end<tempSize; ++end) if(temp[end]!=temp[start]) break;

			m_ulStart	= start;
			m_ulEnd		= end;

			ImmReleaseContext(hWnd, hImc);
		}
	}

	return true;
}

/*----------------------------------------------------------------------------*/ 
bool	CIME::OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_comp.resize(0);
	m_ulStart = m_ulEnd = 0;
	m_reading.resize(0);

#ifdef DEFAULT_IME	// IME 기본 상태로 동작하도록 함.
	return false;
#else
	return true;
#endif
}

/*----------------------------------------------------------------------------*/ 

#ifdef DEFAULT_IME
bool	CIME::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc;

	switch (wParam) 
	{
	case IMN_OPENCANDIDATE:
	case IMN_CHANGECANDIDATE:
		hImc = ImmGetContext(hWnd);
		if(hImc) { 

			//memset(NULL, 0, 10);

			m_reading.resize(0);

			int candidateLen = ImmGetCandidateListW(hImc, 0, NULL, 0);

			if(candidateLen > 0) { 

				m_candidate.resize(candidateLen);

				ImmGetCandidateListW(hImc, 0, (CANDIDATELIST*)&m_candidate[0], candidateLen);
			}

			ImmReleaseContext(hWnd, hImc);
		}
		return false;

	case IMN_CLOSECANDIDATE:
		m_candidate.resize(0);
		return false;

	case IMN_SETCONVERSIONMODE:
	case IMN_SETOPENSTATUS:
		CheckToggleState(hWnd);
		return false;

	case IMN_PRIVATE:
		GetPrivateReadingString(hWnd);

		// Trap some messages to hide reading window
		switch( m_dwId[0] )
		{
		case IMEID_CHT_VER42:
		case IMEID_CHT_VER43:
		case IMEID_CHT_VER44:
		case IMEID_CHS_VER41:
		case IMEID_CHS_VER42:
			if((lParam==1)||(lParam==2)) return false;

			break;

		case IMEID_CHT_VER50:
		case IMEID_CHT_VER51:
		case IMEID_CHT_VER52:
		case IMEID_CHT_VER60:
		case IMEID_CHS_VER53:
			if((lParam==16)||(lParam==17)||(lParam==26)||(lParam==27)||(lParam==28)) return false;

			break;
		}
		break;
	}

	return false;
}
#else
bool	CIME::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc;

	switch (wParam) 
	{
	case IMN_OPENCANDIDATE:
	case IMN_CHANGECANDIDATE:
		hImc = ImmGetContext(hWnd);
		if(hImc) { 

			//memset(NULL, 0, 10);

			m_reading.resize(0);

			int candidateLen = ImmGetCandidateListW(hImc, 0, NULL, 0);

			if(candidateLen > 0) { 

				m_candidate.resize(candidateLen);

				ImmGetCandidateListW(hImc, 0, (CANDIDATELIST*)&m_candidate[0], candidateLen);
			}

			ImmReleaseContext(hWnd, hImc);
		}

		if( m_bLockConidateList == true )
			ImmNotifyIME(hImc , NI_CLOSECANDIDATE, 0, 0); 

		return true;

	case IMN_CLOSECANDIDATE:
		m_candidate.resize(0);

		return true;

	case IMN_SETCONVERSIONMODE:

        CheckToggleState(hWnd);
		return false;

    case IMN_PRIVATE:
        GetPrivateReadingString(hWnd);

        // Trap some messages to hide reading window
        switch( m_dwId[0] )
        {
            case IMEID_CHT_VER42:
            case IMEID_CHT_VER43:
            case IMEID_CHT_VER44:
            case IMEID_CHS_VER41:
            case IMEID_CHS_VER42:
				if((lParam==1)||(lParam==2)) return true;

                break;

            case IMEID_CHT_VER50:
            case IMEID_CHT_VER51:
            case IMEID_CHT_VER52:
            case IMEID_CHT_VER60:
            case IMEID_CHS_VER53:
                if((lParam==16)||(lParam==17)||(lParam==26)||(lParam==27)||(lParam==28)) return true;

                break;
        }
        break;
	}

	return false;
}
#endif

/*----------------------------------------------------------------------------*/ 
bool	CIME::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam) 
	{
	case '\r':
	case '\n':
#ifdef TAIWAN_DAYI_IME
		if( m_hkl == _CHT_HKL_DAYI )
		{
			HIMC hIme = ImmGetContext(hWnd);
			ImmNotifyIME( hIme, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmReleaseContext(hWnd, hIme);
		}
#endif
		//m_input.resize(0);
		break;
	case '\b':
		if(m_input.size()) m_input.resize(m_input.size() - 1);
		break;	
	case '\t':
	case 27:
	case 127:
		break;
	default:
		if( (int)m_input.size() < m_nLenght )
		{
			if( m_bNumberOnly == false )
			{
				if(wParam > 31)
				{
					wchar_t temp;
#ifdef THAI_CHAR_CHECK	// 태국어는 유니코드 이므로 멀티바이트 변환없이 대입 코드 추가	lks007 12.08.08.
					temp = (TCHAR)wParam;
#else
					MultiByteToWideChar(m_codePage, MB_COMPOSITE | MB_ERR_INVALID_CHARS, (char*)&wParam, 1, &temp, 1);
#endif

#ifdef THAI_SPECIALWORD
					if(m_bKEY_ALTDOWN == false)
						m_input.push_back(temp);

					if(m_bKEY_ALTUP == false )
					{
						m_bKEY_ALTDOWN = false;
						m_bKEY_ALTUP = true;
					}
#else
					m_input.push_back(temp);
#endif
				}
			}
			else
			{
				if( ( wParam >= L'0' )&&( wParam <= L'9' ) )
				{
					wchar_t temp;
					MultiByteToWideChar(m_codePage, 0, (char*)&wParam, 1, &temp, 1);
					m_input.push_back(temp);
				}
			}
		}
		break;
	}

	return true;
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetInput(char* text, int len)
{
	return ConvertString(m_codePage, m_input.c_str(), m_input.size(), text, len);
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetComp(char* text, int len)
{
	return ConvertString(m_codePage, m_comp.c_str(), m_comp.size(), text, len);
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetReading(char* text, int len)
{
	return ConvertString(m_codePage, m_reading.c_str(), m_reading.size(), text, len);
}

/*----------------------------------------------------------------------------*/ 

int		CIME::GetInputComp( TCHAR* text)
{
	wcscpy( text, m_input.c_str() );
	wcscat( text, m_comp.c_str() );
	return m_input.size() + m_comp.size();
}

void	CIME::GetInput(std::wstring* pString)
{
	*pString = m_input;
}

int		CIME::GetInput(TCHAR* text)
{
	wcscpy( text, m_input.c_str() );
	return m_input.size();
}

int		CIME::GetInput(TCHAR* text, int len)
{
	wcsncpy( text, m_input.c_str(), len );
	return m_input.size();
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetComp(TCHAR* text)
{
	wcscpy( text, m_comp.c_str() );
	return m_comp.size();
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetReading(TCHAR* text)
{
	wcscpy( text, m_reading.c_str() );
	return m_reading.size();
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetCandidate(DWORD index, char* text, int len) 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		CANDIDATELIST* candidateList = (CANDIDATELIST*)&m_candidate[0];

		if(index >= candidateList->dwCount) {
			return 0;
		} else {

			if(m_bUnicodeIME) {
				
				wchar_t* wText = (wchar_t*)(&m_candidate[0] + candidateList->dwOffset[index]);

				return ConvertString(m_codePage, wText, wcslen(wText), text, len);

			} else {

				char* temp = (char*)(&m_candidate[0] + candidateList->dwOffset[index]);

				if(text == 0) {
					return strlen(temp);
				} else {
					int tempLen = strlen(temp);

					if(len < tempLen) {
						return 0;
					} else {
						memcpy(text, temp, tempLen);
						return tempLen;
					}
				}
			}
		}
	}
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetCandidateCount() 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
	}
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetCandidateSelection() 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		if(PRIMARYLANGID(m_langId) == LANG_KOREAN)
			return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
		else
			return ((CANDIDATELIST*)&m_candidate[0])->dwSelection;
	}
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetCandidatePageSize() 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageSize;
	}
}

/*----------------------------------------------------------------------------*/ 
int		CIME::GetCandidatePageStart() 
{
	if(m_candidate.empty()) {
		return 0;
	} else {
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageStart;
	}
}

/*----------------------------------------------------------------------------*/ 
void	CIME::GetUnderLine(int* start, int* end) 
{ 	
	*start = WideCharToMultiByte(m_codePage, 0, m_comp.c_str(), m_ulStart, NULL, 0, NULL, NULL); 
	*end = WideCharToMultiByte(m_codePage, 0, m_comp.c_str(), m_ulEnd, NULL, 0, NULL, NULL); 
}

/*----------------------------------------------------------------------------*/ 
void	CIME::GetImeId()
{
    char    szTmp[1024];

	m_dwId[0] = m_dwId[1] = 0;

    if(!((m_hkl==_CHT_HKL) || (m_hkl==_CHT_HKL2) || (m_hkl==_CHS_HKL)))
		return;
        
	if ( !ImmGetIMEFileNameA( m_hkl, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
        return;

    if ( !_GetReadingString ) {

        if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) ) {

	        return;
        }
    }

    DWORD   dwVerHandle;
    DWORD   dwVerSize = GetFileVersionInfoSizeA( szTmp, &dwVerHandle );

    if( dwVerSize ) {

        LPVOID  lpVerBuffer = alloca( dwVerSize );

        if( GetFileVersionInfoA( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) ) {

			LPVOID  lpVerData;
			UINT    cbVerData;

            if( VerQueryValueA( lpVerBuffer, "\\", &lpVerData, &cbVerData ) ) {

                DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
                dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
                if( _GetReadingString
                    ||
                    ( m_langId == LANG_CHT &&
                        ( dwVer == MAKEIMEVERSION(4, 2) || 
                        dwVer == MAKEIMEVERSION(4, 3) || 
                        dwVer == MAKEIMEVERSION(4, 4) || 
                        dwVer == MAKEIMEVERSION(5, 0) ||
                        dwVer == MAKEIMEVERSION(5, 1) ||
                        dwVer == MAKEIMEVERSION(5, 2) ||
                        dwVer == MAKEIMEVERSION(6, 0) ) )
                    ||
                    ( m_langId == LANG_CHS &&
                        ( dwVer == MAKEIMEVERSION(4, 1) ||
                        dwVer == MAKEIMEVERSION(4, 2) ||
                        dwVer == MAKEIMEVERSION(5, 3) ) ) ) {

                    m_dwId[0] = dwVer | m_langId;
                    m_dwId[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------*/ 
void	CIME::SetupImeApi(HWND hWnd)
{
    char szImeFile[MAX_PATH + 1];

	_GetReadingString = NULL;
    _ShowReadingWindow = NULL;

	if( ImmGetIMEFileNameA( m_hkl, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) != 0 ) {

		if( m_hDllIme ) FreeLibrary( m_hDllIme );
	    
		m_hDllIme = LoadLibraryA( szImeFile );

		if ( m_hDllIme ) {
			_GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
				( GetProcAddress( m_hDllIme, "GetReadingString" ) );
			_ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
				( GetProcAddress( m_hDllIme, "ShowReadingWindow" ) );

			if( _ShowReadingWindow ) {
				HIMC hImc = ImmGetContext(hWnd);
				if(hImc) {
#ifdef DEFAULT_IME	// IME 기본 상태로 동작하도록 함.
					_ShowReadingWindow( hImc, true );
#else
					_ShowReadingWindow( hImc, false );
#endif
					ImmReleaseContext(hWnd, hImc);
				}
			}
		}
	}
}

/*----------------------------------------------------------------------------*/ 
bool	CIME::GetReadingWindowOrientation()
{
    bool bHorizontalReading = ( m_hkl == _CHS_HKL ) || ( m_hkl == _CHT_HKL2 ) || ( m_dwId[0] == 0 );
    if( !bHorizontalReading && ( m_dwId[0] & 0x0000FFFF ) == LANG_CHT )
    {
        char szRegPath[MAX_PATH];
        HKEY hKey;
        DWORD dwVer = m_dwId[0] & 0xFFFF0000;
        strcpy( szRegPath, "software\\microsoft\\windows\\currentversion\\" );
        strcat( szRegPath, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? "MSTCIPH" : "TINTLGNT" );
        LONG lRc = RegOpenKeyExA( HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hKey );
        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = RegQueryValueExA( hKey, "Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
            if (lRc == ERROR_SUCCESS)
            {
                if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
                       ( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
                     ||
                     ( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
                       (BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
                   )
                {
                    bHorizontalReading = true;
                }
            }
            RegCloseKey( hKey );
        }
    }

	return bHorizontalReading;
}

/*----------------------------------------------------------------------------*/ 
void	CIME::GetPrivateReadingString(HWND hWnd)
{
    if( !m_dwId[0] ) {
        m_reading.resize(0);
        return;
    }

	HIMC hImc = ImmGetContext(hWnd);
    if( !hImc ) {
        m_reading.resize(0);
        return;
    }

    
    DWORD dwErr = 0;

    if( _GetReadingString ) {

        UINT uMaxUiLen;
        BOOL bVertical;
        // Obtain the reading string size
        int wstrLen = _GetReadingString( hImc, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen );

		if( wstrLen == 0 ) {
			m_reading.resize(0);
		} else {
			wchar_t *wstr = (wchar_t*)alloca(sizeof(wchar_t) * wstrLen);
            _GetReadingString( hImc, wstrLen, wstr, (PINT)&dwErr, &bVertical, &uMaxUiLen );
			m_reading.assign(wstr, wstr+wstrLen);
		}

		m_bVerticalReading = bVertical ? true : false;

		ImmReleaseContext(hWnd, hImc);

    } else {

        // IMEs that doesn't implement Reading String API
		wchar_t* temp;
		DWORD tempLen;
	    bool bUnicodeIme = false;
		INPUTCONTEXT *lpIC = g_imm.LockIMC(hImc);

		if(lpIC == NULL) {
			temp = NULL;
			tempLen = 0;
		} else {
			LPBYTE p = 0;
			switch( m_dwId[0] )
			{
				case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
				case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
				case IMEID_CHT_VER44: // xnew ChanJie IME98b          : 4.4.x.x // WinXP
					p = *(LPBYTE *)((LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + 24 );
					if( !p ) break;
					tempLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
					dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
					temp = (wchar_t *)( p + 56 );
					bUnicodeIme = true;
					break;

				case IMEID_CHT_VER50: // 5.0.x.x // WinME
					p = *(LPBYTE *)( (LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + 3 * 4 );
					if( !p ) break;
					p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
					if( !p ) break;
					tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
					dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
					temp = (wchar_t *)(p + 1*4 + (16*2+2*4) + 5*4);
					bUnicodeIme = false;
					break;

				case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
				case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
				case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
					p = *(LPBYTE *)((LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + 4);
					if( !p ) break;
					p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
					if( !p ) break;
					tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
					dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
					temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
					bUnicodeIme = true;
					break;

				// the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
				case IMEID_CHS_VER41:
					{
						int nOffset;
						nOffset = ( m_dwId[1] >= 0x00000002 ) ? 8 : 7;

						p = *(LPBYTE *)((LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + nOffset * 4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 7*4 + 16*2*4);
						dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
						dwErr = min( dwErr, tempLen );
						temp = (wchar_t *)(p + 6*4 + 16*2*1);
						bUnicodeIme = true;
					}
					break;

				case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
					{
						OSVERSIONINFOA osi;
						osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
						GetVersionExA( &osi );

						int nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(wchar_t) : sizeof(char);
						p = *(LPBYTE *)((LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
						dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
						temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
						bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
					}
					break;

				default:
					temp = NULL;
					tempLen = 0;
					break;
			}
		}

		if(tempLen == 0) {
			m_reading.resize(0);
		} else {
			if( bUnicodeIme ) {
				m_reading.assign(temp, tempLen);
			} else {
				int wstrLen = MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, NULL, 0); 
				wchar_t* wstr = (wchar_t*)alloca(sizeof(wchar_t)*wstrLen);
				MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, wstr, wstrLen); 
				m_reading.assign(wstr, wstrLen);
			}
		}

		g_imm.UnlockIMCC(lpIC->hPrivate);
		g_imm.UnlockIMC(hImc);

		m_bVerticalReading = !GetReadingWindowOrientation();
    }

	ImmReleaseContext(hWnd, hImc);
}

void CIME::SetImeActive( bool bValue )
{
	m_bImeActive = bValue;

	if( bValue )
		ImmAssociateContext( GAMEAPP_ST.GetHWnd(), m_himOrg);
	else
		ImmAssociateContext( GAMEAPP_ST.GetHWnd(), NULL );
}

/*----------------------------------------------------------------------------*/ 
void	CIME::CheckToggleState(HWND hWnd)
{
	if( !m_bImeActive )
		return;

	if( ( m_bEdit == false )&&( m_bUsingEditDlg == false ) )
	{
		if( m_bLockChangeLanguage == true )
		{
			Native_Off( hWnd );
		}
		return;
	}

	if( m_bLockChangeLanguage == false )
	{
		/* Update Indicator */ 
		switch (PRIMARYLANGID(m_langId)) 
		{
		case LANG_KOREAN:
			m_bVerticalCandidate = false;
			m_wszCurrIndicator = g_aszIndicator[INDICATOR_KOREAN];
			break;

		case LANG_JAPANESE:
			m_bVerticalCandidate = true;
			m_wszCurrIndicator = g_aszIndicator[INDICATOR_JAPANESE];
			break;

		case LANG_CHINESE:

			m_bVerticalCandidate = true;
			switch(SUBLANGID(m_langId)) 
			{
			case SUBLANG_CHINESE_SIMPLIFIED:
				m_bVerticalCandidate = m_dwId[0] == 0;
				m_wszCurrIndicator = g_aszIndicator[INDICATOR_CHS];
				break;

			case SUBLANG_CHINESE_TRADITIONAL:
				m_wszCurrIndicator = g_aszIndicator[INDICATOR_CHT];
				break;

			default:
				m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
				break;
			}
			break;

		default:
			m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
			break;
		}
	}
	else
	{
		m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
	}
	

    if( m_wszCurrIndicator == g_aszIndicator[INDICATOR_NON_IME] )
    {
        char szLang[10];
        GetLocaleInfoA( MAKELCID( m_langId, SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, szLang, sizeof(szLang) );
        m_wszCurrIndicator[0] = szLang[0];
        m_wszCurrIndicator[1] = towlower( szLang[1] );
    }

	
	
	/* Check Toggle State */
	if( m_bLockChangeLanguage == false )
	{
		bool bIme = ImmIsIME( m_hkl ) != 0;
		HIMC hImc = ImmGetContext(hWnd);
		if( hImc ) {

			if( ( PRIMARYLANGID(m_langId) == LANG_CHINESE ) && bIme )
			{
				DWORD dwConvMode, dwSentMode;
				ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
				m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;

			}
			else
			{
				m_ImeState = ( bIme && ImmGetOpenStatus(hImc) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
			}
			ImmReleaseContext(hWnd, hImc);

		}
		else
		{
			m_ImeState = IMEUI_STATE_OFF;
		}
	}
	else
	{
		Native_Off( hWnd );
	}
}

/*----------------------------------------------------------------------------*/ 
int	GetCharsetFromLang( LANGID langid )
{
	switch( PRIMARYLANGID(langid) )
	{
	case LANG_JAPANESE:
		return SHIFTJIS_CHARSET;
	case LANG_KOREAN:
		return HANGEUL_CHARSET;
	case LANG_CHINESE:
		switch( SUBLANGID(langid) )
		{
		case SUBLANG_CHINESE_SIMPLIFIED:
			return GB2312_CHARSET;
		case SUBLANG_CHINESE_TRADITIONAL:
			return CHINESEBIG5_CHARSET;
		default:
			return ANSI_CHARSET;
		}
	case LANG_GREEK:
		return GREEK_CHARSET;
	case LANG_TURKISH:
		return TURKISH_CHARSET;
	case LANG_HEBREW:
		return HEBREW_CHARSET;
	case LANG_ARABIC:
		return ARABIC_CHARSET;
	case LANG_ESTONIAN:
	case LANG_LATVIAN:
	case LANG_LITHUANIAN:
		return BALTIC_CHARSET;
	case LANG_THAI:
		return THAI_CHARSET;
	case LANG_CZECH:
	case LANG_HUNGARIAN:
	case LANG_POLISH:
	case LANG_CROATIAN:
	case LANG_MACEDONIAN:
	case LANG_ROMANIAN:
	case LANG_SLOVAK:
	case LANG_SLOVENIAN:
		return EASTEUROPE_CHARSET;
	case LANG_RUSSIAN:
	case LANG_BELARUSIAN:
	case LANG_BULGARIAN:
	case LANG_UKRAINIAN:
		return RUSSIAN_CHARSET;
	case LANG_VIETNAMESE:
		return VIETNAMESE_CHARSET;
	default:
		return ANSI_CHARSET;
	}
}

/*----------------------------------------------------------------------------*/ 
int	GetCodePageFromCharset( int charset )
{
	switch( charset )
	{
	case SHIFTJIS_CHARSET:
		return 932;
	case HANGUL_CHARSET:
		return 949;
	case GB2312_CHARSET:
		return 936;
	case CHINESEBIG5_CHARSET:
		return 950;
	case GREEK_CHARSET:
		return 1253;
	case TURKISH_CHARSET:
		return 1254;
	case HEBREW_CHARSET:
		return 1255;
	case ARABIC_CHARSET:
		return 1256;
	case BALTIC_CHARSET:
		return 1257;
	case THAI_CHARSET:
		return 874;
	case EASTEUROPE_CHARSET:
		return 1250;
	case VIETNAMESE_CHARSET:
		return 1258;	
	default:
		return 1252;
	}
}

/*----------------------------------------------------------------------------*/ 
int	GetCodePageFromLang( LANGID langid )
{
	return GetCodePageFromCharset(GetCharsetFromLang(langid));
}

/*----------------------------------------------------------------------------*/ 

#ifdef DEFAULT_IME
bool CIME::IMEProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_CREATE )
		OnInputLanguageChange(hWnd, 0, (LPARAM)GetKeyboardLayout(0));	

	COMPOSITIONFORM cf;
	HIMC hImc;

	switch (message) 
	{
		/* IME Messages */ 
	case WM_INPUTLANGCHANGE:
		OnInputLanguageChange(hWnd, wParam, lParam);
		break;

	case WM_IME_SETCONTEXT:
		return true;

	case WM_IME_STARTCOMPOSITION:
		hImc = ImmGetContext(hWnd);
		if( hImc )
		{
			cf.dwStyle = CFS_POINT;

#ifdef IME_WINDOW_POSITION
			if( g_bIMEPosition == true )
			{
				cf.ptCurrentPos.x = g_nIMEWidth;
				cf.ptCurrentPos.y = g_nIMEHeight;
			}
			else
			{
				cf.ptCurrentPos.x = g_nScreenWidth - (g_nScreenWidth / 8);
				cf.ptCurrentPos.y = g_nScreenHeight - (g_nScreenHeight / 8);
			}
#endif

			ImmSetCompositionWindow(hImc, &cf);
			ImmReleaseContext(hWnd, hImc);
		}
		return true;

	case WM_IME_COMPOSITION:
		if(OnComposition(hWnd, wParam, lParam)) {
#ifdef MACROKEY_NOTINPUT
			if( m_bEdit == false )
			{
				HIMC hIme = ImmGetContext(GAMEAPP_ST.GetHWnd());
				ImmNotifyIME( hIme, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			}
#endif
			return false;
		}
		break;

	case WM_IME_ENDCOMPOSITION:
		if(OnEndComposition(hWnd, wParam, lParam)) {
			return false;
		}
		break;

	case WM_IME_NOTIFY:
		//return false;
		if(OnNotify(hWnd, wParam, lParam)) {
			return false;
		}
		break;
	case WM_CHAR:
		OnChar(hWnd, wParam, lParam);
		break;
		/* End of IME Messages */ 
	}

	return true;
}
#else
bool CIME::IMEProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_CREATE )
		OnInputLanguageChange(hWnd, 0, (LPARAM)GetKeyboardLayout(0));	

	switch (message) 
	{
	/* IME Messages */ 
	case WM_INPUTLANGCHANGE:
		OnInputLanguageChange(hWnd, wParam, lParam);
		break;

	case WM_IME_SETCONTEXT:
		lParam = 0;
		break;

	case WM_IME_STARTCOMPOSITION:
		return false;

	case WM_IME_COMPOSITION:
		if(OnComposition(hWnd, wParam, lParam)) {
			return false;
		}
		break;

	case WM_IME_ENDCOMPOSITION:
		if(OnEndComposition(hWnd, wParam, lParam)) {
			return false;
		}
		break;

	case WM_IME_NOTIFY:
		//return false;
		if(OnNotify(hWnd, wParam, lParam)) {
			return false;
		}
		break;
	case WM_CHAR:
		OnChar(hWnd, wParam, lParam);
		break;
	/* End of IME Messages */ 
	}

	return true;
}
#endif

bool	CIME::GetEdit()
{
	return m_bEdit;
}

void	CIME::SetEdit(bool bEdit, int nLength, const TCHAR* str)
{
	m_nLenght = nLength;

	m_bEdit = bEdit;

	if( m_bEdit == false )
	{
		m_bNumberOnly = false;
	}	

	if( m_bLockChangeLanguage == false )
	{
		if( ( m_bEdit == false )&&( GetImeState() == 1 ) )
		{
			Native_Off( GAMEAPP_ST.GetHWnd() );
		}
		else if( GetImeState() == 1 )
		{
			Native_On( GAMEAPP_ST.GetHWnd() );
		}
	}
	else
	{
		Native_Off( GAMEAPP_ST.GetHWnd() );
	}
		

	if(str)
	{
		m_input = str;
	}
	else
	{
		m_input.resize(0);
	}

	m_comp.resize(0);
	m_reading.resize(0);
}

void CIME::ReleaseText()
{
	m_input.resize(0);
	m_comp.resize(0);
	m_reading.resize(0);
}

void CIME::SetUsingEditDlg( bool bUsing )
{
	m_bUsingEditDlg = bUsing;

	if( m_bLockChangeLanguage == false )
	{
		//if( ( m_bUsingEditDlg == false )&&( GetImeState() == 1 ) )	// 키보드 영문으로 초기화 코드 제거
		//{
		//	Native_Off( GAMEAPP_ST.GetHWnd() );
		//}
		//else if( GetImeState() == 1 )

		if( GetImeState() == 1 )
		{
			Native_On( GAMEAPP_ST.GetHWnd() );
		}
	}
	else
	{
		/*if( ( m_bUsingEditDlg == true )&&( GetImeState() == 0 ) )
			Native_Off( GAMEAPP_ST.GetHWnd() );*/
	}
#ifdef MACROKEY_NOTINPUT	// 단축키 입력이 ime 에 남아 채팅 입력 시 해당 키 값이 나옴, 아래 함수로 강제 삭제
	HIMC hIme = ImmGetContext(GAMEAPP_ST.GetHWnd());
	ImmNotifyIME( hIme, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
#endif
}

void CIME::Native_On(HWND hWnd)
{
    HIMC hIMC;
    DWORD dwConv, dwSent, dwTemp;

    hIMC = ImmGetContext(hWnd);
    ImmGetConversionStatus(hIMC, &dwConv, &dwSent);

    //if ((dwConv & IME_CMODE_NATIVE))
    //    return;

    dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
    dwTemp |= IME_CMODE_NATIVE;

    dwConv = dwTemp;

	if( hIMC )
	{
		if( PRIMARYLANGID(m_langId) == LANG_CHINESE )
		{			
			m_ImeState = ( dwConv & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;

		}
		else
		{
			m_ImeState = ( hIMC && ImmGetOpenStatus(hIMC) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
		}
		ImmSetConversionStatus(hIMC, dwConv, dwSent);
		ImmReleaseContext(hWnd, hIMC);
	}
	else
	{
		m_ImeState = IMEUI_STATE_OFF;
	}
}

// void CIME::ImeUi_EnableIme( bool bEnable )
// {
// 	if( bEnable )
// 		ImmAssociateContext( GAMEAPP_ST.GetHwnd(), g_himcOrg );
// 	else
// 		ImmAssociateContext( GAMEAPP_ST.GetHwnd(), NULL );
// 
// 	g_bImeEnabled = bEnable;
// 
// 	HWND hwndImeDef = ImmGetDefaultIMEWnd(GAMEAPP_ST.GetHwnd());
// 	if ( hwndImeDef )// Fix for Zooty #3995: prevent CHT IME toobar from showing up
// 	{
// 		SendMessageA(hwndImeDef, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0);
// 		SendMessageA(hwndImeDef, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0);
// 	}
// 
// 	CheckToggleState(GAMEAPP_ST.GetHwnd());
// }

void CIME::Native_Off(HWND hWnd)
{
    HIMC hIMC;
    DWORD dwConv, dwSent, dwTemp;

    hIMC = ImmGetContext(hWnd);
    ImmGetConversionStatus(hIMC, &dwConv, &dwSent);

    /*if (!(dwConv & IME_CMODE_NATIVE))
        return;*/

    dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
    dwConv = dwTemp;

    if( hIMC )
	{
		if( PRIMARYLANGID(m_langId) == LANG_CHINESE )
		{			
			m_ImeState = ( dwConv & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;

		}
		else
		{
			m_ImeState = ( hIMC && ImmGetOpenStatus(hIMC) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
		}
		if( m_ImeState == IMEUI_STATE_ON )
		{
			ImmSetConversionStatus(hIMC, dwConv, dwSent);
		}		
		ImmReleaseContext(hWnd, hIMC);
	}
	else
	{
		m_ImeState = IMEUI_STATE_OFF;
	}
}

void CIME::AddInput( TCHAR* szAdd, int nAddLength )
{
	int nCheckLength = m_nLenght - 1;

	if( m_bNumberOnly == false )
	{
		for( int i=0; i<nAddLength; ++i )
		{
			if( ( (int)m_input.size() > nCheckLength )||( szAdd[ i ] == NULL ) )
				return;

			m_input.push_back( szAdd[ i ] );
		}
	}
	else
	{
		for( int i=0; i<nAddLength; ++i )
		{
			if( ( (int)m_input.size() > nCheckLength )||( szAdd[ i ] == NULL ) )
				return;

			if( ( szAdd[ i ] < L'0' )||( szAdd[ i ] > L'9' ) )
				return;

			m_input.push_back( szAdd[ i ] );
		}
	}
}

#ifdef TAIWAN_DAYI_IME
bool CIME::DaYiComparison()
{
	if( m_hkl == _CHT_HKL_DAYI )
	{
		return true;
	}
	return false;
}
#endif