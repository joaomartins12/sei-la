//---------------------------------------------------------------------------
//
// 파일명 : Win32App.h
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
namespace App
{
	class CWin32App
	{
	public:
		static void __stdcall UnicodeToUTF8(std::wstring const& wsSource, std::string& sDest);
		static void __stdcall UnicodeToMultiByte(std::wstring const& wsSource, std::string& sDest);
		static void __stdcall UTF8ToUnicode(std::string const& sSource, std::wstring& wsDest);
		static void __stdcall MultiByteToUnicode(std::string const& sSource, std::wstring& wsDest);

	public:
		enum
		{
			WINDOWS_FORM_STYPE = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		};
		enum PROCESS_STATE
		{
			PROCESS_RUN,PROCESS_STOP,PROCESS_EXIT,
		};

		CWin32App();
		virtual ~CWin32App();

	protected:
		virtual BOOL OnInitialize() = 0;	              // 초기화						
		virtual void OnIdle()       = 0;                  // 루프
		virtual void OnIdleExtern() {}					  // 외부루프
		virtual void OnTerminate()  = 0;                  // 종료
		virtual BOOL OnMsgHandler(const MSG& p_kMsg) = 0; // WndProc 메시지

	public:
		BOOL Startup( LPCTSTR p_lpszWndClass, LPCTSTR p_lpszWndTitle, 
			UINT p_uiX, UINT p_uiY,
			UINT p_uiWidth, UINT p_uiHeight, 
			BOOL p_bFullMode );

		int  Run();
		void Shutdown();

		void	SetProcessState( PROCESS_STATE const& eState );

		void CalculateSize(WORD uiWidth, WORD uiHeight, bool bFullMode);
		void ReSize(WORD uiWidth, WORD uiHeight, bool bFullMode, bool bScreenModeChanged);
		void GetWorkScreenWorkArea(RECT& rcScreen);

		// Get
		HINSTANCE   GetHInstance() const;
		HWND		GetHWnd()      const;
		UINT        GetWidth()     const;
		UINT        GetHeight()    const;
		BOOL        GetFullMode()  const;
		//_wstring GetDocumentDirectory(void) const;

		//void		BeginPageFalut();
		//bool		EndPageFalut();

	protected:
		// WndProc
		static LRESULT CALLBACK WndProc( HWND   p_hWnd, 
										 UINT   p_uiMessage, 
										 WPARAM p_wParam, 
										 LPARAM p_lParam );

		void UpdateScreenSize(void);

	protected:
		static CWin32App* ms_pkWin32App;
		//DWORD GetPageFaultCount();

	protected:
		HINSTANCE   m_hInstance;
		HWND		m_hWnd;
		UINT        m_uiWidth;
		UINT        m_uiHeight;
		BOOL        m_bFullMode;

		DWORD		m_dwStyle;
		DWORD		m_dwExStyle;

		UINT		m_uiWindowModeScreenX;
		UINT		m_uiWindowModeScreenY;
		RECT		m_uiWindowModeWorkSpaceRect;

	private:
		PROCESS_STATE m_eProcessState;
	};
}
//---------------------------------------------------------------------------