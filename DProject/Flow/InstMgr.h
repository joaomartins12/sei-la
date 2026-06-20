//---------------------------------------------------------------------------
//
// 파일명 : InstMgr.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#pragma once

//#include <hash_map>
#include <map>

//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
class CInstMgr : public NiMemObject
{
public:
	CInstMgr();
	~CInstMgr();

public:
	void     AddInst(KeyType p_ID, DataType kData);
	DataType GetInst(KeyType p_ID);
	DataType DetachInst(KeyType p_ID);
	void	 Receive(CInstMgr& mgr);
	void     AllDetachInst();
	BOOL     DelInst(KeyType p_ID);
	void	 DelBegin(void);
	DataType DetachBegin(void);
	void     AllDelInst();

	int      GetSize() const;				  
	BOOL     IsEmpty() const;

	void     GotoBegin();
	BOOL     GotoNext();
	BOOL     GotoNext(int p_iPos);
	KeyType  GetCurKey() const;
	DataType GetCurInst() const;
	BOOL     IsValid() const;

	std::map<KeyType, DataType> &GetMap();

private:
	typedef          std::map<KeyType, DataType>             STL_MAP_INST;
	typedef typename std::map<KeyType, DataType>::iterator   STL_MAP_INST_IT;
	typedef typename std::map<KeyType, DataType>::value_type STL_MAP_INST_VT;

	STL_MAP_INST    m_map_Inst;
	STL_MAP_INST_IT m_map_Iter;

};

#include "InstMgr.inl"
//---------------------------------------------------------------------------