
#pragma once 

//class cData_CoolTime : public NiMemObject
//{
//public:
//	enum eTYPE{
//		EVOLUTION				= 0,
//		ITEM_USESERIES_1,
//		ITEM_USESERIES_2,
//		ITEM_USESERIES_3,
//		ITEM_USESERIES_4,
//		ITEM_USESERIES_5,
//
//		MAX_COUNT,
//	};
//
//public:
//	void		Delete(){}
//	void		Init();
//	void		Update();
//
//protected:
//	CsCoolTimeSeq		m_CoolTime[ MAX_COUNT ];
//
//public:
//	bool		IsCoolTime( int type ){ assert_cs( type < MAX_COUNT ); return m_CoolTime[ type ].IsCoolTime(); }
//	float		GetEndTime( int type ){ assert_cs( type < MAX_COUNT ); return m_CoolTime[ type ].GetEndTime(); }
//	float		GetRemaime( int type ){ assert_cs( type < MAX_COUNT ); return m_CoolTime[ type ].GetRemainT(); }
//
//	void		SetCoolTime( int type );
//};