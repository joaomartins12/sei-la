
#pragma once

class cToolTip_Right : public cTooltip
{
public:
	cToolTip_Right();
	~cToolTip_Right(){ Delete(); }

public:
	void			Delete();
	void			Init();	
	void			Render();
	void			Render( CsPoint ptPos );
	void			ResetDevice();

	void			SetRightTooltip( CsC_AvObject* pTarget ); //우하단 툴팁용

	void			Update( float const& fDeltaTime );

// protected:
// 	void			_Update();

protected:	
	cProgressBar*	m_pHPBar;

	//포인터만 존재
	CsC_AvObject*	m_pTarget;
};