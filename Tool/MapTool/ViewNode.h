
#pragma once 

class CViewNode : public CViewBase
{
protected:
	_DlgNode			m_DlgNode;
public:
	CViewNode();
	~CViewNode();

public:
	virtual void		_Init();
	virtual _DlgBase*	_GetDlg(){ return &m_DlgNode; }
};