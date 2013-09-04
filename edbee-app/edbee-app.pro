
QT += core gui
QT -= sql
QT += widgets

TARGET = edbee
TEMPLATE = app

RC_FILE = main.rc
ICON = images/icon.icns

INCLUDEPATH += $$PWD/../config


SOURCES +=\
    main.cpp \
    ui/mainwindow.cpp \
    ui/filetreesidewidget.cpp \
    application.cpp \
    ui/gotowidget.cpp \
    ui/findwidget.cpp

HEADERS  += \
	config.h \
    ui/mainwindow.h \
    ui/filetreesidewidget.h \
    application.h \
    ui/gotowidget.h \
    ui/findwidget.h


RESOURCES += \
	main.qrc

OTHER_FILES += \
	images/icon.icns \
	images/icon.ico \
	images/icon.png


## Install all app data files to the application bundle
## TODO: We need to find a way to copy these files next to the exe file on windows (And later we need to check linux)
APP_DATA_FILES.files = $$files(../edbee-data/*)
APP_DATA_FILES.path = Contents/Resources

QMAKE_BUNDLE_DATA += APP_DATA_FILES


# in the clean step also delete the .app file
CLEAN_EXTRA_DIRS = edbee.app
QMAKE_CLEAN += -r $$CLEAN_EXTRA_DIRS


#QMAKE_INFO_PLIST    = Info.plist

OTHER_FILES += ../edbee-data/keymaps/*
OTHER_FILES += ../edbee-data/syntaxfiles/*


## Extra dependencies
##====================
include(../vendor/qslog/QsLog.pri)
include(../vendor/qtawesome/QtAwesome.pri)


## edbee-lib dependency
##=======================

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../edbee-lib/release/ -ledbee-lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../edbee-lib/debug/ -ledbee-lib
else:unix:!symbian: LIBS += -L$$OUT_PWD/../edbee-lib/ -ledbee-lib

INCLUDEPATH += $$PWD/../edbee-lib
DEPENDPATH += $$PWD/../edbee-lib

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../edbee-lib/release/edbee-lib.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../edbee-lib/debug/edbee-lib.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../edbee-lib/libedbee-lib.a
