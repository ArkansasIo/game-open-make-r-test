QT += core xml
TEMPLATE = app
TARGET = openrpgmaker_engine_demo

CONFIG += console
CONFIG -= app_bundle

SOURCES += \
    battleengine.cpp \
    eventcompiler.cpp \
    eventruntime.cpp \
    gamedatabase.cpp \
    gamestate.cpp \
    main.cpp \
    tilemap.cpp

HEADERS += \
    battleengine.h \
    eventcompiler.h \
    eventruntime.h \
    gamedatabase.h \
    gamestate.h \
    tilemap.h
