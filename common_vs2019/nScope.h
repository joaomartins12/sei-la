

#pragma once


namespace nScope
{
	enum
	{
		User		=  100,
		Log			=  300,
		Core		=  700,
		Gate		=  900,
		Game		= 1000,

		Skill		= 1100,
		Inven		= 1200,
		// 20151015 Quest와 프로토콜 번호가 겹쳐서 Quest를 11000대로 이동
		Tamer		= 1300,		

		Msg			= 1400,
		Trade		= 1500,
		Monster		= 1600,

		Svr			= 1700,
		Net			= 1900,
		Guild		= 2100,

		Scanner		= 2200,
		Party		= 2300,
		Buddy		= 2400,

		Debug		= 3000,
		Event		= 3100,
		Digimon		= 3200,

		Login		= 3300,
		CashShop	= 3400,
		Item		= 3900,
		Buff		= 4000,

		InsMap		= 4100,
		MiniEvent	= 5000,
		Manager		= 9700,

		Pass2		= 9800,
		Shutdown	= 9850,
		Master		= 9900,
		Certify		= 10000,

		// 20151015
		Quest		= 11000,

		GameTwo		= 16000,

		LoadData	= 20000,

		// 20190115
		ManagerTwo	= 25000,	//매니저 관련 패킷 부족으로 추가

		// 20170621
		Duel		= 30000,			// 대결(PVP) 새롭게 추가

		// 20170807
		Relocate	= 50000,
	};
};



