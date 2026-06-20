#pragma once

#define ELEMENT_EQAUL	100

namespace nsBaseElement
{
	enum eElementType{ eNo, eDa, eVa, eVi, eUn, eMax };
	enum eCompareType{ eAtt, eDef };
}

struct sBaseElement
{
	sBaseElement() : nTargetType( nsBaseElement::eMax ), nPoint( 0 ) {}
	~sBaseElement() {}

	int				nTargetType;
	int				nPoint;		// 우세 110 / 동일 100 / 열세 90
};

typedef std::vector< sBaseElement > VEC_CMP_POINT;
typedef std::vector< sBaseElement >::iterator VEC_CMP_POINT_IT;
typedef std::vector< sBaseElement >::const_iterator VEC_CMP_POINT_CIT;

typedef std::map< int, VEC_CMP_POINT > MAP_BASE_ELEMENT;
typedef std::map< int, VEC_CMP_POINT >::iterator MAP_BASE_ELEMENT_IT;
typedef std::map< int, VEC_CMP_POINT >::const_iterator MAP_BASE_ELEMENT_CIT;

typedef std::map< int, MAP_BASE_ELEMENT > MAP_CMP_ELEMENT;
typedef std::map< int, MAP_BASE_ELEMENT >::iterator MAP_CMP_ELEMENT_IT;
typedef std::map< int, MAP_BASE_ELEMENT >::const_iterator MAP_CMP_ELEMENT_CIT;