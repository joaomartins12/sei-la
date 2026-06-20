#pragma once

#include "../00.Contents/BattleSystemContent.h"

class CBattleDigimonListUI : public cScriptUI, public CBattleSystemContents::ObserverType
{
	struct sDigimonData : public CUSTOMDATA
	{
		sDigimonData(unsigned int idx, std::wstring const& name ) : m_nDigimonIDX(idx),m_nDigimonClassName(name)
		{}
		~sDigimonData() {};

		unsigned int m_nDigimonIDX;
		std::wstring m_nDigimonClassName;
	};

public:
	CBattleDigimonListUI();
	virtual ~CBattleDigimonListUI();

public:
	bool			Create();
	BOOL			UpdateMouse();					
	BOOL			UpdateKeyboard(const MSG& p_kMsg);
	void			ResetDevice();

	virtual	bool	Construct(void);
	virtual void	Render();

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void			Destroy();

	void			_MakeDigimonListUI();
	void			_UpdateDigimonListUI( bool bVisible );

	void			_ChangeSelectDigimon(void* pSender, void* pData);
	void			_DigimonLeftMove(void* pSender, void* pData);
	void			_DigimonRightMove(void* pSender, void* pData);

	void			_ChangeStep(BRegist::eRegistState const& step);
private:
	/**		참여 가능 디지몬 목록 UI	*/
	cButton*		m_pDigimonListLeftMoveBtn;
	cButton*		m_pDigimonListRightMoveBtn;
	cGridListBox*	m_pDigimonGridListBox;
	/**								*/
};