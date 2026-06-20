


#include "pType.h"

#include "nlib/cLock.h"


cLock g_Lock_Type_IDX;
static uint s_nNextIDX[nClass::End] = {0, }; // m_nClass의 표현 가능 수

cType::cType()
{
}
cType::cType(uint uid) : m_nUID(uid)
{
}
cType::~cType(void)
{
}

void cType::SetClass(uint nClass)
{
	xassert(nClass<nClass::End, "nClass is very large");

	g_Lock_Type_IDX.lock();
		m_nClass = nClass;
		m_nIDX   = ++ s_nNextIDX[nClass];
	g_Lock_Type_IDX.unlock();

	xassert(m_nIDX<4000, "m_nIDX is very large");
}
//u4 cType::GetType(void) const
//{ 
//	return m_nType;
//} // nType 참조
//u4 cType::GetClass(void)
//{ 
//	return m_nClass;
//} // nClass 참조
//u2 cType::GetUID(void)   
//{ 
//	return m_nUID;  
//} // unique id	(nClass 값 포함됨)
//u2 cType::GetIDX(void)  
//{ 
//	return m_nIDX;
//} // nClass 내에서 unique idx

void	cType::Reset()	  
{
	m_nTypeAll = 0;
}
void	cType::SetTypeAll(u4 TypeAll)
{ 
	m_nTypeAll = TypeAll;
}
bool cType::IsDigimon(void)
{
	return m_nClass == nClass::Digimon;
}
bool cType::IsTamer(void)
{ 
	return m_nClass == nClass::Tamer;
}
bool cType::IsItem(void)   
{ 
	return m_nClass == nClass::Item;
}
bool cType::IsMonster(void)
{ 
	return m_nClass == nClass::Monster;
}
u4 cType::GetClass(u2 nUID) 
{ 
	return nUID >> 14;
}

u4 cType::GetIDX(u4 nUID)
{ 
	return nUID & 0x0FFF;
}
u4 cType::GetTestIDX( uint nClass )
{		
	xassert(nClass<nClass::End, "nClass is very large");
	//static u2 s_nNextIDX2[nClass::End] = {0, }; // m_nClass+m_nType의 표현 가능 수

	cType type;
	type.m_nClass = nClass;
	type.m_nIDX = ++s_nNextIDX[ type.m_nClass ];
	xassert(type.m_nIDX<4000, "m_nIDX is very large");
	return type.m_nIDX;		
}

uint cType::GetMinUID(uint nClass)
{
	xassert(nClass, "nClass is null");
	xassert(nClass<nClass::End, "nClass is not valid");

	uint nMinUID = (nClass << 14) + 1;
	return nMinUID;
}

uint cType::GetMaxUID(uint nClass)
{
	xassert(nClass, "nClass is null");
	xassert(nClass<nClass::End, "nClass is not valid");

	g_Lock_Type_IDX.lock();
	uint nMaxUID = (nClass << 14) | s_nNextIDX[nClass] + 1;
	g_Lock_Type_IDX.unlock();

	return nMaxUID;
}

void cType::SetIDX(uint nIDX) 
{ 
	m_nIDX = nIDX;
}		// 특수한 경우가 아니라면 사용하면 안됨

void cType::SetIDX( u4 nIDX, u4 nClass ) 
{ 
	m_nIDX = nIDX; m_nClass = nClass;
}	// 던젼에서 사용

void cType::SetType(uint nType) 
{ 
	m_nType = nType;
}

u4	 cType::GetTypeAll(void) 
{ 
	return m_nTypeAll;
}
