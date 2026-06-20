#pragma once

class IUIContentBase;
class CMacroProtectContents;

class AdaptMacroProtect
{
public:
	explicit AdaptMacroProtect(void);
	virtual ~AdaptMacroProtect(void);

	bool	SetContents(IUIContentBase* pContents);

	bool	IsMacroProtectUsing() const;

private:
	CMacroProtectContents* m_pContents;
};