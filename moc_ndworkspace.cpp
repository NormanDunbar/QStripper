/****************************************************************************
** Meta object code from reading C++ file 'ndworkspace.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ndworkspace.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ndworkspace.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NDWorkspace[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      30,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NDWorkspace[] = {
    "NDWorkspace\0\0tileVertically()\0"
    "tileHorozontally()\0"
};

void NDWorkspace::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NDWorkspace *_t = static_cast<NDWorkspace *>(_o);
        switch (_id) {
        case 0: _t->tileVertically(); break;
        case 1: _t->tileHorozontally(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData NDWorkspace::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NDWorkspace::staticMetaObject = {
    { &QWorkspace::staticMetaObject, qt_meta_stringdata_NDWorkspace,
      qt_meta_data_NDWorkspace, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NDWorkspace::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NDWorkspace::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NDWorkspace::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NDWorkspace))
        return static_cast<void*>(const_cast< NDWorkspace*>(this));
    return QWorkspace::qt_metacast(_clname);
}

int NDWorkspace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWorkspace::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
