
#pragma once 
#pragma pack( 4 )

enum eKEY_SET
{
	KS_OBJ_DEL = 0,
	KS_OBJ_DETACH,
	KS_OBJ_P_ROT,
	KS_OBJ_M_ROT,
	KS_OBJ_INIT_ROT,
	KS_TER_NODE_CHILD,
	KS_TER_NODE_PARENT,
	KS_RB_UNDO,
	KS_RB_REDO,
	KS_OBJ_COPY,
	KS_OBJ_RENDERING,
	KS_TAB_SEL1,
	KS_SLIDE_B_LEFT,
	KS_SLIDE_B_RIGHT,
	KS_SLIDE_S_LEFT,
	KS_SLIDE_S_RIGHT,
	KS_FILE_SAVEAS,
	KS_FILE_LOADAS,
	KS_OBJ_SAVE1,
	KS_OBJ_SAVE2,
	KS_OBJ_SAVE3,
	KS_OBJ_SAVE4,
	KS_OBJ_SAVE5,
	KS_OBJ_SAVE6,
	KS_OBJ_SAVE7,
	KS_OBJ_SAVE8,
	KS_OBJ_SAVE9,
	KS_OBJ_SAVE10,
	KS_OBJ_LOAD1,
	KS_OBJ_LOAD2,
	KS_OBJ_LOAD3,
	KS_OBJ_LOAD4,
	KS_OBJ_LOAD5,
	KS_OBJ_LOAD6,
	KS_OBJ_LOAD7,
	KS_OBJ_LOAD8,
	KS_OBJ_LOAD9,
	KS_OBJ_LOAD10,
	KS_OBJ_SELECTALL,
	KS_OBJ_TERRAIN_ATTACH,
	KS_FULLSCREEN,
	KS_COUNT,
};

enum eKEY_PLAG
{
	KP_SHIPT	= 0x0000000f,
	KP_CONTROL	= 0x000000f0,
	KP_ALT		= 0x00000f00,
};

class cConfig
{

public:
#define	CONFIG_PATH				L".\\ToolRgs\\MapTool.cfg"
#define CONFIG_VERSION			0x00000109
#define CONFIG_STRUCT_COUNT		5

	struct sSTRUCT_HEADER{
		DWORD	s_dwStructSize;
	};
	struct sHEADER : public sSTRUCT_HEADER{
		void Reset();
		DWORD	s_dwVersion;
		DWORD	s_dwStructCount;
	};
	struct sKEYSET : public sSTRUCT_HEADER{
		struct sKEYINFO{
			sKEYINFO(){ Init(); }
			void Init(){ s_btKey = 0x00; s_dwPlag = 0x00000000; }
			BYTE s_btKey;	DWORD s_dwPlag;
		};
		void Reset();

		sKEYINFO		s_KeyInfo[ KS_COUNT ];		
	};
	struct sAUTOSAVE : public sSTRUCT_HEADER{
		void Reset();
		bool			s_bAutoSave;
		float			s_fAutoSaveTime;
		int				s_nSaveIndex;
	};
	struct sROLLBACK : public sSTRUCT_HEADER{
		void Reset();
		int				s_nRollBackCount;
	};
	struct sENVIRONMENT : public sSTRUCT_HEADER{
		void Reset();
		int				s_nSleepTick;		
		bool			s_bWriteExcel;
		bool			s_bWriteList;
		bool			s_bWritePortal;
		bool			s_bWriteCharLight;
	};

protected:
	sHEADER				m_Header;
	sKEYSET				m_KeySet;
	sAUTOSAVE			m_AutoSave;
	sROLLBACK			m_RollBack;
	sENVIRONMENT		m_Environment;

public:
	sKEYSET*			GetKeySet(){ return &m_KeySet; }
	sAUTOSAVE*			GetAutoSave(){ return &m_AutoSave; }
	sROLLBACK*			GetRollBack(){ return &m_RollBack; }
	sENVIRONMENT*		GetEnvironment(){ return &m_Environment; }

public:
	void	Reset();
	void	Apply();
	void	Save();
	void	Load();
};

extern cConfig			g_Config;