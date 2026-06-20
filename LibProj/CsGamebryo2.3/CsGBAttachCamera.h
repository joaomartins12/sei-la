
#pragma once

class CsGBAttachCamera : public NiMemObject
{
public:
	virtual ~CsGBAttachCamera(){ Delete(); }

	// ==== Attach Camera
protected:
	struct sINFO : public NiMemObject
	{
		sINFO(){ s_pCamera = NULL; }
		void	Delete(){ s_pCamera = 0; }
		void	Reset(){ s_fAniTime = 0.0f; }

		NiNodePtr		s_pCamera;
		NiTransform		s_InverseTR;
		float			s_fAniTime;
		float			s_fEndTime;
		bool			s_bOwner;
	};
	NiTStringPointerMap< sINFO* >*		m_pmapInfo;
	sINFO*								m_pCurApplyCamera;

public:
	void			Delete();
	void			Init();

	void			AttachCamera( char* cName, bool bOwner );
	void			Update( float fDeltaTime, NiCamera* pSrcCamera );

	NiCamera*		GetCameraObj(){ if( m_pCurApplyCamera == NULL ){ return NULL; } return (NiCamera*)m_pCurApplyCamera->s_pCamera->GetAt( 0 ); }
	sINFO*			GetCurApplyCamera(){ return m_pCurApplyCamera; }
};

