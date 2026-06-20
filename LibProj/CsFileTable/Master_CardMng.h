#pragma once 

class CsMaster_CardMng
{
public:
	void			Delete();
	bool			Init( char* cPath );
	void			SaveBin( char* cPath );	
	void			SaveBinFilter(FILE* fp);
	void			LoadBinFilter(FILE* fp);

private:
	void			_LoadExcel();
	void			_LoadExcelLeader();
	void			_LoadExcelLeaderAbility();
	void			_LoadExcelPath();
	void			_LoadExcelFilter();

	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );
	void			_LoadFilePackFilter(int nHandle);

public:
	bool								IsMasterCard( int nID );
	bool								IsMasterCardbyMonsterID( int nID );
	CsMaster_Card*						GetMasterCard( int nID );
	CsMaster_Card*						GetMasterCardbyMonsterID( int nID );
	CsMaster_Card::MAP*					GetMasterCardMap();

	bool								IsMasterCardLeader( int nID );
	CsMaster_CardLeader*				GetMasterCardLeader( int nID );
	CsMaster_CardLeader::MAP*			GetMasterCardLeaderMap();

	bool								IsMasterCardLeaderAbility( int nID );
	CsMaster_CardLeaderAbility*			GetMasterCardLeaderAbility( int nID );
	CsMaster_CardLeaderAbility::MAP*	GetMasterCardLeaderAbilityMap();

// 	bool								IsMasterCardPathDigimon( int nID ){ return ( m_mapMaster_PathDigimon.find( nID ) != m_mapMaster_PathDigimon.end() ); }
// 	CsMaster_Path_Digimon*				GetMasterCardPathDigimon( int nID );
// 	CsMaster_Path_Digimon::MAP*			GetMasterCardPathDigimonMap(){return &m_mapMaster_PathDigimon;}
// 
// 	bool								IsMasterCardPathPlate( int nID ){ return ( m_mapMaster_PathPlate.find( nID ) != m_mapMaster_PathPlate.end() ); }
// 	CsMaster_Path_Plate*				GetMasterCardPathPlate( int nID );
// 	CsMaster_Path_Plate::MAP*			GetMasterCardPathPlateMap(){return &m_mapMaster_PathPlate;}

	bool								IsMasterFilter( DWORD mapGroupID, DWORD mapID, DWORD sealID );
	CsMaster_Filter::sINFO*				GetMasterFilter( DWORD mapGroupID, DWORD mapID, DWORD sealID );
	CsMaster_Filter::MapGroup*			GetMasterFilterMap();
	bool								InsertMasterFilter( CsMaster_Filter::sINFO* pPath );
	int									GetMasterFilterSize();


	std::string							GetSealDigimonImgPath(DWORD const& dwDigimonID) const;
	std::string							GetSealGridNifFile(int const& nGridType) const;
	std::string							GetSealGridBackFile(int const& nGridType) const;
	std::string							GetSealElementalImgPath(int const& nElType) const;
	std::string							GetSealAttributeImgPath(int const& nAttriType) const;

private:
	CsMaster_Card::MAP					m_mapMaster_Card;
	CsMaster_CardLeader::MAP			m_mapMaster_CardLeader;
	CsMaster_CardLeaderAbility::MAP		m_mapMaster_CardLeaderAbility;
	CsMaster_Filter::MapGroup			m_mapMaster_Filter;

	MAP_SEALMASTER_DIGIIMGPATH			m_mapMaster_PathDigimon;
	MAP_SEALMASTER_PLATHPATH			m_mapMaster_PathPlate;
	MAP_SEALMASTER_ELEMENTAL			m_mapMaster_PathElemental;
	MAP_SEALMASTER_ATTRIBUTE			m_mapMaster_PathAttribute;
};
