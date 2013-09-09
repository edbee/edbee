
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
    ui/findwidget.cpp

HEADERS  += \
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

#QMAKE_INFO_PLIST    = Info.plist
OTHER_FILES += ../edbee-data/keymaps/*
OTHER_FILES += ../edbee-data/syntaxfiles/*


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

# Copies the given files to the destination directory
#defineTest(copyToDestdir) {
#    files = $$1

#    for(FILE, files) {
#        DDIR = $$OUT_PWD

#        # Replace slashes in paths with backslashes for Windows
#        win32:FILE ~= s,/,\\,g
#        win32:DDIR ~= s,/,\\,g

#        MYFILECOPY += @echo "Copying $$FILE" $$escape_expand(\\n\\t)

#        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
#    }

#    export(QMAKE_POST_LINK)
#}

#win32 {
#    copyToDestdir( $$files(../edbee-data/*/*) )
#}

win32 {
    DATA_SOURCE_PATH=$$PWD/../edbee-data

    Release:DATA_TARGET_PATH = $$OUT_PWD/release/data
    Debug:DATA_TARGET_PATH = $$OUT_PWD/debug/data

    win32:DATA_SOURCE_PATH ~= s,/,\\,g
    win32:DATA_TARGET_PATH ~= s,/,\\,g
    copyfiles.commands = $$QMAKE_COPY_DIR $$DATA_SOURCE_PATH $$DATA_TARGET_PATH
}
macx {
#    copyfiles.commands = cp <from> <to>
}
QMAKE_EXTRA_TARGETS += copyfiles
POST_TARGETDEPS += copyfiles



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

