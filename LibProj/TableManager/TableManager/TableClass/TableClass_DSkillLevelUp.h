
#pragma once 

BEGIN_NAME_TABLE

class CDSkillLvUpTable : public IDataTableEvent
{
public:
	CDSkillLvUpTable();
	~CDSkillLvUpTable();

	typedef std::map<USHORT, sEvoMaxkSkillLv>	ItemMaxLvCont;
	typedef std::map<UINT, sExpansionCondition>	ItemLvOpenCont;

	
	virtual void		InitTable(int const& nServiceType) override;
	virtual void		ReleaseTable() override;

private:
	bool				_LoadData_SkillLvStart(DataStream& pReader);
	bool				_LoadData_SkillLvOpen(DataStream& pReader);


private:
	ItemMaxLvCont		m_MaxLvCont;
	ItemLvOpenCont		m_LvOpenCont;
};

END_NAME_TABLE