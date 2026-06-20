#pragma once

#include "../../ContentsSystem/ContentsSystem.h"
#include "common_vs2019/pPass2.h"

class LoginContents
	: public IUIContentSubject< LoginContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eEventType
	{
		eRequest_Login,
	};

public:
	LoginContents(void);
	virtual ~LoginContents(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);

	bool		StartSelectServer(wchar *id, wchar *pass, bool bSelectServer );
	void		TryToLogin( const TCHAR* strID, const TCHAR* strPass );

	void		GameExit(void) const;

	void		ShowOpening();
	void		ShowIntro();

	bool		IsEnableLoginTime();
	void		ResetLoginTime();

	bool		IsShowVideo() const;
	void		SetShowVideo(bool bShow);
	void		ToggleIDSaveCheckBox(bool bCheck);
	void		ToggleUseBarcode(bool bCheck);

	void		RenderBackgroundMap();
	void		UpdateBackgroundMap(float const& fAccumTime);

	void		Load3DBackgroundData();
	void		Remove3DBackgroundData();
	
private:
	void		LogInTimeSeqReset(void* pData);
	void		RecvLogInRequest(void* pData);
	void		RecvLoginWaitTime(void* pData);
	void		RecvUseBarcodeFalse(void* pData);	//2017-07-11-nova
	bool		Load3DEffect(std::string const& loadFileName);

#ifdef USE_BARCODE_REDER
	bool		CheckBarcodeReader();
#endif	

private:
	CsTimeSeq		m_LogInTimeSeq;
	bool			m_bShowVideo;	//2016-02-04-nova	인트로와 제작진 VK_RETURN 키로 넘겼을때 로그인으로 바로 가지 않도록

	CsNodeObj		m_Effect;
};
