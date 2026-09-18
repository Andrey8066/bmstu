/****************************************************************************
** Meta object code from reading C++ file 'osm_data_loader.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/osm_data_loader.hpp"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'osm_data_loader.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13OSMDataLoaderE_t {};
} // unnamed namespace

template <> constexpr inline auto OSMDataLoader::qt_create_metaobjectdata<qt_meta_tag_ZN13OSMDataLoaderE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "OSMDataLoader",
        "dataLoaded",
        "",
        "QJsonArray",
        "rawJSON",
        "queryType",
        "errorOccurred",
        "errorMessage",
        "fetchOSMData",
        "timeout",
        "QRect",
        "area",
        "getDefaultArea",
        "OSMDataLoaderArea",
        "setArea",
        "buildOverpassQuery",
        "timout",
        "saveToJSONFile",
        "data",
        "filename"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'dataLoaded'
        QtMocHelpers::SignalData<void(const QJsonArray &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'fetchOSMData'
        QtMocHelpers::SlotData<QByteArray(const QString &, const int &, const QRect &)>(8, 2, QMC::AccessPublic, QMetaType::QByteArray, {{
            { QMetaType::QString, 5 }, { QMetaType::Int, 9 }, { 0x80000000 | 10, 11 },
        }}),
        // Slot 'fetchOSMData'
        QtMocHelpers::SlotData<QByteArray(const QString &, const int &)>(8, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QByteArray, {{
            { QMetaType::QString, 5 }, { QMetaType::Int, 9 },
        }}),
        // Slot 'fetchOSMData'
        QtMocHelpers::SlotData<QByteArray(const QString &)>(8, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QByteArray, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'fetchOSMData'
        QtMocHelpers::SlotData<QByteArray()>(8, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QByteArray),
        // Slot 'getDefaultArea'
        QtMocHelpers::SlotData<OSMDataLoaderArea() const>(12, 2, QMC::AccessPublic, 0x80000000 | 13),
        // Slot 'setArea'
        QtMocHelpers::SlotData<void(const OSMDataLoaderArea &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 11 },
        }}),
        // Slot 'buildOverpassQuery'
        QtMocHelpers::SlotData<QString(const int &, const QString &)>(15, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::Int, 16 }, { QMetaType::QString, 5 },
        }}),
        // Slot 'saveToJSONFile'
        QtMocHelpers::SlotData<QString(const QJsonArray &, const QString &)>(17, 2, QMC::AccessPublic, QMetaType::QString, {{
            { 0x80000000 | 3, 18 }, { QMetaType::QString, 19 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<OSMDataLoader, qt_meta_tag_ZN13OSMDataLoaderE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject OSMDataLoader::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13OSMDataLoaderE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13OSMDataLoaderE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13OSMDataLoaderE_t>.metaTypes,
    nullptr
} };

void OSMDataLoader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OSMDataLoader *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->dataLoaded((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: { QByteArray _r = _t->fetchOSMData((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QRect>>(_a[3])));
            if (_a[0]) *reinterpret_cast<QByteArray*>(_a[0]) = std::move(_r); }  break;
        case 3: { QByteArray _r = _t->fetchOSMData((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QByteArray*>(_a[0]) = std::move(_r); }  break;
        case 4: { QByteArray _r = _t->fetchOSMData((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QByteArray*>(_a[0]) = std::move(_r); }  break;
        case 5: { QByteArray _r = _t->fetchOSMData();
            if (_a[0]) *reinterpret_cast<QByteArray*>(_a[0]) = std::move(_r); }  break;
        case 6: { OSMDataLoaderArea _r = _t->getDefaultArea();
            if (_a[0]) *reinterpret_cast<OSMDataLoaderArea*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->setArea((*reinterpret_cast<std::add_pointer_t<OSMDataLoaderArea>>(_a[1]))); break;
        case 8: { QString _r = _t->buildOverpassQuery((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 9: { QString _r = _t->saveToJSONFile((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (OSMDataLoader::*)(const QJsonArray & , const QString & )>(_a, &OSMDataLoader::dataLoaded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (OSMDataLoader::*)(const QString & )>(_a, &OSMDataLoader::errorOccurred, 1))
            return;
    }
}

const QMetaObject *OSMDataLoader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OSMDataLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13OSMDataLoaderE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int OSMDataLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void OSMDataLoader::dataLoaded(const QJsonArray & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void OSMDataLoader::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
