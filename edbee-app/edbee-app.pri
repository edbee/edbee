QT += core gui
QT -= sql
QT += widgets

TARGET = edbee
TEMPLATE = app


RC_FILE = main.rc
ICON = images/icon.icns

# This seems to be required for Windows
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD


SOURCES +=\
    main.cpp \
    ui/mainwindow.cpp \
    ui/filetreesidewidget.cpp \
    application.cpp \
    ui/gotowidget.cpp \
    ui/findwidget.cpp \
    models/edbeeconfig.cpp \
    ui/windowmanager.cpp \
    models/workspace.cpp \
    io/workspaceserializer.cpp \
    commands/revealinsidebarcommand.cpp \
    util/fileutil.cpp \
    ui/tabwidget.cpp \
    models/workspacemanager.cpp \
    io/appstateserializer.cpp

HEADERS  += \
    ui/mainwindow.h \
    ui/filetreesidewidget.h \
    application.h \
    ui/gotowidget.h \
    ui/findwidget.h \
    models/edbeeconfig.h \
    ui/windowmanager.h \
    models/workspace.h \
    io/workspaceserializer.h \
    commands/revealinsidebarcommand.h \
    util/fileutil.h \
    ui/tabwidget.h \
    models/workspacemanager.h \
    io/appstateserializer.h


RESOURCES += \
	main.qrc

OTHER_FILES += \
	images/icon.icns \
	images/icon.ico \
	images/icon.png

#QMAKE_INFO_PLIST    = Info.plist
OTHER_FILES += ../edbee-data/config/*
OTHER_FILES += ../edbee-data/keymaps/*
OTHER_FILES += ../edbee-data/syntaxfiles/*
OTHER_FILES += ../edbee-data/themes/*


# in the clean step also delete the .app file
CLEAN_EXTRA_DIRS = edbee.app
QMAKE_CLEAN += -r $$CLEAN_EXTRA_DIRS


## Extra data files
##==================

## Install all app data files to the application bundle
## TODO: We need to find a way to copy these files next to the exe file on windows (And later we need to check linux)
APP_DATA_FILES.files = $$files(../edbee-data/*)
APP_DATA_FILES.path = Contents/Resources

QMAKE_BUNDLE_DATA += APP_DATA_FILES

win32 {
    DATA_SOURCE_PATH=$$PWD/../edbee-data

    Release:DATA_TARGET_PATH = $$OUT_PWD/release/data
    Debug:DATA_TARGET_PATH = $$OUT_PWD/debug/data

    win32:DATA_SOURCE_PATH ~= s,/,\\,g
    win32:DATA_TARGET_PATH ~= s,/,\\,g
    copyfiles.commands = $$QMAKE_COPY_DIR $$DATA_SOURCE_PATH $$DATA_TARGET_PATH
}

QMAKE_EXTRA_TARGETS += copyfiles
POST_TARGETDEPS += copyfiles


## Extra dependencies
##====================
#include(../vendor/qslog/QsLog.pri)
include(../vendor/qtawesome/QtAwesome.pri)

