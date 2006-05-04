/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Mon 24. Apr 13:32:22 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      19,   11,   11,   11, 0x08,
      26,   11,   11,   11, 0x08,
      35,   11,   11,   11, 0x08,
      41,   11,   11,   11, 0x08,
      48,   11,   11,   11, 0x08,
      56,   11,   11,   11, 0x08,
      64,   11,   11,   11, 0x08,
      78,   11,   11,   11, 0x08,
      97,   11,   11,   11, 0x08,
     110,   11,   11,   11, 0x08,
     123,   11,   11,   11, 0x08,
     149,   11,  139,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0open()\0save()\0saveAs()\0cut()\0copy()\0paste()\0about()\0"
    "updateMenus()\0updateWindowMenu()\0ExportText()\0ExportHTML()\0"
    "ExportDocbook()\0MdiChild*\0createMdiChild()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
	return static_cast<void*>(const_cast<MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: open(); break;
        case 1: save(); break;
        case 2: saveAs(); break;
        case 3: cut(); break;
        case 4: copy(); break;
        case 5: paste(); break;
        case 6: about(); break;
        case 7: updateMenus(); break;
        case 8: updateWindowMenu(); break;
        case 9: ExportText(); break;
        case 10: ExportHTML(); break;
        case 11: ExportDocbook(); break;
        case 12: { MdiChild* _r = createMdiChild();
            if (_a[0]) *reinterpret_cast< MdiChild**>(_a[0]) = _r; }  break;
        }
        _id -= 13;
    }
    return _id;
}
