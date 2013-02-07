TEMPLATE = app
TARGET = tst_language
DESTDIR = ../../../bin
INCLUDEPATH += ../../../src/lib/

QT = core testlib
CONFIG += depend_includepath testcase
CONFIG   += console
CONFIG   -= app_bundle

SOURCES += \
    tst_language.cpp

include(../../../src/lib/use.pri)
include(../../../src/app/shared/logging/logging.pri)
