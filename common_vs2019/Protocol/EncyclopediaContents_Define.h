#pragma once

struct sEncyclopediaOpendedData
{
	sEncyclopediaOpendedData():nDigimonID(0),nLevel(0),nEnchant_AT(0),nEnchant_BL(0),nEnchant_CT(0),nEnchant_EV(0),nSize(0),bIsReward(0)
//#ifdef SERVER_KSW_DIGIMONBOOK_EXTEND_181116
		,nSlotOpened(0),nEnchant_HP(0)
//#endif
	{
//#ifndef SERVER_KSW_DIGIMONBOOK_EXTEND_181116
		memset( nIsOpen, 0, sizeof(u1)*9 );
//#endif
	}
	~sEncyclopediaOpendedData() {};

	u4 nDigimonID;		// 1. 계열체 디지몬 번호
	u2 nLevel;			// 2. 계열체 레벨

//#ifdef SERVER_KSW_DIGIMONBOOK_EXTEND_181116
	u8 nSlotOpened;		// 3. 디지몬 오픈 여부
//#else
	u1 nIsOpen[9];	// 3. 디지몬 오픈 여부
//#endif

	u2 nEnchant_AT;		// 4. 강화 :: 공격력
	u2 nEnchant_BL;		// 5. 강화 :: 블럭
	u2 nEnchant_CT;		// 6. 강화 :: 크리티컬
	u2 nEnchant_EV;		// 7. 강화 :: 회피
	u2 nEnchant_HP;		// 7. 강화 :: HP
	n2 nSize;			// 8. 계열체 사이즈
	u1 bIsReward;		// 9. 보상여부, 0: 아직 수령 안함, 1: 아이템 획득했음	
};