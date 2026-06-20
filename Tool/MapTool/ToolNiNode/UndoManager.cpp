#include "StdAfx.h"
#include "UndoManager.h"

int UNDO::Manager::CommnandID = 0;
UNDO::Manager::Manager(void)
:MAX_COMMAND_COUNT(50.0f)
{
}

UNDO::Manager::~Manager(void)
{
	Terminate();
}

void UNDO::Manager::Initizlize(void)
{
}
void UNDO::Manager::Update(const float& fDeltaTime)
{


}
void UNDO::Manager::Render(void)
{

}
void UNDO::Manager::Terminate(void)
{
	while(m_Undos.empty() == false)
	{
		Command* pkDeletedCommnad = m_Undos.back();
		SAFE_DELETE(pkDeletedCommnad);
		m_Undos.pop_back();
	}

	while(m_Redos.empty() == false)
	{
		Command* pkDeletedCommnad = m_Redos.back();
		SAFE_DELETE(pkDeletedCommnad);
		m_Redos.pop_back();
	}
	
}
void UNDO::Manager::Reset(void)
{
	Terminate();
	CommnandID = 0;
}
void UNDO::Manager::AddCommand(Command* pkCommand)
{
	if(m_Undos.size() > MAX_COMMAND_COUNT)
	{
		Command* pkDeletedCmd = m_Undos.back();
		SAFE_DELETE(pkDeletedCmd);
		m_Undos.pop_back();
	}
	m_Undos.push_front(pkCommand);
}
int UNDO::Manager::AddCommand(EDIT_TYPE eType, Snapshot* pkSnapshots)
{
	Command* pkCmd = NULL;
	switch(eType)
	{
	case UNDO::TRANSLATE:
		pkCmd = new UNDO::Move();
		break;
	case UNDO::ROTATE:
		pkCmd = new UNDO::Rotate();
		break;
	case UNDO::SCALE:
		pkCmd = new UNDO::Scale();
		break;
	case UNDO::PATH_TRANSLATE:
		pkCmd = new UNDO::PathTranslate();
		break;
	case UNDO::OBJ_CREATE:
		pkCmd = new UNDO::ObjCreate();
		break;
	case UNDO::OBJ_DELETE:
		pkCmd = new UNDO::ObjDelete();
		break;
	case UNDO::PATH_CREATE:
		pkCmd = new UNDO::PathCreate();
		break;
	case UNDO::PATH_DELETE:
		pkCmd = new UNDO::PathDelete();
		break;
	case UNDO::CELLOBJ_TRANSLATE:
		pkCmd = new UNDO::CellObjTranslate();
		break;
	case UNDO::CELLOBJ_CREATE:
		pkCmd = new UNDO::CellObjCreate();
		break;
	}
	if(pkCmd != NULL)
	{
		pkCmd->SetUndo(pkSnapshots);
		int CurrCUID = ++CommnandID;
		pkCmd->SetCUID(CurrCUID);
		AddCommand(pkCmd);
		return CurrCUID;
	}
	return -1;
}

void UNDO::Manager::AddRedoSnapshot(int CUID, Snapshot* pkSnapshot)
{
	CommandIter kIter = m_Undos.begin();
	for(; kIter != m_Undos.end(); ++kIter)
	{
		if((*kIter)->GetCUID() == CUID){
			(*kIter)->SetRedo(pkSnapshot);
		}
	}
	while(m_Redos.empty() == false)
	{
		Command* pkDeletedCommnad = m_Redos.back();
		SAFE_DELETE(pkDeletedCommnad);
		m_Redos.pop_back();
	}

}
void UNDO::Manager::ExcuteUndo(void)
{
	if(m_Undos.empty() == true)
		return;
	Command* pkCmd= m_Undos.front();
	pkCmd->Undo();
	m_Undos.pop_front();
	m_Redos.push_front(pkCmd);
}
void UNDO::Manager::ExcuteRedo(void)
{
	if(m_Redos.empty() == true)
		return;
	Command* pkCmd= m_Redos.front();
	pkCmd->Redo();
	m_Redos.pop_front();
	m_Undos.push_front(pkCmd);
}