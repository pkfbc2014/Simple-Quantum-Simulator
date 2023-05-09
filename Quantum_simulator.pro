QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dataset.cpp \
    example1.cpp \
    example2.cpp \
    example3.cpp \
    main.cpp \
    resultform.cpp \
    widget.cpp

HEADERS += \
    dataset.h \
    example1.h \
    example2.h \
    example3.h \
    resultform.h \
    widget.h

FORMS += \
    example1.ui \
    example2.ui \
    example3.ui \
    resultform.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    gates/images.qrc

DISTFILES += \
    Icon.ico \
    myIcon.ico \
    opera.py

RC_ICONS = Icon.ico

win32: LIBS += -LE:/Anaconda-3.9/libs/ -lpython39

INCLUDEPATH += E:/Anaconda-3.9/libs
DEPENDPATH += E:/Anaconda-3.9/libs

INCLUDEPATH += E:/Anaconda-3.9/include
DEPENDPATH += E:/Anaconda-3.9/include

win32:!win32-g++: PRE_TARGETDEPS += E:/Anaconda-3.9/libs/python39.lib
# else:win32-g++: PRE_TARGETDEPS += E:/Anaconda-3.9/libs/libpython39.a
