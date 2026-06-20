#pragma once

#include "UndoManager/UndoCommand.h"
namespace UNDO
{

typedef std::deque<Command*> Commands;
typedef Commands::iterator CommandIter;
enum EDIT_TYPE
{
	TRANSLATE,
	ROTATE,
	SCALE,
	PATH_TRANSLATE,
	OBJ_CREATE,
	OBJ_DELETE,
	PATH_CREATE,
	PATH_DELETE,
	CELLOBJ_TRANSLATE,
	CELLOBJ_CREATE,
	CELLOBJ_DELETED,
	END,
};

class Manager : public CSingleton<UNDO::Manager>
{
public:
	void Initizlize(void);
	void Update(const float& fDeltaTime);
	void Render(void);
	void Terminate(void);
	void Reset(void);

public: 
	void AddCommand(Command* pkCommand);
	int AddCommand(EDIT_TYPE eType, Snapshot* pkSnapshots);
	void AddRedoSnapshot(int CUID, Snapshot* pkSnapshot);
	void ExcuteUndo(void);
	void ExcuteRedo(void);
private:
	Commands m_Undos;
	Commands m_Redos;
	float MAX_COMMAND_COUNT;

	static int CommnandID;
public:
	Manager(void);
	~Manager(void);
};

}


