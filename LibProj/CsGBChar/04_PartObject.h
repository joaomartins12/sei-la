
#pragma once

namespace nsPART
{
	enum ePART
	{
		Head		=	0,
		Coat		=	1,
		Glove		=	2,
		Pants		=	3,
		Shoes		=	4,
		MAX_APPLY_STATE_COUNT,		//	5		능력치 적용되는 것까지의 카운트

		Costume		=	5,
		MAX_CHANGE_PART_COUNT ,		//	6		장착 ( 메쉬의 교체 ) 까지의 카운트

		Glass		=	6,
						
		MAX_ATTACH_COUNT  ,			//	7		어태치 까지의 인덱스 카운트 

		Necklace	=	7,
		Ring		=	8,
		Earring		=	9,
		// 차후 Earring 추가 시 9번으로 추가하고 Aura를 10번으로 해야 함. (서버도 이런식으로 해야 함.) 변경완료
		EquipAura		=	10,				//오라 파츠 추가 chu8820 
#ifdef SDM_TAMER_XGUAGE_20180628
		XAI				= 11,
#endif
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
		Bracelet		= 12,
#endif
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
		Digivice,					// 디지바이스 이펙트를 위해 추가 - 서버에는 없음 주의할 것
#endif
		MAX_TOTAL_COUNT  ,			//	10		총 갯수 (오라 뺀 갯수 = 9)

		PART_NODE_CONSTANT	=	2,						// 노드검사에서의 추가 인덱싱
	};	
}

namespace nsCONDITION_PART
{
	enum eCONDITION
	{
		Barcode		=	nsPART::MAX_TOTAL_COUNT,

		_DUMMY_MAX_COUNT,
		MAX_COUNT	=	_DUMMY_MAX_COUNT - nsPART::MAX_ATTACH_COUNT,
	};
}

#define PART_INFO_COUNT			( nsPART::MAX_ATTACH_COUNT + nsCONDITION_PART::MAX_COUNT )

class CsC_PartObject : public CsC_AvObject
{
	CsCRTTI_H
	THREAD_MEMPOOL_H( CsC_PartObject )
	
public:
	CsC_PartObject();

public:	
	virtual void		Delete();
	virtual void		DeleteUpdate();

	virtual void		Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp );

	virtual void		Update( float fDeltaTime, bool bAnimation = true );
	virtual void		Render();

	//====================================================================================================
	//		Load
	//====================================================================================================
protected:
	bool				m_bInitialLoadPart;

protected:
	virtual bool		_IsInitialLoad(){ return m_bInitialLoadPart; }

	//====================================================================================================
	//	Part
	//====================================================================================================

public:
	struct sPART_INFO : public NiMemObject
	{
		sPART_INFO(){ s_nFileTableID = 0; s_pRefLoad = NULL; }
		void	Reset(){ s_nFileTableID = 0; _DeleteRefLoad(); }
		void	ChangePart( UINT nFileTableID ){ s_nFileTableID = nFileTableID; _DeleteRefLoad(); }
		void	SetRefLoad( sLOAD_THREAD_REF* pRefLoad ){ _DeleteRefLoad(); s_pRefLoad = pRefLoad; }

	public:
		UINT				s_nFileTableID;
	public:
		bool				IsData(){ return ( s_pRefLoad != NULL ); }
	protected:		
		sLOAD_THREAD_REF*	s_pRefLoad;
	protected:
		void				_DeleteRefLoad(){ if( s_pRefLoad != NULL ){ s_pRefLoad->DecreaseRef();	s_pRefLoad = NULL; } }
	};

	/*struct sREADY_PART_INFO : public NiMemObject
	{
		THREAD_MEMPOOL_H( CsC_PartObject::sREADY_PART_INFO )

		void Delete(){ SAFE_NIDELETE( s_pNiNode ); if( s_pRefLoad ){ s_pRefLoad->DecreaseRef(); s_pRefLoad = NULL; } }

		int						s_nPartIndex;
		DWORD					s_nFileTableID;
		sLOAD_THREAD_REF*		s_pRefLoad;
		NiNode*					s_pNiNode;		
		int						s_ePartType;
		};*/

	struct sSETTING_PART
	{
		void Reset(){ s_nSettingID = 0; s_nRemainTime = (UINT)-1; }
		DWORD		s_nSettingID;
		UINT		s_nRemainTime;
	};

protected:
	CsCriticalSection	s_CS_UpdateReadyPart;
	char				m_cWorkingFolder[ OBJECT_PATH_LEN ];
	sPART_INFO			m_PartInfo[ PART_INFO_COUNT ];
	sSETTING_PART		m_SettingPart[ nsPART::MAX_TOTAL_COUNT ];
	CThreadSafeQueue< sLOAD_PART* >			m_queueReadyPart;

public:
	char*				GetWorkingFolder(){ return m_cWorkingFolder; }
	bool				IsPart( int nPartIndex ){ assert_cs( nPartIndex < PART_INFO_COUNT ); return m_PartInfo[ nPartIndex ].IsData(); }
	sSETTING_PART*		GetSettingPartArray(){ return m_SettingPart; }

