
#pragma once 

class cHelper
{
public:
	void		ResetDevice();
	void		Delete();
	void		Init();

protected:
	cSprite*	m_pBG;


public:
	void		Update();
	void		Render();
};