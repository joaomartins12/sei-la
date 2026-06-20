
#pragma once 
#pragma pack( push, 4 )

namespace CsFPS
{
	class CsFileHash
	{
	public:

		struct sINFO
		{
			sINFO():s_nPlag(0),s_nDataSize(0),s_nAllocSize(0),s_nHash(0),s_nOffset(0)
			{};

			int				s_nPlag;
			size_t			s_nDataSize;				// 청크헤더를 제외한 실 데이터 사이즈	( 가변적 - 추가 데이터 넣을경우에 작아질수도 있다 )
			size_t			s_nAllocSize;				// 청크헤더를 제외한 실 데이터 사이즈	( 고정적 - 초기 Add 시에 해당 사이즈 만큼 고정 )
			size_t			s_nHash;			
			UINT64			s_nOffset;					// 청크헤더를 건너띈 실 데이터 오프셋
		};

		struct sINFONew
		{
			size_t			s_nHash;
			int				s_nPlag;
			UINT64			s_nOffset;
			size_t			s_nDataSize;				// 청크헤더를 제외한 실 데이터 사이즈	( 가변적 - 추가 데이터 넣을경우에 작아질수도 있다 )
			size_t			s_nAllocSize;				// 청크헤더를 제외한 실 데이터 사이즈	( 고정적 - 초기 Add 시에 해당 사이즈 만큼 고정 )
		};

		CsFileHash();
		~CsFileHash();
		
	private:
		enum ePLAG
		{
			PG_TYPE_DATA	= 0x00000001,
			PG_TYPE_DUMMY	= 0x00000002,
			PG_TYPE_MASK	= 0x000000ff,
		};
	public:
		void			Delete();
		
		void			New( LPCSTR szHashPath );
		DWORD			Load( LPCSTR szHashFile );
		
		std::string		GetPath(){ return m_szFilePath; }
		FHHeader*		GetHeader(){ return &m_Header; }

		std::map< size_t, sINFO* >*		GetHashMap(){ return &m_mapHash; }
		std::list< sINFO* >*			GetDummyVector(){ return &m_vpDummy; }

		UINT64			CalOffset( size_t nData );
		UINT64			CalOffset( LPCSTR cFilePath, size_t nData );
		size_t			GetTotalFileCount();

		bool			IsExist( size_t nHash ) const;

		sINFO *			GetHashInfo( size_t nHash );

		void			SetAllocCount( int nAllocCount );
		void			SaveFile();

	private:
		friend class CsFilePack;
		void			_AddFile( LPCSTR szPath, UINT64 nOffset, size_t nDataSize );
		void			_EditFile( LPCSTR szPath, UINT64 nOffset, size_t nDataSize );
		void			_PointerInsertMap();

	protected:
		std::string					m_szFilePath;
		FHHeader					m_Header;
		sINFO*						__m_pDeletePointer;						// 메모리 제거를 위한 배열포인터
		int							m_nAllocCount;
		std::map< size_t, sINFO* >	m_mapHash;
		std::list< sINFO* >			m_vpDummy;
	};
}

#pragma pack( pop )