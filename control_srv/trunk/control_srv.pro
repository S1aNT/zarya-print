QT += core \
    network \
    sql
QT -= gui
TARGET = control_srv
CONFIG += console
CONFIG += debug
CONFIG += precompile_header \
    warn_on
TEMPLATE = app
GLOBAL_DIR =../../common/trunk
BUILD_DIR  =../../build
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

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = $${GLOBAL_DIR}/pre_header.h
DEPENDPATH += .
INCLUDEPATH += $${GLOBAL_DIR}
HEADERS += $${GLOBAL_DIR}/mytypes.h \
	   $${GLOBAL_DIR}/config.h \
           $${GLOBAL_DIR}/message.h \
           errorlogger.h \
           localserver.h \
           controlsrv.h \
    msgdispatcher.h \
    db_gateway.h

SOURCES += main.cpp \
           $${GLOBAL_DIR}/message.cpp \
           errorlogger.cpp \
           localserver.cpp \
           controlsrv.cpp \
    msgdispatcher.cpp \
    db_gateway.cpp

TRANSLATIONS = control_srv_ru.ts
include(../../extern_src/qtservice-2.6_1-opensource/src/qtservice.pri)
