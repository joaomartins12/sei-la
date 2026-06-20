
#pragma once 

#define IF_BUFFINFO_MAX_ROW			2

#define	IF_BUFF_MY_T				0
#define	IF_BUFF_MY_D				1
#define	IF_BUFF_SYSTEM				2

#define	IF_BUFF_TARGET				3
		  
#define	IF_BUFF_PARTY1_T			4
#define	IF_BUFF_PARTY1_D			5
		  
#define	IF_BUFF_PARTY2_T			6
#define	IF_BUFF_PARTY2_D			7
		  
#define	IF_BUFF_PARTY3_T			8
#define	IF_BUFF_PARTY3_D			9
		  
#define	IF_BUFF_PARTY4_T			10
#define	IF_BUFF_PARTY4_D			11

#define IF_BUFF_ACHIEVE				999

class cBuffInfo : public cBaseWindow
{
public:
	cBuffInfo();

public:
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_BUFFINFO; }
	virtual void				Destroy();
	virtual void				DeleteResource();
	virtual void				Create( int nValue = 0 );

	virtual void				PlayOpenSound(){}
	virtual void				PlayCloseSound(){}

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();
	virtual void				OnRButtonUp( CsPoint pos );
	virtual void				Render();

	virtual void				ResetDevice();	

	// 메모리잡힌 컨트롤
protected:
	int							m_nType;	
	int							m_nDeltaX;
	int							m_nDeltaY;
	int							m_nStructCount;
	int							m_nMAX_Col;
#ifdef BUFFINFO_FIX
	int							m_nBuffCnt;	// 버프갯수, 버프 윈도우 사이즈 결정
#endif

	cImage*						m_pBGImage;
	cBuffData*					m_pBuffData;
	cText*						m_pTextTime;

	cButton*					m_pPlusButton;
	cButton*					m_pMinusButton;

protected:
	struct sBuffWin
	{
		cButton*					s_pCurButton;	// 포인터만 존재
		cPopUp*						s_pPopup;
		bool						s_bRenderPopup;
	};
	sBuffWin					m_sWin[ 2 ];

	USHORT						m_Buff_Client_Type;
public:
	void						SetBuffUser(USHORT	ctype);
	USHORT						GetBuffUser() const;

public:
	void						SetBuffData( cBuffData* pData );
	void						ReleaseData();
	void						SetEncyBuff( u2 nIdx, u4 nEndTime ) 
	{ 
		if( m_pBuffData )
			m_pBuffData->_SetEncyBuff( nIdx, nEndTime ); 
	}
#ifdef BUFFINFO_FIX
	void						RenderEncyBuff( u2 nIdx, CsPoint ptIconSize, CsPoint ptDeltaPos = CsPoint::ZERO );	// 덱 버프 렌더
	bool						SetEncyTooltip( u2 nPosIdx, u2 nApplyIdx, CsPoint ptDeltaPos = CsPoint::ZERO );		// 덱 버프 툴팁
#endif

private:
	int				m_AddSystempBuffPosY;	// ADD_QUICKSLOT 오픈 상태일때 시스템 버프 표시 높이 구분
public:
	//퀵슬롯 오픈여부에 따라 위치 옮겨야됨
	CsPoint			m_ptRootPos;	
	void			SetAddSystempBuffPosY(int y);

public:
#ifdef BUFFINFO_FIX
	CsPoint			m_ptResolution;	//해상도, 해상도변경에 따른 변화 셋팅
	bool			_IsRenderPopUp( int nIdx ) { return m_sWin[nIdx].s_bRenderPopup; }
#endif

#ifdef GUILD_RENEWAL
	cImage*			m_pGuildBuffIcon;	// 적용 중인 길드 스킬
#endif

	int GetDeltaPosY(void);
};