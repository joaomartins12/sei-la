#pragma once

#ifndef __LOGIN_H__
#define __LOGIN_H__

#include "../00.Contents/LoginContents.h"

class CLogin : public cScriptUI, public LoginContents::ObserverType
{
public:
	CLogin();
	virtual ~CLogin();

public:
	void	LoginInit();
	void	Render();
	void	Render3DModel();
	void	RenderScreenUI();

	void	Update(float fDeltaTime);
	void	Update3DAccumtime( float const& fAccumTime );	
	BOOL	UpdateMouse();						//2016-01-13-nova	마우스 입력처리
	BOOL	UpdateKeyboard(const MSG& p_kMsg);	//2016-01-13-nova	키보드 입력처리

	virtual	bool		Construct(void);

private:
	void	Destroy();
	void	ResetDevice();
	void	SetFocusEdit();
	void	ReleaseFocusEdit();

	void	_MakeCopyright();
	void	_MakeLoginBackWindow(bool bVisible);
	void	_MakeLoginWindow(bool bVisible);

	// [4/27/2016 hyun] 추가
#ifdef USE_BARCODE_REDER
	void	PressBarcodeCheckBox(void* pSender, void* pData);
#endif
	void	PressIntroButton(void* pSender, void* pData);
	void	PressOpeningButton(void* pSender, void* pData);
	void	PressLoginButton(void* pSender, void* pData);
	void	PressExitButton(void* pSender, void* pData);
	void	PressIDSaveCheckBox(void* pSender, void* pData);
	void	PressConfigeButton(void* pSender, void* pData);
	void	ChangeIP(void* pSender, void* pData);
	void	SetAccountInfoFromIni();
	void	_HomepageOpen(void* pSender, void* pData);

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	
private:
	cScriptUI		m_LogIn;
	cEditBox*		m_pEditBoxID;
	cEditBox*		m_pEditBoxPW;
// 	cButton*		m_pIntro;
// 	cButton*		m_pCredit;
	cCheckBox*		m_pIDSaveCheckBox;
	cCheckBox*		m_pUseBarcodeCheckBox;
	cStringList		m_Copyright;

	cButton*		m_pLoginBtn;
	cButton*		m_pHomePageBtn;

	cScriptUI		m_LogInBack;
	cButton*		m_pExitBtn;		// 게임 종료
	cButton*		m_pConfigeBtn;	// 환경 설정 버튼

 	cStringList		m_Msg;
 	cStringList		m_Msg2;
};

#endif