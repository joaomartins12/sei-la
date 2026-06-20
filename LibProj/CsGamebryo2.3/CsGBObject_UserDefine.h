
#pragma once

struct sUDD_BASE;
struct sUSER_DEFINE : public NiMemObject
{
	CSGBMEMPOOL_H( sUSER_DEFINE );

	sUSER_DEFINE();
	virtual ~sUSER_DEFINE();

	enum eTYPE{
		UD_NOSHADOW				= 0x00000001,
		UD_ANIMATION			= 0x00000002,
		UD_ACTORMANAGER			= 0x00000004,
		UD_USE_TEXTYPE			= 0x00000008,

		UD_CAMERA_ALPHA			= 0x00000010,
		UD_CAMERA_ALPHA_PICK	= 0x00000020,
		UD_TABLE_OBJ			= 0x00000040,
		UD_CHAR_SHADOW			= 0x00000080,

		UD_NO_FOG				= 0x00000100,
		UD_NO_LIGHT				= 0x00000200,
		UD_PORTAL				= 0x00000400,
		UD_TERRAIN_BASE_PICK	= 0x00000800,

		UD_ROTATION				= 0x00001000,
		UD_ROTATION_LIMIT		= 0x00002000,
		UD_CAMERA_COLLISION		= 0x00004000,
		UD_MERGE				= 0x00008000,

		UD_DARKMAP				= 0x00010000,
		UD_TERRAIN_HIDE_PICK	= 0x00020000,
		UD_ALWAYS_ANIMATION		= 0x00040000,
		UD_ACT					= 0x00080000,
		
		UD_ANIMATION_RANDOM		= 0x00100000,
		UD_SOUND				= 0x00200000,
		UD_SKIN					= 0x00400000,
		UD_F1					= 0x00800000,		

		UD_NO_CLIPPING			= 0x01000000,
		UD_NO_PICK				= 0x02000000,
		UD_QUEST_ON				= 0x04000000,
		UD_QUEST_OFF			= 0x08000000,

		UD_EFFECTRENDER			= 0x10000000,
		UD_PATH					= 0x20000000,

		//UDF_DYNAMIC_POS		= UD_ANIMATION | UD_ALWAYS_ANIMATION | UD_MOVE,
		UDF_FIRSTRENDER		= UD_TERRAIN_BASE_PICK | UD_TERRAIN_HIDE_PICK | UD_F1,
		UDF_CAMERAALPHA		= UD_CAMERA_ALPHA | UD_CAMERA_ALPHA_PICK,
		UDF_CAMERA_PICK		= UD_CAMERA_ALPHA_PICK | UD_CAMERA_COLLISION,
		//UDF_TB_TYPE			= UD_TERRAIN_BASE | UD_TERRAIN_BASE_PICK,
		UDF_ROTATION		= UD_ROTATION | UD_ROTATION_LIMIT,
		UDF_HAVE_CHILD		= UD_MERGE,
		UDF_ANIMATION		= UD_ANIMATION | UD_ALWAYS_ANIMATION | UD_ANIMATION_RANDOM | UD_ACTORMANAGER,
	};

	void		Reset(){ s_dwPlag = 0; s_dwVisiblePlag = 0; assert_cs( s_mapUDData.empty() == true ); }
	void		Delete();
	void		DeleteData( eTYPE type );
	bool		IsData( eTYPE type ){ return ( s_mapUDData.find( type ) != s_mapUDData.end() ); }
	sUDD_BASE*	GetData( eTYPE type ){ assert_cs( IsData( type ) == true );  return s_mapUDData[ type ]; }	
	void		InsertData( sUDD_BASE* pData, eTYPE type ){ assert_cs( s_mapUDData.find( type ) == s_mapUDData.end() ); s_mapUDData[ type ] = pData; }	

	std::map< DWORD, sUDD_BASE* >	s_mapUDData;

	DWORD		s_dwPlag;
	DWORD		s_dwVisiblePlag;
};

typedef bool (*CALLBACK_QuestCheck)( int nQuest );

struct sUDD_BASE
{
	sUDD_BASE() {};
	virtual ~sUDD_BASE() {};

	virtual void					Delete() = 0;
	virtual sUSER_DEFINE::eTYPE		GetType() = 0;

	// ==== Quest

public:
	static CALLBACK_QuestCheck		m_CallBack_QuestCheck;
public:	
	static void						ResistCallBack_QuestCheck( void* func ){ m_CallBack_QuestCheck = (CALLBACK_QuestCheck)func; }
};

struct sUDD_DARKMAP : public sUDD_BASE
{
	sUDD_DARKMAP():s_dwTerrainBaseShadowMapSize(0){};
	~sUDD_DARKMAP(){};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_DARKMAP; }
	static sUSER_DEFINE::eTYPE	Type(){ return sUSER_DEFINE::UD_DARKMAP; }

	void	Delete(){}

	DWORD		s_dwTerrainBaseShadowMapSize;
};

struct sUDD_ANI_RANDOM : public sUDD_BASE
{
	sUDD_ANI_RANDOM():s_nStartFrame(0),s_nEndFrame(0){};
	~sUDD_ANI_RANDOM(){};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_ANIMATION_RANDOM; }
	static sUSER_DEFINE::eTYPE	Type(){ return sUSER_DEFINE::UD_ANIMATION_RANDOM; }

	void	Delete(){}

	int		s_nStartFrame;
	int		s_nEndFrame;
};

