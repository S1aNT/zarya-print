QT += sql \
    network
TEMPLATE += app
TARGET = surd_gui
CONFIG += warn_on \
    qt \
    precompile_header
GLOBAL_DIR = ../../common/trunk
BUILD_DIR = ../../build
EXTERN_DIR = ../../extern_src
message (Path $${EXTERN_DIR}/qt_single_apps/qtsingleapplication.pri)
win32:DESTDIR = c:/opt/zarya
else:DESTDIR = /opt/zarya
CONFIG(debug, debug|release) { 
    message(Build Debug!)
    RCC_DIR = $${BUILD_DIR}/$${TARGET}/debug/rcc
    MOC_DIR = $${BUILD_DIR}/$${TARGET}/debug/moc
    OBJECTS_DIR = $${BUILD_DIR}/$${TARGET}/debug/obj
    DESTDIR = $${DESTDIR}/debug
}
else { 
    message(Build Release!)
    RCC_DIR = $${BUILD_DIR}/$${TARGET}/release/rcc
    MOC_DIR = $${BUILD_DIR}/$${TARGET}/release/moc
    OBJECTS_DIR = $${BUILD_DIR}/$${TARGET}/release/obj
    DESTDIR = $${DESTDIR}/release
}
LANGUAGE = C++
include($${EXTERN_DIR}/qt_single_apps/qtsingleapplication.pri)

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = $${GLOBAL_DIR}/pre_header.h
DEPENDPATH += .
INCLUDEPATH += $${GLOBAL_DIR} \
    $${EXTERN_DIR}
HEADERS += mainwindow.h \
    $${GLOBAL_DIR}/mytypes.h \
    $${GLOBAL_DIR}/message.h \
    $${GLOBAL_DIR}/config.h \
    qledindicator.h \
    engine.h \
    networkclient.h \
    form.h \
    exttreeview.h \
    editprinterproperty.h
SOURCES += main.cpp \
    mainwindow.cpp \
    $${GLOBAL_DIR}/message.cpp \
    qledindicator.cpp \
    engine.cpp \
    networkclient.cpp \
    form.cpp \
    exttreeview.cpp \
    editprinterproperty.cpp
RESOURCES = images.qrc
TRANSLATIONS = auditor_ru.ts
FORMS += form.ui
