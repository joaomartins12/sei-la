
#pragma once 

class cTip : public NiMemObject
{
public:
	static	void		GlobalInit();
	static	void		GlobalShotDown();

public:
	void				Init();
	void				PreResetMap();
	void				Delete();

	void				Update(float const& fDeltaTime);

protected:
	CsTimeSeq			m_TimeSeq;
};

extern cTip*		g_pTip;