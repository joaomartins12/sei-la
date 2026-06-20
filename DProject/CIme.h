#ifndef __IME_H__
#define __IME_H__

#include <imm.h>
#include <objbase.h>
#include <dimm.h>

#include <string>
#include <vector>

class CIMM
{
public:
	CIMM();
	virtual ~CIMM();

	void	Init();

	INPUTCONTEXT*	LockIMC(HIMC hImc);
	BOOL			UnlockIMC(HIMC hImc);
	LPVOID			LockIMCC( HIMCC );
	BOOL			UnlockIMCC( HIMCC );

protected:

	HINSTANCE		m_hDllImm32;

	INPUTCONTEXT*	(WINAPI * _ImmLockIMC)( HIMC );
	BOOL			(WINAPI * _ImmUnlockIMC)( HIMC );
	LPVOID			(WINAPI * _ImmLockIMCC)( HIMCC );
	BOOL			(WINAPI * _ImmUnlockIMCC)( HIMCC );
};



/*----------------------------------------------------------------------------*/ 
int GetCharsetFromLang(LANGID);
int	GetCodePageFromLang( LANGID langid );

/*----------------------------------------------------------------------------*/

class CIME
{
public:
	CIME();
	~CIME();

	void		InitInput();

	/* messages */ 
	void		OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
	bool		OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);
	bool		OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);
	bool		OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
	bool		OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);

	int			GetInput(char* text, int len);
	int			GetComp(char* text, int len);
	int			GetReading(char* text, int len);

	int			GetInputComp(TCHAR* text);

	void		GetInput(std::wstring* pString);
	int			GetInput(TCHAR* text);
	int			GetInput(TCHAR* text, int len);

	int			GetComp(TCHAR* text);
	int			GetReading(TCHAR* text);

	int			GetCandidate(DWORD index, char* text, int len);
	int			GetCandidateCount();
	int			GetCandidateSelection();
	int			GetCandidatePageSize();
	int			GetCandidatePageStart();
	void		GetUnderLine(int* start, int* end);

	/* property */ 
	bool		IsVerticalReading(){ return m_bVerticalReading; }
	bool		IsVerticalCandidate(){ return m_bVerticalCandidate; }
	WORD		GetCodePage() { return m_codePage; }
	LANGID		GetLangId() { return m_langId; }
	int			GetCharSet() { return GetCharsetFromLang(m_langId); }
	wchar_t*	GetIndicator() { return m_wszCurrIndicator; }
	int			GetImeState() { return m_ImeState; }

	bool		IMEProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool		GetEdit();
	void		SetEdit(bool bEdit, int nLength = 20, const TCHAR* str=NULL);
	void		ReleaseText();

	void		Native_On(HWND hWnd);
	void		Native_Off(HWND hWnd);

	//int			GetLimitLength(){ return m_nLenght+1; }
	void		AddInput( TCHAR* szAdd, int nAddLength );

	void		SetImeActive( bool bValue );

protected:
	void		SetupImeApi(HWND hWnd);
	void		GetImeId();
	bool		GetReadingWindowOrientation();
	void		GetPrivateReadingString(HWND hWnd);
	void		CheckToggleState(HWND hWnd);

protected:
	std::wstring		m_input;
	std::wstring		m_comp;
	std::wstring		m_reading;
	std::vector<BYTE>	m_candidate;

	bool				m_bImeActive;// IME기능을 활성 비활성
	HIMC				m_himOrg;

	int					m_ulStart;
	int					m_ulEnd;

	HKL					m_hkl;
	LANGID				m_langId;
	WORD				m_codePage;

	bool				m_bUnicodeIME;
	bool				m_bVerticalReading;
	bool				m_bVerticalCandidate;
	int					m_ImeState;
	wchar_t*			m_wszCurrIndicator;

	DWORD				m_dwId[2];

	HINSTANCE			m_hDllIme;

	UINT				(WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
	BOOL				(WINAPI * _ShowReadingWindow)( HIMC, BOOL );
	int					m_nLenght;

protected:
	bool				m_bEdit;
	bool				m_bNumberOnly;
	bool				m_bUsingEditDlg;
	bool				m_bLockConidateList;
	bool				m_bLockChangeLanguage;
	
#ifdef NOT_ENTER_CREATENAME
	bool				m_bCombination;	// 단어 조합중인지에 대한 flag
#endif

public:
	void				SetUsingEditDlg( bool bUsing );

	void				SetNumberOnly( bool bNumberOnly ){ m_bNumberOnly = bNumberOnly; }

	void				SetLockConidateList( bool bLock ){ m_bLockConidateList = bLock; }
	void				SetLockChangeLanguage( bool bLock ){ m_bLockChangeLanguage = bLock; }
	bool				IsLockChangeLanguage() const { return m_bLockChangeLanguage; }
#ifdef THAI_SPECIALWORD

private:
	bool				m_bKEY_ALTDOWN;
	bool				m_bKEY_ALTUP;
public:
	void				SetKEY_ALTDOWN()	{ m_bKEY_ALTDOWN = true; }
	void				SetKEY_ALTUP()		{ m_bKEY_ALTUP = false;  }
	std::wstring*		Get_InputString()	{ return &m_input; }

#endif

#ifdef NOT_ENTER_CREATENAME
	bool				GetCombination() { return m_bCombination; }
	void				SetCombination(bool bFlag) { m_bCombination = bFlag; }
#endif

#ifdef TAIWAN_DAYI_IME
	bool				DaYiComparison();	// DaYi 입력법인지 검사
#endif

};

extern CIME g_IME;

#endif