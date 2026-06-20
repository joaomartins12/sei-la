
#pragma once

namespace nsOBJECT_010013
{
	struct sPATHINFO
	{
		sPATHINFO(){ Reset(); }

		void	Delete(){
			SAFE_DELETE_ARRAY( s_pData ); SAFE_DELETE_ARRAY( s_pPECount );
			if( s_pptTranse != NULL ){ NiDelete[] s_pptTranse; s_pptTranse = NULL; }
		}
		void	Reset(){ s_uiPEGroupCount = 0; s_uiTotalPECount = 0; s_pptTranse = NULL; s_pPECount = NULL; s_pData = NULL; }

		UINT				s_uiPEGroupCount;
		UINT				s_uiTotalPECount;
		NiPoint2*			s_pptTranse;
		USHORT*				s_pPECount;
		long*				s_pData;
	};

	enum eSAVE_TYPE{
		ST_FIRST_OBJECT = 100,
		ST_EXIST_OBJECT,
	};
	struct sFILE_HEADER
	{
		DWORD		s_dwSaveType;
		DWORD		s_dwInfoSize;
	};
	struct sINFO
	{
		void	Reset(){
			s_dwUniqID = 0;
			s_cObjectPath[ 0 ] = NULL;
			s_dwTexTypePlag = 0;
			s_PathInfo.Reset();
		}
		BYTE	GetTextureTypeIndex( BYTE btGroupIndex ){ return (BYTE)( s_dwTexTypePlag & ( 0x0000000f<<(4*btGroupIndex) ) )>>(4*btGroupIndex); }
		void	SetTextureTypeIndex( BYTE btGroupIndex, BYTE btTexType ){
			s_dwTexTypePlag &= ~( 0x0000000f<<(4*btGroupIndex) );
			s_dwTexTypePlag |= ( btTexType << (4*btGroupIndex) );}

		DWORD						s_dwUniqID;
		char						s_cObjectPath[ OBJECT_PATH_LEN ];

		bool						s_bCheckDoAnimation;
		float						s_fStopAniTime;
		float						s_fAniSpeed;
		char						s_Dummy[ 88 ];
		NiTransform					s_trObject;
		nsCSGBFUNC::sTERRAIN_BOUND	s_Bound;

		//===== UserDefine
		DWORD						s_dwUserDefinePlag;
		DWORD						s_dwTexTypePlag;

		//===== PathEngine
		sPATHINFO					s_PathInfo;
	};
	// 이미 존재하는 오브젝트일경우
	struct sEXIST_INFO
	{
		DWORD						s_dwUniqID;
	};
	struct sUSER_DEFINE
	{
		void	Reset(){ s_dwPlag = 0; assert_cs( s_mapTextureGroup.empty() == true ); }
		void	Delete();

		enum eTYPE{
			UD_NOSHADOW			= 0x00000001,
			UD_ANIMATION		= 0x00000002,
			UD_SNAP				= 0x00000004,
			UD_USE_TEXTYPE		= 0x00000008,
			UD_CAMERA_ALPHA		= 0x00000010,
			UD_CAMERA_ALPHA_PICK= 0x00000020,
			UD_MOVE				= 0x00000040,
			UD_TERRAIN_BASE		= 0x00000080,
			UD_NO_FOG			= 0x00000100,
			UD_NO_LIGHT			= 0x00000200,
			UD_PORTAL			= 0x00000400,

			UDF_DYNAMIC_POS		= UD_ANIMATION | UD_MOVE,
			UDF_CAMERAALPHA		= UD_CAMERA_ALPHA | UD_CAMERA_ALPHA_PICK,
		};
		DWORD		s_dwPlag;

		// UD_TERRAIN_BASE
		DWORD		s_dwTerrainBaseShadowMapSize;

		// UD_USE_TEXTYPE
		struct sTEXTURE_GROUP
		{
			void	Delete();
			BYTE	s_btMaterialID;
			std::map< BYTE, nsCSFILE::sFileName_c* >	s_mapFileName;
		};
		std::map< BYTE, sTEXTURE_GROUP* >	s_mapTextureGroup;
	};
}

struct sFIELD_INFO_010013
{
	// ==== Data
	float*				s_pfHeightField;	
};

struct sTR_PATHINFO_010013
{
	nsOBJECT_010013::sPATHINFO	s_PathInfo;
	char*						s_pType;
	int							s_nDataTotalCount;
};

namespace nsSPEEDCELL_010012
{
	struct sOBJECT_INFO
	{
		char	s_cPath[ OBJECT_PATH_LEN ];
		float	s_fCheckTimeMin;
		float	s_fCheckTimeMax;
		float	s_fAniSpeedMin;
		float	s_fAniSpeedMax;
	};
}
namespace nsSPEEDCELL_010011
{
	struct sOBJECT_INFO
	{
		char	s_cPath[ OBJECT_PATH_LEN ];
	};
}

struct sROOTINFO_010011
{		
	// ===== Map
	DWORD	s_dwMapResVersion;

	// X Vert
	int		s_nXVertCount;
	int		s_nXVertCountInNode;
	// Y Vert
	int		s_nYVertCount;
	int		s_nYVertCountInNode;

	// Height
	float	s_fHeightRate;
	// Grid
	int		s_nGridSize;							// 한 패치의 그리드 넓이
	// Tree
	int		s_nTreeLevel;							// 쿼드트리 레벨
	int		s_nNodeCountInSide;						// 한변에서의 노드 갯수
	int		s_nTotalLeafCount;						// 총 리프 갯수 ( 빈노드 포함 )

	// Height
	TCHAR	s_szHeightMapFile[ MAX_PATH ];

	// Init Size
	int		s_nInitMapCoverSize;
	int		s_nInitMapSize;
	float	s_fDetailMapSize;

