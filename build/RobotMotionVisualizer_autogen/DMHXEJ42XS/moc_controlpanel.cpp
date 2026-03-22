/****************************************************************************
** Meta object code from reading C++ file 'controlpanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../gui/controlpanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'controlpanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ControlPanel_t {
    QByteArrayData data[26];
    char stringdata0[276];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ControlPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ControlPanel_t qt_meta_stringdata_ControlPanel = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ControlPanel"
QT_MOC_LITERAL(1, 13, 13), // "paramsChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "roll"
QT_MOC_LITERAL(4, 33, 5), // "pitch"
QT_MOC_LITERAL(5, 39, 3), // "yaw"
QT_MOC_LITERAL(6, 43, 2), // "tx"
QT_MOC_LITERAL(7, 46, 2), // "ty"
QT_MOC_LITERAL(8, 49, 2), // "tz"
QT_MOC_LITERAL(9, 52, 13), // "waypointFrame"
QT_MOC_LITERAL(10, 66, 21), // "std::vector<tf::Mat4>"
QT_MOC_LITERAL(11, 88, 9), // "waypoints"
QT_MOC_LITERAL(12, 98, 8), // "tf::Mat4"
QT_MOC_LITERAL(13, 107, 9), // "T_current"
QT_MOC_LITERAL(14, 117, 8), // "segIndex"
QT_MOC_LITERAL(15, 126, 15), // "onSliderChanged"
QT_MOC_LITERAL(16, 142, 8), // "resetAll"
QT_MOC_LITERAL(17, 151, 15), // "toggleAnimation"
QT_MOC_LITERAL(18, 167, 11), // "animateStep"
QT_MOC_LITERAL(19, 179, 11), // "addWaypoint"
QT_MOC_LITERAL(20, 191, 14), // "removeWaypoint"
QT_MOC_LITERAL(21, 206, 14), // "clearWaypoints"
QT_MOC_LITERAL(22, 221, 13), // "playWaypoints"
QT_MOC_LITERAL(23, 235, 12), // "waypointStep"
QT_MOC_LITERAL(24, 248, 11), // "applyMatrix"
QT_MOC_LITERAL(25, 260, 15) // "applyQuaternion"

    },
    "ControlPanel\0paramsChanged\0\0roll\0pitch\0"
    "yaw\0tx\0ty\0tz\0waypointFrame\0"
    "std::vector<tf::Mat4>\0waypoints\0"
    "tf::Mat4\0T_current\0segIndex\0onSliderChanged\0"
    "resetAll\0toggleAnimation\0animateStep\0"
    "addWaypoint\0removeWaypoint\0clearWaypoints\0"
    "playWaypoints\0waypointStep\0applyMatrix\0"
    "applyQuaternion"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ControlPanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    6,   79,    2, 0x06 /* Public */,
       9,    3,   92,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    0,   99,    2, 0x08 /* Private */,
      16,    0,  100,    2, 0x08 /* Private */,
      17,    0,  101,    2, 0x08 /* Private */,
      18,    0,  102,    2, 0x08 /* Private */,
      19,    0,  103,    2, 0x08 /* Private */,
      20,    0,  104,    2, 0x08 /* Private */,
      21,    0,  105,    2, 0x08 /* Private */,
      22,    0,  106,    2, 0x08 /* Private */,
      23,    0,  107,    2, 0x08 /* Private */,
      24,    0,  108,    2, 0x08 /* Private */,
      25,    0,  109,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double,    3,    4,    5,    6,    7,    8,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 12, QMetaType::Int,   11,   13,   14,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ControlPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ControlPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->paramsChanged((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4])),(*reinterpret_cast< double(*)>(_a[5])),(*reinterpret_cast< double(*)>(_a[6]))); break;
        case 1: _t->waypointFrame((*reinterpret_cast< const std::vector<tf::Mat4>(*)>(_a[1])),(*reinterpret_cast< const tf::Mat4(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->onSliderChanged(); break;
        case 3: _t->resetAll(); break;
        case 4: _t->toggleAnimation(); break;
        case 5: _t->animateStep(); break;
        case 6: _t->addWaypoint(); break;
        case 7: _t->removeWaypoint(); break;
        case 8: _t->clearWaypoints(); break;
        case 9: _t->playWaypoints(); break;
        case 10: _t->waypointStep(); break;
        case 11: _t->applyMatrix(); break;
        case 12: _t->applyQuaternion(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ControlPanel::*)(double , double , double , double , double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControlPanel::paramsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ControlPanel::*)(const std::vector<tf::Mat4> & , const tf::Mat4 & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControlPanel::waypointFrame)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ControlPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ControlPanel.data,
    qt_meta_data_ControlPanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ControlPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ControlPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ControlPanel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ControlPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void ControlPanel::paramsChanged(double _t1, double _t2, double _t3, double _t4, double _t5, double _t6)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ControlPanel::waypointFrame(const std::vector<tf::Mat4> & _t1, const tf::Mat4 & _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
