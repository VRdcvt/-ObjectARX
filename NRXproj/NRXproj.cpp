//подключенные заголовочные файлы
#include <vector>
#include "pch.h"
#include "framework.h"
#include "NRXproj.h"
#include "AcDbCrossCircle.h"
#include "reactor.h"
#pragma comment(lib, "CrossCircle.lib")


//функция в которой происходит построение связи и создание и присвоение связей в ректорах
void assocLines()
{
    AcDbDatabase* pDb;
    pDb = acdbHostApplicationServices()->workingDatabase();
    double Start_x, Start_y, End_x, End_y;
    ads_name en; ads_point p;
    AcDbObjectId aId, bId, cId;
    int es1 = acedEntSel(_T("\nВыберите объект \"крест-круг\": "), en, p);
    //при выборе на пустом месте в области модели в нанокад, произойдет сброс команды
    if (es1 != RTNORM) {
        acutPrintf(L"\nОбъект не выбран. ");
        return;
    }
    AcDbObjectId First_Id;
    acdbGetObjectId(First_Id, en);
    AcDbEntityPointer pEnt1(First_Id, AcDb::kForRead);
    AcDbObjectPointer<AcDbCrossCircle> ptrCrossCircle1(First_Id, AcDb::kForWrite);
    ptrCrossCircle1->setDatabaseDefaults(pDb);
    ptrCrossCircle1->close();
    if (pEnt1.openStatus() != Acad::eOk)
        return;
    //при выборе примитива "крест-круг" объект подсвечивается и программа запоминает координаты его центра
    //при выборе любого примитива, кроме крест-круг, произойдет сброс команды
    if (First_Id.objectClass() == AcDbCrossCircle::desc()) {
        pEnt1->highlight();
        AcGePoint3d point_1;
        point_1 = ptrCrossCircle1->center();
        Start_x = point_1.x;
        Start_y = point_1.y;
    }
    else {
        acutPrintf(L"\nОбъект не выбран. ");
        return;
    }
    addToModelSpace(aId, ptrCrossCircle1);



    es1 = acedEntSel(_T("\nВыберите другой объект \"крест-круг\": "), en, p);
    //при выборе второго любого примитива, кроме крест-круг, произойдет сброс команды
    //первый выбранный объект "крест круг" потеряет свечение
    if (es1 != RTNORM) {
        acutPrintf(L"\nОбъект не выбран. ");
        pEnt1->unhighlight();
        return;
    }
    AcDbObjectId Second_Id;
    acdbGetObjectId(Second_Id, en);
    AcDbEntityPointer pEnt2(Second_Id, AcDb::kForRead);
    AcDbObjectPointer<AcDbCrossCircle> ptrCrossCircle2(Second_Id, AcDb::kForWrite);
    ptrCrossCircle2->setDatabaseDefaults(pDb);
    ptrCrossCircle2->close();
    if (pEnt2.openStatus() != Acad::eOk) {
        pEnt1->unhighlight();
        return;
    }
    //при выборе второго примитива "крест-круг", но не того, что выбран первым, программа запоминает координаты его центра
    //при выборе второго любого примитива, кроме крест-круг (но не того что выбран первым), произойдет сброс команды
    //первый выбранный объект "крест круг" потеряет свечение
    if ((Second_Id.objectClass() == AcDbCrossCircle::desc()) && (First_Id != Second_Id)) {
        AcGePoint3d point_2;
        point_2 = ptrCrossCircle2->center();
        End_x = point_2.x;
        End_y = point_2.y;
        pEnt1->unhighlight();
    }
    else {
        acutPrintf(L"\nОбъект не выбран. ");
        pEnt1->unhighlight();
        return;
    }
    addToModelSpace(bId, ptrCrossCircle2);



    //Создание отрезка, который связывает два объекта "крест-круг"
    //Началом и концом отрезка служат точки с координатами центров объектов "крест-круг"
    AcGePoint3d startPt(Start_x, Start_y, 0.0);
    AcGePoint3d endPt(End_x, End_y, 0.0);
    AcDbLine* pLine = new AcDbLine;
    pLine->setDatabaseDefaults(pDb);
    pLine->setStartPoint(startPt);
    pLine->setEndPoint(endPt);
    addToModelSpace(cId, pLine);
    pLine->close();



    //Создание словарей с названиями объектов и названием списков
    //Как я понял, в них записываются все объекты участвующие в реакторах
    //Могу ошибаться, не уверен
    AcDbDictionary* pNamedObj;
    AcDbDictionary* pNameList;
    pDb->getNamedObjectsDictionary(pNamedObj,
        AcDb::kForWrite);
    if (pNamedObj->getAt(_T("ASDK_DICT"),
        (AcDbObject*&)pNameList, AcDb::kForWrite)
        == Acad::eKeyNotFound)
    {
        pNameList = new AcDbDictionary;
        AcDbObjectId DictId;
        pNamedObj->setAt(_T("ASDK_DICT"), pNameList, DictId);
    }
    pNamedObj->close();



    //Создание объекта класса, отвечающий за событие (в данном случае событием называется перемещение объекта в пространстве модели нанокад)
    AsdkObjectToNotify* pObj = new AsdkObjectToNotify();
    pObj->eLinkage(cId);
    AcDbObjectId objId;
    if ((pNameList->getAt(_T("object_to_notify_A"), objId))
        == Acad::eKeyNotFound)
    {
        pNameList->setAt(_T("object_to_notify_A"), pObj, objId);
        pObj->close();
    }
    else {
        delete pObj;
        ads_printf(_T("object_to_notify_A already exists\n"));
    }


    ptrCrossCircle1->addPersistentReactor(objId);
    ptrCrossCircle1->close();


    pObj = new AsdkObjectToNotify();
    pObj->eLinkage(cId);


    if ((pNameList->getAt(_T("object_to_notify_B"), objId))
        == Acad::eKeyNotFound)
    {
        pNameList->setAt(_T("object_to_notify_B"), pObj, objId);
        pObj->close();
    }
    else {
        delete pObj;
        ads_printf(_T("object_to_notify_B already exists\n"));
    }
    pNameList->close();


    ptrCrossCircle2->addPersistentReactor(objId);
    ptrCrossCircle2->close();
}



void initApp() {
    //Регистрация пользовательских команд
    ncedRegCmds->addCommand(L"CrCircleLink_GROUP",
        L"_CrCircleLink",
        L"CrCircleLink",
        ACRX_CMD_TRANSPARENT,
        assocLines);
    AsdkObjectToNotify::rxInit();
    ncrxBuildClassHierarchy();
}



void uninitApp() {
    //Удаление пользовательских команд
    ncedRegCmds->removeGroup(L"CrCircleLink_GROUP");
    deleteAcRxClass(AsdkObjectToNotify::desc());
}



//Точка входа
extern "C" __declspec(dllexport) NcRx::AppRetCode
ncrxEntryPoint(NcRx::AppMsgCode msg, void* appId)
{
    switch (msg)
    {
    case NcRx::kInitAppMsg:
        ncrxDynamicLinker->unlockApplication(appId);
        ncrxDynamicLinker->registerAppMDIAware(appId);
        initApp();
        break;
    case NcRx::kUnloadAppMsg:
        uninitApp();
        break;
    }
    return NcRx::kRetOK;
}
