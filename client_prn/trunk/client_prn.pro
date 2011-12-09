DEFINES = D_MYDEBUG
QT += network \
    sql
TARGET = client_prn
CONFIG -= app_bundle
CONFIG += precompile_header \
    warn_on \
    console \
    debug
TEMPLATE = app
GLOBAL_DIR = ../../common/trunk
BUILD_DIR = ../../build
EXTERN_DIR = ../../extern_src
IFACE_DIR = ../../interfaces/
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
    $${IFACE_DIR}
HEADERS += $${GLOBAL_DIR}/mytypes.h \
    $${GLOBAL_DIR}/message.h \
    $${GLOBAL_DIR}/config.h \
    $${GLOBAL_DIR}/networkclient.h \
    $${GLOBAL_DIR}/mytypes.h \
    $${GLOBAL_DIR}/commands.h \
    $${GLOBAL_DIR}/myscene.h \
    $${GLOBAL_DIR}/mytextitem.h \
    $${GLOBAL_DIR}/picitem.h \
    $${IFACE_DIR}/igs_plugin.h \
    $${IFACE_DIR}/iauth_plugin.h \
    mainwindow.h \
    intropage.h \
    editcardpage.h \
    previewpage.h \
    finishpage.h \
    controller.h \
    elogger.h \
    fakenetans.h \
    templatesdatagateway.h \
    selectway.h \
    viewport.h \
    infowindow.h \
    printtask.h \
    prnsenddlg.h
SOURCES += main.cpp \
    $${GLOBAL_DIR}/message.cpp \
    $${GLOBAL_DIR}/networkclient.cpp \
    $${GLOBAL_DIR}/myscene.cpp \
    $${GLOBAL_DIR}/mytextitem.cpp \
    $${GLOBAL_DIR}/picitem.cpp \
    mainwindow.cpp \
    intropage.cpp \
    editcardpage.cpp \
    previewpage.cpp \
    finishpage.cpp \
    controller.cpp \
    elogger.cpp \
    fakenetans.cpp \
    templatesdatagateway.cpp \
    selectway.cpp \
    viewport.cpp \
    infowindow.cpp \
    printtask.cpp \
    prnsenddlg.cpp
RESOURCES = images.qrc
TRANSLATIONS = client_prn_ru.ts
FORMS += prnsenddlg.ui
