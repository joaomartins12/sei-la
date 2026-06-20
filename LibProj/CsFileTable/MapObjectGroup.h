#pragma once 

class CsMapObjectGroup
{
public:
	typedef std::list<CsMapObject*>					LIST_OBJECT;
	typedef std::list<CsMapObject*>::iterator		LIST_OBJECT_IT;
	typedef std::list<CsMapObject*>::const_iterator	LIST_OBJECT_CIT;

	typedef std::map<DWORD/* ObjectId */, LIST_OBJECT >					OBJECT_MAP;
	typedef std::map<DWORD/* ObjectId */, LIST_OBJECT >::iterator			OBJECT_MAP_IT;
	typedef std::map<DWORD/* ObjectId */, LIST_OBJECT >::const_iterator	OBJECT_MAP_CIT;

	
	
public:
	void				Init();
	void				Delete();

	OBJECT_MAP const&	GetMapObject() const;

	bool				IsObject( DWORD const& dwObjecIdx ) const;
	LIST_OBJECT const*	FindObjectList( DWORD const& dwObjecIdx ) const;
	LIST_OBJECT *		AddNewObject(DWORD const& dwObjecIdx);
	void				GetActorObjectID( std::list<DWORD>& findObjID ) const;
	void				DeleteObjectFormIdx( DWORD const& dwObjecIdx );
	void				DeleteActorFactor( DWORD const& dwObjecIdx, BYTE const& byFactor1, BYTE const& byFactor2 );
	void				DeleteActorFactor( DWORD const& dwObjecIdx, BYTE const& byFactor1 );
	int					GetMaxFactorCount() const; 

private:
	OBJECT_MAP			m_mapObject;
};
