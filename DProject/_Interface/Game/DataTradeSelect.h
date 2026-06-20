#pragma once

class DataTradeSelect : public cBaseWindow, public DataTradeContents::ObserverType
{
public:
	DataTradeSelect(void);
	virtual ~DataTradeSelect(void);

	////////////							COMMON				//////////////////
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DATA_TRADE_SELECT; }

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

protected:
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();	
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	////////////						UNCOMMON				//////////////////
private:
	cButton*				m_pCloseButton;					// 닫기 버튼
	cButton*				m_pExitButton;					// 나가기 버튼

private:
	struct sSelectState : public CUSTOMDATA
	{
		sSelectState( int nState ) : s_nSelectState( nState ) {}
		~sSelectState() {}

		int				s_nSelectState;
	};

	struct sSelectInfo
	{
		sSelectInfo() : s_pSelectBtn(NULL),s_pSelectTxt(NULL),s_pItemCntTxt(NULL),s_pItemIcon(NULL) {}
		~sSelectInfo() {}

		cString::sANIBUTTON*	s_pSelectBtn;
		cString::sTEXT*			s_pSelectTxt;
		cString::sTEXT*			s_pItemCntTxt;
		cString::sICON*			s_pItemIcon;
	};
	std::map< int, sSelectInfo >	m_mapSelectInfo;	// SystemType::eSTATE, sSelectInfo
	cGridListBox*					m_pSelectGrid;

private:
	void					_MakeSelectInfo();
	void					_AddSelectInfo( int const& nState );

	void					_SetTooltip( cGridListBoxItem const* pOverItem );

	void					_OnClickSelectButton( void* pSender, void* pData );
};