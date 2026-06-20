
#pragma once 

class CsMapObjectMng
{
public:
	typedef	std::map< DWORD/*Map ID*/, CsMapObjectGroup* >					GROUP_MAP;
	typedef	std::map< DWORD/*Map ID*/, CsMapObjectGroup* >::iterator		GROUP_MAP_IT;
	typedef	std::map< DWORD/*Map ID*/, CsMapObjectGroup* >::const_iterator	GROUP_MAP_CIT;

	void			Delete();
	bool			Init( char* cPath, nsCsFileTable::eFILE_TYPE ft = nsCsFileTable::g_eFileType );

	void			Save();
	void			SaveBin( char* cPath );
	void			Reload( char* cPath );


	bool					IsGroup( DWORD const& dwMapID ) const;
	bool					IsGroup( DWORD const& dwMapID, DWORD const& dwObjectID ) const;
	CsMapObjectGroup const* GetGroup( DWORD const& dwMapID ) const;

	CsMapObjectGroup::LIST_OBJECT const* GetFactorList( DWORD const& dwMapID, DWORD const& dwObjId ) const;

	void			DeleteGroup( DWORD const& dwMapID );
	void			DeleteActorObject( DWORD const& dwMapID, DWORD const& dwObjId );
	void			DeleteActorFactor1( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1 );
	void			DeleteActorFactor2( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, BYTE const& byFactor2 );

	void			AddActorOrFactor( DWORD const& dwMapID, DWORD const& dwObjId, CsMapObject::sFACTOR_INFO addInfo );
	void			AddActorAndFactor( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, CsMapObject::sFACTOR_INFO addInfo );

	void			ChangeActorOrFactor( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, CsMapObject::sFACTOR_INFO addInfo );
	void			ChangeActorAndFactor( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, BYTE const& byFactor2, CsMapObject::sFACTOR_INFO addInfo );

	void			ChangeOrderNumber( DWORD const& dwMapID, DWORD const& dwObjId, int const& nOrderNum );

	CsMapObject::sFACTOR_INFO const* GetFactorInfo( DWORD const& dwMapID, DWORD const& dwObjId, BYTE const& byFactor1, BYTE const& byFactor2 ) const;
	
	

protected:
	void			_LoadExcel();
	void			_SaveExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

private:
	int				GetMaxColCount() const;

private:
	GROUP_MAP		m_mapMapObjectGroup;
};
