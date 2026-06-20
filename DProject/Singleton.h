//---------------------------------------------------------------------------
//
// 파일명 : Singleton.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
//#pragma once
#ifndef __SINGLETON_
#define __SINGLETON_

#include <assert.h>

//Alloc
#ifdef ALLOC_GB
#define GNew			NiNew
#define GDelete			NiDelete
#define GMemObject      NiMemObject
#else
class CNullObject
{};
#define GNew			new
#define GDelete			delete
#define GMemObject      CNullObject 
#endif

//---------------------------------------------------------------------------
template <class T> class CSingleton	: public GMemObject
{
public:
	CSingleton()
	{
		ms_pkSingleton = (T*)this;
	}
	~CSingleton()
	{
		assert(ms_pkSingleton); 
		ms_pkSingleton = 0;
	}

	static T& GetSingleton() 
	{
		assert(ms_pkSingleton);
		return (*ms_pkSingleton); 
	}

	static T* GetSingletonPtr()
	{ return (ms_pkSingleton); }

private:
	static T* ms_pkSingleton;
};
//---------------------------------------------------------------------------
template <class T> T* CSingleton <T>::ms_pkSingleton = 0;

//---------------------------------------------------------------------------

#define CREATE_SINGLETON(T)	 GNew T;
#define DESTROY_SINGLETON(T) GDelete T;
//---------------------------------------------------------------------------
#endif