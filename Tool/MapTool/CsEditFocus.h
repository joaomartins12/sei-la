
#pragma once 


class CsEditFosus : public CsEdit
{

public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};