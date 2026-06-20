#pragma once 
#include "UserServerMove.h"


class CTutorialTamerUser	:	public	CTamerUser
{
	CsCRTTI_H
public:
	CTutorialTamerUser();
	~CTutorialTamerUser();

public:
	virtual void		Init( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwServerOption );
	virtual void		Init( uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwServerOption );
	virtual void		Update( float fDeltaTime, bool bAnimation = true );

protected:
	void				_Init();
	virtual void		_UpdatePortal(const float& fDeltaTime);
};