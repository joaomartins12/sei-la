
#pragma once 


struct sNPC_EXTRA_BASE
{
	nsCsNpcTable::eNPC_EXTRA	s_eNpcExtra;
	virtual void				Delete() = 0;
	virtual void				Init() = 0;
};

struct sNPC_EXTRA_QUEST : public sNPC_EXTRA_BASE
{
	enum eSTATE{ NEQ_HIDE, NEQ_SHOW };
	enum eACT_TYPE{ NEA_QUEST_COMP, NEA_QUEST_PROCCOMP, };
	sNPC_EXTRA_QUEST(){ s_eNpcExtra = nsCsNpcTable::NE_QUEST; s_eInitState = NEQ_SHOW; }

	struct sACT
	{
		sACT(){}
		void	Delete();

		eACT_TYPE				s_eActType;				
		eSTATE					s_eCompState;		// 완료 되면 변경시킬 상태 변화
		std::list< int >		s_list;
	};

	eSTATE					s_eInitState;
	CsVectorPB< sACT* >		s_vpAct;

	virtual void				Delete();
	virtual void				Init();

	CsVectorPB< sACT* >*		GetVector(){ return &s_vpAct; }
};
