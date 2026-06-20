

#pragma once 

#define DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT		200.0f
#define	DIGIMONUSER_FOLLOW_RANGE							2600.0f

class CDigimon : public CsC_AvObject
{
public:
	void				DeletePath();
	void				DoPos_ByTamer();
	void				SetTargetPos();

	void				Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp );

	void				Update( float fDeltaTime, bool bAnimation =true );

protected:
	void				_UpdateMove();
	bool				_IsDoMove();
	void				_MoveStop_ByArriveDist();
	NiPoint2			_GetArrivePos_ByTamer();

	


protected:
	CsOnceTimeSeq		m_MoveOnceTimeSeq;
	CsCoolTimeSeq		m_MoveCoolTimeSeq;
};



class CTamer : public CsC_PartObject
{
public:
	void				DeletePath();
};
