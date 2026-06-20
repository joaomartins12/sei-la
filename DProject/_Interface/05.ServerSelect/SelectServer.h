
#pragma once 

#include "../00.Contents/ServerSelectContents.h"

class CSelectServer : public cScriptUI, public ServerSelectContents::ObserverType
{
	struct sSeverNumber : public CUSTOMDATA
	{
		sSeverNumber(UINT nNum)
			: m_nNumber(nNum)
		{}
		~sSeverNumber() {};
		UINT m_nNumber;
	};

public:
	CSelectServer();
	virtual ~CSelectServer();

public:
	bool					Init();
	void					ResetServerList();
	void					Close();
	void					Render();
	void					RenderScreenUI();
	BOOL					UpdateMouse();						//2016-01-13-nova	마우스 입력처리
	BOOL					UpdateKeyboard(const MSG& p_kMsg);	//2016-01-13-nova	키보드 입력처리

	void					ResetDevice();
	virtual	bool			Construct(void);

	void					Render3DModel();
	void					Update3DAccumtime( float const& fAccumTime );

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);


private:
	void					_MakeCopyright();
	void					_MakeServerListBack();
	void					_MakeServerList();
	void					_ChangeListSize( size_t const& nServerCount );
	void					_MakeListControl( size_t const& nServerCount );
	void					LastConnectedServerSelect(UINT nSelectServerIdx);

	// [4/27/2016 hyun] 추가
	void					PressOkButton(void* pSender, void* pData);
	void					PressCancelButton(void* pSender, void* pData);
	void					PressRefreshButton(void* pSender, void* pData);
	void					SeletedServer(void*pSender, void* pData);

	// @@ 여기까지

protected:
	cScriptUI				m_SelectBack;
	cButton*				m_pConnectServerBtn;
	cButton*				m_pGotoBackBtn;

	cScriptUI				m_SelectListWindow;	
	cButton*				m_pListRefreshBtn;
	cGridListBox*			m_pServerListBox;

	cStringList				m_Copyright;
};