	// BaseMap
	int		s_nBaseMPPG;

	// AlphaMap
	int		s_nAlphaMPPG;		

	// ShadowMap
	int		s_nShadowMPPG;

	//====================================================================================
	// 더미
	//====================================================================================
	char	s_Dummy2[ 6 ];

	// EffectMap
	int		s_nEffectMPPG;

protected:
	// Object
	DWORD	s_dwUniqObjectID;

public:
	// ===== Fog
	NiColor				s_FogColor;
	float				s_fFogStart;
	float				s_fFogEndTerrain;
	float				s_fFogEndObject;

	// ===== Clipping
	float				s_fClipObject_Far;
	float				s_fClipTerrain_Far;

	DWORD				s_dwMapID;

	// ==== ShadowColor
	NiColor				s_ShadowColor;

	NiColor				s_BackGroundColor;

	// ===== Clip
	int					s_nClipCellObject;
	int					s_nCellObject_Size;

	// ==== Dummy
	char				s_Dummy[ 32 ];
};

namespace nsOBJECT_010010
{
	struct sPATHINFO
	{
		sPATHINFO(){ Reset(); }

		void	Delete(){
			SAFE_DELETE_ARRAY( s_pData ); SAFE_DELETE_ARRAY( s_pPECount );
			if( s_pptTranse != NULL ){ NiDelete[] s_pptTranse; s_pptTranse = NULL; }
		}
		void	Reset(){ s_uiPEGroupCount = 0; s_uiTotalPECount = 0; s_pptTranse = NULL; s_pPECount = NULL; s_pData = NULL; }

		UINT				s_uiPEGroupCount;
		UINT				s_uiTotalPECount;
		NiPoint2*			s_pptTranse;
		USHORT*				s_pPECount;
		long*				s_pData;
	};

	enum eSAVE_TYPE{
		ST_FIRST_OBJECT = 100,
		ST_EXIST_OBJECT,
	};
	struct sFILE_HEADER
	{
		DWORD		s_dwSaveType;
		DWORD		s_dwInfoSize;
	};
	struct sINFO
	{
		void	Reset(){
			s_dwUniqID = 0;
			s_cObjectPath[ 0 ] = NULL;
			s_dwTexTypePlag = 0;
			s_PathInfo.Reset();
		}
		BYTE	GetTextureTypeIndex( BYTE btGroupIndex ){ return (BYTE)( s_dwTexTypePlag & ( 0x0000000f<<(4*btGroupIndex) ) )>>(4*btGroupIndex); }
		void	SetTextureTypeIndex( BYTE btGroupIndex, BYTE btTexType ){
			s_dwTexTypePlag &= ~( 0x0000000f<<(4*btGroupIndex) );
			s_dwTexTypePlag |= ( btTexType << (4*btGroupIndex) );}

		DWORD						s_dwUniqID;
		char						s_cObjectPath[ OBJECT_PATH_LEN ];

		bool						s_bCheckDoAnimation;
		float						s_fStopAniTime;
		float						s_fAniSpeed;
		char						s_Dummy[ 88 ];
		NiTransform					s_trObject;
		nsCSGBFUNC::sTERRAIN_BOUND	s_Bound;

		//===== UserDefine
		DWORD						s_dwUserDefinePlag;
		DWORD						s_dwTexTypePlag;

		//===== PathEngine
		sPATHINFO					s_PathInfo;
	};
	// 이미 존재하는 오브젝트일경우
	struct sEXIST_INFO
	{
		DWORD						s_dwUniqID;
	};
	struct sUSER_DEFINE
	{
		void	Reset(){ s_dwPlag = 0; assert_cs( s_mapTextureGroup.empty() == true ); }
		void	Delete();

		enum eTYPE{
			UD_NOSHADOW			= 0x00000001,
			UD_ANIMATION		= 0x00000002,
			UD_SNAP				= 0x00000004,
			UD_USE_TEXTYPE		= 0x00000008,
			UD_CAMERA_ALPHA		= 0x00000010,
			UD_CAMERA_ALPHA_PICK= 0x00000020,
			UD_MOVE				= 0x00000040,
			UD_TERRAIN_BASE		= 0x00000080,
			UD_NO_FOG			= 0x00000100,
			UD_NO_LIGHT			= 0x00000200,
			UD_PORTAL			= 0x00000400,

			UDF_DYNAMIC_POS		= UD_ANIMATION | UD_MOVE,
			UDF_CAMERAALPHA		= UD_CAMERA_ALPHA | UD_CAMERA_ALPHA_PICK,
		};
		DWORD		s_dwPlag;

		// UD_TERRAIN_BASE
		DWORD		s_dwTerrainBaseShadowMapSize;

		// UD_USE_TEXTYPE
		struct sTEXTURE_GROUP
		{
			void	Delete();
			BYTE	s_btMaterialID;
			std::map< BYTE, nsCSFILE::sFileName_c* >	s_mapFileName;
		};
		std::map< BYTE, sTEXTURE_GROUP* >	s_mapTextureGroup;
	};
}

namespace nsOBJECT_01000c
{
	struct sPATHINFO
	{
		sPATHINFO(){ Reset(); }

		void	Delete(){
			SAFE_DELETE_ARRAY( s_pData ); SAFE_DELETE_ARRAY( s_pPECount );
			if( s_pptTranse != NULL ){ NiDelete[] s_pptTranse; s_pptTranse = NULL; }
		}
		void	Reset(){ s_uiPEGroupCount = 0; s_uiTotalPECount = 0; s_pptTranse = NULL; s_pPECount = NULL; s_pData = NULL; }

		UINT				s_uiPEGroupCount;
		UINT				s_uiTotalPECount;
		NiPoint2*			s_pptTranse;
		USHORT*				s_pPECount;
		long*				s_pData;
	};

