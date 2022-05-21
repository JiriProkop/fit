TARGET = square
CONFIG += c11
CONFIG -= qt

QMAKE_CC = gcc
QMAKE_LINK = gcc

unix {
    LIBS += -lm
}

INCLUDEPATH += include/

HEADERS += include/util.h
SOURCES += src/util.c \
           src/square.c

