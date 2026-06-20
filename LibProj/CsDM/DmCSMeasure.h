#pragma once

#ifndef DM_MEASURE_H
#define DM_MEASURE_H

#include "DmCSSupport.h"

namespace DmCS
{
	struct Measure
	{
		template< bool CUSTOM, bool POINTER >
		struct Select
		{
			template< typename T >
			static dm_size_t exec(T conref iv)
			{
				return sizeof( T );
			}
		};

		template< bool CUSTOM >
		struct Select< CUSTOM, true >
		{
			template< typename T >
			static dm_size_t exec(T cptr piv)
			{
				return Measure::exec( *piv );
			}
		};

		template<>
		struct Select< true, false >
		{
			template< typename T >
			static dm_size_t exec(T conref siv)
			{
				return siv.Size();
			}
		};

		template< typename T >
		static dm_size_t exec(T conref iv)
		{
			struct Checked { enum { custom = DmCSCheckCustom( T ), ptr = DmCSCheckPtr( T ) }; };
			return Select< Checked::custom, Checked::ptr >::exec( iv );
		}

		template< typename T, dm_dword_t SIZE >
		static dm_size_t exec(T (&aiv)[ SIZE ])
		{
			dm_size_t size = sizeof( SIZE );
			for( dm_dword_t i = 0; i < SIZE; ++i )
			{
				size += exec( aiv[ i ] );
			}
			return size;
		}

		template< typename T >
		static dm_size_t exec(std::basic_string< T > conref iv_str)
		{
			return exec( iv_str.size() ) + ( sizeof( T ) * iv_str.size() );
		}

		template< typename T >
		static dm_size_t exec(std::vector< T > conref iv_vec)
		{
			typedef std::vector< T >	ew_invec;

			dm_size_t size = exec( iv_vec.size() );
			ew_invec::const_iterator it = iv_vec.begin();
			while( it != iv_vec.end() )
			{
				T conref iv = *it;
				++it;

				size += exec( iv );
			}

			return size;
		}

		template< typename T >
		static dm_size_t exec(std::list< T > conref iv_list)
		{
			typedef std::list< T >	ew_inlist;

			dm_size_t size = exec( iv_list.size() );
			ew_inlist::const_iterator it = iv_list.begin();
			while( it != iv_list.end() )
			{
				T conref iv = *it;
				++it;

				size += exec( iv );
			}

			return size;
		}

		template< typename T >
		static dm_size_t exec(std::set< T > conref iv_set)
		{
			typedef std::set< T >	ew_inset;

			dm_size_t size = exec( iv_set.size() );
			ew_inset::const_iterator it = iv_set.begin();
			while( it != iv_set.end() )
			{
				T conref iv = *it;
				++it;

				size += exec( iv );
			}

			return size;
		}

		template< typename T, typename U, typename less, typename alloc >
		static dm_size_t exec(std::map< T, U, less, alloc > conref iv_map)
		{
			typedef std::map< T, U, less, alloc > ew_inmap;

			dm_size_t size = exec( iv_map.size() );
			ew_inmap::const_iterator it = iv_map.begin();
			while( it != iv_map.end() )
			{
				T conref ivk = it->first;
				U conref ivm = it->second;
				++it;

				size += exec( ivk );
				size += exec( ivm );
			}

			return size;
		}
	};
}

#endif//ECLIPSE_WAR_MEASURE_H