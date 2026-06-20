// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "NiCollisionPCH.h"

#include <NiAVObject.h>
#include "NiCollision.h"
#include "NiCollisionWorkflow.h"
#include "NiCollisionMetrics.h"
#include "NiCollisionTraversals.h"
#include "NiPick.h"

//---------------------------------------------------------------------------
NiPick::NiPick(unsigned int uiResultsInitialSize, 
    unsigned int uiResultsGrowBy) :
    m_pickResults(uiResultsInitialSize, uiResultsGrowBy)
{
    m_ePickType = FIND_ALL;
    m_eSortType = SORT;
    m_eIntersectType = TRIANGLE_INTERSECT;
    m_eCoordinateType = WORLD_COORDINATES;
    m_bFrontOnly = true;
    m_bObserveAppCullFlag = false;
    m_spRoot = 0;
    m_bReturnTexture = false;
    m_bReturnNormal = false;
    m_bReturnSmoothNormal = false;
    m_bReturnColor = false;
    m_pLastAddedRecord = 0;
}
//---------------------------------------------------------------------------
NiPick::~NiPick()
{

}
//---------------------------------------------------------------------------
void NiPick::ClearResultsArray()
{
    m_pickResults.ClearArray();
}

#if defined (_PS3) && defined(ENABLE_PARALLEL_PICK)
//---------------------------------------------------------------------------
void NiPick::PickObjectsBegin(const NiPoint3 &kOrigin, const NiPoint3 &kDir,
    bool bAppend)
{
    // If not appending, clear earlier pick results.
    if (!bAppend)
        ClearResultsArray();

    // Find intersections.
    m_bFindResult = NiCollisionTraversals::FindIntersections(
        kOrigin, kDir, *this, m_spRoot);
}
//---------------------------------------------------------------------------
bool NiPick::PickObjectsEnd()
{
    NIMETRICS_COLLISION_SCOPETIMER(PICK_TIME);

#if NIMETRICS
    unsigned int uiMetricsPreviousPickSize = m_pickResults.GetEffectiveSize();
    m_uiNodeComparisons = 0;
    m_uiTriComparisons = 0;
#endif

#if defined(_PS3) && defined(ENABLE_PARALLEL_PICK)
  //  // Early out if we are only looking for the first intersection
  //  if (m_eSortType == NO_SORT || m_eIntersectType == BOUND_INTERSECT || 
  //      (m_pickResults.GetEffectiveSize() > 0))
  //      return true;

    NiCollisionWorkflow::FinishWorkflows(this);
    bool bRet = m_pickResults.GetEffectiveSize() > 0 ? true : m_bFindResult;
#endif
    // Record number of new pick results found
    NIMETRICS_COLLISION_ADDVALUE(PICK_RESULTS, 
        m_pickResults.GetEffectiveSize() - uiMetricsPreviousPickSize);
    NIMETRICS_COLLISION_ADDVALUE(PICK_COMPARISONS_NODE, m_uiNodeComparisons);
    NIMETRICS_COLLISION_ADDVALUE(PICK_COMPARISONS_TRI, m_uiTriComparisons);

    if (!bRet || !m_pickResults.GetEffectiveSize())
        return false;

    // When using BOUND_INTERSECT, sort mode is ignored.
    if (m_eSortType == NO_SORT || m_eIntersectType == BOUND_INTERSECT)
        return true;

    if (m_pickResults.GetEffectiveSize() > 1)
    {
        if (m_ePickType == FIND_ALL)
        {
            m_pickResults.SortResults();
        }
        else 
        {
            m_pickResults.FindClosest();
        }
    }

    return true;
}
#endif

//---------------------------------------------------------------------------
bool NiPick::PickObjects(const NiPoint3 &kOrigin, const NiPoint3 &kDir,
    bool bAppend)
{
    NIMETRICS_COLLISION_SCOPETIMER(PICK_TIME);

    // If not appending, clear earlier pick results.
    if (!bAppend)
        ClearResultsArray();

#if NIMETRICS
    unsigned int uiMetricsPreviousPickSize = m_pickResults.GetEffectiveSize();
    m_uiNodeComparisons = 0;
    m_uiTriComparisons = 0;
#endif


    // Find intersections.
    bool bRet = NiCollisionTraversals::FindIntersections(kOrigin, kDir, *this,
        m_spRoot);
#if defined(_PS3) && defined(ENABLE_PARALLEL_PICK)
    NiCollisionWorkflow::FinishWorkflows(this);
    bRet = m_pickResults.GetEffectiveSize() > 0 ? true : bRet;
#endif
    // Record number of new pick results found
    NIMETRICS_COLLISION_ADDVALUE(PICK_RESULTS, 
        m_pickResults.GetEffectiveSize() - uiMetricsPreviousPickSize);
    NIMETRICS_COLLISION_ADDVALUE(PICK_COMPARISONS_NODE, m_uiNodeComparisons);
    NIMETRICS_COLLISION_ADDVALUE(PICK_COMPARISONS_TRI, m_uiTriComparisons);

    if (!bRet || !m_pickResults.GetEffectiveSize())
        return false;

    // When using BOUND_INTERSECT, sort mode is ignored.
    if (m_eSortType == NO_SORT || m_eIntersectType == BOUND_INTERSECT)
        return true;

    if (m_pickResults.GetEffectiveSize() > 1)
    {
        if (m_ePickType == FIND_ALL)
        {
            m_pickResults.SortResults();
        }
        else 
        {
            m_pickResults.FindClosest();
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPick::Results::SortResults()
{
    // Sort array by intersection distance from viewer, closest to
    // farthest.  Uses bubble sort since number of intersections is
    // typically small.
    for (unsigned int i = 0; i < GetSize(); i++)
    {
        Record* pRecord0 = GetAt(i);
        NIASSERT(pRecord0);

        for (unsigned int j = i+1; j < GetSize(); j++)
        {
            Record* pRecord1 = GetAt(j);
            NIASSERT(pRecord1);

            if (pRecord1->GetDistance() < pRecord0->GetDistance())
            {
                // Swap
                SetAt(j, pRecord0);
                SetAt(i, pRecord1);
                pRecord0 = pRecord1;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPick::Results::FindClosest()
{
    // If no records, nothing to do.
    if (!GetSize())
        return;

    // Sort array by intersection distance from viewer.  Eliminate all
    // but the closest intersected object.
    unsigned int uiClosest = 0;
    Record* pRecordClosest = GetAt(uiClosest);
    NIASSERT(pRecordClosest);

    for (unsigned int i = 1; i < GetSize(); i++)
    {
        Record* pRecordTest = GetAt(i);
        NIASSERT(pRecordTest);

        bool bTestIsCloser = (pRecordTest->GetDistance() < 
            pRecordClosest->GetDistance());

        pRecordClosest = bTestIsCloser ? pRecordTest : pRecordClosest;
        uiClosest = bTestIsCloser ? i : uiClosest;
    }

    // Swap closest with first entry and remove all other entries.
    SetAt(uiClosest, GetAt(0));
    SetAt(0, pRecordClosest);
    m_uiValidResults = 1;
}
//---------------------------------------------------------------------------
