#pragma once

BEGIN_NAME_TABLE
struct sItemExchange
{		
	sItemExchange():s_dwNpcID(0),s_dwItemIndex(0),s_dwItemID(0),s_dwExchange_Code_A(0),s_dwExchange_Code_B(0),s_dwCount(0)
		,s_dwExchange_Code_C(0),s_dwExchange_Code_D(0),s_dwPropertyA_Price(0),s_dwPropertyB_Price(0),s_dwPropertyC_Price(0),s_dwPropertyD_Price(0)
	{}

	DWORD		s_dwNpcID;
	USHORT		s_dwItemIndex;
	DWORD		s_dwItemID;

	int			s_dwExchange_Code_A;
	int			s_dwExchange_Code_B;
	int			s_dwExchange_Code_C;
	int			s_dwExchange_Code_D;
	
	USHORT		s_dwPropertyA_Price;
	USHORT		s_dwPropertyB_Price;
	USHORT		s_dwPropertyC_Price;
	USHORT		s_dwPropertyD_Price;

	USHORT		s_dwCount;
};
END_NAME_TABLE