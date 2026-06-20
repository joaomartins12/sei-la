#pragma once

#include "../00.Contents/ServerRelocateContents.h"

class CTermsAndConditionUI;
class CBeCautiousUI;
class CRequestPageUI;

class CServerRelocatePage : public cScriptUI, public CServerRelocateContents::ObserverType
{
public:
	CServerRelocatePage();
	virtual ~CServerRelocatePage();

public:
	bool	Create();
	void	Update3DAccumtime( float const& fDeltaTime );
	void	Update(float const& fDeltaTime);
	BOOL	UpdateMouse();						//2016-01-13-nova	마우스 입력처리

	virtual	bool	Construct(void);
	virtual void	Render3DModel();
	virtual void	Render();

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void			_CreateUI();
	void			Destroy();
	void			ResetDevice();
	void			_SetPage(int const& nPageType);

private:
	CTermsAndConditionUI*	m_pTermsNConditionsUI;	// 이용 약관 페이지
	CBeCautiousUI*			m_pBeCautiousUI;		// 주의 사항 페이지
	CRequestPageUI*			m_pRequestPageUI;		// 서버 변경 요청 페이지
};