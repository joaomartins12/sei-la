#pragma once

class CViewSetUp : public CViewBase
{
protected:
	_DlgSetup m_DlgSetUp;
public:
	CViewSetUp(void);
	~CViewSetUp(void);
	virtual void		_Init();
	virtual _DlgBase*	_GetDlg(){ return &m_DlgSetUp; }

};