public:
	virtual void		ThreadCallBack_AttachPart( sLOAD_PART* pPart );
	virtual void		ThreadCallBack_AttachPart( std::queue< sLOAD_PART* >* pPartQueue );
	virtual void		SetScale( float fScale, bool bOrgScale = true );	

	struct sCHANGE_PART_INFO
	{
		sCHANGE_PART_INFO(){ s_nFileTableID = 0; s_nPartIndex = 0; s_nRemainTime = 0; }
		UINT					s_nFileTableID;
		int						s_nPartIndex;
		UINT					s_nRemainTime;		
	};

	virtual void		ChangePart( sCHANGE_PART_INFO* pPartInfo );
	void				ChangePart_ByTool( sCHANGE_PART_INFO* pPartInfo );

	virtual bool		GetFileName_FromID( char* pOut, sCHANGE_PART_INFO* pPartInfo );

public:
	virtual void		ThreadLoad_Part( sCHANGE_PART_INFO* pItemCodeArray, bool bIncludeBaseKFM = true );
	virtual void		DeletePart( int nPartIndex );

	virtual void		ToOriginalPart( UINT nFileTableID );	

protected:	
	void				_UpdateReadyPart();
	void				_SetPart( NiNode* pPart, int nPartIndex );

	//====================================================================================================
	//	Path
	//====================================================================================================
public:
	virtual void		DeletePath();
};


// 
// namespace nsCard
// {
// 	enum ePART
// 	{		
// 		FP		=	0,
// 		BP		=	1,	
// 		BE		=	2,
// 		MFD		=	3,
// 		DSCR	=	4,
// 		GR		=	5,
// 		NM		=	6,
// 		TD		=	7,
// 		TE		=	8,
// 		FE		=	9,
// 		MAX_TOTAL_COUNT,
// 	};	
// }
// 
// class CsC_CardObject : public CsC_AvObject
// {
// 	CsCRTTI_H
// 	THREAD_MEMPOOL_H( CsC_CardObject )
// 	
// public:
// 	CsC_CardObject();
// 
// public:	
// 	virtual void		Delete();
// 	virtual void		DeleteUpdate();
// 
// 	virtual void		Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp );
// 
// 	virtual void		Update( float fDeltaTime, bool bAnimation = true );
// 	virtual void		Render();
// 
// 	//====================================================================================================
// 	//		Load
// 	//====================================================================================================
// protected:
// 	bool				m_bInitialLoadPart;
// 
// protected:
// 	virtual bool		_IsInitialLoad(){ return m_bInitialLoadPart; }
// 
// 	//====================================================================================================
// 	//	Part
// 	//====================================================================================================
// 
// public:
// 	struct sPART_INFO : public NiMemObject
// 	{
// 		sPART_INFO(){ s_nFileTableID = 0; s_pRefLoad = NULL; }
// 		void	Reset(){ s_nFileTableID = 0; _DeleteRefLoad(); }
// 		void	ChangePart( UINT nFileTableID ){ s_nFileTableID = nFileTableID; _DeleteRefLoad(); }
// 		void	SetRefLoad( sLOAD_THREAD_REF* pRefLoad ){ _DeleteRefLoad(); s_pRefLoad = pRefLoad; }
// 
// 	public:
// 		UINT				s_nFileTableID;
// 	public:
// 		bool				IsData(){ return ( s_pRefLoad != NULL ); }
// 	protected:		
// 		sLOAD_THREAD_REF*	s_pRefLoad;
// 	protected:
// 		void				_DeleteRefLoad(){ if( s_pRefLoad != NULL ){ s_pRefLoad->DecreaseRef();	s_pRefLoad = NULL; } }
// 	};
// 
// protected:	
// 
// 	CsCriticalSection	s_CS_UpdateReadyPart;
// 	char				m_cWorkingFolder[ OBJECT_PATH_LEN ];
// 	sPART_INFO			m_PartInfo[ nsCard::MAX_TOTAL_COUNT ];
// 
// 	CThreadSafeQueue< sLOAD_PART* >			m_queueReadyPart;
// 
// public:
// 	char*				GetWorkingFolder(){ return m_cWorkingFolder; }
// 	bool				IsPart( int nPartIndex ){ assert_cs( nPartIndex < nsPART::MAX_TOTAL_COUNT ); return m_PartInfo[ nPartIndex ].IsData(); }
// 
// public:
// 	virtual void		ThreadCallBack_AttachPart( sLOAD_PART* pPart );
// 	virtual void		ThreadCallBack_AttachPart( std::queue< sLOAD_PART* >* pPartQueue );
// 
// 	struct sCHANGE_PART_INFO
// 	{
// 		sCHANGE_PART_INFO(){}
// 		char					s_cPartPath[ OBJECT_PATH_LEN ];		
// 		NiPixelData*			m_pPixelData;
// 	};
// 
// 
// 	virtual bool		GetFileName_FromID( char* pOut, sCHANGE_PART_INFO* pPartInfo );
// 
// public:
// 	virtual void		ThreadLoad_Part( sCHANGE_PART_INFO* pItemCodeArray, bool bIncludeBaseKFM = true );
// 	virtual void		DeletePart( int nPartIndex );
// 
// protected:	
// 	void				_UpdateReadyPart();
// 	void				_SetPart( NiNode* pPart, int nPartIndex );
// 
// public:
// 	virtual void		DeletePath();
// 
// };