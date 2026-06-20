
#pragma once 

class CEmployment : public CsC_AvObject
{
	THREAD_MEMPOOL_H( CEmployment )
	CsCRTTI_H

public:
	CEmployment::CEmployment();


public:
	virtual void		Delete();
	virtual void		PreDelete();
	virtual void		Init( uint nUID, uint nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp );

public:
	virtual void		Update( float fDeltaTime, bool bAnimation = true );
	virtual void		Render();
	virtual void		Render_PostChar();	

	//====================================================================================
	//	파일 테이블
	//====================================================================================
protected:
	CsDigimon*			m_pFT_Digimon;

public:
	CsDigimon*			GetDigimonFT();

	//====================================================================================
	//		CallBack
	//====================================================================================
public:
	virtual bool		CallBack_EndofSequence( DWORD dwAniID );

	//====================================================================================
	//		Sound
	//====================================================================================
public:
	virtual void*		PlaySound( char* cSoundPath );

	//====================================================================================================
	//	부가 정보
	//====================================================================================================
public:	
	virtual UINT			GetClass();
	virtual UINT			GetUniqID();
	virtual NiColor			GetNameColor();
	virtual UINT			GetModelID()  const;
	virtual ANI::eTYPE		GetRandomIdle();
};