	enum eSAVE_TYPE{
		ST_FIRST_OBJECT = 100,
		ST_EXIST_OBJECT,
	};
	struct sFILE_HEADER
	{
		DWORD		s_dwSaveType;
		DWORD		s_dwInfoSize;
	};
	struct sINFO
	{
		void	Reset(){
			s_cObjectPath[ 0 ] = NULL;
			s_dwTexTypePlag = 0;
			s_PathInfo.Reset();
		}
		BYTE	GetTextureTypeIndex( BYTE btGroupIndex ){ return (BYTE)( s_dwTexTypePlag & ( 0x0000000f<<(4*btGroupIndex) ) )>>(4*btGroupIndex); }
		void	SetTextureTypeIndex( BYTE btGroupIndex, BYTE btTexType ){
			s_dwTexTypePlag &= ~( 0x0000000f<<(4*btGroupIndex) );
			s_dwTexTypePlag |= ( btTexType << (4*btGroupIndex) );}

		DWORD						s_dwUniqID;
		char						s_cObjectPath[ OBJECT_PATH_LEN ];
		float						s_fScale;
		char						s_Dummy[ 96 ];
		NiTransform					s_trObject;
		nsCSGBFUNC::sTERRAIN_BOUND	s_Bound;

		//===== UserDefine
		DWORD						s_dwUserDefinePlag;
		DWORD						s_dwTexTypePlag;

		//===== PathEngine
		sPATHINFO					s_PathInfo;
	};
	// 이미 존재하는 오브젝트일경우
	struct sEXIST_INFO
	{
		DWORD						s_dwUniqID;
	};
	struct sUSER_DEFINE
	{
		void	Reset(){ s_dwPlag = 0; assert_cs( s_mapTextureGroup.empty() == true ); }
		void	Delete();

		enum eTYPE{
			UD_NOSHADOW			= 0x00000001,
			UD_ANIMATION		= 0x00000002,
			UD_SNAP				= 0x00000004,
			UD_USE_TEXTYPE		= 0x00000008,
			UD_CAMERA_ALPHA		= 0x00000010,
			UD_CAMERA_ALPHA_PICK= 0x00000020,
			UD_MOVE				= 0x00000040,
			UD_TERRAIN_BASE		= 0x00000080,
			UD_NO_FOG			= 0x00000100,
			UD_NO_LIGHT			= 0x00000200,
			UD_PORTAL			= 0x00000400,

			UDF_DYNAMIC_POS		= UD_ANIMATION | UD_MOVE,
			UDF_CAMERAALPHA		= UD_CAMERA_ALPHA | UD_CAMERA_ALPHA_PICK,
		};
		DWORD		s_dwPlag;

		// UD_TERRAIN_BASE
		DWORD		s_dwTerrainBaseShadowMapSize;

		// UD_USE_TEXTYPE
		struct sTEXTURE_GROUP
		{
			void	Delete();
			BYTE	s_btMaterialID;
			std::map< BYTE, nsCSFILE::sFileName_c* >	s_mapFileName;
		};
		std::map< BYTE, sTEXTURE_GROUP* >	s_mapTextureGroup;
	};
}

struct sNiTriINFO_01000b
{
	NiPoint2			s_ptOffsetPos;
	int					s_nVertCount;
	int					s_nTemp1;				// 이거도 남는다
	float				s_fTemp2;				// 이거도 남는다
	unsigned short		s_usNiTerrainIndex;		

	// ==== Object
	DWORD				s_dwObjectCount;

	// ==== Texture
	char				s_cDetailMap0[ MAX_PATH ];
	char				s_cDetailMap1[ MAX_PATH ];
	char				s_cDetailMap2[ MAX_PATH ];
	char				s_cDetailMap3[ MAX_PATH ];	
	char				s_cDetailMap4[ MAX_PATH ];

	// ==== Shader
	int					s_nShaderCord;

	// ==== Data
	float*				s_pfHeight;
};

namespace nsOBJECT_01000a
{
	struct sPATHINFO
	{
		sPATHINFO(){ Reset(); }

		void	Delete(){
			SAFE_DELETE_ARRAY( s_pData ); SAFE_DELETE_ARRAY( s_pPECount );
			if( s_pptTranse != NULL ){ NiDelete[] s_pptTranse; s_pptTranse = NULL; }
		}
		void	Reset(){ s_uiPEGroupCount = 0; s_uiTotalPECount = 0; s_pptTranse = NULL; s_pPECount = NULL; s_pData = NULL; }

		UINT				s_uiPEGroupCount;
		UINT				s_uiTotalPECount;
		NiPoint2*			s_pptTranse;
		USHORT*				s_pPECount;
		long*				s_pData;
	};

	enum eSAVE_TYPE{
		ST_FIRST_OBJECT = 100,
		ST_EXIST_OBJECT,
	};
	struct sFILE_HEADER
	{
		DWORD		s_dwSaveType;
		DWORD		s_dwInfoSize;
	};
	struct sINFO
	{
		void	Reset(){
			s_cObjectPath[ 0 ] = NULL;
			s_dwTexTypePlag = 0;
			s_PathInfo.Reset();
		}
		BYTE	GetTextureTypeIndex( BYTE btGroupIndex ){ return (BYTE)( s_dwTexTypePlag & ( 0x0000000f<<(4*btGroupIndex) ) )>>(4*btGroupIndex); }
		void	SetTextureTypeIndex( BYTE btGroupIndex, BYTE btTexType ){
			s_dwTexTypePlag &= ~( 0x0000000f<<(4*btGroupIndex) );
			s_dwTexTypePlag |= ( btTexType << (4*btGroupIndex) );}

		DWORD						s_dwUniqID;
		char						s_cObjectPath[ MAX_PATH ];
		NiTransform					s_trObject;
		nsCSGBFUNC::sTERRAIN_BOUND	s_Bound;

