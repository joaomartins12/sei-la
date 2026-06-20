#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class SecondPasswordContents
	: public IUIContentSubject< SecondPasswordContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eEventType
	{
		eSet_ConversionStr,				// 변환 문자 설정 이벤트

		eCreate_InitRegistWindow,		// 최초 설정창 생성 이벤트
		eCreate_RegistConfirmWindow,	// 설정 확인창 생성 이벤트

		eCreate_CurrentPasswordWindow,	// 변경 전 현재 비밀 번호를 입력창
		eCreate_PwChangeWindow,			// 변경 기존 비번 입력 창 생성 이벤트
		eCreate_PwChangeConfirmWindow,	// 새로 변경할 비번 입력 창 생성 이벤트

		eCreate_AskPasswordWindow,		// 비번 일치 확인창 생성 이벤트
		eCreate_AskPwChangeWindow,		// 비번 변경 요청 확인창 생성 이벤트
	};

	enum ePassWindowType	{ eWindowType_Regist = 0, eWindowType_AskPW, eEnd };

public:
	SecondPasswordContents(void);
	virtual ~SecondPasswordContents(void);

	virtual int const	GetContentsIdentity(void) const;
	virtual bool		Initialize(void);
	virtual void 		UnInitialize(void);
	virtual bool 		IntraConnection(ContentsSystem& System);
	virtual void 		Update(float const& fElapsedTime);
	virtual void 		NotifyEvent(int const& iNotifiedEvt);
	virtual void 		MakeMainActorData(void);
	virtual void 		ClearMainActorData(void);
	virtual void		MakeWorldData(void);
	virtual	void		ClearWorldData(void);
	virtual void		NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	
public:
	//////////////////////////////////////////////////////////////////////////
	// 2차 비밀번호 입력 관련 함수
	bool				SendCurrent2ndPassword();	// 서버에 2차 비밀 번호 확인 요청	

	//////////////////////////////////////////////////////////////////////////
	// 2차 비밀 번호 등록 관련 함수
	void				SaveRegistPassword();			// 등록할 2차 비밀 번호 저장
	void				CheckRegistPasswordAndSend();	// 저장 된 2차 비밀 번호와 다시 입력한 비밀 번호 비교 후 서버에 등록 요청

	//////////////////////////////////////////////////////////////////////////
	// 2차 비밀 번호 변경 관련 함수
	void				OpendChangeWindow();			// 2차 비밀 번호 변경 윈도우 열기 
	void				SaveCurrentPassword();			// 변경전 사용하고 있는 비밀 번호 저장
	void				SaveChangePassword();			// 변경 할 비밀번호 저장
	void				CheckChangePasswordAndSend();	// 변경 할 비밀 번호 비교 후 서버에 변경 패킷 보냄

	void				GotoBackFromAskWindow();		// 2차 비밀 번호 입력 창에서 취소시 처리
	void				Cancel2ndPassChange();			// 2차 비밀 번호 변경 창에서 취소시 처리
	void				Cancel2ndPassChangingWindow();// 2차 비밀 번호 변경 창에서 취소시 처리
	void				Cancel2ndPassRegistWindow();


	const int			GetPassWindowType() const;

	std::wstring		GetCurrPassword() const;
	void				ClearPasswordData();

	void				ClearInputPassword();
	void				DeleteOnePassNumber();
	bool				AddPasswordData(TCHAR const& pNumber);

	void				SkipInputSecondPassword(bool bHideOneDay);

	void				RenderBackgroundMap();
	void				UpdateBackgroundMap(float const& fAccumTime);

	void				Load3DBackgroundData();
	void				Remove3DBackgroundData();

private:
	bool				Load3DEffect(std::string const& loadFileName);

private:
	void				Recv2ndPasswordCheck( void* pData );
	void				Recv2ndPasswordChange( void* pData );
	void				Recv2ndPasswordRegister( void* pData );

	bool				_CheckPasswordUnabled( std::wstring const& pw, size_t nMinSize, size_t nMaxSize );

private:
	ePassWindowType 	m_ePassWindowType;

	std::wstring		m_strCurrPW;
	std::wstring		m_strPrePW;		// 
	std::wstring		m_strConfirmPW;	// 비교용 데이터

	CsNodeObj			m_Effect;
};
