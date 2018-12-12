/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      19,   11,   11,   11, 0x08,
      25,   11,   11,   11, 0x08,
      32,   11,   11,   11, 0x08,
      40,   11,   11,   11, 0x08,
      48,   11,   11,   11, 0x08,
      55,   11,   11,   11, 0x08,
      69,   11,   11,   11, 0x08,
      88,   11,   11,   11, 0x08,
     101,   11,   11,   11, 0x08,
     114,   11,   11,   11, 0x08,
     130,   11,   11,   11, 0x08,
     142,   11,   11,   11, 0x08,
     154,   11,   11,   11, 0x08,
     166,   11,   11,   11, 0x08,
     178,   11,   11,   11, 0x08,
     194,  189,   11,   11, 0x08,
     219,  212,   11,   11, 0x08,
     239,   11,   11,   11, 0x08,
     252,   11,   11,   11, 0x08,
     268,   11,   11,   11, 0x08,
     284,   11,   11,   11, 0x08,
     302,   11,   11,   11, 0x08,
     321,  314,   11,   11, 0x08,
     358,  352,   11,   11, 0x08,
     391,  352,   11,   11, 0x08,
     414,   11,   11,   11, 0x08,
     437,   11,  427,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0open()\0cut()\0copy()\0paste()\0"
    "about()\0help()\0updateMenus()\0"
    "updateWindowMenu()\0ExportText()\0"
    "ExportHTML()\0ExportDocbook()\0ExportPDF()\0"
    "ExportODF()\0ExportRST()\0ExportASC()\0"
    "TextBold()\0size\0TextSize(QString)\0"
    "family\0TextFamily(QString)\0TextItalic()\0"
    "TextUnderline()\0TextSubScript()\0"
    "TextSuperScript()\0FilePrint()\0Format\0"
    "FormatChanged(QTextCharFormat)\0event\0"
    "dragEnterEvent(QDragEnterEvent*)\0"
    "dropEvent(QDropEvent*)\0openRecent()\0"
    "MdiChild*\0createMdiChild()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->open(); break;
        case 1: _t->cut(); break;
        case 2: _t->copy(); break;
        case 3: _t->paste(); break;
        case 4: _t->about(); break;
        case 5: _t->help(); break;
        case 6: _t->updateMenus(); break;
        case 7: _t->updateWindowMenu(); break;
        case 8: _t->ExportText(); break;
        case 9: _t->ExportHTML(); break;
        case 10: _t->ExportDocbook(); break;
        case 11: _t->ExportPDF(); break;
        case 12: _t->ExportODF(); break;
        case 13: _t->ExportRST(); break;
        case 14: _t->ExportASC(); break;
        case 15: _t->TextBold(); break;
        case 16: _t->TextSize((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->TextFamily((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->TextItalic(); break;
        case 19: _t->TextUnderline(); break;
        case 20: _t->TextSubScript(); break;
        case 21: _t->TextSuperScript(); break;
        case 22: _t->FilePrint(); break;
        case 23: _t->FormatChanged((*reinterpret_cast< const QTextCharFormat(*)>(_a[1]))); break;
        case 24: _t->dragEnterEvent((*reinterpret_cast< QDragEnterEvent*(*)>(_a[1]))); break;
        case 25: _t->dropEvent((*reinterpret_cast< QDropEvent*(*)>(_a[1]))); break;
        case 26: _t->openRecent(); break;
        case 27: { MdiChild* _r = _t->createMdiChild();
            if (_a[0]) *reinterpret_cast< MdiChild**>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