		//===== UserDefine
		DWORD						s_dwUserDefinePlag;
		DWORD						s_dwTexTypePlag;

		//===== PathEngine
		sPATHINFO					s_PathInfo;
	};
	// 이미 존재하는 오브젝트일경우
	struct sEXIST_INFO
	{
		DWORD						s_dwUniqID;
	};
	struct sUSER_DEFINE
	{
		void	Reset(){ s_dwPlag = 0; assert_cs( s_mapTextureGroup.empty() == true ); }
		void	Delete();

		enum eTYPE{
			UD_NOSHADOW			= 0x00000001,
			UD_ANIMATION		= 0x00000002,
			UD_SNAP				= 0x00000004,
			UD_USE_TEXTYPE		= 0x00000008,
			UD_CAMERA_ALPHA		= 0x00000010,
			UD_CAMERA_ALPHA_PICK= 0x00000020,
			UD_MOVE				= 0x00000040,
			UD_TERRAIN_BASE		= 0x00000080,
			UD_NO_FOG			= 0x00000100,
			UD_NO_LIGHT			= 0x00000200,
			UD_PORTAL			= 0x00000400,

			UDF_DYNAMIC_POS		= UD_ANIMATION | UD_MOVE,
			UDF_CAMERAALPHA		= UD_CAMERA_ALPHA | UD_CAMERA_ALPHA_PICK,
		};
		DWORD		s_dwPlag;

		// UD_TERRAIN_BASE
		DWORD		s_dwTerrainBaseShadowMapSize;

		// UD_USE_TEXTYPE
		struct sTEXTURE_GROUP
		{
			void	Delete();
			BYTE	s_btMaterialID;
			std::map< BYTE, nsCSFILE::sFileName_c* >	s_mapFileName;
		};
		std::map< BYTE, sTEXTURE_GROUP* >	s_mapTextureGroup;
	};
}

struct sSKYBOXINFO_01000a
{
	char			s_cSkyBox[ MAX_PATH ];
	float			s_fHeight;
};

struct sFIELD_INFO_01000a
{
	// ==== Data
	float*				s_pfHeightField;
	NiPoint3*			s_pptNormalField;
};
struct sTR_PATHINFO_01000a
{
	nsOBJECT_01000a::sPATHINFO	s_PathInfo;
};
struct sROOTINFO_01000a
{
	// ===== Map
	DWORD	s_dwMapResVersion;

	// X Vert
	int		s_nXVertCount;
	int		s_nXVertCountInNode;
	// Y Vert
	int		s_nYVertCount;
	int		s_nYVertCountInNode;

	// Height
	float	s_fHeightRate;
	// Grid
	int		s_nGridSize;							// 한 패치의 그리드 넓이
	// Tree
	int		s_nTreeLevel;							// 쿼드트리 레벨
	int		s_nNodeCountInSide;						// 한변에서의 노드 갯수
	int		s_nTotalLeafCount;						// 총 리프 갯수 ( 빈노드 포함 )

	// Height
	TCHAR	s_szHeightMapFile[ MAX_PATH ];

	// Init Size
	int		s_nInitMapCoverSize;
	int		s_nInitMapSize;
	float	s_fDetailMapSize;

	// BaseMap
	int		s_nBaseMPPG;

	// AlphaMap
	int		s_nAlphaMPPG;		

	// ShadowMap
	int		s_nShadowMPPG;
	int		s_nShadowConstant;
	BYTE	s_btObjectShadowColor;
	BYTE	s_btTerrainShadowColor;

	// EffectMap
	int		s_nEffectMPPG;

	// Object
	DWORD	s_dwUniqObjectID;

	// ===== Fog
	NiColor				s_FogColor;
	float				s_fFogStart;
	float				s_fFogEndTerrain;
	float				s_fFogEndObject;

	// ===== Clipping
	float				s_fClipObject_Far;
	float				s_fClipTerrain_Far;

	DWORD				s_dwMapID;

	// ==== Dummy
	char				s_Dummy[ 64 ];		// 256 사이즈의 더미 확보
};

struct sUSER_DEFINE_010008
{
	enum eTYPE{
		UD_NOSHADOW				= 0x00000001,
		UD_ANIMATION			= 0x00000002,
		UD_USE_TEXTYPE			= 0x00000008,
		UD_CAMERA_ALPHA			= 0x00000010,
		UD_CAMERA_ALPHA_PICK	= 0x00000020,
		UD_MOVE					= 0x00000040,
		UD_TERRAIN_BASE			= 0x00000080,			
		UD_NO_FOG				= 0x00000100,
		UD_NO_LIGHT				= 0x00000200,
		UD_PORTAL				= 0x00000400,
		UD_TERRAIN_BASE_PICK	= 0x00000800,
		UD_ROTATION				= 0x00001000,
		UD_ROTATION_LIMIT		= 0x00002000,
		UD_CAMERA_COLLISION		= 0x00004000,
		UD_MERGE				= 0x00008000,
		UD_DARKMAP				= 0x00010000,

		UD_EFFECTRENDER			= 0x10000000,

		//UD_FIRST_1				= 0x20000000,

		UDF_DYNAMIC_POS		= UD_ANIMATION | UD_MOVE,
		UDF_CAMERAALPHA		= UD_CAMERA_ALPHA | UD_CAMERA_ALPHA_PICK,
		UDF_CAMERA_PICK		= UD_CAMERA_ALPHA_PICK | UD_CAMERA_COLLISION,
		UDF_TB_TYPE			= UD_TERRAIN_BASE | UD_TERRAIN_BASE_PICK,
		//UDF_First			= UD_FIRST_1,
		UDF_ROTATION		= UD_ROTATION | UD_ROTATION_LIMIT,
		UDF_HAVE_CHILD		= UD_MERGE,
	};
	DWORD		s_dwPlag;	
};

