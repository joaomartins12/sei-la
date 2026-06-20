#pragma once 

#include "AchieveContents.h"

class cAchieve : public cBaseWindow, public AchieveContents::ObserverType
{
public:
	struct sCategotyType : public CUSTOMDATA
	{
		sCategotyType(int nMainIdx, int nSubIdx)
			: m_nMainIdx(nMainIdx),m_nSubIdx(nSubIdx)
		{}
		~sCategotyType() {};

		int m_nMainIdx;
		int m_nSubIdx;
	};

	struct sDispTitleSeq : public CUSTOMDATA
	{
		sDispTitleSeq(int seq,int questID): m_nSeq(seq), m_nQuestID(questID)
		{}
		~sDispTitleSeq() {};

		int m_nSeq;
		int m_nQuestID;
	};

public:
	cAchieve();
	virtual ~cAchieve();	

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream) {}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_ACHIEVE; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();

	virtual void			OnLButtonUp( CsPoint pos ) {}

	virtual void			ResetDevice();

	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnMoveWindow();

private:
	cButton*				m_pCancelButton;
	cButton*				m_pCheckAllAchieve;	
	cButton*				m_pDispTitleApplyBtn;

	cTreeBox*				m_pAchieveCategoty;
	cListBox*				m_pAchieveListBox;
	cScrollBar*				m_pScrollBar;

	cString::sSPRITE*		sSprite;	

	std::list<cSprite*>		m_DispTitleSprList;

	int						m_nTotalPoint;
	int						m_nMainSel;

	int						m_nDispTitleSel;
	int						m_nDispTitleQuest;

	void					CreateTitle();
	void					MakeCategoty();

	void					SetGroup(int mainIdx, int subIdx);		//¾÷Àû
	void					SetDispTitle();							//È¹µæÅ¸ÀÌÆ²
	void					SelectDispTitle(int seq, int questID);	//È¹µæÅ¸ÀÌÆ² ¼±ÅÃ¼³Á¤
	void					DisplayToolTip(bool dispTitle, int questID);
	void					RenderToolTip(CsAchieve::sINFO* pFTInfo);

	void					PressCancelBtn( void* pSender, void* pData );
	void					PressCheckAllAchieveBtn( void* pSender, void* pData );	
	void					PressTitleApplyBtn( void* pSender, void* pData );	
	void					SelectedCategoty(void* pSender, void* pData);	
};