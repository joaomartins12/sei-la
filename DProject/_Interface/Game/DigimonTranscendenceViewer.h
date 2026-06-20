
#pragma once 
#include "DigimonArchiveSubViewer.h"
class CDigimonTanscendenceViewer : public DigimonArchiveSubViewer, public CDigimonArchiveContents::ObserverType
{
	enum DIGIMON
	{
		USING,
		ACCOMPANY1,
		ACCOMPANY2,
		ACCOMPANY3,
		END,
	};

	struct sMaterialCtr
	{
		sMaterialCtr():pkSlotIcon(NULL),dwDigimonIDX(0),nArchiveSlot(0){};
		~sMaterialCtr(){};

		void	SetIcon( DWORD const& dwIdx, int const& nSlot );
		void	SetEmpty();
		bool	IsRegist() const;
		cSprite* pkSlotIcon;
		DWORD	dwDigimonIDX;
		int		nArchiveSlot;
	};

	struct sSubMaterialCtr
	{
		sSubMaterialCtr():pkItemIcon(NULL),dwItemIDX(0),pCountText(NULL),sHaveCount(0),sNeedCount(0){}
		~sSubMaterialCtr() {}

		void	SetIcon( DWORD const& dwIdx, int const& nHaveCount, int const& nNeedCount );
		void	SetEmpty();		
		void	UpdateMaterialCount( int const& nMaterialCount);
		DWORD	GetItemCode() const;

		cString::sICON*	pkItemIcon;
		cString::sTEXT* pCountText;
		DWORD			dwItemIDX;
		int				sHaveCount;
		int				sNeedCount;
	};

	struct sEffectType : public CUSTOMDATA
	{
		sEffectType( int nType ):nChargeType( nType ) {};
		~sEffectType() {};

		int nChargeType;
	};

public:
	CDigimonTanscendenceViewer();
	~CDigimonTanscendenceViewer();

	virtual	bool			Construct(void);

public:
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( cWindow* pkRoot,  int nValue = 0 );
	virtual BOOL			UpdateMouse();
	virtual void			ResetDevice();
	virtual void			DeleteScript();
	void					UpdateViewer(void);

	virtual void			ReleaseSelect(void);
	virtual void			ReleaseSelect(CDigimonArchiveContents::SELECT_PART ePart);
	virtual void			OnChangeSelectedDigimon( const cData_PostLoad::sDATA* pkDATA, int SelectedPart );

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					CreateTaticsDigimonUI();
	void					UpdateSlotIcon(DWORD dwDigimonID, cGridListBoxItem* pkItem);
	void					UpdateLevelIcon(DWORD dwDigimonID,cGridListBoxItem* pkItem);
	void					OnMouseRClickInAccompany(void* pSender, void* pData);
	void					OnMouseMoveOnItem(void* pSender, void* pData);
	void					OnMouseMoveOnAccompanyItem(void* pSender, void* pData);
	void					OnMouseDownOnAccompanyItem(void* pSender, void* pData);
	void					OnMouseUpOnAccompanyItem(void* pSender, void* pData);
	void					OnMouseMoveOnMaterialDigimon(void* pSender, void* pData);
	void					OnMouseMoveOnSubMaterialItem(void* pSender, void* pData);

	void					OnItemClicked_UsingDigimon(void* pSender, void* pData);
	void					OnItemClicked_AccompanyDigimon(void* pSender, void* pData);
	void					PressNormalAddExpBtn(void* pSender, void* pData);
	void					PressTranscendBtn(void* pSender, void* pData);
	void					PressAdvancedAddExpBtn(void* pSender, void* pData);
	void					OnMouseRClickMaterialRemove(void* pSender, void* pData);
	void					_MakeEmptyMaterialSlot( int const& nEmptyCount );
	void					_MakeEmptySubMaterialSlot( int const& nEmptyCount );
	void					_UpdateExpProgressbar( const cData_PostLoad::sDATA* pkDATA );
	void					_UpdateSubMaterial(int const& nIdx, DWORD const& dwItemIDX, int const& nHaveCount, int const& nNeedCount );

	void					_UpdateMaterialCount( int const& nMaterialCount );
	void					_RegistDigimonUpdate( DWORD const& dwSelDigimonIDX );
	void					_SetMaterialDigimon( int const& nIdx, CDigimonArchiveContents::sTranscendMaterialInfo const& transData );

	void					_UpdateTranscendNeedMoney( const cData_PostLoad::sDATA* pkDATA );
	void					_SetSubMaterialItem( DWORD const& dwSelDigimonIDX );
	int						_GetRegistMaterialDigimonCount();


	void					_EffectEnd(void* pSender, void* pData);

	void					_SetChargeExpStart( int const& nChargeType );
	void					_SetChargeExpEnd();
	void					_ResultExpCharge( bool const& bSuccess, bool const& bOverExp );

	void					_OnClickHelpButton( void* pSender, void* pData );

private:
	cGridListBox*			m_pkPlayer;
	cGridListBox*			m_pkAccompanies;
	cGridListBox*			m_pkGridMaterial;
	cGridListBox*			m_pkInsertDigimon;
	cButton*				m_pNormalExpAddBtn;
	cButton*				m_pAdvancedExpAddBtn;
	cButton*				m_pTranscendBtn;
	cMoneyIF*				m_pSpendMoney;
	cUI_ProgressBar*		m_pProgressbar;
	cUI_ProgressBar*		m_pOverProgressbar;

	cGridListBox*			m_pSubMaterial;
	cSpriteAni*				m_pEffect;
	cSpriteAni*				m_pResultEffect;
	cText*					m_pExpGuageText;
	cSpriteAni*				m_pSuccessEffect;
	cSpriteAni*				m_pExcellentEffect;

	cButton*				m_pHelpBtn;

	std::map<int,sMaterialCtr>	m_materialList;
	std::list<sSubMaterialCtr>	m_subMaterialList;
};