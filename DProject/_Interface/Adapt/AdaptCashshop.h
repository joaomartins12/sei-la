#pragma once

class IUIContentBase;
class NewCashshopContents;

class AdaptCashshop
{
public:
	explicit AdaptCashshop(void);
	virtual ~AdaptCashshop(void);

	bool	SetContents(IUIContentBase* pContents);
	void	GetAllIncludePackageItemsFromProductIDX(DWORD const& dwProductID, std::wstring & wsProductName, LIST_PACKAGE_ITEMS & listData);

private:
	NewCashshopContents* m_pContents;
};