
#pragma once 

namespace nsCSGBFUNC
{
	// 프로퍼티 상태
	struct CsProperty : public NiMemObject
	{
		CsProperty():s_pApplyNode(NULL){}
		virtual ~CsProperty(){}

		enum ePROPERT_TYPE{ PT_MATERIAL, };
		ePROPERT_TYPE	s_eType;
		NiAVObject*		s_pApplyNode;

		static void DeleteProperty( CsProperty* pProp );
		virtual void Delete(){}
		virtual void Init( NiAVObject* pApplyNode ){ s_pApplyNode = pApplyNode; }
	};

	struct CsMaterialProp : public CsProperty
	{
		//THREAD_MEMPOOL_H( nsCSGBFUNC::CsMaterialProp )

		CsMaterialProp():s_pVertexColorProp(NULL), s_bUseAlphaProp(false), s_bAlphaBlend(false), s_bAlphaTest(false), s_bExistMatProp(false){}
		~CsMaterialProp(){}

		virtual void Delete(){ s_pVertexColorProp = 0; }
		virtual void Init( NiAVObject* pObj ){ CsProperty::Init( pObj ); s_eType = PT_MATERIAL; s_pVertexColorProp = NULL; }

		// 메터리얼
		NiVertexColorPropertyPtr	s_pVertexColorProp;

		bool						s_bUseAlphaProp;
		bool						s_bAlphaBlend;
		bool						s_bAlphaTest;

		bool						s_bExistMatProp;

		NiTexturingProperty::ApplyMode	s_eTextureApplyMode;		
	};

	struct CsBackupProperty : public NiMemObject
	{
		CsBackupProperty() {};
		virtual ~CsBackupProperty(){ Delete(); }

		void			Delete();
		CsProperty*		PopProperty( NiAVObject* pApplyNode );
		std::list< CsProperty* >	s_listProperty; 
	};

	void InsertList_Geom( CsVectorPB< CGeometry* >* vpGeom, NiAVObject* pAvObject, int nGeomType );
	void InsertList_Geom_IncludeHide( CsVectorPB< CGeometry* >* vpGeom, NiAVObject* pAvObject, int nGeomType );
	void InsertList_Geom_Sorting( CsVectorPB< CGeometry* >* vpGeom, NiAVObject* pAvObject, int nGeomType );
	void _InsertList_Geom_Sorting(	CsVectorPB< CGeometry* >* vpGeom, CsVectorPB< CGeometry* >* vpSortGeom, NiAVObject* pAvObject, int nGeomType );

	NiTransformController*	GetTransformController( NiAVObject* pObject );
	void InitAnimation( NiObjectNET* pObject, NiTimeController::AnimType eAnimType, NiTimeController::CycleType eCycleType );
	void StartAnimation( NiObjectNET* pObject, float fStartTime );
	void StopAnimation( NiObjectNET* pObject );

	bool EndAnimation( NiObjectNET* pObject );
	void __EndAnimationTime( float& fReturn, NiObjectNET* pObject );
	void EndAnimationTime( float& fReturn, NiObjectNET* pObject );

	void SetTextureApplyMode( NiObjectNET* pObject, NiTexturingProperty::ApplyMode eApplyMode );
	void SetTextureFilterMode( NiObjectNET* pObject, NiTexturingProperty::FilterMode eFilterMode );


	void SetMaterial_Alpha_Emittance( NiObjectNET* pObject, float fAlpha );
	void Set_Emittance( NiObjectNET* pObject, NiColor Emittance );
	void SetMaterial_Color( NiObjectNET* pObject, NiColor color );

	void SetMaterial_Alpha( NiObjectNET* pObject, float fAlpha, CsBackupProperty* pBackupProperty );
	void _SetMaterial_Alpha( NiObjectNET* pObject, float fAlpha, CsBackupProperty* pBackupProperty );
	void _SetAlphaBlend( NiObjectNET* pObject );
	void SetMaterial_Alpha( NiObjectNET* pObject, float fAlpha );	
	void SetPropertyBack( NiObjectNET* pObject, CsBackupProperty* pBackupProperty );	

	NiAVObject* GetFirstAppCulledObject( NiAVObject* pNode, char* cKey, size_t siKeyLen );

	struct sTERRAIN_BOUND : public NiMemObject
	{
		sTERRAIN_BOUND(){ Reset(); }
		inline void Reset(){ s_ptMin = NiPoint2( FLT_MAX, FLT_MAX ); s_ptMax = NiPoint2( -FLT_MAX, -FLT_MAX ); }
		inline bool InBoundCheck( sTERRAIN_BOUND Bound );
		inline bool InBoundCheck( float x, float y );
		inline bool CrossBoundCheck( sTERRAIN_BOUND Bound );
		inline void ReCalInBound( sTERRAIN_BOUND* pBound );

		NiPoint2	s_ptMin;
		NiPoint2	s_ptMax;
	};

	bool CalMinMaxBound( NiObjectNET* pObject, NiTransform Transform, sTERRAIN_BOUND* pBound );
	bool CalMinMax3DBound_Local( NiObjectNET* pObject, NiPoint3* ptMin, NiPoint3* ptMax );

	NiNode*				GetRootNiNode( NiAVObject* pObject );
	NiAVObject*			GetFirst_HaveTexProp( NiAVObject* pObject );
	NiAVObject*			GetNext_HaveTexProp( NiAVObject* pObject, NiAVObject* pSrcChild, bool* bSearchChild = NULL );

	NiAVObject*			GetEnableFirstNode( NiNode* pNode );
	NiGeometry*			GetFirstGeomety( NiAVObject* pObject );
	NiGeometry*			GetSkinGeomety( NiAVObject* pObject );
	void				GetSkinGeometyList( std::list< NiAVObject* >* pList, NiAVObject* pObject );
	bool				IsAlphaBlend( NiAVObject* pObject );

	NiPoint3			CalMullRom( NiPoint3* p0, NiPoint3* p1, NiPoint3* p2, NiPoint3* p3, float t );

	float				CalRotation( NiPoint3 vSrcPos, NiPoint3 vSrcDir, NiPoint3 vDestPos );

	void				SetBit( NiAVObject* pNiObject, bool bVal, unsigned short nMask );

	NiScreenElementsPtr	MakeScreenElements(NiTexture* pBaseTexture);
}


#include "CsGBFunc.inl"

