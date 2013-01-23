/****************************************************************************
** Meta object code from reading C++ file 'networkthread.h'
**
** Created: Tue Feb 2 17:46:05 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "networkthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'networkthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_networkthread[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_networkthread[] = {
    "networkthread\0\0OnShardList()\0"
};

const QMetaObject networkthread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_networkthread,
      qt_meta_data_networkthread, 0 }
};

const QMetaObject *networkthread::metaObject() const
{
    return &staticMetaObject;
}

void *networkthread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_networkthread))
        return static_cast<void*>(const_cast< networkthread*>(this));
    return QThread::qt_metacast(_clname);
}

int networkthread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnShardList(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void networkthread::OnShardList()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
