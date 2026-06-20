
#pragma once 


class cChangeTamer : public cBaseWindow, public cInventoryContents::ObserverType
{
public:
	cChangeTamer();
	~cChangeTamer();

	virtual bool			Construct(void);
	virtual void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CHANGETAMER; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey();
	virtual void			OnMoveWindow();

private:
	struct sUIInfo : public CUSTOMDATA
	{
		sUIInfo( int nVal1, int nVal2 ) : nValue1( nVal1 ), nValue2( nVal2 ) {}
		~sUIInfo() {}

		int				nValue1;
		int				nValue2;
	};

	struct sCommentUI
	{
		sCommentUI() : pNameBg(NULL), pCommentBg(NULL), pNameTxt(NULL), pCommentTxt(NULL) {}
		~sCommentUI() {}

		void			SetOnOff(bool bIsOn);
		void			SetUI( std::wstring const& wsName, std::wstring const& wsComment );

		cSprite*		pNameBg;
		cSprite*		pCommentBg;
		cText*			pNameTxt;
		cText*			pCommentTxt;
	};

	sCommentUI		m_sCommentUI;

	cListBox*		m_pChangeList;
	cButton*		m_pCancelButton;
	cButton*		m_pOkButton;

private:
	void 			_CreateChangeAdvanture();
	void 			_CreateChangeSavers(int const& nValue);

	void			_MakeChangeList(int const& nCount);
	void 			_AddChangeInfo(int const& nIndex, int const& nTamerID);

	void			_InitChangeListData(int const& nInvenIndex, std::list< int >& lChangeList);
	void			_ChangeTamerDiscript(std::wstring const& wsName, std::wstring const& wsComment);

private:
	void 			_OnClickChangeList(void* pSender, void* pData);
	void 			_OnClickChangeOK(void* pSender, void* pData);
	void			_OnClickClose(void* pSender, void* pData);
};