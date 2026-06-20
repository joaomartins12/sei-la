
#pragma once


//========================================================================================================
//		Load
//========================================================================================================

struct sLOAD_THREAD_REF : public NiMemObject
{
	sLOAD_THREAD_REF(){ s_nRef = 0; s_dwLastDeleteTime = 0; }
	~sLOAD_THREAD_REF() {};

	void			IncreaseRef(){ _SYNCHRONIZE_THREAD_(&s_CS); ++s_nRef; }
	void			DecreaseRef(){ 
		_SYNCHRONIZE_THREAD_(&s_CS);
		if( --s_nRef == 0 ){ s_dwLastDeleteTime = GetTickCount(); }
		assert_cs( s_nRef >= 0 );
	}
	int				GetRef(){ _SYNCHRONIZE_THREAD_(&s_CS); return s_nRef; }
	const DWORD		GetLastDeleteTime(){ return s_dwLastDeleteTime; }

private:
	int					s_nRef;	
	DWORD				s_dwLastDeleteTime;
	CsCriticalSection	s_CS;
};

struct sRES_LOAD : public NiMemObject
{
	enum eTYPE{ PART, NIF, KFM, };
	static void	GlobalShutDown();

	sRES_LOAD(){ s_dwLoadID = s_dwVisiblePlag = 0; s_pRefLoad = NULL; }
	virtual ~sRES_LOAD() {};

	void	Delete();
	void	Clone( sRES_LOAD* pDestObject );
	virtual eTYPE	GetType() = 0;

	DWORD						s_dwLoadID;
	DWORD						s_dwVisiblePlag;
	sLOAD_THREAD_REF*			s_pRefLoad;
};

//========================================================================================================
//		Part
//========================================================================================================
struct sLOAD_PART : public sRES_LOAD
{
	THREAD_MEMPOOL_H( sLOAD_PART )

	sLOAD_PART(){ s_pNiNode = NULL; s_nFileTableID = 0; }
	virtual ~sLOAD_PART(){ s_pNiNode = 0; }

	inline void Delete(){ SAFE_NIDELETE( s_pNiNode ); s_nFileTableID = 0; s_nPartIndex = 0; s_ePartType = 0; sRES_LOAD::Delete(); }
	void Clone( sLOAD_PART* pDestObject );
	virtual eTYPE	GetType(){ return PART; }

	NiNode*		s_pNiNode;
	DWORD		s_nFileTableID;
	int			s_nPartIndex;
	int			s_ePartType;
};

//========================================================================================================
//		Nif
//========================================================================================================
struct sLOAD_NIF : public sRES_LOAD
{
	THREAD_MEMPOOL_H( sLOAD_NIF )

	sLOAD_NIF(){ s_pNiNode = NULL; s_pABVWire = NULL; }
	virtual ~sLOAD_NIF(){ s_pNiNode = 0; }

	inline void Delete(){ s_pNiNode = 0; s_pABVWire = NULL; sRES_LOAD::Delete(); }
	void Clone( sLOAD_NIF* pDestObject, bool bABVWire );
	virtual eTYPE	GetType(){ return NIF; }

	NiNodePtr	s_pNiNode;
	NiNode*		s_pABVWire;
};


//========================================================================================================
//		KFM
//========================================================================================================

struct sLOAD_KFM : public sRES_LOAD
{
	THREAD_MEMPOOL_H( sLOAD_KFM )

	sLOAD_KFM(){ s_pActorMng = NULL; s_pABVWire = NULL; s_pSequenceMng = NULL;}
	virtual ~sLOAD_KFM(){ s_pActorMng = 0; s_pABVWire = 0; s_pSequenceMng = NULL;}

	void Delete();
	void Clone( sLOAD_KFM* pDestObject, bool bABVWire );
	virtual eTYPE	GetType(){ return KFM; }

	NiActorManagerPtr	s_pActorMng;
	NiNode*				s_pABVWire;	
	CsC_LoadSequence*	s_pSequenceMng;
	DWORD				s_dwChageApplyModel;
};