namespace nsOBJECT_010007
{
	struct sPATHINFO
	{
		sPATHINFO(){ Reset(); }

		void	Delete(){
			SAFE_DELETE_ARRAY( s_pData ); SAFE_DELETE_ARRAY( s_pPECount );
			if( s_pptTranse != NULL ){ NiDelete[] s_pptTranse; s_pptTranse = NULL; }
		}
		void	Reset(){ s_uiPEGroupCount = 0; s_uiTotalPECount = 0; s_pptTranse = NULL; s_pPECount = NULL; s_pData = NULL; }

		UINT				s_uiPEGroupCount;
		UINT				s_uiTotalPECount;
		NiPoint2*			s_pptTranse;
		USHORT*				s_pPECount;
		long*				s_pData;
	};

	enum eSAVE_TYPE{
		ST_FIRST_OBJECT = 100,
		ST_EXIST_OBJECT,
	};
	struct sFILE_HEADER
	{
		DWORD		s_dwSaveType;
		DWORD		s_dwInfoSize;
	};
	struct sINFO
	{
		void	Reset(){
			s_cObjectPath[ 0 ] = NULL;
			s_dwTexTypePlag = 0;
			s_PathInfo.Reset();
		}
		BYTE	GetTextureTypeIndex( BYTE btGroupIndex ){ return (BYTE)( s_dwTexTypePlag & ( 0x0000000f<<(4*btGroupIndex) ) )>>(4*btGroupIndex); }
		void	SetTextureTypeIndex( BYTE btGroupIndex, BYTE btTexType ){
			s_dwTexTypePlag &= ~( 0x0000000f<<(4*btGroupIndex) );
			s_dwTexTypePlag |= ( btTexType << (4*btGroupIndex) );}

		DWORD						s_dwUniqID;
		char						s_cObjectPath[ MAX_PATH ];
		NiTransform					s_trObject;
		nsCSGBFUNC::sTERRAIN_BOUND	s_Bound;

		//===== UserDefine
		DWORD						s_dwUserDefinePlag;
		DWORD						s_dwTexTypePlag;

		//===== PathEngine
		sPATHINFO					s_PathInfo;
	};
	// 이미 존재하는 오브젝트일경우
	struct sEXIST_INFO
	{
		DWORD						s_dwUniqID;
	};
	struct sUSER_DEFINE
	{
		void	Reset(){ s_dwPlag = 0; assert_cs( s_mapTextureGroup.empty() == true ); }
		void	Delete();

		enum eTYPE{
			UD_NOSHADOW			= 0x00000001,
			UD_ANIMATION		= 0x00000002,
			UD_SNAP				= 0x00000004,
			UD_USE_TEXTYPE		= 0x00000008,
			UD_CAMERA_ALPHA		= 0x00000010,
			UD_CAMERA_ALPHA_PICK= 0x00000020,
			UD_MOVE				= 0x00000040,
			UD_TERRAIN_BASE		= 0x00000080,
			UD_NO_FOG			= 0x00000100,
			UD_NO_LIGHT			= 0x00000200,
			UD_PORTAL			= 0x00000400,

			UDF_DYNAMIC_POS		= UD_ANIMATION | UD_MOVE,
			UDF_CAMERAALPHA		= UD_CAMERA_ALPHA | UD_CAMERA_ALPHA_PICK,
		};
		DWORD		s_dwPlag;

		// UD_TERRAIN_BASE
		DWORD		s_dwTerrainBaseShadowMapSize;

		// UD_USE_TEXTYPE
		struct sTEXTURE_GROUP
		{
			void	Delete();
			BYTE	s_btMaterialID;
			std::map< BYTE, nsCSFILE::sFileName_c* >	s_mapFileName;
		};
		std::map< BYTE, sTEXTURE_GROUP* >	s_mapTextureGroup;
	};
}

struct sROOTINFO_010008
{
	DWORD	s_dwMapResVersion;

	// X Vert
	int		s_nXVertCount;
	int		s_nXVertCountInNode;
	// Y Vert
	int		s_nYVertCount;
	int		s_nYVertCountInNode;

	// Height
	float	s_fHeightRate;
	// Grid
	int		s_nGridSize;							// 한 패치의 그리드 넓이
	// Tree
	int		s_nTreeLevel;							// 쿼드트리 레벨
	int		s_nNodeCountInSide;						// 한변에서의 노드 갯수
	int		s_nTotalLeafCount;						// 총 리프 갯수 ( 빈노드 포함 )

	// Height
	TCHAR	s_szHeightMapFile[ MAX_PATH ];

	// Init Size
	int		s_nInitMapCoverSize;
	int		s_nInitMapSize;
	float	s_fDetailMapSize;

	// BaseMap
	int		s_nBaseMPPG;

	// AlphaMap
	int		s_nAlphaMPPG;		

	// ShadowMap
	int		s_nShadowMPPG;
	int		s_nShadowConstant;
	BYTE	s_btObjectShadowColor;
	BYTE	s_btTerrainShadowColor;

	// EffectMap
	int		s_nEffectMPPG;
	// Object
	DWORD	s_dwUniqObjectID;

	// ==== Data
	float*				s_pfHeightField;
	NiPoint3*			s_pptNormalField;

	// ==== SkyBox
	char				s_cSkyBox[ MAX_PATH ];

	// ===== Fog
	NiColor				s_FogColor;
	float				s_fFogStart;
	float				s_fFogEndTerrain;
	float				s_fFogEndObject;

	// ===== Clipping
	float				s_fClipObject_Far;
	float				s_fClipTerrain_Far;

	DWORD				s_dwMapID;

	nsOBJECT_010007::sPATHINFO	s_PathInfo;
};

