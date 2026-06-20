#pragma once

class IUIContentBase;
class CMainFrameContents;

class AdaptMainFrame
{
public:
	explicit AdaptMainFrame(void);
	virtual ~AdaptMainFrame(void);

	bool	SetContents(IUIContentBase* pContents);

	CsC_AvObject*	GetTargetObject() const;

private:
	CMainFrameContents* m_pContents;
};