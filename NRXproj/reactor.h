#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <aced.h>
#include <dbents.h>
#include <dbsymtb.h>
#include <dbapserv.h>
#include <adslib.h>
#include "tchar.h"

//Cобытие перемещения примитива крест-круг
class AsdkObjectToNotify : public AcDbObject/*, IDs*/
{
public:
    ACRX_DECLARE_MEMBERS(AsdkObjectToNotify);
    AsdkObjectToNotify() {};
    void eLinkage(AcDbObjectId i, double f = 1.0)
    {
        mId = i;
        mFactor = f;
    };
    void modified(const AcDbObject*);
    Acad::ErrorStatus dwgInFields(AcDbDwgFiler*);
    Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;
    Acad::ErrorStatus dxfInFields(AcDbDxfFiler*);
    Acad::ErrorStatus dxfOutFields(AcDbDxfFiler*) const;

private:
    AcDbObjectId mId;
    double mFactor;
};



//Макрос вызывающий событие перемещения при сдвиге примитива
ACRX_DXF_DEFINE_MEMBERS(AsdkObjectToNotify, AcDbObject,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    0, \
    ASDKOBJECTTONOTIFY, persreac);



//Функция перемещения одной из двух точек примитива "отрезок" в центр одного из примитивов "крест-круг"
void AsdkObjectToNotify::modified(const NcDbObject* pObj)
{
    AcDbCrossCircle* crCircle = AcDbCrossCircle::cast(pObj);
    AcGePoint3d newCOORD = crCircle->center();
    acutPrintf(_T("\nРеактор связывает объект id: %lx с объектом id: %lx.\n"),
        crCircle->objectId().asOldId(), mId.asOldId());
    AcDbLine* pLine2;
    AcDbObjectId id;
    AcDbObjectPointer<AcDbCrossCircle> ptrCrossCircle;
    if (acdbOpenObject(pLine2, mId, AcDb::kForWrite) == Acad::eOk)
    {
        AcDbBlockTableRecordPointer pModelSpace(ACDB_MODEL_SPACE, acdbCurDwg(), AcDb::kForRead);
        assert(pModelSpace.openStatus() == Acad::eOk);
        AcDbBlockTableRecordIterator* pIterator;
        pModelSpace->newIterator(pIterator);
        //проходим итератором по всем примитовам проекта
        for (;!pIterator->done(); pIterator->step())
        {
            pIterator->getEntityId(id);
            if (id == pLine2->id()) {
                continue;
            }
            //далее вычисляем какой из примитивов после перемещения остался на месте, а какой сдвинулся
            //меняем координату отрезка, в соответситвии с новым центром сдвинутого примитива
            ptrCrossCircle.open(id,AcDb::kForRead);
            if (ptrCrossCircle.object()->center() == pLine2->startPoint()) {
                pLine2->setEndPoint(newCOORD);
                pLine2->close();
                return;
            }
            if (ptrCrossCircle.object()->center() == pLine2->endPoint()) {
                pLine2->setStartPoint(newCOORD);
                pLine2->close();
                return;
            }
        }
        delete pIterator;
    }
}



Acad::ErrorStatus AsdkObjectToNotify::dwgInFields(AcDbDwgFiler* filer)
{
    assertWriteEnabled();
    AcDbObject::dwgInFields(filer);
    filer->readItem(&mFactor);
    filer->readItem((AcDbSoftPointerId*)&mId);
    return filer->filerStatus();
}



Acad::ErrorStatus AsdkObjectToNotify::dwgOutFields(AcDbDwgFiler* filer) const
{
    assertReadEnabled();
    AcDbObject::dwgOutFields(filer);
    filer->writeItem(mFactor);
    filer->writeItem((AcDbSoftPointerId&)mId);
    return filer->filerStatus();
}



Acad::ErrorStatus AsdkObjectToNotify::dxfInFields(AcDbDxfFiler* filer)
{
    assertWriteEnabled();

    Acad::ErrorStatus es;
    if ((es = AcDbObject::dxfInFields(filer))
        != Acad::eOk)
    {
        return es;
    }

    // Check if we're at the right subclass data marker
    //
    if (!filer->atSubclassData(_T("AsdkObjectToNotify"))) {
        return Acad::eBadDxfSequence;
    }

    struct resbuf rbIn;

    while (es == Acad::eOk) {
        if ((es = filer->readItem(&rbIn)) == Acad::eOk) {
            if (rbIn.restype == AcDb::kDxfReal) {
                mFactor = rbIn.resval.rreal;
            }
            else if (rbIn.restype
                == AcDb::kDxfSoftPointerId)
            {
                // ObjectIds are filed in as ads_names
                // 
                acdbGetObjectId(mId, rbIn.resval.rlname);
            }
            else {   // invalid group
                return(filer->pushBackItem());
            }
        }
    }
    return filer->filerStatus();
}



Acad::ErrorStatus AsdkObjectToNotify::dxfOutFields(AcDbDxfFiler* filer) const
{
    assertReadEnabled();
    AcDbObject::dxfOutFields(filer);
    filer->writeItem(AcDb::kDxfSubclass,
        _T("AsdkObjectToNotify"));
    filer->writeItem(AcDb::kDxfReal, mFactor);
    filer->writeItem(AcDb::kDxfSoftPointerId, mId);
    return filer->filerStatus();
}



void addToModelSpace(AcDbObjectId& objId, AcDbEntity* pEntity)
{
    AcDbBlockTable* pBlockTable;
    AcDbBlockTableRecord* pSpaceRecord;

    acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pBlockTable, AcDb::kForRead);

    pBlockTable->getAt(ACDB_MODEL_SPACE, pSpaceRecord,
        AcDb::kForWrite);
    pBlockTable->close();

    pSpaceRecord->appendAcDbEntity(objId, pEntity);
    pSpaceRecord->close();

    return;
}