struct sROOTINFO_010007
{
	DWORD	s_dwMapResVersion;

	// X Vert
	int		s_nXVertCount;
	int		s_nXVertCountInNode;
	// Y Vert
	int		s_nYVertCount;
	int		s_nYVertCountInNode;

	// Height
	float	s_fHeightRate;
	// Grid
	int		s_nGridSize;							// 한 패치의 그리드 넓이
	// Tree
	int		s_nTreeLevel;							// 쿼드트리 레벨
	int		s_nNodeCountInSide;						// 한변에서의 노드 갯수
	int		s_nTotalLeafCount;						// 총 리프 갯수 ( 빈노드 포함 )

	// Height
	TCHAR	s_szHeightMapFile[ MAX_PATH ];

	// Init Size
	int		s_nInitMapCoverSize;
	int		s_nInitMapSize;
	float	s_fDetailMapSize;

	// BaseMap
	int		s_nBaseMPPG;

	// AlphaMap
	int		s_nAlphaMPPG;		

	// ShadowMap
	int		s_nShadowMPPG;
	int		s_nShadowConstant;
	BYTE	s_btObjectShadowColor;
	BYTE	s_btTerrainShadowColor;

	// EffectMap
	int		s_nEffectMPPG;

protected:
	// Object
	DWORD	s_dwUniqObjectID;

public:
	// ==== Data
	float*				s_pfHeightField;
	NiPoint3*			s_pptNormalField;

	// ==== SkyBox
	char				s_cSkyBox[ MAX_PATH ];

	// ===== Fog
	NiColor				s_FogColor;
	float				s_fFogStart;
	float				s_fFogEndTerrain;
	float				s_fFogEndObject;

	// ===== Clipping
	float				s_fClipObject_Far;
	float				s_fClipTerrain_Far;

	DWORD				s_dwMapID;
};

namespace nsOBJECT_010006
{
	struct sPATHINFO
	{
		sPATHINFO(){ Reset(); }

		void	Delete(){
			SAFE_DELETE_ARRAY( s_pData ); SAFE_DELETE_ARRAY( s_pPECount );
			if( s_pptTranse != NULL ){ NiDelete[] s_pptTranse; s_pptTranse = NULL; }
		}
		void	Reset(){ s_uiPEGroupCount = 0; s_uiTotalPECount = 0; s_pptTranse = NULL; s_pPECount = NULL; s_pData = NULL; }

		UINT				s_uiPEGroupCount;
		UINT				s_uiTotalPECount;
		NiPoint2*			s_pptTranse;
		USHORT*				s_pPECount;
		long*				s_pData;
	};

	enum eSAVE_TYPE{
		ST_FIRST_OBJECT = 100,
		ST_EXIST_OBJECT,
	};
	struct sFILE_HEADER
	{
		DWORD		s_dwSaveType;
		DWORD		s_dwInfoSize;
	};
	struct sINFO
	{
		void	Reset(){
			s_cObjectPath[ 0 ] = NULL;
			s_dwTexTypePlag = 0;
			s_PathInfo.Reset();
		}
		BYTE	GetTextureTypeIndex( BYTE btGroupIndex ){ return (BYTE)( s_dwTexTypePlag & ( 0x0000000f<<(4*btGroupIndex) ) )>>(4*btGroupIndex); }
		void	SetTextureTypeIndex( BYTE btGroupIndex, BYTE btTexType ){
			s_dwTexTypePlag &= ~( 0x0000000f<<(4*btGroupIndex) );
			s_dwTexTypePlag |= ( btTexType << (4*btGroupIndex) );}

		DWORD						s_dwUniqID;
		char						s_cObjectPath[ MAX_PATH ];
		NiTransform					s_trObject;
		nsCSGBFUNC::sTERRAIN_BOUND	s_Bound;

		//===== UserDefine
		DWORD						s_dwTexTypePlag;

		//===== PathEngine
		sPATHINFO					s_PathInfo;
	};
	// 이미 존재하는 오브젝트일경우
	struct sEXIST_INFO
	{
		DWORD						s_dwUniqID;
	};
	struct sUSER_DEFINE
	{
		void	Reset(){ s_dwPlag = 0; assert_cs( s_mapTextureGroup.empty() == true ); }
		void	Delete();

		enum eTYPE{
			UD_NOSHADOW			= 0x00000001,
			UD_ANIMATION		= 0x00000002,
			UD_SNAP				= 0x00000004,
			UD_USE_TEXTYPE		= 0x00000008,
			UD_CAMERA_ALPHA		= 0x00000010,
			UD_CAMERA_ALPHA_PICK= 0x00000020,
			UD_MOVE				= 0x00000040,
			UD_TERRAIN_BASE		= 0x00000080,
			UD_NO_FOG			= 0x00000100,
			UD_NO_LIGHT			= 0x00000200,
			UD_PORTAL			= 0x00000400,

			UDF_DYNAMIC_POS		= UD_ANIMATION | UD_MOVE,
			UDF_CAMERAALPHA		= UD_CAMERA_ALPHA | UD_CAMERA_ALPHA_PICK,
		};
		DWORD		s_dwPlag;

		// UD_TERRAIN_BASE
		DWORD		s_dwTerrainBaseShadowMapSize;

		// UD_USE_TEXTYPE
		struct sTEXTURE_GROUP
		{
			void	Delete();
			BYTE	s_btMaterialID;
			std::map< BYTE, nsCSFILE::sFileName_c* >	s_mapFileName;
		};
		std::map< BYTE, sTEXTURE_GROUP* >	s_mapTextureGroup;
	};
}

struct sROOTINFO_010006
{
	DWORD	s_dwMapResVersion;

	// X Vert
	int		s_nXVertCount;
	int		s_nXVertCountInNode;
	// Y Vert
	int		s_nYVertCount;
	int		s_nYVertCountInNode;

