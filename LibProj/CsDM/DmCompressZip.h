#pragma once

#ifdef _DM_USE_COMPRESSZIP_

#ifndef DM_FILE_SYSTEM_COMPRESS_ZIP_H
#define DM_FILE_SYSTEM_COMPRESS_ZIP_H

namespace DmCS
{
	struct CompressZip
	{
		static dm_ulong_t Crc32(dm_bytevec conref i_vec);
		static bool Compress(dm_bytevec conref i_vec, dm_bytevec& o_vec);
		static bool UnCompress(dm_bytevec conref i_vec, dm_bytevec& o_vec);
	};
}

#endif//ECLIPSE_WAR_FILE_SYSTEM_COMPRESS_ZIP_H

#endif