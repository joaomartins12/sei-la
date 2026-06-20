#pragma once

#define MAX_BATTLELOG				25			// 가지고 있는 채팅 줄 갯수
#define BATTLELOG_LENGTH			128		
#define MAX_LINE					7

class ChatBattleLogWindow : public ChatBase

{
public:
	ChatBattleLogWindow(void);
	virtual ~ChatBattleLogWindow(void);
	
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
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CHAT_BATTLELOG_WINDOW; }
	struct sBattleInfo : public NiMemObject 
	{
		sBattleInfo(){ memset( s_szBattleLog, 0, sizeof( s_szBattleLog ) ); }

		float		s_fRemainTime;
		NiColor		s_Color;
		TCHAR		s_szBattleLog[ BATTLELOG_LENGTH ];		
	};

	typedef	std::list< sBattleInfo* >			LIST_BATTLE_INFO;
	typedef	std::list< sBattleInfo* >::iterator	LIST_BATTLE_INFO_IT;

	LIST_BATTLE_INFO			m_pBattleLogList;
	std::list< sBattleInfo* >::iterator InfoIter;

private:	
	void		_UpdateText( TCHAR* szText, NiColor vColor );
	void		SetLog( CsC_AvObject* pHitter, CsC_AvObject* pTarget, float nRemainTime, int nTotalDem, sRECV_HITTER_INFO* pHitType, sRECV_TARGET_INFO* pDemType );	
	void		SetLog( CsC_AvObject* pHitter, CsC_AvObject* pTarget, float nRemainTime );

private:	// Notify 내 함수
	void		_AddSixLog( ContentsStream const& kStream );
};