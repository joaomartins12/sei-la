
#pragma once 
#include "DigimonArchiveSubViewer.h"
class CDigimonIncubatorViewer : public DigimonArchiveSubViewer, public CDigimonArchiveContents::ObserverType
{
	enum BT_TYPE
	{		
		_SLOT,
		_ITEM,
		_START,
		_END,
	};
	struct BT_INFO : CUSTOMDATA
	{		
		int BTTYPE;
		int ParentIncuSlotIdx;
		bool isIncubating;
		BT_INFO()
			:BTTYPE(0),
			ParentIncuSlotIdx(0){
		}
	};
	struct INCU_INFO : CUSTOMDATA
	{
		int IncuSlotInfo;
		INCU_INFO()
			:IncuSlotInfo(0){}
	};

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream){};
public:
	virtual	bool			Construct(void);
	CDigimonIncubatorViewer();
	~CDigimonIncubatorViewer();
public:
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( cWindow* pkRoot,  int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			UpdateMouse();
	virtual void			Render();
	virtual void			ResetDevice();
	virtual void			DeleteScript();


	void				ReleaseSelect(void);
	void				OnItemClickedInIncuList(void* pSender, void* pData);
	void				OnItemRClickedInIncuList(void* pSender, void* pData);
	void				OnRButotnUpOnSprite(void* pSender, void* pData);
	void				OnDragUpOnSprite(void* pSender, void* pData);
	void				OnMouseDownOnSprite(void* pSender, void* pData);
	void				OnMouseUpStartIncubating(void* pSender, void* pData);
	void				OnMouseUpOnGridList(void* pkSender, void* pData);
	void				OnMouseDownOnItem(void* pkSender, void* pData);
	void				OnMouseMoveOnItem(void* pkSender, void* pData);
	void				OnMouseMoveOnSprite(void* pkSender, void* pData);
	int					OnRecvFacilitation( u1 nSlot, u4 nRemainTime );
	int					OnRecvIncubator(int DigimonSlotIdx, int ItemIdx, int RemainTime );
	void				UpdateViewer(void);
	void				UpdateElement(cString* pkString, CsDigimon::sINFO* pFTDigimon);
	void				UpdateRemainTime(void);
private:
	cGridListBox*		m_pIncuListBox;
	cText*				m_pIncubatorDescTitle;
	cText*				m_pIncubatorDesc;
	bool				m_bisDragable;
	float				m_fElapsedTime;
};