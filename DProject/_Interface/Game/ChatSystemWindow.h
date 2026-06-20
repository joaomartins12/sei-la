#pragma once

class ChatSystemWindow : public ChatBase

{
public:
	ChatSystemWindow(void);
	virtual ~ChatSystemWindow(void);

////////////							COMMON				//////////////////

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();	
	virtual void			ResetMap();
	virtual bool			Close( bool bSound /* = true  */ );
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	virtual	void			StringListRender();
	virtual void			SetAlpha(float alpha);
	virtual void			ControlUpdate(int SclrollSize);

	// event 함수
protected:	
	virtual	void			_ChageBGSize( void * sender, void* pData );	// 확대를 위한 함수
	virtual	void			_ChageBGUpdate( void * sender, void* pData );	// 확대를 위한 조절 함수
	virtual	void			_MoveUi( void * sender, void* pData );			// 이동을 위한 함수

////////////						UNCOMMON				//////////////////

private:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CHAT_SYSTEM_WINDOW; }

private:
	TCHAR			m_szLastStr[ 512 ];
private:
	void			_SetText( TCHAR* szText, bool bParcing, bool bCheckSameString );
	void			_AddSetText(ContentsStream const& kStream);

	void			_SetTextWithColor(ContentsStream const& kStream); // [4/14/2016 hyun] 아이템 컬러 적용
};