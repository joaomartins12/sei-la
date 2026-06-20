#include "stdafx.h"
#include "DmStorageFile.h"

namespace DmCS
{
	namespace Storage
	{		
		//////////////////////////////////////////////////////////////////////////////////////
		File::File(void)
			: m_pFile( NULL )
		{
		}

		File::~File(void)
		{
			Close();
		}

		bool File::Open(dm_string_t conref sFile, dm_string_t conref sOpenType )
		{
			_wfopen_s( &m_pFile, sFile.c_str(), sOpenType.c_str() );//, ccs=UNICODE
			if(!m_pFile)
			{
				wprintf( L"File Open False : %s\n", sFile.c_str() );
				return false;
			}

			return true;		
		}

		bool File::Create(dm_string_t conref sFile, dm_string_t conref sOpenType )
		{
			_wfopen_s( &m_pFile, sFile.c_str(), sOpenType.c_str() );
			if(!m_pFile)
			{
				wprintf( L"File Open False : %s\n", sFile.c_str() );
				return false;
			}

			return true;
		}

		dm_slong_t File::Size(void)
		{
			if( !Invalid() )
				return 0;

			fseek(m_pFile, 0, SEEK_END);
			dm_slong_t size = ftell(m_pFile);
			fseek(m_pFile, 0, SEEK_SET);
			return size;
		}

		void File::Close(void)
		{
			if( Invalid() )
				fclose(m_pFile);
		}

		bool File::Seek(dm_slong_t conref offset) const
		{
			if( !Invalid() )
				return false;

			fseek(m_pFile, offset, SEEK_SET);
			return true;
		}

		bool File::Read(dm_bytevec & p) const
		{
			if( !Invalid() )
				return false;

			fread( &p.at(0), sizeof(dm_byte_t), p.size(), m_pFile );
			return true;
		}

		bool File::Invalid(void) const
		{
			return ( NULL != m_pFile );
		}
		//////////////////////////////////////////////////////////////////////////////////////

	};
};