#pragma once

#ifndef DM_SYSTEM_STORAGE_FILE_FN_H
#define DM_SYSTEM_STORAGE_FILE_FN_H

#include <stdarg.h>

namespace DmCS
{
	namespace Storage
	{
		struct File
		{
			File(void);
			~File(void);

			bool Open(dm_string_t conref sFile, dm_string_t conref sOpenType = L"rb");
			bool Create(dm_string_t conref sFile, dm_string_t conref sOpenType = L"wb");
			dm_slong_t Size(void);
			void Close(void);

			bool Seek(dm_slong_t conref offset) const;
			bool Read(dm_bytevec & p) const;

			template<typename T>
			bool Write( T conref value )
			{
				if( !Invalid() )
					return false;

				fwrite( &value, sizeof(T), 1, m_pFile );
				return true;
			}

			template<typename T>
			bool Write( T const * value, size_t len )
			{
				if( !Invalid() )
					return false;

				fwrite( value, sizeof(T), len, m_pFile );
				return true;
			}

			template<typename T>
			bool Write( std::basic_string< T > conref value )
			{
				if( !Invalid() )
					return false;

				fwrite( value.c_str(), sizeof(T), value.size(), m_pFile );
				return true;
			}
 
			bool Write( dm_bytevec conref value )
			{
				if( !Invalid() )
					return false;

				fwrite( &value.at(0), sizeof(dm_byte_t), value.size(), m_pFile );
				return true;
			}

			bool WriteVA( dm_str_t cptr szformat, ... )
			{
				if( !Invalid() )
					return false;

				va_list		args;
				dm_string_t stmp;
				stmp.resize( 1024 );

				va_start( args, szformat );
				dm_sint_t iLength = ::vswprintf_s( &stmp.at( 0 ), 1024, szformat, args );
				va_end( args );

				fwrite( stmp.c_str(), sizeof(dm_str_t), iLength, m_pFile );
				return true;
			}

			bool WriteVA( dm_str cptr szformat, ... )
			{
				if( !Invalid() )
					return false;

				va_list		args;
				dm_string stmp;
				stmp.resize( 1024 );

				va_start( args, szformat );
				int iLength = ::vsprintf_s( &stmp.at( 0 ), 1024, szformat, args );
				va_end( args );

				fwrite( stmp.c_str(), sizeof(dm_str), iLength, m_pFile );
				return true;
			}

			bool Invalid(void) const;

		private:
			FILE*	m_pFile;
		};

	};
}
#endif