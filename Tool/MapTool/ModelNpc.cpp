
#include "stdafx.h"
#include "ModelNpc.h"

cModel_Npc::cModel_Npc()
{
}

void cModel_Npc::Delete()
{
	cModel_Base::Delete();
}

void cModel_Npc::Init( DWORD dwNpcID, cModel_Res* pModelRes )
{
	m_dwNpcID = dwNpcID;

	m_pAM = pModelRes->Clone();
	m_pAM->GetNIFRoot()->UpdateProperties();

	m_fAniTime = 0.0f;

	cModel_Base::_Init( pModelRes->GetModelKey() );
}

cModel_Npc* cModel_Npc::Clone( bool bResist )
{
	/*cModel_Npc* pNpc = g_ModelMng.m_mempoolModelNpc.Pop();
	pNpc->Init( m_dwNpcID, g_Preview.GetChar()->GetModelRes( m_dwModelKey ) );
	NiTransform tr = GetTransform();	
	pNpc->SetTransform( GetTransform() );
	pNpc->SetInitPos( m_ptInitPos );
	pNpc->CopyAction( this );
	if( bResist == true )
		pNpc->CalInitAction();
	return pNpc;*/

	return NULL;
}

void cModel_Npc::SetTransform( NiTransform tr )
{
	m_pAM->GetNIFRoot()->SetLocalTransform( tr );
	Update( 0.0f, false );
}

void cModel_Npc::SetPos( NiPoint3 pos )
{
	m_pAM->GetNIFRoot()->SetTranslate( pos );
	Update( 0.0f, false );

	CalInitPos();
}

void cModel_Npc::CalInitPos()
{
	m_ptInitPos = NiPoint2( GetNiNode()->GetWorldTranslate().x, GetNiNode()->GetWorldTranslate().y );
}


