
#pragma once

BEGIN_NAME_TABLE

class CAchieveTable : public IDataTableEvent
{
public:
	typedef std::map<DWORD, sAchieveData>	MAP_ACHIEVE_DATA;
	typedef std::map<int,sAchieveTitle>		MAP_ACHIEVE_TITLE;

public:
	CAchieveTable();
	~CAchieveTable();

	virtual void				InitTable(int const& nServiceType) override;
	virtual void				ReleaseTable() override;
	bool						IsAchieve( DWORD const& dwQuestID ) const;
	sAchieveData const*			GetAchieve( DWORD const& dwQuestID ) const;
	MAP_ACHIEVE_TITLE const&	GetTitleData() const;
	MAP_ACHIEVE_DATA const&		GetAchieveData() const;

private:
	bool						LoadData(DataStream& pReader);
	bool						InsertString(ITbImpl const* pStringData);


private:
	MAP_ACHIEVE_DATA			m_kAchieveData;
	MAP_ACHIEVE_TITLE			m_kAchieveTitle;
};

END_NAME_TABLE