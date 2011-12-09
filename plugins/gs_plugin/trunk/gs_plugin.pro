TEMPLATE = lib
QT -= gui

TARGET = gs_plugin
CONFIG += plugin \
    warn_on \
    precompile_header \
    debug
INTERFACE_DIR = ../../../interfaces
GLOBAL_DIR = ../../../common/trunk
BUILD_DIR = ../../../build

win32:DESTDIR = c:/opt/zarya/plugins
else:DESTDIR = /opt/zarya/plugins
CONFIG(debug, debug|release) {
message(Build Debug!)
    RCC_DIR = $${BUILD_DIR}/$${TARGET}/debug/rcc
    MOC_DIR =  $${BUILD_DIR}/$${TARGET}/debug/moc
    OBJECTS_DIR =  $${BUILD_DIR}/$${TARGET}/debug/obj
    mac|unix:TARGET = $$join(TARGET,,,_debug)
    win32:TARGET = $$join(TARGET,,d)
}
else {
    message(Build Release!)
    RCC_DIR =  $${BUILD_DIR}/$${TARGET}/release/rcc
    MOC_DIR =  $${BUILD_DIR}/$${TARGET}/release/moc
    OBJECTS_DIR =  $${BUILD_DIR}/$${TARGET}/release/obj
    }
                                        


INCLUDEPATH += $${INTERFACE_DIR} \
    	       $${GLOBAL_DIR}

LANGUAGE = C++

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = $${GLOBAL_DIR}/pre_header.h
HEADERS += $${GLOBAL_DIR}/pre_header.h \
    gs_plugin.h \
    $${INTERFACE_DIR}/igs_plugin.h \
    $${GLOBAL_DIR}/mytypes.h \
    $${GLOBAL_DIR}/proc_thread.h 

SOURCES = gs_plugin.cpp \
    $${GLOBAL_DIR}/proc_thread.cpp
