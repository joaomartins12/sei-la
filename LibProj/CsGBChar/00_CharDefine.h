
#pragma once


//========================================================================================================
//		일반 디파인
//========================================================================================================
#define		CHAR_NAME_LEN			16
#define		MAX_TEXTKEY_LEN			128

//========================================================================================================
//		Char Prop 
//========================================================================================================

enum ePROP{
					PR_ANI_NONE			= 0x00000000,
					PR_ANI_ENABLE		= 0x00000001,
					PR_ANI_LOOP			= 0x00000002,
					_PR_ANI_MASK		= 0x0000000f,

					PR_PATH_NONE		= 0x00000000,
					PR_PATH_NORMAL		= 0x00000010,
					PR_PATH_USER		= 0x00000020,
					_PR_PATH_MASK		= 0x000000f0,

					PR_ATTACK_NONE		= 0x00000000,
					PR_ATTACK_ENABLE	= 0x00000100,
					_PR_ATTACK_MASK		= 0x00000f00,

					PR_EFFECT_NONE		= 0x00000000,
					PR_EFFECT_ENABLE	= 0x00001000,
					_PR_EFFECT_MASK		= 0x0000f000,

					PR_MOVECON_NONE		= 0x00000000,
					PR_MOVECON_ENABLE	= 0x00010000,
					_PR_MOVECON_MASK	= 0x000f0000,
};

enum eOPTION{
				OP_FOG			= 0x00000001,
				OP_ABVWIRE		= 0x00000010,
				OP_LIGHT		= 0x00000100,
				OP_STAT			= 0x00001000,
				OP_USERLIGHT	= 0x00010000,
				_OP_ALL			= OP_FOG | OP_ABVWIRE | OP_LIGHT | OP_STAT,
};

struct sCREATEINFO : public NiMemObject
{
	sCREATEINFO(){ Reset(); }

	void		Reset(){ s_dwPropPlag = s_dwOptionPlag = 0; s_eInsertGeomType = CGeometry::Normal; }

	bool		IsPropPlag( DWORD pr ){ assert_cs( pr != 0 ); return ( pr & s_dwPropPlag ) != 0; }
	DWORD		GetPropPlag( DWORD mask ){ assert_cs( (mask != 0)&&(mask!=_PR_ANI_MASK) ); return ( mask & s_dwPropPlag ); }

	bool		IsOptionPlag( DWORD op ){ assert_cs( op != 0 ); return ( op & s_dwOptionPlag ) != 0; }	

	DWORD					s_dwPropPlag;
	DWORD					s_dwOptionPlag;
	CGeometry::eGEOM_TYPE	s_eInsertGeomType;
};


//========================================================================================================
//		Char RTTI
//========================================================================================================
enum eRTTI
{
	RTTI_NONE			=	0,
	RTTI_OBJECT,
	RTTI_AVOBJECT,
	RTTI_PARTOBJECT,
	//RTTI_CARDOBJECT,
	RTTI_EFFECTOBJECT,
	RTTI_DUMMYNPC,
	RTTI_EVENTOBJECT,
	RTTI_EVENTTAMEROBJECT,
	RTTI_TACTICSOBJECT,
	RTTI_DPARTOBJECT,
	RTTI_LIB_MAXCOUNT,
};

struct sRTTI_INFO : public NiMemObject
{
public:
	sRTTI_INFO( int rtti, sRTTI_INFO* pParentRTTIInfo ){ m_eRTTI = (eRTTI)rtti; m_pParentInfo = pParentRTTIInfo; }

	eRTTI			m_eRTTI;
	sRTTI_INFO*		m_pParentInfo;
};

#define CsCRTTI_H		\
	protected:\
	static sRTTI_INFO	m_RTTIInfo;\
	public:\
	eRTTI				GetRTTI(){ return m_RTTIInfo.m_eRTTI; }\
	virtual eRTTI		GetLeafRTTI()const{ return m_RTTIInfo.m_eRTTI; }\
	virtual sRTTI_INFO*	GetRTTIInfo(){ return &m_RTTIInfo; }\
	bool			IsKindOf( int RTTI )\
{\
	sRTTI_INFO* pInfo = GetRTTIInfo();\
	while( pInfo )\
{\
	if( pInfo->m_eRTTI == RTTI )\
	return true;\
	pInfo = pInfo->m_pParentInfo;\
}\
	return false;\
}


#define CsCRTTI_CPP( parentcalssname, classname, curRTTI )	\
	sRTTI_INFO		classname::m_RTTIInfo( curRTTI, &parentcalssname::m_RTTIInfo );
#define CsCROOTRTTI_CPP( classname, curRTTI )	\
	sRTTI_INFO		classname::m_RTTIInfo( curRTTI, NULL );


