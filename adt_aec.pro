#-------------------------------------------------
#
# Project created by QtCreator 2016-06-06T20:45:54
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = adt_aec
#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app

target.files = adt_aec
target.path =/opt/aec

INSTALLS += target

#LIBS    +=-L./lib -lmediastreamer_base
LIBS    +=-L./lib -lADT_AECG4_cortex-a9_V5_04_03_hf_fpic -ladt_agc_v3_07_01_hf_fpic -ladt_nr2_V3_04_hf_fpic
LIBS    +=-L/opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/lib -lasound


INCLUDEPATH +=./common/include
INCLUDEPATH +=./include
INCLUDEPATH +=/opt/poky/1.5.3/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/include

SOURCES += main.cpp

HEADERS += \
    include/aecg4.h \
    include/iaecg4.h

DISTFILES += \
    adt_aec.conf
