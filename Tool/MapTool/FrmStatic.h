

#pragma once

class CFrameStatic : public CFrameBase
{
	DECLARE_DYNCREATE(CFrameStatic)
public:
	CFrameStatic();
	virtual ~CFrameStatic();


public:
protected:
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	DECLARE_MESSAGE_MAP()
};