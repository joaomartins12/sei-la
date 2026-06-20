#pragma once

namespace UNDO
{
	typedef std::map<DWORD, NiTransform> Snapshots;
	typedef Snapshots::iterator SnapshotIter;

	typedef std::map<int, NiPoint3> ModelVtPos;
	typedef ModelVtPos::iterator ModelVtIter;
	struct ModelVtInfo
	{
		ModelVtPos mVTPos;
	};
	typedef std::map<int, ModelVtInfo> ModelSnapshots;
	typedef ModelSnapshots::iterator ModelSnapshotIter;

	struct PathInfo
	{
		std::string mFullPath;
		NiTransform mLastTranform;
	};
	typedef std::list<PathInfo> ModelPath;
	typedef ModelPath::iterator ModelPathIter;

	struct CellObjInfo
	{
		int BaseIdx;
		int CellIdx;
		int ObjIdx;
		NiTransform mLastTranform;
	};
	typedef std::list<CellObjInfo> CellObjs;
	typedef CellObjs::iterator CellObjIter;
class Snapshot
{
public:
	void AddSnapshot(DWORD UID, const NiTransform& kTr);
	void AddSnapshot(DWORD UID, NiPoint3* pkPos, int VtCnt);
	void AddSnapshot(std::string kFullPath, const NiTransform& kTr);
	void AddSnapshot(int iBaseIdx, int CellIdx, int ObjIdx, const NiTransform& kTr);

	const Snapshots& GetSnapshots(void){return m_Snapshots;};
	const ModelSnapshots& GetModelSnapshots(void){return m_ModelSnapshots;};
	const ModelPath& GetModelPath(void){return m_ModelFullPaths;};
	const CellObjs& GetCellObj(void){return m_CellObjs;};

private:
	Snapshots m_Snapshots;
	ModelSnapshots m_ModelSnapshots;
	ModelPath m_ModelFullPaths;
	CellObjs m_CellObjs;
public:
	Snapshot(void);
	~Snapshot(void);
};

class Command
{
public:
	virtual void Undo(void){}
	virtual void Redo(void){}
	virtual void SetUndo(Snapshot* pkSnapshot);
	virtual void SetRedo(Snapshot* pkSnapshot);

	virtual void SetCUID(const int& iID){CUID = iID;}
	virtual int GetCUID(void){return CUID;}
protected:
	void Terminate();

	Snapshot* m_pkUndo;
	Snapshot* m_pkRedo;
	int CUID;
public:
	Command(void);
	~Command(void);
};

class Move: public Command
{

public:
	void Undo(void);
	void Redo(void);
public:
	Move(void);
	~Move(void);
};
class Scale: public Command
{
public:
	void Undo(void);
	void Redo(void);
public:
	Scale(void);
	~Scale(void);
};
class Rotate: public Command
{
public:
	void Undo(void);
	void Redo(void);
public:
	Rotate(void);
	~Rotate(void);
};
class PathTranslate: public Command
{
public:
	void Undo(void);
	void Redo(void);
public:
	PathTranslate(void);
	~PathTranslate();
};
class ObjCreate: public Command
{
public:
	void Undo(void);
	void Redo(void);
public:
	ObjCreate(void);
	~ObjCreate();
};
class ObjDelete: public Command
{
public:
	void Undo(void);
	void Redo(void);
public:
	ObjDelete(void);
	~ObjDelete();
};
class PathCreate: public Command
{
public:
	void Undo(void);
	void Redo(void);
public:
	PathCreate(void);
	~PathCreate();
};
class PathDelete: public Command
{
public:
	void Undo(void);
	void Redo(void);
public:
	PathDelete(void);
	~PathDelete();
};
class CellObjTranslate: public Command
{
public:
	void Undo(void);
	void Redo(void);
public:
	CellObjTranslate(void);
	~CellObjTranslate();
};
class CellObjCreate: public Command
{
public:
	void Undo(void);
	void Redo(void);
public:
	CellObjCreate(void);
	~CellObjCreate();
};
}