	// Height
	float	s_fHeightRate;
	// Grid
	int		s_nGridSize;							// 한 패치의 그리드 넓이
	// Tree
	int		s_nTreeLevel;							// 쿼드트리 레벨
	int		s_nNodeCountInSide;						// 한변에서의 노드 갯수
	int		s_nTotalLeafCount;						// 총 리프 갯수 ( 빈노드 포함 )

	// Height
	TCHAR	s_szHeightMapFile[ MAX_PATH ];

	// Init Size
	int		s_nInitMapCoverSize;
	int		s_nInitMapSize;
	float	s_fDetailMapSize;

	// BaseMap
	int		s_nBaseMPPG;

	// AlphaMap
	int		s_nAlphaMPPG;		

	// ShadowMap
	int		s_nShadowMPPG;
	int		s_nShadowConstant;
	BYTE	s_btObjectShadowColor;
	BYTE	s_btTerrainShadowColor;

	// EffectMap
	int		s_nEffectMPPG;

protected:
	// Object
	DWORD	s_dwUniqObjectID;

public:
	// ==== Data
	float*				s_pfHeightField;
	NiPoint3*			s_pptNormalField;

	// ==== SkyBox
	char				s_cSkyBox[ MAX_PATH ];

	// ===== Fog
	NiColor				s_FogColor;
	float				s_fFogStart;
	float				s_fFogEndTerrain;
	float				s_fFogEndObject;

	// ===== Clipping
	float				s_fClipObject_Far;
	float				s_fClipTerrain_Far;
};

namespace nsOBJECT_010005
{
	struct sPATHINFO
	{
		UINT				s_uiPEGroupCount;
		UINT				s_uiTotalPECount;
		NiPoint2*			s_pptTranse;
		USHORT*				s_pPECount;
		long*				s_pData;
	};

	enum eSAVE_TYPE{
		ST_FIRST_OBJECT = 100,
		ST_EXIST_OBJECT,
	};
	struct sFILE_HEADER
	{
		DWORD		s_dwSaveType;
		DWORD		s_dwInfoSize;
	};
	struct sINFO
	{
		DWORD						s_dwUniqID;
		char						s_cObjectPath[ MAX_PATH ];
		NiTransform					s_trObject;
		nsCSGBFUNC::sTERRAIN_BOUND	s_Bound;

		//===== UserDefine
		DWORD						s_dwTexTypePlag;

		//===== PathEngine
		sPATHINFO					s_PathInfo;
	};
	// 이미 존재하는 오브젝트일경우
	struct sEXIST_INFO
	{
		DWORD						s_dwUniqID;
	};
	struct sUSER_DEFINE
	{
		enum eTYPE{
			UD_NOSHADOW			= 0x00000001,
			UD_ANIMATION		= 0x00000002,
			UD_SNAP				= 0x00000004,
			UD_USE_TEXTYPE		= 0x00000008,
			UD_CAMERA_ALPHA		= 0x00000010,
			UD_MOVE				= 0x00000020,
			UD_TERRAIN_BASE		= 0x00000040,

			UDF_DYNAMIC_POS		= UD_ANIMATION | UD_MOVE,
		};
		DWORD		s_dwPlag;

		// UD_TERRAIN_BASE
		DWORD		s_dwTerrainBaseShadowMapSize;

		// UD_USE_TEXTYPE
		struct sTEXTURE_GROUP
		{
			BYTE	s_btMaterialID;
			std::map< BYTE, nsCSFILE::sFileName_c* >	s_mapFileName;
		};
		std::map< BYTE, sTEXTURE_GROUP* >	s_mapTextureGroup;		
	};

	struct sTB_OBJECT_INFO
	{		
		NiPoint2*			s_pTBObjectUV;
		UINT				s_uiTBObjectUVCount;
		BYTE*				s_pTBObjectPixel;
		UINT				s_uiTBObjectPixelCount;
	};
}

namespace nsOBJECT_010000
{
	struct sPATHINFO
	{
		unsigned short				s_usPECount;
		NiPoint2					s_ptTranse;
		long*						s_pData;
	};

	enum eSAVE_TYPE{
		ST_FIRST_OBJECT = 100,
		ST_EXIST_OBJECT,
	};
	struct sFILE_HEADER
	{
		DWORD		s_dwSaveType;
		DWORD		s_dwInfoSize;
	};
	struct sINFO
	{
		DWORD						s_dwUniqID;
		char						s_cObjectPath[ MAX_PATH ];
		NiTransform					s_trObject;
		nsCSGBFUNC::sTERRAIN_BOUND	s_Bound;

		//===== UserDefine
		DWORD						s_dwTexTypePlag;

		//===== PathEngine
		sPATHINFO					s_PathInfo;
	};
	// 이미 존재하는 오브젝트일경우
	struct sEXIST_INFO
	{
		DWORD						s_dwUniqID;
	};
	struct sUSER_DEFINE
	{
		enum eTYPE{
			UD_NOSHADOW			= 0x00000001,
			UD_ANIMATION		= 0x00000002,
			UD_SNAP				= 0x00000004,
			UD_USE_TEXTYPE		= 0x00000008,
			UD_CAMERA_ALPHA		= 0x00000010,
			UD_MOVE				= 0x00000020,
			UD_TERRAIN_BASE		= 0x00000040,

			UDF_DYNAMIC_POS		= UD_ANIMATION | UD_MOVE,
		};
		DWORD		s_dwPlag;

		// UD_TERRAIN_BASE
		DWORD		s_dwTerrainBaseShadowMapSize;

		// UD_USE_TEXTYPE
		struct sTEXTURE_GROUP
		{
			BYTE	s_btMaterialID;
			std::map< BYTE, nsCSFILE::sFileName_c* >	s_mapFileName;
		};
		std::map< BYTE, sTEXTURE_GROUP* >	s_mapTextureGroup;
	};

