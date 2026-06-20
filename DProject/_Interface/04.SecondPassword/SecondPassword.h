#pragma once 

#include "../00.Contents/SecondPasswordContents.h"

class cSecondPassword : public cScriptUI, public SecondPasswordContents::ObserverType
{
	struct sPasswordNumber : public CUSTOMDATA
	{
		sPasswordNumber(TCHAR const* nNum)
			: m_nNumber(*nNum)
		{}
		~sPasswordNumber() {};
		TCHAR m_nNumber;
	};

public:
	cSecondPassword();
	virtual ~cSecondPassword();

public:
	void				Init();
	void				ResetDevice();

	void				Update();
	void				Render();
	void				Render3DModel();
	void				RenderScreenUI();

	BOOL				UpdateMouse();			
	void				Update3DAccumtime( float const& fAccumTime );				//2016-01-13-nova	마우스 입력처리
	BOOL				UpdateKeyboard(const MSG& p_kMsg);	//2016-01-13-nova	키보드 입력처리

	virtual	bool		Construct(void);

private:
	void				Destroy();	

	void				SetConversionStr();
	void				ClearUI();
	void				ConfirmError(void* pData);
	void				MakeCopyright();

	void				_CreatePassNumberButton();

	void				_CreateWindow_Regist();		// 비번 등록 창 생성
	void				_CreateWindow_RegistConfirm();	// 비번 등록 확인 창 생성

	void				_CreateWindow_AskPW();			// 비번 확인 창 생성

	void				_CreateWindow_CurrentPwWindow(); // 비번 변경전 현재 사용 하고 있는 비밀 번호 입력 창 
	void				_CreateWindow_PwChange();		// 비번 변경 창 생성
	void				_CreateWindow_PwChangeConfirm();// 비번 변경 확인 창 생성

	void				_CreateWindow_UsingAsk();		// 비번 등록 요청 윈도우 생성
	void				_CreateWindow_Change2ndPassword_Ask();	// 비번 변경 요청 윈도우 생성

	
	//////////////////////////////////////////////////////////////////////////
	void				ClickNumberButton(void* pSender, void *pData);	// 키 입력 패드 
	void				PressDeleteButton(void* pSender, void *pData);	// 키 입력 패드의 삭제 버튼
	void				PressClearButton(void* pSender, void *pData);	// 키 입력 패드의 지우기 버튼

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	void				PressCheckOkButton(void* pSender, void *pData);		// 2차 비밀 번호 확인 창의 확인 버튼 이벤트
	void				PressCheckCancelButton(void* pSender, void *pData);	// 2차 비밀 번호 확인 창의 취소 버튼 이벤트
	void				Press2ndPassChangeButton( void* pSender, void* pData );

	//////////////////////////////////////////////////////////////////////////
	void				PressUsingButton(void* pSender, void* pData);	// 2차 비밀 번호 등록 요청 창의 등록 버튼 이벤트
	void				PressLaterButton(void* pSender, void* pData);	// 2차 비밀 번호 등록 요청 창의 취소 버튼 이벤트

	//////////////////////////////////////////////////////////////////////////
	void				Press2ndPassRegistFirst(void* pSender, void* pData); // 2차 비밀 번호 등록 창의 확인 버튼 이벤트
	void				Press2ndPassRegistSend(void* pSender, void* pData); // 2차 비밀 번호 등록 확인창의 확인 버튼 이벤트
	void				PressRegistCancelButton(void* pSender, void *pData);	// 2차 비밀 번호 확인 창의 취소 버튼 이벤트

	//////////////////////////////////////////////////////////////////////////
	void				Press2ndPassSaveCurrent(void* pSender, void* pData);	// 변경전 사용하고 있는 비밀 번호 저장
	void				Press2ndPassSaveChange(void* pSender, void* pData);	// 2차 비밀 번호 변경 창의 확인 버튼 이벤트
	void				Press2ndPassChangeSend(void* pSender, void* pData);		// 2차 비밀 번호 변경 확인창의 확인 버튼 이벤트
	void				PressChangePWOkButton(void* pSender, void *pData);		// 2차 비밀 번호 변경 승인
	void				PressChangePWCancelButton(void* pSender, void *pData);	// 2차 비밀 번호 변경 취소
	void				PressCancleChangeButton( void* pSender, void* pData );


protected:
	virtual	void		Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	cText*				m_ConversionStr;
	cStringList			m_StringList;
	cStringList			m_Copyright;

	cButton*			m_pOk;
	cButton*			m_pDelete;
	cButton*			m_pClear;	
	cButton*			m_pCancel;
	cButton*			m_pChange;
	std::list<cButton*>	m_NumButtonList;

	cScriptUI			m_KeyPadWindow;
	cScriptUI			m_AskWindow;

	cButton*			m_pUsingSPW;
	cButton*			m_pUsingLaterSPW;
	cCheckBox*			m_pCheckUseSPW;
};