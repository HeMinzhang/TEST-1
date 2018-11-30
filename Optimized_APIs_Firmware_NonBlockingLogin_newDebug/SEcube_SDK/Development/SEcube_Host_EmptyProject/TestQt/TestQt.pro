QT += core
QT -= gui

CONFIG += c++11

TARGET = TestQt
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../secube-host/aes256.c \
    ../secube-host/crc16.c \
    ../secube-host/L0.c \
    ../secube-host/L1.c \
    ../secube-host/pbkdf2.c \
    ../secube-host/se3_common.c \
    ../secube-host/se3comm.c \
    ../secube-host/sha256.c

HEADERS += \
    ../secube-host/aes256.h \
    ../secube-host/crc16.h \
    ../secube-host/L0.h \
    ../secube-host/L1.h \
    ../secube-host/pbkdf2.h \
    ../secube-host/se3_common.h \
    ../secube-host/se3c0def.h \
    ../secube-host/se3c1def.h \
    ../secube-host/se3comm.h \
    ../secube-host/sha256.h
