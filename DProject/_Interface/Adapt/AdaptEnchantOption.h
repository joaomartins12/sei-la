#pragma once

class IUIContentBase;
class cEnchantOptionContents;

class AdaptEnchantOption
{
public:
	explicit AdaptEnchantOption(void);
	virtual ~AdaptEnchantOption(void);

	bool			SetContents( IUIContentBase* pContents );
	bool			IsProcessingEnchant() const;

private:
	cEnchantOptionContents* m_pContents;
};