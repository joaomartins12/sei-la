
#pragma once 

class CViewMainTab : public CViewBase
{
protected:
	_DlgMainTab			m_DlgMainTab;
public:
	CViewMainTab();
	~CViewMainTab();

public:
	virtual void		_Init();
	virtual _DlgBase*	_GetDlg(){ return &m_DlgMainTab; }
};