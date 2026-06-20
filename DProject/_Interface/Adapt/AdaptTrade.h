#pragma once

class IUIContentBase;
class CTradeContents;

class AdaptTradeContents
{
public:
	explicit AdaptTradeContents(void);
	virtual ~AdaptTradeContents(void);

	bool	SetContents(IUIContentBase* pContents);

	bool	IsTrading() const;

private:
	CTradeContents* m_pContents;
};