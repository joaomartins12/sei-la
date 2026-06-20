
#pragma once 

#pragma pack(push)
#pragma pack(4)

#define MAX_HELP_CHUNK_LEN			8

class CsHelp
{
public:
	enum eTYPE{ STRING, IMAGE };

public:
	struct sTEXT
	{
		enum eTEXTTYPE{ TT_NORMAL, TT_LINK, };
		sTEXT(){ s_nTextSize = 10; s_nBold = 0; s_Color[ 0 ] = 0xff; s_Color[ 1 ] = 0xff; s_Color[ 2 ] = 0xff; s_TextType = TT_NORMAL; s_TextTypeVal1 = s_TextTypeVal2 = 0; }
		~sTEXT(){ Delete(); }		

		void			Delete();
		void			Save( FILE* fp );
		void			Load( FILE* fp );
		void			LoadFilePack( int nHandle );
		bool			Check();
		
		SHORT					s_nTextSize;
		SHORT					s_nBold;
		BYTE					s_Color[ 3 ];
		std::wstring			s_Text;

		USHORT					s_TextType;
		USHORT					s_TextTypeVal1;
		USHORT					s_TextTypeVal2;
		CsRect					s_rcRegion;
	};
	struct sELEMENT
	{		
		virtual eTYPE GetType() = 0;

		CsPoint			s_ptPos;
	};
	struct sSTRING : public sELEMENT
	{
		sSTRING(){ s_nLineDeltaHeight = 25; }
		~sSTRING(){ Delete(); }
		virtual eTYPE GetType(){ return STRING; }

		void			Delete();
		void			DeleteText( int nIndex );
		void			DeleteText( CsHelp::sTEXT* pTextFT );
		void			DeleteText_ExceptFirst();

		void			Save( FILE* fp );
		void			Load( FILE* fp );		
		void			LoadFilePack( int nHandle );		

		void			Check();
		void			CheckCombine();
		CsHelp::sTEXT*	CheckText( CsHelp::sTEXT* pTextFT );
		bool			IsText( CsHelp::sTEXT* pTextFT );

		CsHelp::sTEXT*	AddText();
		CsHelp::sTEXT*	AddTextAfter( CsHelp::sTEXT* pTextFT );
		CsHelp::sTEXT*	AddTextBefore( CsHelp::sTEXT* pTextFT );
		CsHelp::sTEXT*	GetFirstText();
		CsHelp::sTEXT*	GetLastText();
		CsHelp::sTEXT*	GetPrevText( CsHelp::sTEXT* pTextFT );
		CsHelp::sTEXT*	GetNextText( CsHelp::sTEXT* pTextFT );
		
		int						s_nLineDeltaHeight;
		CsVectorPB< sTEXT* >	s_vpText;
	};
	struct sIMAGE : public sELEMENT
	{
		enum eIMAGE_TYPE{ IT_NORMAL, IT_VIDEO };

		sIMAGE(){ s_ImagePath.clear(); s_nImageType = IT_NORMAL; }
		~sIMAGE(){ Delete(); }
		virtual eTYPE GetType(){ return IMAGE; }

		void			Delete(){}
		void			Save( FILE* fp );
		void			Load( FILE* fp );
		void			LoadFilePack( int nHandle );

		int						s_nImageType;
		CsPoint					s_ptSize;
		std::string				s_ImagePath;
	};
	struct sCONTAINER
	{
		sCONTAINER(){ s_nLimitScroll = 50; }
		~sCONTAINER(){ Delete(); }
		void				Delete();
		void				DeleteString( CsHelp::sSTRING* pString );		
		void				DeleteImage( CsHelp::sIMAGE* pImage );

		void				Save( FILE* fp );
		void				Load( FILE* fp );
		void				LoadFilePack( int nHandle );
		void				ToEng( CsHelp* pParent );

		void				Check();
		CsHelp::sSTRING*	AddString();
		CsHelp::sIMAGE*		AddImage( sIMAGE::eIMAGE_TYPE it = sIMAGE::IT_NORMAL );

		int						s_nLimitScroll;
		CsVectorPB< sSTRING* >	s_vpString;
		CsVectorPB< sIMAGE* >	s_vpImage;
	};

public:
	CsHelp();
public:
	void		Delete(){}
	void		Save( FILE* fp );
	void		Load( FILE* fp );
	void		LoadFilePack( int nHandle );

	void		ToEng();

public:
	enum eREQUIRE_TYPE{ RT_NONE, RT_TLEVEL, RT_QUEST_RECV, RT_MAP_ARRIVE, RT_PARTNERMON_LEVEL };
	struct sTITLE
	{
		sTITLE(){ s_szTitle[ 0 ] = NULL; s_eReqType = RT_NONE; s_nReqID1 = s_nReqID2 = 0; s_nUniqID = 0; }
		UINT			s_nUniqID;
		TCHAR			s_szTitle[ 64 ];
		eREQUIRE_TYPE	s_eReqType;
		int				s_nReqID1;
		int				s_nReqID2;
	};
protected:	
	sTITLE		m_Title;

public:
	//void		SetTitle( TCHAR* sz ){ _tcscpy_s( m_szTitle, 64, sz ); }
	sTITLE*		GetTitle(){ return &m_Title; }

protected:
	sCONTAINER		m_Body;
public:
	sCONTAINER*		GetBody(){ return &m_Body; }
};

#pragma pack(pop)