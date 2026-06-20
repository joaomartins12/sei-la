#pragma once 
//2017-03-07-nova

class cRewardMonthlyUI : public IEventViewer
{
public:
	cRewardMonthlyUI();
	virtual ~cRewardMonthlyUI();

public:
	virtual void		Create(cWindow* pkRoot, int nValue = 0 );
	virtual BOOL		UpdateMouse();
	virtual void		RenderScript();

protected:
	virtual	void		Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool		Construct(void);

private:
	void				UpdateRewardData();
	void				UpdateTimeMsg( int const& nDate );

private:
	cGridListBox*		m_pItemGride;
	cStringList			sDateText;
};