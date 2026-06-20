#pragma once
#include "CriticalsectionUtil.h"

#pragma pack( push, 4 )

namespace CsFPS
{
	class CsFilePack
	{
	public:
		CsFilePack();
		~CsFilePack();

	protected:
		char			m_szFilePath[ MAX_PATH ];
		FPHeader		m_Header;
		int				m_nHandle;
		CsFileHash*		m_pBuddyHash;

		CSSpinLock	*	m_CS;
		//CRITICAL_SECTION	m_CS;

	public:
		void			Delete();

		void			New( CsFileHash* pBuddyHash, char const* szPath );
		DWORD			Load( CsFileHash* pBuddyHash, char const* szPath, bool bWrite );

// 		void			SeekLock(){ EnterCriticalSection( &m_CS ); }
// 		void			SeekUnlock(){ LeaveCriticalSection( &m_CS ); }
		void			SeekLock();
		void			SeekUnlock();

		int				__Handle(){ return m_nHandle; }

		bool			_AddFile( char const* cDataPath, char const* cFilePath, char* pBuffer );
		bool			_EditFile( char const* cDataPath, char const* cFilePath, char* pBuffer );
		bool			_EditFile( CsFilePack* pPatch_Pack, CsFileHash::sINFO* pPatch_HashInfo );
		bool			_EditFile( CsFilePack* pPatch_Pack, CsFileHash* pPatch_Hash );

		sCHUNK*			_GetChunk( sCHUNK* pChunk, UINT64 nOffset );
		char*			_GetData( char** ppData, UINT64 nOffset, size_t nSize );
		void			_GetData( std::vector<unsigned char>& ppData, UINT64 nOffset, size_t nSize );
		int				_GetFileHandle( UINT64 nOffset );
	};
}

#pragma pack( pop )