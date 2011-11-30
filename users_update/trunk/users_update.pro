QT       += core \
            network
QT       -= gui

TARGET = users_update
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += precompile_header \
            warn_on
CONFIG   +=debug

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
           $${GLOBAL_DIR}/message.h \
           $${GLOBAL_DIR}/config.h \
    pwdparser.h \
    netclient.h \
    netmsgsender.h

SOURCES += main.cpp \
           $${GLOBAL_DIR}/message.cpp \
    pwdparser.cpp \
    netclient.cpp \
    netmsgsender.cpp

TRANSLATIONS = users_update_ru.ts
