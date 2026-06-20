#pragma once

#ifndef DM_READ_H
#define DM_READ_H

#include "DmCSSupport.h"

namespace DmCS
{
	struct Read
	{
		template< bool CUSTOM, bool POINTER >
		struct Select
		{
			template< typename T >
			static bool exec(T& iv, dm_bytevec conref datavec, dm_size_t& offset)
			{
				dm_size_t remain = datavec.size() - offset;
				dm_size_t tsize = sizeof( T );
				if( remain < tsize )
					return false;

				::memcpy_s( &iv, remain, &datavec.at( offset ), tsize );
				offset += tsize;
				return true;
			}
		};

		template< bool CUSTOM >
		struct Select< CUSTOM, true >
		{
			template< typename T >
			static bool exec(T*& piv, dm_bytevec conref datavec, dm_size_t& offset)
			{
				if( 0 == piv )
					piv = new T();

				return Read::exec( *piv, datavec, offset );
			}
		};

		template<>
		struct Select< true, false >
		{
			template< typename T >
			static bool exec(T& siv, dm_bytevec conref datavec, dm_size_t& offset)
			{
				return siv.Read( datavec, offset );
			}
		};


		template< typename T >
		static bool exec(T& iv, dm_bytevec conref datavec, dm_size_t& offset)
		{
			struct Checked { enum { custom = DmCSCheckCustom( T ), ptr = DmCSCheckPtr( T ) }; };
			return Select< Checked::custom, Checked::ptr >::exec( iv, datavec, offset );
		}

		template< typename T, dm_dword_t SIZE >
		static bool exec(T (&aiv)[ SIZE ], dm_bytevec conref datavec, dm_size_t& offset)
		{
			dm_size_t size = 0;
			if( !exec( size, datavec, offset ) )
				return false;

			if( size != SIZE )
				return false;

			for( dm_dword_t i = 0; i < SIZE; ++i )
			{
				if( !exec( aiv[ i ], datavec, offset ) )
					return false;
			}

			return true;
		}

		template< typename T >
		static bool exec(std::basic_string< T >& iv_str, dm_bytevec conref datavec, dm_size_t& offset)
		{
			dm_size_t size = 0;
			if( !exec( size, datavec, offset ) )
				return false;

			dm_size_t remain = datavec.size() - offset;
			dm_size_t tsize = sizeof( T ) * size;
			if( remain < tsize )
				return false;

			iv_str.resize( size );
			::memcpy_s( &iv_str.at( 0 ), tsize, &datavec.at( offset ), tsize );
			offset += tsize;
			return true;
		}

		template< typename T >
		static bool exec(std::vector< T >& iv_vec, dm_bytevec conref datavec, dm_size_t& offset)
		{
			typedef std::vector< T >	ew_invec;

			dm_size_t size = 0;
			if( !exec( size, datavec, offset ) )
				return false;

			for( dm_size_t i = 0; i < size; ++i )
			{
				T iv = T();
				if( !exec( iv, datavec, offset ) )
					return false;

				iv_vec.push_back( iv );
			}

			return true;
		}

		template< typename T >
		static bool exec(std::list< T >& iv_list, dm_bytevec conref datavec, dm_size_t& offset)
		{
			typedef std::list< T >	ew_inlist;

			dm_size_t size = 0;
			if( !exec( size, datavec, offset ) )
				return false;

			for( dm_size_t i = 0; i < size; ++i )
			{
				T iv = T();
				if( !exec( iv, datavec, offset ) )
					return false;

				iv_list.push_back( iv );
			}

			return true;
		}

		template< typename T >
		static bool exec(std::set< T >& iv_set, dm_bytevec conref datavec, dm_size_t& offset)
		{
			typedef std::set< T >	ew_inset;

			dm_size_t size = 0;
			if( !exec( size, datavec, offset ) )
				return false;

			for( dm_size_t i = 0; i < size; ++i )
			{
				T iv = T();
				if( !exec( iv, datavec, offset ) )
					return false;

				iv_set.insert( iv );
			}

			return true;
		}

		template< typename T, typename U, typename less, typename alloc >
		static bool exec(std::map< T, U, less, alloc >& iv_map, dm_bytevec conref datavec, dm_size_t& offset)
		{
			typedef std::map< T, U, less, alloc >	dm_inmap;

			dm_size_t size = 0;
			if( !exec( size, datavec, offset ) )
				return false;

			for( dm_size_t i = 0; i < size; ++i )
			{
				T ivk = T();
				U ivm = U();

				if( !exec( ivk, datavec, offset ) )
					return false;

				if( !exec( ivm, datavec, offset ) )
					return false;

				iv_map.insert( dm_inmap::value_type( ivk, ivm ) );
			}

			return true;
		}
	};
}

#endif//DM_READ_H