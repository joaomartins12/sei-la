#pragma once

class cRewardDailyCheckUI : public IEventViewer
{
public:
	cRewardDailyCheckUI();
	virtual ~cRewardDailyCheckUI();

protected:
	virtual	void		Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool		Construct(void);

public:
	virtual void		Create(cWindow* pkRoot, int nValue = 0 );
	virtual BOOL		UpdateMouse();
	virtual void		SetSubViewer( int const& nSubType );

private:
	cText*				m_pTitle;
	cText*				m_pDescript;
	cText*				m_pEventDate;
	cGridListBox*		m_pRewardGrid;

	int					m_nSubIndex;

private:
	void				_ConvertEventDateFormat( std::wstring& wsEventDate );

	void				_MakeRewardItemGrid();
	void				_SetRewardItemGrid();
	void				_AddRewardItem( int const& nIndex, uint const& nItemID, uint const& nItemCount,
										uint const& nTodayIndex, std::wstring const& wsDate, bool const& bCheck );
};