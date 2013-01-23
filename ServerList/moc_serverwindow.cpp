/****************************************************************************
** Meta object code from reading C++ file 'serverwindow.h'
**
** Created: Tue Feb 2 17:46:04 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "serverwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serverwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ServerWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      20,   14,   13,   13, 0x08,
      56,   13,   13,   13, 0x08,
      83,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ServerWindow[] = {
    "ServerWindow\0\0index\0"
    "on_cbx_div_currentIndexChanged(int)\0"
    "on_action_Quit_triggered()\0on_shard_list()\0"
};

const QMetaObject ServerWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ServerWindow,
      qt_meta_data_ServerWindow, 0 }
};

const QMetaObject *ServerWindow::metaObject() const
{
    return &staticMetaObject;
}

void *ServerWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ServerWindow))
        return static_cast<void*>(const_cast< ServerWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ServerWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_cbx_div_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: on_action_Quit_triggered(); break;
        case 2: on_shard_list(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
