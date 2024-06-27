QT += core gui multimedia texttospeech
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mycountdown
TEMPLATE = app

SOURCES += \
        dateTimeWidget.cpp \
        main.cpp \
        mainwindow.cpp \
        person.cpp

HEADERS += \
        dateTimeWidget.h \
        mainwindow.h \
        person.h

FORMS += \
        mainwindow.ui

photo.files = $$PWD/photos
photo.path = $$OUT_PWD/photos
INSTALLS += photo


RESOURCES += resources.qrc