struct sUDD_TexType : public sUDD_BASE
{
	sUDD_TexType(){};
	~sUDD_TexType(){};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_USE_TEXTYPE; }
	static sUSER_DEFINE::eTYPE	Type(){ return sUSER_DEFINE::UD_USE_TEXTYPE; }

	void	Delete();

	// UD_USE_TEXTYPE
	struct sTEXTURE_GROUP : public NiMemObject
	{
		sTEXTURE_GROUP():s_btMaterialID(0) {};
		~sTEXTURE_GROUP() {};

		void	Delete();
		BYTE	s_btMaterialID;
		std::map< BYTE, nsCSFILE::sFileName_c* >	s_mapFileName;
	};
	std::map< BYTE, sTEXTURE_GROUP* >	s_mapTextureGroup;
};


struct sUDD_Rotation : public sUDD_BASE
{
	sUDD_Rotation():s_fRotSpeed(0.0f),s_fRotLimit_Min(0.0f),s_fRotLimit_Max(0.0f){};
	~sUDD_Rotation(){};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UDF_ROTATION; }
	static sUSER_DEFINE::eTYPE	Type(){ return sUSER_DEFINE::UDF_ROTATION; }

	void	Delete(){}

	float		s_fRotSpeed;
	float		s_fRotLimit_Min;
	float		s_fRotLimit_Max;
};

struct sUDD_Merge : public sUDD_BASE
{
	sUDD_Merge(){};
	~sUDD_Merge(){};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_MERGE; }
	static sUSER_DEFINE::eTYPE	Type(){ return sUSER_DEFINE::UD_MERGE; }

	void	Delete();

	struct sINFO
	{
		sINFO():s_nNiNodeChildIndex(0),s_nSlotID(0){};
		~sINFO(){};
		int						s_nNiNodeChildIndex;
		int						s_nSlotID;
		char					s_cFileName[ MAX_PATH ];
	};

	std::map< DWORD, sINFO* >	s_mapInfo;
};

struct sUDD_Sound : public sUDD_BASE
{
	sUDD_Sound():s_fLoopTime(0.0f),s_fMinDist(0.0f),s_fMaxDist(0.0f) {};
	~sUDD_Sound() {};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_SOUND; }
	static sUSER_DEFINE::eTYPE	Type(){ return sUSER_DEFINE::UD_SOUND; }

	void	Delete(){}

	char	s_cFileName[ MAX_FILENAME ];
	float	s_fLoopTime;
	float	s_fMinDist;
	float	s_fMaxDist;
};

struct sUDD_QuestOn : public sUDD_BASE
{
	sUDD_QuestOn() {};
	~sUDD_QuestOn() {};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_QUEST_ON; }
	static sUSER_DEFINE::eTYPE		Type(){ return sUSER_DEFINE::UD_QUEST_ON; }

	void	Delete();

	struct sINFO
	{
		sINFO():s_bCompleate(false) {};
		~sINFO(){};
		bool					s_bCompleate;
		std::list< int >		s_listQuest;
		void		Delete(){ s_listQuest.clear(); s_bCompleate = false; }
	};
	std::list< sINFO* >			s_listInfo;
	bool		IsOn();
	bool		IsOn( int nCompQuest );
};

struct sUDD_QuestOff : public sUDD_BASE
{
	sUDD_QuestOff() {};
	~sUDD_QuestOff() {};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_QUEST_OFF; }
	static sUSER_DEFINE::eTYPE		Type(){ return sUSER_DEFINE::UD_QUEST_OFF; }

	void	Delete();

	struct sINFO
	{
		sINFO():s_bCompleate(false) {};
		~sINFO() {};

		bool					s_bCompleate;
		std::list< int >		s_listQuest;
		void		Delete(){ s_listQuest.clear(); s_bCompleate = false; }
	};
	std::list< sINFO* >			s_listInfo;

	bool		IsOff();
	bool		IsOff( int nCompQuest );
};


struct sUDD_ACTOR : public sUDD_BASE
{
	sUDD_ACTOR() {};
	~sUDD_ACTOR() {};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_ACTORMANAGER; }
	static sUSER_DEFINE::eTYPE		Type(){ return sUSER_DEFINE::UD_ACTORMANAGER; }

	void	Delete(){}

	char	s_cKey[ 32 ];
};

struct sUDD_ACT : public sUDD_BASE
{
	sUDD_ACT():s_nHashCode(0) {};
	~sUDD_ACT() {};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_ACT; }
	static sUSER_DEFINE::eTYPE		Type(){ return sUSER_DEFINE::UD_ACT; }

	void	Delete(){}

	DWORD	s_nHashCode;
};

struct sUDD_PATH : public sUDD_BASE
{
	sUDD_PATH():s_nHashCode(0),s_pDynamicAgent(NULL) {};
	~sUDD_PATH() {};

	virtual sUSER_DEFINE::eTYPE		GetType(){ return sUSER_DEFINE::UD_PATH; }
	static sUSER_DEFINE::eTYPE		Type(){ return sUSER_DEFINE::UD_PATH; }

	void	Delete(){ SAFE_DELETE( s_pDynamicAgent ); }

	DWORD	s_nHashCode;
	iAgent*	s_pDynamicAgent;
	int		s_nPos[ 2 ];
	int		s_nData[ 2 ];
};





