#pragma once

#ifndef DM_SYSTEM_GUARD_H
#define DM_SYSTEM_GUARD_H

namespace DmCS
{
	template< typename T, typename END_FN >
	class Guard
	{
	public:
		typedef T* vType;

		Guard(vType instance)
			: Instance( instance )
			, IsDelete( true )
		{}

		~Guard(void)
		{
			Delete();
		}

		bool operator!(void) const
		{
			return ( 0 == Instance );
		}

		operator bool(void) const
		{
			return !operator!();
		}

		vType operator->(void) const
		{
			return Instance;
		}

		vType GetPtr(void) const
		{
			return Instance;
		}

		void Dismiss(void)
		{
			IsDelete = false;
		}

		void Delete(void)
		{
			if( !IsDelete )
				return;

			END_FN::Exec( Instance );
			Instance = 0;
		}

	private:
		vType	Instance;
		bool	IsDelete;
	};
}

#endif//DM_SYSTEM_GUARD_H
