#pragma once

#ifndef DM_SUPPORT_H
#define DM_SUPPORT_H


namespace DmCS
{
	struct Custom
	{
	public:
		virtual bool Read(dm_bytevec conref datavec, dm_size_t& offset)
		{ 
			return false; 
		}
		
		virtual bool Write(dm_bytevec& datavec, dm_size_t& offset) const
		{ 
			return false; 
		}
		
		virtual dm_size_t Size(void) const
		{
			return 0;
		}
	};

	template< typename T >
	struct PointerTraits
	{
		enum
		{
			rst = false
		};

		typedef T type;
		typedef T* pointer_type;
	};

	template< typename T >
	struct PointerTraits< T* >
	{
		enum
		{
			rst = true
		};

		typedef T type;
		typedef T* pointer_type;
	};

	template< class T, class U >
	class Conversion
	{
		typedef char Small;

		class Big
		{
			char dummy[ 2 ];
		};

		static Small	Test(T);
		static Big		Test(...);
		static U		MakeU();
	public:
		enum 
		{ 
			able = sizeof( Test(MakeU()) ) == sizeof( Small ),
			same = false,
		};
	};

	template< class T >
	class Conversion< T, T >
	{
	public:
		enum { able = 1, same = 1 };
	};
}

#define DmCSSuperSubclass(super, sub)\
	( DmCS::Conversion< super cptr, sub cptr >::able && !DmCS::Conversion< void cptr, super cptr >::same )

#define DmCSSuperSubclass_Strict(super, sub)\
	( DmCSSuperSubclass( super, sub ) && !DmCS::Conversion< super const, sub const >::same )

#define DmCSCheckPtr(x)\
	DmCS::PointerTraits< x >::rst

#define DmCSCheckCustom(x)\
	DmCSSuperSubclass( DmCS::Custom, DmCS::PointerTraits< x >::type )



#define DmCSCustomVFnDecl()\
	virtual bool Read(dm_bytevec conref datavec, dm_size_t& offset);\
	virtual bool Write(dm_bytevec& datavec, dm_size_t& offset) const;\
	virtual dm_size_t Size(void) const;


#endif//DM_SUPPORT_H
