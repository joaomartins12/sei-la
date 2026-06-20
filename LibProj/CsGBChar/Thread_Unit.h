
#pragma once

struct sTCUnit : NiMemObject
{
public:
	enum eTYPE
	{
		DeleteFile,
		KFM_File,
		NIF_File,
		All_Part_File,
		One_Part_File,
		Part_Delete,
		LoadFileTable,
		//CARD_File,
		DPart_File,
	};	

	enum eLoadType
	{
		eNormal = 0,	//일반로드
		eFast,		//우선로드		
	};

	static CThreadSafeQueue< sTCUnit* >		m_MemPool;
	static void				DeleteInstance( sTCUnit* pInst );
	static sTCUnit*			NewInstance( int nType, DWORD dwLoadID = -1 );

	sTCUnit():s_pLoadedObject(NULL),s_dwLoadID(0),s_nValue1(0),s_pValue1(NULL),s_dwChangeApplyModelID(0),s_eLoadType(eNormal)
	{
		memset(s_cFilePath, 0, sizeof(char)*OBJECT_PATH_LEN);
	}

	virtual ~sTCUnit() {};

	virtual void			Delete();


	sCREATEINFO		s_Prop;
	CsC_Object*		s_pLoadedObject;
	char			s_cFilePath[ OBJECT_PATH_LEN ];
	DWORD			s_dwLoadID;

	int				s_nValue1;
	void*			s_pValue1;
	DWORD			s_dwChangeApplyModelID;

protected:
	eTYPE 			s_eThreadCharType;
	eLoadType		s_eLoadType;

public:
	eTYPE			GetCharType(){ return s_eThreadCharType; }
	eLoadType		GetLoadType(){ return s_eLoadType; }
	void			SetLoadType( eLoadType eType ) { s_eLoadType = eType; }
};

struct sTCUnitPart : public sTCUnit
{
	enum ePART_TYPE{
		NONE_DATA			= 0x00000000,
		PT_PART				= 0x00000001,
		PT_COSTUME_PUTON	= 0x00000002,
		PT_COSTUME_PUTOFF	= 0x00000004,
		MASK_PART			= 0x000000ff,
		AT_ATTACH_PUTON		= 0x00000100,
		MASK_ATTACH			= 0x0000ff00,
	};

	static CThreadSafeQueue< sTCUnitPart* >		m_MemPool;

	sTCUnitPart():s_nPartIndex(0),s_ePartType(NONE_DATA)
	{

	}

	~sTCUnitPart() {};

	// 파츠 오브젝트 로드에 쓰임
	struct sPART_INFO
	{
		sPART_INFO():s_nFileTableID(0)
		{
			memset( s_cPartName, 0, sizeof(char)* OBJECT_NAME_LEN);
		}
		~sPART_INFO()
		{
			s_nFileTableID = 0;
			memset( s_cPartName, 0, sizeof(char)* OBJECT_NAME_LEN);
		}

		void Reset();

		DWORD	s_nFileTableID;
		char	s_cPartName[ OBJECT_NAME_LEN ];
	};

	virtual void	Delete();

	int				s_nPartIndex;								// 하나만 로드할때의 파트 인덱스
	ePART_TYPE		s_ePartType;
	sPART_INFO		s_PartInfo[ nsPART::MAX_APPLY_STATE_COUNT ];
	sPART_INFO		s_AttachInfo[ nsPART::MAX_ATTACH_COUNT - nsPART::MAX_CHANGE_PART_COUNT ];
};

struct sDCUnitPart : public sTCUnit
{
	static CThreadSafeQueue< sDCUnitPart* >		m_MemPool;

	sDCUnitPart() :s_nPartIndex(0)
	{
	}

	~sDCUnitPart() {};

	// 파츠 오브젝트 로드에 쓰임
	struct sDPART_INFO
	{
		sDPART_INFO()
		{
			memset(s_cPartPath, 0, sizeof(char)* OBJECT_PATH_LEN);
		}
		~sDPART_INFO() {};

		void Reset();
		char	s_cPartPath[OBJECT_PATH_LEN];
	};

	virtual void	Delete();
	int				s_nPartIndex;	
};
// 
// struct sTCardPart : public sTCUnit
// {
// public:
// 	enum ePART_TYPE{
// 		NONE_DATA			= 0x00000000,
// 		PT_PART				= 0x00000001,
// 		MASK_PART			= 0x000000ff,
// 	};
// 
// 	static CThreadSafeQueue< sTCardPart* >		m_MemPool;
// 
// 	sTCardPart():s_nPartIndex(0),s_ePartType(NONE_DATA)
// 	{
// 		for( int n = 0; n < nsCard::MAX_TOTAL_COUNT; ++n )
// 			s_PartInfo[n].Reset();
// 	}
// 
// 	~sTCardPart() {};
// 
// 	// 파츠 오브젝트 로드에 쓰임
// 	struct sPART_INFO
// 	{		
// 		sPART_INFO():m_pPixelData(NULL)
// 		{
// 			memset( s_cPartPath, 0, sizeof(char)* OBJECT_PATH_LEN);
// 		}
// 		~sPART_INFO() {};
// 
// 		void Reset();
// 		char	s_cPartPath[ OBJECT_PATH_LEN ];
// 		NiPixelData*		m_pPixelData;
// 		//NiPoint2				m_pTextSize;
// 
// 	};
// 
// 	virtual void	Delete();
// 
// 	ePART_TYPE		s_ePartType;
// 	int				s_nPartIndex;								// 하나만 로드할때의 파트 인덱스
// 	sPART_INFO		s_PartInfo[ nsCard::MAX_TOTAL_COUNT ];	
// };
