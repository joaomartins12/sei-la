#include "StdAfx.h"
#include "Gizmo.h"
using namespace GIZMO;
//cGizmo::cGizmo(void)
//{
//}
//
cGizmo::~cGizmo(void)
{
	m_NifPath = "";
	m_SelectedAxisName = "";
	m_spModel = 0;
	m_pkCurrEditable = NULL;
}
