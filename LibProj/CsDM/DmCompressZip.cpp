#include "stdafx.h"
#include "DmCompressZip.h"

#ifdef _DM_USE_COMPRESSZIP_

#include "zlib.h"

#pragma optimize( "", off )

namespace DmCS
{
	dm_ulong_t CompressZip::Crc32(dm_bytevec conref i_vec)
	{
		dm_ulong_t ulCrc = ::crc32( 0, Z_NULL, 0 );
		ulCrc = ::crc32( ulCrc, &i_vec.at( 0 ), i_vec.size() );
		return ulCrc;
	}

	bool CompressZip::Compress(dm_bytevec conref i_vec, dm_bytevec& o_vec)
	{
		dm_dword_t dwSize = static_cast< dm_dword_t >( i_vec.size() * 1.001f + 12 );
		dm_bytevec tmpvec( dwSize );

		if( Z_OK == ::compress( (Bytef*)&tmpvec.at( 0 ), (uLongf*)&dwSize, (Bytef*)&i_vec.at( 0 ), i_vec.size() ) )
		{
		//	if( i_vec.size() > dwSize )//무조건 압축 하기 위함.
		//	{
				tmpvec.resize( dwSize );
				o_vec.swap( tmpvec );
				return true;
		//	}
		}
		return false;
	}

	bool CompressZip::UnCompress(dm_bytevec conref i_vec, dm_bytevec& o_vec)
	{
		dm_bytevec tmpvec;
		std::size_t size_uncompressed = 0;

		do 
		{
			std::size_t resize_to = size_uncompressed + 2 * i_vec.size();
			tmpvec.resize(resize_to);
			int ret = ::uncompress( (Bytef*)&tmpvec.at( 0 ), (uLongf*)&resize_to, (Bytef*)&i_vec.at( 0 ), i_vec.size() );
			if (ret != Z_STREAM_END && ret != Z_OK && ret != Z_BUF_ERROR)
				return false;

			if( ret == Z_OK )
			{
				size_uncompressed = resize_to;
				break;
			}

			size_uncompressed += resize_to;
		} while ( 1 );

		tmpvec.resize( size_uncompressed );
		o_vec.swap( tmpvec );
		return true;
	}
}

#pragma optimize( "", on )

#endif