	struct sTB_OBJECT_INFO
	{		
		NiPoint2*			s_pTBObjectUV;
		UINT				s_uiTBObjectUVCount;
		BYTE*				s_pTBObjectPixel;
		UINT				s_uiTBObjectPixelCount;
	};
}

struct sROOTINFO_010004
{
	DWORD	s_dwMapResVersion;

	// X Vert
	int		s_nXVertCount;
	int		s_nXVertCountInNode;
	// Y Vert
	int		s_nYVertCount;
	int		s_nYVertCountInNode;

	// Height
	float	s_fHeightRate;
	// Grid
	int		s_nGridSize;							// 한 패치의 그리드 넓이
	// Tree
	int		s_nTreeLevel;							// 쿼드트리 레벨
	int		s_nNodeCountInSide;						// 한변에서의 노드 갯수
	int		s_nTotalLeafCount;						// 총 리프 갯수 ( 빈노드 포함 )

	// Height
	TCHAR	s_szHeightMapFile[ MAX_PATH ];

	// Init Size
	int		s_nInitMapCoverSize;
	int		s_nInitMapSize;
	float	s_fDetailMapSize;

	// BaseMap
	int		s_nBaseMPPG;

	// AlphaMap
	int		s_nAlphaMPPG;		

	// ShadowMap
	int		s_nShadowMPPG;
	int		s_nShadowConstant;
	BYTE	s_btObjectShadowColor;
	BYTE	s_btTerrainShadowColor;

	// EffectMap
	int		s_nEffectMPPG;

protected:
	// Object
	DWORD	s_dwUniqObjectID;

public:
	// ==== Data
	float*				s_pfHeightField;
	NiPoint3*			s_pptNormalField;

	// ==== SkyBox
	char				s_cSkyBox[ MAX_PATH ];

	// ===== Fog
	NiColor				s_FogColor;
	float				s_fFogStart;
	float				s_fFogEndTerrain;
	float				s_fFogEndObject;
};

struct sROOTINFO_010003
{
	DWORD	s_dwMapResVersion;

	// X Vert
	int		s_nXVertCount;
	int		s_nXVertCountInNode;
	// Y Vert
	int		s_nYVertCount;
	int		s_nYVertCountInNode;

	// Height
	float	s_fHeightRate;
	// Grid
	int		s_nGridSize;							// 한 패치의 그리드 넓이
	// Tree
	int		s_nTreeLevel;							// 쿼드트리 레벨
	int		s_nNodeCountInSide;						// 한변에서의 노드 갯수
	int		s_nTotalLeafCount;						// 총 리프 갯수 ( 빈노드 포함 )

	// Height
	TCHAR	s_szHeightMapFile[ MAX_PATH ];

	// Init Size
	int		s_nInitMapCoverSize;
	int		s_nInitMapSize;
	float	s_fDetailMapSize;

	// BaseMap
	int		s_nBaseMPPG;

	// AlphaMap
	int		s_nAlphaMPPG;		

	// ShadowMap
	int		s_nShadowMPPG;
	int		s_nShadowConstant;
	BYTE	s_btObjectShadowColor;
	BYTE	s_btTerrainShadowColor;

	// EffectMap
	int		s_nEffectMPPG;

protected:
	// Object
	DWORD	s_dwUniqObjectID;

public:
	// ==== Data
	float*				s_pfHeightField;
	NiPoint3*			s_pptNormalField;

	// ==== SkyBox
	char				s_cSkyBox[ MAX_PATH ];
};

struct sNiTriINFO_010000
{
	NiPoint2			s_ptOffsetPos;
	int					s_nVertCount;
	int					s_nTemp1;				// 이거도 남는다
	float				s_fTemp2;				// 이거도 남는다
	unsigned short		s_usNiTerrainIndex;		

	// ==== Object
	DWORD				s_dwObjectCount;

	// ==== Texture
	char				s_cDetailMap0[ MAX_PATH ];
	char				s_cDetailMap1[ MAX_PATH ];
	char				s_cDetailMap2[ MAX_PATH ];
	char				s_cDetailMap3[ MAX_PATH ];

	// ==== Shader
	int					s_nShaderCord;

	// ==== Data
	float*				s_pfHeight;
};

struct sROOTINFO_010000
{
	DWORD	s_dwMapResVersion;

	// X Vert
	int		s_nXVertCount;
	int		s_nXVertCountInNode;
	// Y Vert
	int		s_nYVertCount;
	int		s_nYVertCountInNode;

	// Height
	float	s_fHeightRate;
	// Grid
	int		s_nGridSize;							// 한 패치의 그리드 넓이
	// Tree
	int		s_nTreeLevel;							// 쿼드트리 레벨
	int		s_nNodeCountInSide;						// 한변에서의 노드 갯수
	int		s_nTotalLeafCount;						// 총 리프 갯수 ( 빈노드 포함 )

	// Height
	TCHAR	s_szHeightMapFile[ MAX_PATH ];

	// Init Size
	int		s_nInitMapCoverSize;
	int		s_nInitMapSize;
	float	s_fDetailMapSize;

	// BaseMap
	int		s_nBaseMPPG;

	// AlphaMap
	int		s_nAlphaMPPG;		

	// ShadowMap
	int		s_nShadowMPPG;
	int		s_nShadowConstant;
	BYTE	s_btObjectShadowColor;
	BYTE	s_btTerrainShadowColor;

	// EffectMap
	int		s_nEffectMPPG;

protected:
	// Object
	DWORD	s_dwUniqObjectID;

public:
	// ==== Data
	float*				s_pfHeightField;
	NiPoint3*			s_pptNormalField;
};