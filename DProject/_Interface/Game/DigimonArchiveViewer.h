#pragma once 
#include "../00.Contents/DigimonArchiveContents.h"
#include "DigimonArchiveSubViewer.h"
class CDigimonArchiveViewer : public cBaseWindow, public CDigimonArchiveContents::ObserverType
{

	enum eTabType
	{
		eInfoViewer = 0,
		eIncubator,
		eTranscend,
	};
	struct sTabType : public CUSTOMDATA
	{
		sTabType(eTabType eType):eViewerType(eType) {}
		~sTabType() {};

		eTabType	eViewerType;
	};
	struct SlotInfo : public CUSTOMDATA
	{
		const int GetSlotIdx(void){return slotidx;}
		void SetSlotIdx(const int iSlotIdx){slotidx = iSlotIdx;}
		SlotInfo(int islotidx, bool bisOpened)
			:slotidx(islotidx),
			isOpened(bisOpened)
		{
		}
		int slotidx;
		bool isOpened;
	};
protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		


public:
	static 	std::string		GetLVToPath(int iCurrLV);

	CDigimonArchiveViewer();
	~CDigimonArchiveViewer();

	virtual	bool			Construct(void);
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TACTICSHOUSE; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice(void);
	virtual void			ResetMap(void){ Close( false ); }
	virtual bool			Close( bool bSound = true );
	BOOL					UpdateMouse();					
	BOOL					UpdateKeyboard(const MSG& p_kMsg);	//2016-01-13-nova	키보드 입력처리
	bool					OnEscapeKey();

	virtual void			UserDie(){ Close( false ); }
/////////////////////////////////////////////////////////////////////////////////////	//
	void					RecvInitializeArchive(void);
	void					RecvUpdateArchive(void);
	void					UpdateOpenPos(void);
	void					UpdateOpenNPC(void);
	void					RecvChangeSelectedDigimon(int SelectedPart);
	
	void					UpdateSlotIcon(CDigimonArchiveContents::DigimonInArchive* pkArchiveInfo, cGridListBoxItem* pkItem);
	void					ResetSlotIcon(cGridListBoxItem* pkItem);
	void					OnLButtonDown( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );
	/////////////////////////////////////////////////////////////////////////////////////	//

//
	void			OnClickMenuTab(void* pSender, void* pData);
	void			OnClickShowInfoViewer(void);
	void			OnClickShowIncuViewer(void);
	void			OnClickShowTranscendViewer(void);
	void			OnClickChangeFilterMode(void* pSender, void* pData);
	void			OnClickChangeSorterMode(void* pSender, void* pData);
	void			OnClickSerching(void* pSender, void* pData);
	void			OnClickResetSerching(void* pSender, void* pData);
//
	void			OnItemSelectedInDigimonList(void* pSender, void* pData);
	void			OnItemClickedInDigimonList(void* pSender, void* pData);
	void			OnMouseDownOnItem(void* pkSender, void* pData);
	void			OnMouseUpOnGridList(void* pkSender, void* pData);
	void			OnMouseMoveOnItem(void* pkSender, void* pData);
	void			OnMouseRClickInArchive(void* pkSender, void* pData);
	void			OnMouseLeaveFormArchive(void* pkSender, void* pData);
	void			ResetGridList(int iViewerCnt);

	void			MakeFilterComboBox(void);
	void			MakeSorterComboBox(void);
	void			OnChangeSechingEditBox(void* pkSender, void* pData);
	void			OnEnterFocusSechingEditBox(void* pkSender, void* pData);
	void			OnReleaseFocusSechingEditBox(void* pkSender, void* pData);

	bool			UpdateAutoClose(void);
	bool			IsIncubatorModeOn(void);
	void			OnRClick_Inventory_Item(void* pkInvenSLotIDx);

private:
	NiPoint2					mOpenedPos;//아이템에 의해 뷰어가 켜질 때 아이템 쓴 장소의 포지션
	CsPoint						mSlotSize;
	CsC_AvObject*				mpOpenedNPC;		// NPC에 의해 뷰어가 켜질 때 NPC의 포인터 
	cComboBox*					m_pkFilterCB;
	cComboBox*					m_pkSortingCB;
	cRadioButton*				m_pTabMenu;
	cGridListBox*				m_pkGridArchive;
	cButton*					m_pMagnifierBT;		//Filtering Digimons in Archive
	cButton*					m_pResetSerchingBT;   // Sort Digimons in Archive;
	cButton*					m_pCloseBT;
	cEditBox*					m_pEditSerchingDigimon;
	cText*						mTTDigimonCntInArchive;
	cText*						mTTDigimonOpenedCnt;
	NiColor						mDefaultColor_OpenedSlotCnt;
	bool						m_bisDragable;

	DigimonArchiveSubViewer*	m_pkCurrViewer;
	DigimonArchiveSubViewer*	m_pkInfoViewer;
	DigimonArchiveSubViewer*	m_pkIncuViewer;
	DigimonArchiveSubViewer*	m_pkTranscendViewer;
};