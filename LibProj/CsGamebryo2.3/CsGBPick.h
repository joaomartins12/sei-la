
#pragma once 

class CsGBPick : public NiMemObject
{
protected:
	static NiPick*		m_pNiTriPick;
	static NiPick*		m_pNiBoundPick;
public:
	CsGBPick(	bool bObserveAppCullFlag = true, 
				NiPick::SortType eSort = NiPick::SORT,
				bool bFrontOnly = true,
				bool bReturnColor = false );
	~CsGBPick();

	static void			Delete();
	static void			Create( NiPick::PickType ePick, NiPick::CoordinateType eCoordinate,
								bool bFrontOnly, bool bReturnNormal );

		   bool			Pick_IsBound( NiAVObject* pTarget, NiPoint3 origin, NiPoint3 dir );
		   bool			Pick( NiAVObject* pTarget, NiPoint3 origin, NiPoint3 dir, bool bAppend );	
		   NiPick::Record*	GetRecord();

	static void			SetFrontOnly( bool bFrontOnly );
	static void			SetObserveAppCull( bool bAppCull );
	
protected:
	inline void			Clear();
};

#include "CsGBPick.inl"