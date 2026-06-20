#pragma once

#ifndef DM_WRITE_H
#define DM_WRITE_H

#include "DmCSSupport.h"

namespace DmCS
{
	struct Write
	{
		template< bool CUSTOM, bool POINTER >
		struct Select
		{
			template< typename T >
			static bool exec(T conref iv, dm_bytevec& datavec, dm_size_t& offset)
			{
				dm_size_t remain = datavec.size() - offset;
				dm_size_t tsize = sizeof( T );
				if( remain < tsize )
					return false;

				::memcpy_s( &datavec.at( offset ), remain, &iv, tsize );
				offset += tsize;
				return true;
			}
		};

		template< bool CUSTOM >
		struct Select< CUSTOM, true >
		{
			template< typename T >
			static bool exec(T cptr piv, dm_bytevec& datavec, dm_size_t& offset)
			{
				return Write::exec( *piv, datavec, offset );
			}
		};

		template<>
		struct Select< true, false >
		{
			template< typename T >
			static bool exec(T conref siv, dm_bytevec& datavec, dm_size_t& offset)
			{
				return siv.Write( datavec, offset );
			}
		};


		template< typename T >
		static bool exec(T conref iv, dm_bytevec& datavec, dm_size_t& offset)
		{
			struct Checked { enum { custom = DmCSCheckCustom( T ), ptr = DmCSCheckPtr( T ) }; };
			return Select< Checked::custom, Checked::ptr >::exec( iv, datavec, offset );
		}

		template< typename T, dm_dword_t SIZE >
		static bool exec(T (&aiv)[ SIZE ], dm_bytevec& datavec, dm_size_t& offset)
		{
			if( !exec( SIZE, datavec, offset ) )
				return false;

			for( dm_dword_t i = 0; i < SIZE; ++i )
			{
				if( !exec( aiv[ i ], datavec, offset ) )
					return false;
			}

			return true;
		}

		template< typename T >
		static bool exec(std::basic_string< T > conref iv_str, dm_bytevec& datavec, dm_size_t& offset)
		{
			if( !exec( iv_str.size(), datavec, offset ) )
				return false;

			dm_size_t remain = datavec.size() - offset;
			dm_size_t tsize = sizeof( T ) * iv_str.size();

			if( remain < tsize )
				return false;

			::memcpy_s( &datavec.at( offset ), remain, iv_str.c_str(), tsize );
			offset += tsize;
			return true;
		}

		template< typename T >
		static bool exec(std::vector< T > conref iv_vec, dm_bytevec& datavec, dm_size_t& offset)
		{
			typedef std::vector< T >	ew_invec;

			if( !exec( iv_vec.size(), datavec, offset ) )
				return false;

			ew_invec::const_iterator it = iv_vec.begin();
			while( it != iv_vec.end() )
			{
				T conref iv = *it;
				++it;

				if( !exec( iv, datavec, offset ) )
					return false;
			}

			return true;
		}

		template< typename T >
		static bool exec(std::list< T > conref iv_list, dm_bytevec& datavec, dm_size_t& offset)
		{
			typedef std::list< T >	ew_inlist;

			if( !exec( iv_list.size(), datavec, offset ) )
				return false;

			ew_inlist::const_iterator it = iv_list.begin();
			while( it != iv_list.end() )
			{
				T conref iv = *it;
				++it;

				if( !exec( iv, datavec, offset ) )
					return false;
			}

			return true;
		}

		template< typename T >
		static bool exec(std::set< T > conref iv_set, dm_bytevec& datavec, dm_size_t& offset)
		{
			typedef std::set< T >	ew_inset;

			if( !exec( iv_set.size(), datavec, offset ) )
				return false;

			ew_inset::const_iterator it = iv_set.begin();
			while( it != iv_set.end() )
			{
				T conref iv = *it;
				++it;

				if( !exec( iv, datavec, offset ) )
					return false;
			}

			return true;
		}

		template< typename T, typename U, typename less, typename alloc >
		static bool exec(std::map< T, U, less, alloc > conref iv_map, dm_bytevec& datavec, dm_size_t& offset)
		{
			typedef std::map< T, U, less, alloc >	ew_inmap;

			if( !exec( iv_map.size(), datavec, offset ) )
				return false;

			ew_inmap::const_iterator it = iv_map.begin();
			while( it != iv_map.end() )
			{
				T conref ivk = it->first;
				U conref ivm = it->second;
				++it;

				if( !exec( ivk, datavec, offset ) )
					return false;

				if( !exec( ivm, datavec, offset ) )
					return false;
			}

			return true;
		}
	};
}

#endif//DM_WRITE_H