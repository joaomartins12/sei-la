#pragma once
#define dfSKILLLEVELSTEP 5
#define dfSKILLOPENEVO 10


struct stDigimonSkillOpenLevel
{
	stDigimonSkillOpenLevel():nEvoStep(0) , nStartLevel(0)
	{
		ZeroMemory(nMaxSkillLevel , sizeof(nMaxSkillLevel));
	};
	u1 nEvoStep;
	u2 nStartLevel;
	u1 nMaxSkillLevel[dfSKILLLEVELSTEP + 1];	//0번째는 기본레벨 MAX
};

struct stDigimonSkillOpenSubType
{
	stDigimonSkillOpenSubType():nItemSubtype(0), nItemOpenStep(0)
	{
		ZeroMemory(nEvoLevel , sizeof(nEvoLevel));
	};
	u4 nItemSubtype;
	u1 nItemOpenStep;
	u1 nEvoLevel[dfSKILLOPENEVO];
	bool HasEvoLevel(u1 nEvoStep)
	{
		if(nEvoStep == 0)
		{
			return false;
		}
		for(int i = 0 ; i < dfSKILLOPENEVO ; i++)
		{
			if(nEvoStep == nEvoLevel[i])
			{
				return true;
			}				
		}
		return false;
	}
};