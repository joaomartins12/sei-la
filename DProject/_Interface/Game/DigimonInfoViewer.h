
#pragma once 
#include "DigimonArchiveSubViewer.h"
class CDigimonInfoViewer : public DigimonArchiveSubViewer, public CDigimonArchiveContents::ObserverType
{
	enum DIGIMON
	{
		USING,
		ACCOMPANY1,
		ACCOMPANY2,
		ACCOMPANY3,
		END,
	};
protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream){};

public:
	CDigimonInfoViewer();
	~CDigimonInfoViewer();
	virtual	bool			Construct(void);
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( cWindow* pkRoot, int nValue = 0 );
	virtual void			DeleteScript();

	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			UpdateMouse();
	virtual void			Render();
	virtual void			OnChangeSelectedDigimon(const cData_PostLoad::sDATA* pkDATA, int SelectedPart );
	virtual void			ResetDeviceScript();
	
	void			ReleaseSelect(void);
	void			ReleaseSelect(CDigimonArchiveContents::SELECT_PART ePart);
	void			UpdateViewer();
	void			_UpdateRenderDigimon();
	void			UpdateSlotIcon(DWORD dwDigimonID, cGridListBoxItem* pkItem);
	void			UpdateLevelIcon(DWORD dwDigimonID,cGridListBoxItem* pkItem);
	void			OnItemSelected_UsingDigimon(void* pSender, void* pData);
	void			OnItemClicked_UsingDigimon(void* pSender, void* pData);

	void			OnItemSelected_AccompanyDigimon(void* pSender, void* pData);
	void			OnItemClicked_AccompanyDigimon(void* pSender, void* pData);

	void			OnMouseDownOnUsingItem(void* pSender, void* pData);
	void			OnMouseUpOnUsingItem(void* pSender, void* pData);
	void			OnMouseMoveOnItem(void* pSender, void* pData);

	void			OnMouseDownOnAccompanyItem(void* pSender, void* pData);
	void			OnMouseUpOnAccompanyItem(void* pSender, void* pData);
	void			OnMouseMoveOnAccompanyItem(void* pSender, void* pData);
	void			OnMouseRClickInAccompany(void* pSender, void* pData);

	void			RenderSelectedDigimon(const cData_PostLoad::sDATA* pkDATA );
	void			RenderEmpty( void );

protected:
	bool				m_bisDragable;
	cText*				m_pDigimonLV;
	cText*				m_pDigimonName;
	cText*				m_pDigimonHeight;
	cText*				m_pDigimonType;
	cText*				m_pDigimonEnchant;
	cText*				m_pDig_St_Enchant;
	cText*				m_pDig_St_Type;
	cText*				m_pDig_St_Height;
	cImage*				m_pDigimonRankImg;
	CDigimon*		m_pRenderDigimon;
	cRenderTex*		m_pRenderTex;
	cGridListBox*		m_pkPlayer;
	cGridListBox*		m_pkAccompanies;

	cString			m_DigimonElement;
	CsPoint mSelectedDigPos;
	CsPoint mptLevelPos;
	CsPoint mptElementPos;
	CsPoint mptRankImgPos;
};
