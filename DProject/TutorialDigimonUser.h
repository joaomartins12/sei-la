
#pragma once 

#include "UserServerMove.h"

class CTutorialDigimonUser : public CDigimonUser
{
	CsCRTTI_H

public:
	CTutorialDigimonUser();
	~CTutorialDigimonUser();

	virtual void		Init( uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, int nUserIndex, bool bInitPos );
	virtual void		Init( uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, int nUserIndex, bool bInitPos );

public:
	virtual void		Update( float fDeltaTime, bool bAnimation = true );

protected:
	void				_Init();
	void				_UpdateAttack_ClientMode( float fDeltaTime );
};