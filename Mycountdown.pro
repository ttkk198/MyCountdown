QT += core gui multimedia texttospeech
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mycountdown
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        person.cpp

HEADERS += \
        mainwindow.h \
        person.h

FORMS += \
        mainwindow.ui

photo.files = $$PWD/photos
photo.path = $$OUT_PWD/photos
INSTALLS += photo


RESOURCES += resources.qrc
