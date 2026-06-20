
#pragma once 

class CsModelData : public NiMemObject
{
public:
	CsModelData();
	~CsModelData();
	/*struct sLOAD_EVENT : public NiMemObject
	{
		sLOAD_EVENT(){	s_fEventTime = 0.0f; s_nType = 0; s_cText[ 0 ] = NULL; s_dwPlag = 0; s_vOffset = NiPoint3::ZERO; memset( s_Dummy, 0, sizeof(char)*3 );
		s_fEffectScale = 1.0f; s_bParentScale = false;
		}
		float		s_fEventTime;
		int			s_nType;
		int			s_nStaticIndex;
		char		s_cText[ MAX_TEXTKEY_LEN ];

		DWORD		s_dwPlag;
		NiPoint3	s_vOffset;
		float		s_fEffectScale;

		bool		s_bParentScale;

		char		s_Dummy[ 3 ];
	};*/

	struct sSHADER : public NiMemObject
	{
		sSHADER(){ Reset(); }

		void	Reset()
		{
			memset( s_cApplyObjectName, 0, sizeof( char )*32 );
			memset( s_nDummy, 0, sizeof( int )*4 );
			s_eShaderType = 0;
			s_nValue1 = 0;
			s_fValue1 = 0;
			s_fValue2 = 0;
			s_fValue3 = 0;
		}

		char			s_cApplyObjectName[ 32 ];		
		int				s_eShaderType;

		int				s_nValue1;
		float			s_fValue1;
		float			s_fValue2;
		float			s_fValue3;		

		int				s_nDummy[ 4 ];
	};

	struct sEVENT : public NiMemObject
	{
		sEVENT(){	s_fEventTime = 0.0f; s_nType = 0; s_dwPlag = 0; s_vOffset = NiPoint3::ZERO;
					s_fEffectScale = 1.0f; s_bParentScale = false;
					s_fFadeoutTime = 0.4f; 
					s_vValue1 = NiPoint3::ZERO;
					s_vValue2 = NiPoint3::ZERO;
					s_nStaticIndex = 0;
					memset( s_cText, 0, sizeof(char)*MAX_TEXTKEY_LEN );
		}
		float		s_fEventTime;
		int			s_nType;
		int			s_nStaticIndex;
		char		s_cText[ MAX_TEXTKEY_LEN ];

		DWORD		s_dwPlag;
		NiPoint3	s_vOffset;
		float		s_fEffectScale;
		bool		s_bParentScale;

		float		s_fFadeoutTime;
		NiPoint3	s_vValue1;
		NiPoint3	s_vValue2;
	};

	struct sSEQUENCE : public NiMemObject
	{
		sSEQUENCE()	{};
		~sSEQUENCE() {};

		struct sSEQUENCE_INFO
		{
			sSEQUENCE_INFO():s_dwSequenceID(0),s_nEventCount(0),s_nLoopCnt(0),s_nShaderCnt(0)
			{
			}

			DWORD		s_dwSequenceID;
			int			s_nEventCount;
			int			s_nLoopCnt;
			int			s_nShaderCnt;
		};
		sSEQUENCE_INFO			s_SequenceInfo;
		CsVectorPB< sEVENT* >	s_vpEvent;
		CsVectorPB< sSHADER* >	s_vpShader;

		void			Delete();
		sEVENT*			GetEvent( int nIndex ){ return s_vpEvent[ nIndex ]; }
		sSHADER*		GetShader( int nIndex ){ return s_vpShader[ nIndex ]; }

		void			InsertEvent( sEVENT* pEvent );		
		void			DeleteEvent( int nIndex );

		void			InsertShader( sSHADER* pShader );
		void			DeleteShader( int nIndex );
	};

	struct sINFO : public NiMemObject
	{
		sINFO():s_dwID(0),s_fScale(1.0f),s_fHeight(1.0f),s_fWidth(1.0f),s_nSequenceCount(0)
		{
			memset( s_Dummy, 0, sizeof(char)*16 );
			memset( s_cKfmPath, 0, sizeof(char)*OBJECT_PATH_LEN );
		}

		void		Init()
		{	
			s_dwID = 0; 
			s_fScale = 1.0f; 
			s_fHeight = 1.0f; 
			s_fWidth = 1.0f; 
			s_nSequenceCount = 0;
			memset( s_cKfmPath, 0, sizeof(char)*OBJECT_PATH_LEN );
			memset( s_Dummy, 0, sizeof(char)*16 );			
		}

		DWORD		s_dwID;
		char		s_cKfmPath[ OBJECT_PATH_LEN ];
		float		s_fScale;
		float		s_fHeight;
		float		s_fWidth;
		int			s_nSequenceCount;

		char		s_Dummy[ 16 ];
	};
protected:
	sINFO							m_Info;
	std::map< DWORD, sSEQUENCE* >	m_mapSequence;

public:
	void			Delete();
	void			Init(){ m_Info.Init(); }

	sINFO*			GetInfo(){ return &m_Info; }	

	bool			IsSequence( DWORD dwSequenceID ){ return ( m_mapSequence.find( dwSequenceID ) != m_mapSequence.end() ); }
	void			AddSequence( DWORD dwSequenceID );
	void			InsertSequence( DWORD dwSequenceID, sSEQUENCE* pSequence );
	sSEQUENCE*		GetSequence( DWORD dwSequenceID ){ assert_cs( IsSequence( dwSequenceID ) == true ); return m_mapSequence[ dwSequenceID ]; }
	std::map< DWORD, sSEQUENCE* >*	GetSequenceMap(){ return &m_mapSequence; }
};
