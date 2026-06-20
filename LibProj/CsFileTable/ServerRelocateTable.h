
#pragma once 

struct sTransferLimit
{
	enum eLimitType
	{
		eLimit_Money = 1,
	};

	sTransferLimit():m_nLimitType(0),m_nLimitValue(0)
	{}
	~sTransferLimit() {}

	int		m_nLimitType;
	UINT64	m_nLimitValue;
};

typedef std::list<sTransferLimit>					LIST_TRANSFERLIMIT_INFO;
typedef std::list<sTransferLimit>::iterator			LIST_TRANSFERLIMIT_INFO_IT;
typedef std::list<sTransferLimit>::const_iterator	LIST_TRANSFERLIMIT_INFO_CIT;

struct sTransferInfo
{
	sTransferInfo() : m_bEnable(false)
	{}
	~sTransferInfo() {}

	bool						m_bEnable;
	std::wstring				m_serverName;
	LIST_TRANSFERLIMIT_INFO		m_LimitList;
};

typedef std::map<int/*Server IDX*/, sTransferInfo>					MAP_SERVERTRANSFER_INFO;
typedef std::map<int/*Server IDX*/, sTransferInfo>::iterator		MAP_SERVERTRANSFER_INFO_IT;
typedef std::map<int/*Server IDX*/, sTransferInfo>::const_iterator	MAP_SERVERTRANSFER_INFO_CIT;

class CsServerRelocateTBMng
{
public:
	CsServerRelocateTBMng();
	~CsServerRelocateTBMng();

	bool			Init( char* cPath );
	void			Delete();

protected:
	bool			_LoadExcel();
	bool			_LoadBin( char* cPath );
	bool			_LoadFilePack( char* cPath );

public:
	void			SaveBin( char* cPath );	

	MAP_SERVERTRANSFER_INFO const&	GetTransferInfo() const;
	UINT64			GetLimitValueFromType( int const& nType, int const& nServerIDX ) const;
	bool			IsServerRelocateEnable( int const& nServerIDX ) const;

private:
	MAP_SERVERTRANSFER_INFO	m_TransferInfo;
};