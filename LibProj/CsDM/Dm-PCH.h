

#ifndef DM_SYSTEM_PCH_H
#define DM_SYSTEM_PCH_H

#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>


// SAFE
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif	SAFE_DELETE


#define DmExternStrName(name)			extern dm_str_t cptr const name
#define DmDeclStrName(name)				static dm_str_t cptr const name = L#name
#define DmDeclStrNameCustom(name, str)	static dm_str_t cptr const name = L##str
#define DmCheckBit(flag, bit)			( flag & bit )

#define DmSafeDel(x)		{ if( x ){ delete ( x );	( x ) = 0; } }

#define DmDefPropertyG(type, prefix, name)\
	public:									\
		inline type conref name(void) const	\
		{									\
			return m_##prefix##name;		\
		}									\
	protected:								\
		type m_##prefix##name;

#define DmDefPropertyGS(type, prefix, name)\
	public:									\
		inline type conref name(void) const	\
		{									\
			return m_##prefix##name;		\
		}									\
		inline void name(type conref val)		\
		{									\
			m_##prefix##name = val;			\
		}									\
	protected:								\
		type m_##prefix##name;

#define DmDefPointerPropertyGS(type, name)\
	public:								\
		inline type* name(void) const	\
		{								\
			return m_p##name;			\
		}								\
		inline void name(type* p##val)	\
		{								\
			m_p##name = p##val;			\
		}								\
	protected:							\
		type* m_p##name;

#define conref	const&
#define cptr	const*
#define DmDecl	=0

#define DmS(type)		signed type
#define DmU(type)		unsigned type

typedef DmS(char)		dm_schar_t;
typedef DmS(short)		dm_sshort_t;
typedef DmS(int)		dm_sint_t;
typedef DmS(__int64)	dm_sint64_t;
typedef DmS(long)		dm_slong_t;

typedef DmU(char)		dm_byte_t;
typedef DmU(short)		dm_word_t;
typedef DmU(int)		dm_dword_t;
typedef DmU(__int64)	dm_qword_t;
typedef DmU(long)		dm_ulong_t;

typedef wchar_t			dm_str_t;
typedef char			dm_str;
typedef dm_qword_t		dm_version_t;

typedef std::vector< dm_byte_t >		dm_bytevec;
typedef dm_bytevec::size_type			dm_size_t;

typedef std::basic_string< dm_str_t >	dm_string_t;
typedef std::vector< dm_string_t >		dm_stringvec_t;
typedef std::list< dm_string_t >		dm_stringlist_t;

typedef std::basic_string< dm_str >		dm_string;
typedef std::vector< dm_string >		dm_stringvec;
typedef std::list< dm_string >			dm_stringlist;

#define _DM_USE_COMPRESSZIP_

#include "DmCSSupport.h"
#include "DmCSWrite.h"
#include "DmCSRead.h"
#include "DmCSMeasure.h"

#include "DmCompressZip.h"
#include "DmStringfn.h"
#include "DmFileFn.h"
#include "SHA256_Fn.h"
#include "DmTableData.h"
#include "DmSecuritySelf.h"
#include "DmStorageFile.h"

#include "Zipwrapper/utils.hpp"
#include "Zipwrapper/Zipper.h"
#include "Zipwrapper/UnZipper.h"

#include "Zipwrapper/compress.hpp"
#include "Zipwrapper/decompress.hpp"

#endif//DM_SYSTEM_PCH_H
