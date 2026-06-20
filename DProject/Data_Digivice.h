
#pragma once 

class cData_Digivice : public NiMemObject
{
public:
	enum eDIGIVICE_INDEX{ INVALIDE_DIGIVICE_INDEX = INT_MAX, };
public:
	cData_Digivice();

public:
	void	Delete(){}
	void	Init(){}
	void	Reset();

	void	InitNetOff();

protected:
	int				m_nChipsetCount;
	int				m_nTamerSkillCount;

	cItemInfo		m_Chipset[ nLimit::Chipset ];
	cItemInfo		m_TamerSkill[ nLimit::SkillSlot ];

public:
	void			SetTamerSkillCount( int nCount ){ m_nTamerSkillCount = nCount; }
	int				GetTamerSkillCount(){ return m_nTamerSkillCount; }
	void			SetChipsetCount( int nCount );
	int				GetChipsetCount(){ return m_nChipsetCount; }

	cItemInfo*		GetChipset( int nIndex ){ assert_cs( nIndex < nLimit::Chipset ); return &m_Chipset[ nIndex ]; }
	cItemInfo*		GetChipsetArray(){ return m_Chipset; }

	cItemInfo*		GetTamerSkill( int nIndex ){ assert_cs( nIndex < nLimit::SkillSlot ); return &m_TamerSkill[ nIndex ]; }
	cItemInfo*		GetTamerSkillArray(){ return m_TamerSkill; }

	cItemInfo*		GetChipset_TypeL( int nType_L );
	int				GetChipsetIndex_TypeLT( int nType_L, int nType_T );
	int				GetCount_Item_TypeL( int nType_L );

	bool			ItemRemove_NoServer( int nIndex, bool bCalQuest = false );
	
	bool			DoYouHaveJointProgressChipset();

public:
	void			ChipsetToInven( int nSrcIndex, int nInvenIndex );
	void			ChipsetToChipset( int nSrcIndex, int nDestIndex );

public:
	int				GetEmptyChipsetSlot();
};