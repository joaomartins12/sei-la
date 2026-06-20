
#pragma once 

class CItem		:	public	CsC_AvObject
{
	THREAD_MEMPOOL_H( CItem )
	CsCRTTI_H
public:
	virtual void		Delete();
	virtual void		Init( uint nUID, uint nFileTableID, nSync::Pos sPos, sCREATEINFO* pProp, DWORD nOwnerUID );

	virtual void		Update( float fDeltaTime );

protected:
	DWORD				m_nOwnerUID;

public:
	DWORD				GetOwnerUID();

	//====================================================================================
	//		Sound
	//====================================================================================
public:
	virtual void*		PlaySound( char* cSoundPath );

	//====================================================================================================
	//	ºÎ°¡ Á¤º¸
	//====================================================================================================
public:
	virtual UINT		GetClass();
	virtual UINT		GetUniqID();
	virtual UINT		GetModelID() const;
	virtual NiColor		GetNameColor();
};