//---------------------------------------------------------------------------
//
// 파일명 : InstMgr.inl
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
CInstMgr<KeyType, DataType>::CInstMgr()
:m_map_Inst()
,m_map_Iter()
{
	m_map_Inst.clear();
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
CInstMgr<KeyType, DataType>::~CInstMgr()
{
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
void CInstMgr<KeyType, DataType>::AddInst(KeyType p_ID, DataType kData)
{
	// add
	if(!GetInst(p_ID))
	{
		STL_MAP_INST_VT vt(p_ID, kData);
		m_map_Inst.insert(vt);
	}
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
DataType CInstMgr<KeyType, DataType>::GetInst(KeyType p_ID)
{
	// find
	STL_MAP_INST_IT it = m_map_Inst.find(p_ID);
	if(it == m_map_Inst.end()) 
		return NULL;

	return (*it).second;
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
DataType CInstMgr<KeyType, DataType>::DetachInst(KeyType p_ID)
{
	// find
	STL_MAP_INST_IT it = m_map_Inst.find(p_ID);
	if(it == m_map_Inst.end()) 
		return NULL;

	DataType data = (*it).second;

	// Erase
	m_map_Inst.erase(it);

	return data;
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
void CInstMgr<KeyType, DataType>::Receive(CInstMgr& mgr)
{
	STL_MAP_INST_IT it = mgr.m_map_Inst.begin();
	while( it != mgr.m_map_Inst.end() )
	{
		STL_MAP_INST_VT vt( it->first, it->second );
		m_map_Inst.insert( vt );
		++it;
	}

	mgr.AllDetachInst();
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
void CInstMgr<KeyType, DataType>::AllDetachInst()
{
	m_map_Inst.clear();
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
BOOL CInstMgr<KeyType, DataType>::DelInst(KeyType p_ID)
{
	// find
	STL_MAP_INST_IT it = m_map_Inst.find(p_ID);
	if(it == m_map_Inst.end()) 
		return FALSE;

	// del
	SAFE_NIDELETE((*it).second);
	
	m_map_Inst.erase(it);

	return TRUE;
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
void CInstMgr<KeyType, DataType>::DelBegin(void)
{
	STL_MAP_INST_IT it = m_map_Inst.begin();
	if( it != m_map_Inst.end() )
	{
		SAFE_NIDELETE( it->second );
		m_map_Inst.erase( it );
	}
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
DataType CInstMgr<KeyType, DataType>::DetachBegin(void)
{
	DataType Data = 0;
	STL_MAP_INST_IT it = m_map_Inst.begin();
	if( it != m_map_Inst.end() )
	{
		Data = it->second;
		m_map_Inst.erase( it );
	}
	return Data;
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
void CInstMgr<KeyType, DataType>::AllDelInst()
{
	STL_MAP_INST_IT it;

	for( it  = m_map_Inst.begin(); 
		 it != m_map_Inst.end(); 
		 ++it )
	{	
		// del
		SAFE_NIDELETE((*it).second);
	}

	m_map_Inst.clear();
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
int CInstMgr<KeyType, DataType>::GetSize() const
{
	return static_cast<int>(m_map_Inst.size());
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
BOOL CInstMgr<KeyType, DataType>::IsEmpty() const
{
	return m_map_Inst.empty();
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
void CInstMgr<KeyType, DataType>::GotoBegin()
{
	m_map_Iter = m_map_Inst.begin();
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
BOOL CInstMgr<KeyType, DataType>::GotoNext()
{
	m_map_Iter++;

	return IsValid();
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
BOOL CInstMgr<KeyType, DataType>::GotoNext(int p_iPos)
{
	m_map_Iter += p_iPos;

	return IsValid();
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
KeyType  CInstMgr<KeyType, DataType>::GetCurKey() const
{
	return (*m_map_Iter).first; 
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
DataType CInstMgr<KeyType, DataType>::GetCurInst() const
{
	return (*m_map_Iter).second; 
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
BOOL CInstMgr<KeyType, DataType>::IsValid() const
{
	return (m_map_Iter != m_map_Inst.end()) ? TRUE : FALSE;
}
//---------------------------------------------------------------------------
template <class KeyType, class DataType> 
std::map<KeyType, DataType> &CInstMgr<KeyType, DataType>::GetMap() 
{
	return m_map_Inst;
}
//---------------------------------------------------------------------------