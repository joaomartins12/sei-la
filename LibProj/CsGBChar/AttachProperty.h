
#pragma once 

//class CsC_AttachProp : public CsC_Property
//{
//	THREAD_MEMPOOL_H( CsC_AttachProp )
//
//public:
//	enum eATTACH_TYPE
//	{
//		ATT_DLEVEL_EFFECT,
//		ATT_MAX_TYPE,
//	};
//
//	struct sATTACH
//	{
//		sATTACH(){ s_pNiNode = NULL; s_pValue1 = NULL; s_pRefLoad = NULL; }
//		void Delete(){ s_pNiNode = NULL; s_pValue1 = NULL; if( s_pRefLoad ){ s_pRefLoad->DecreaseRef(); s_pRefLoad = NULL; } }
//
//		NiNodePtr			s_pNiNode;
//		sLOAD_THREAD_REF*	s_pRefLoad;
//		void*				s_pValue1;
//	};
//
//protected:
//	int				m_DeleteReadyRefCnt[ ATT_MAX_TYPE ];
//	sATTACH			m_AttachArray[ ATT_MAX_TYPE ];
//
//public:
//	void			AddAttach( void* pData );
//	void			DelAttach( eATTACH_TYPE at );
//	sATTACH*		GetAttach( eATTACH_TYPE at ){ return &m_AttachArray[ at ]; }
//
//protected:
//	void			_UpdateEnviromentEffect();
//
//public:
//	CsC_AttachProp();
//	virtual void			Delete();
//	bool					IsDelete();
//	virtual void			PreDelete();
//
//	void					Init( CsC_AvObject* pParent );
//	virtual void			Update( float fDeltaTime );
//	virtual void			Render();
//
//public:
//	static void GlobalEffectProp( sCharProp* pProp, CsC_AttachProp::eATTACH_TYPE at );
//};