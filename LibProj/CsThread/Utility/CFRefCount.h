/********************************************************************
	created:	2006/05/11
	created:	11:5:2006   18:58
	filename: 	d:\work\Kuf-cod\Network\CFMTLib\Utility\CFRefCount.h
	file path:	d:\work\Kuf-cod\Network\CFMTLib\Utility
	file base:	CFRefCount
	file ext:	h
	author:		celes
	
	Reference Counting을 위한 인터페이스와 구현.

	- CFRefCount와 CFMRefCount를 상속받는 class는 new operator로만 Instance를 만들 수 있음.
*********************************************************************/

#pragma once

template <class T>
class IFRefCount
{
public:
	virtual void AddRef() = 0;
	virtual void Release() = 0;

protected:
	virtual T* GetPtr() const = 0;
};

template <class T>
class CFRefCount : public IFRefCount<T>
{
public:
	void AddRef(){
		m_lCount ++;
	}

	void Release(){
		m_lCount --;
		if( m_lCount == 0 ){
			Destroy();
		}
	}

protected:
	CFRefCount() : m_lCount( 0 )
	{
	}

	T* GetPtr() const{ return static_cast<T*>( this ); }
	void Destroy() { if( GetPtr() != NULL ) delete GetPtr(); }
    
private:
	LONG m_lCount;
};


template <class T>
class CFMRefCount : public IFRefCount<T>
{
public:
	void AddRef(){
		::InterlockedIncrement( &m_lCount );
	}
	void Release(){
		if( ::InterlockedDecrement( &m_lCount ) == 0 ){
			Destroy();
		}
	}

protected:
	CFMRefCount() : m_lCount( 0 )
	{
	}

	T* GetPtr() const{ return (T*)this; }
	void Destroy() { if( GetPtr() != NULL ) delete GetPtr(); }

private:
	volatile LONG m_lCount;
};