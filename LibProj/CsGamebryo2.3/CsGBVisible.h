
#pragma once 


class CsGBVisible : public NiMemObject
{


public:
	enum ePLAG
	{
		VP_BILLBOARD			=	0x00000001,
		VP_PATICLE_CONTAINER	=	0x00000002,
	};	

public:
	static void		OnVisible( CsSortObj* pSortObj, NiAVObject* pObject, DWORD plagVP, float fAniTime );

	//=======================================================================================================
	//
	//		°Ù
	//
	//=======================================================================================================
public:
	static void		GetPlag( DWORD& dwReturn, NiAVObject* pObject );

protected:
	static void		_GetPlag( DWORD& dwReturn, NiAVObject* pObject );

	//=======================================================================================================
	//
	//		Billboard
	//
	//=======================================================================================================
protected:
	static bool		IsBillboard( DWORD plagVP ){ return ( (plagVP & VP_BILLBOARD) != 0); }
	static void		Billboard_Process( NiAVObject* pObject, float fAniTime );
	static void		_Billboard( NiBillboardNode* pNode, float fAniTime );
	static bool		_Billboard_RotateToCenter(	const NiCamera* pkCamera, 
												const NiPoint3& kWorldTranslate, NiPoint3& kCamD, NiPoint3& kCamU, 
												NiPoint3& kCamR );

	//=======================================================================================================
	//
	//		Particle Container
	//
	//=======================================================================================================
protected:
	static bool		IsPaticleContainer( DWORD plagVP ){ return ( (plagVP & VP_PATICLE_CONTAINER) != 0); }
	static void		PaticleContainer_Process( CsSortObj* pSortObj, NiAVObject* pObject, bool bParticle );
};