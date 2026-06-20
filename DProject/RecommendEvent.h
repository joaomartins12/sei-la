#pragma once

#include "RecommendEvent_Contents.h"

class cRecommendEvent : public cBaseWindow, public CRecommendEvent_Contents::ObserverType
{
public:
	struct sRecommendData : public CUSTOMDATA
	{
		sRecommendData( const int nType, const std::wstring TamerName = L"" )
			: m_nTreeItemType( nType ),
			m_strTamerName( TamerName )
		{
		}
		virtual ~sRecommendData() {};

		const int GetItemType() const;

		int				m_nTreeItemType;
		std::wstring	m_strTamerName;
	};

	struct sAchieveTreeItemInfo: public CUSTOMDATA
	{
		sAchieveTreeItemInfo( const int nIndex )
			: index(nIndex)
		{}
		virtual ~sAchieveTreeItemInfo() {};

		int	index;
	};

	cRecommendEvent();
	virtual ~cRecommendEvent();

	virtual	bool			Construct(void);

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	virtual void			OnMoveWindow();
	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			ResetMap(){ Close( false ); }

private:
	void					PressRewardButton( void* pSender, void* pData );
	void					PressCloseButton( void* pSender, void* pData );
	void					ClickTreeItem( void* pSender, void* pData );
	void					ClickTopCategory( void* pSender, void* pData );
	void					ChangeMouseOverCategory( void* pSender, void* pData );
	void					ReleaseMouseOverCategory( void* pSender, void* pData );
	void					OnClickBTInTreeItem(void* pSender, void* pData );

	void					MakeRecommendSubCategory( cTreeBoxItem& Root, const int nRecommendType, const std::wstring strName );
	void					MakeRecommendMainCategory( int nRecommendType );

	void					MakeAchieveTreeBox();
	void					ClearAchieveTreeBox();

	void					CreateAchieveTreeItem( const std::wstring& strTamerName );
	cString*				MakeAchieveContents(int TableIdx, int nMarkIndex, int nMarkValue, int nRecommendType, const std::wstring& strTamerName );
	cTreeBoxItem*		InitAchieveTreeItem( cString* pkContents, int TreeItemIdx );

	void					ResetAchieveTreeItem( const std::wstring& strTamerName );
	void					UpdateAchieveTreeItem( cString* pPurposeString, int RecommendType, int nMarkIndex, int nMarkValue, std::wstring strTamerName );
	std::wstring			MakePurposeText(  int nRecommendType, int nMarkIndex, std::wstring strTamerName = L"" );
	bool					AddPurposeText( cString* pPurposeString, int nMarkIndex, int nRecommendType, const std::wstring strTamerName );
	bool					MakePurposeElement( cString* pPurposeString, int nPurposeIndex, int nMarkValue );

	void					ShowSelectedImage( cTreeBoxItem* pSelectedItem );
	void					HideSelectedImage();
	
	void					ChangeTamerNameString( std::wstring BefoleName, std::wstring NewName );

	void					SetSelectedTamerNameItem( cTreeBoxItem* pItem );
	std::wstring			GetSelectedItemText();
	cString*				GetPurposeString( int nIndex );
private:
	cTreeBox*				m_pRecommendCategory;
	cTreeBoxItem*			mpkAchieves;
	cTreeBoxItem*			m_pSelectedTamerNameItem;
	cButton*				m_pBtnClose;
	cImage*					m_pMarkImage;
	cSprite*				m_pSelectedSprite;
	cSprite*				m_pMouseOverSprite;

	NiColor				mColor_Process;
	NiColor				mColor_Receivable;
	NiColor				mColor_ReceiveComplete;

};