
#pragma once 

#include "HotKey.h"

struct sPickData
{
	sPickData() {Reset();}
	inline void Reset()
	{
		s_Type.m_nUID = 0;
	}
	cType s_Type;
};


class cGlobalInput : public CSingleton< cGlobalInput >
{
public:
	cGlobalInput();

	enum eMOUSE_INPUT
	{
		MOUSEINPUT_DISABLE,
		MOUSEINPUT_ENABLE,
		MOUSEINPUT_CAMERA,
		MOUSEINPUT_PICK,
	};

public:
	void				_Keyboard_Input( const MSG& p_kMsg );	//2015-12-29-nova 윈도우Msg	키보드	
	void				_Mouse_Input( eMOUSE_INPUT eMouse );
	void				KeyboardUpdate();	

private:
	void				Modified(const MSG& p_kMsg);		//2016-01-25-nova	조합키 체크

	bool				KBEventScene(const MSG& p_kMsg);	//2015-12-28-nova	이벤트씬 스킵 또는 다음으로
	bool				KBCashShop(const MSG& p_kMsg);		//2015-12-28-nova	캐쉬샵
	bool				KBChattingEsc(const MSG& p_kMsg);	//2015-12-29-nova	채팅창과 ESC키를 눌렀을경우의 처리

	bool				KeyBoardMenu(const MSG& p_kMsg);	//2015-12-28-nova	HotKeyMng 메뉴 처리
	bool				KeyBoardETC(const MSG& p_kMsg);		//2015-12-29-nova   채팅, 근거리적 선택, 인터페이스 보이기 여부 
	void				KeyBoardMove(const MSG& p_kMsg);	//2015-12-24-nova	키보드로 캐릭터 이동
	void				KeyBoardCamera(const MSG& p_kMsg);	//2015-12-24-nova	키보드로 카메라 조작
	void				KeyBoardCameraUpdate();				//2015-12-24-nova
	

	bool				bKBCamera[4];						//2015-12-24-nova 키보드로 회전에 Update에서 해주어야 자연스럽게 보임

protected:	
	bool				_MacroKey( const MSG& p_kMsg );		//2015-12-29-nova 윈도우Msg	키보드	

	bool				ScreenShot();						//2015-12-30-nova	VK_SNAPSHOT 화면캡쳐

	bool				_IsAbsoluteMacroKey(const MSG& p_kMsg);
	
	//=======================================================================================
	//		마우스 클릭
	//=======================================================================================
protected:
	CsCoolTimeSeq		m_SameClickCoolTime;

	//=======================================================================================
	//		픽킹
	//=======================================================================================
protected:
	sPickData			m_PickData;

public:
	void				ReleasePickData( CsC_AvObject* pObject );
protected:
	bool				_IsSameClick( cType type );

	//=======================================================================================
	//		카메라
	//=======================================================================================
protected:
	bool				m_bCameraRotation;
	CsPoint				m_ptBackupCursorPos;
		
public:
	bool				IsCameraRotation(){ return m_bCameraRotation; }
	bool				ReleaseCameraRotation(){ bool bReturn = m_bCameraRotation; m_bCameraRotation = false; return bReturn; }

	//=======================================================================================
	//		단축키
	//=======================================================================================
private:
	HotKey*				mHotKey;		//2016-01-25-nova	키설정
	uint				iModifier;		//2016-01-25-nova	Ctrl,Alt,Shift키 상태
	CsPoint				m_ClickPos;		//2016-02-03-nova	오른쪽 마우스 버튼 클릭 회전시 클릭위치 저장	
	MSG					m_Msg;			//2016-02-16-nova	윈도우MSG 저장
	

public:
	bool				GetEqualSnKey(WPARAM wparam, uint dmKey);	//2016-01-28-nova	키값이 같은지 비교
	bool				GetEqualModifier(uint dmKey);				//2016-01-28-nova	특수키값 비교
	bool				GetSpecialKeyCheck(const MSG& p_kMsg);		//2016-01-28-nova	Ctrl,Alt,Shift 상태
	HotKey*				GetHotKeyPtr();								//2016-01-28-nova	
	uint				GetModifier();								//2016-01-28-nova
	void				ResetModifier();							//2016-03-08-nova	Modifier 초기화
	void				SetHotKey(HotKey* hotkey);					//2016-01-28-nova	

	bool				IsEnter();									//Enter키 체크
	bool				IsEscape();									//2016-02-16-nova	ESC키 체크
	bool				IsShift();									//2016-03-02-nova	Shift 체크
	bool				IsAlt();									//2016-03-02-nova	Alt 체크
	void				KeyDNReset();								//2016-03-03-nova		KeyDown체크 리셋
	MSG					GetInputMsg() const;						//2016-06-30-eiless84	현재 MSG 확인.
	void				InitMsg();									//2016-06-30-eiless84	현재 MSG 초기화.
};

//---------------------------------------------------------------------------
// Singleton	//2016-01-28-nova
#define GLOBALINPUT_ST			cGlobalInput::GetSingleton()			
#define GLOBALINPUT_STPTR		cGlobalInput::GetSingletonPtr()		
//---------------------------------------------------------------------------
