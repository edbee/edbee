
QT  += core gui
QT  -= sql
QT  += widgets

TARGET = edbee-app-test
TEMPLATE = app


# This seems to be required for Windows
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
DEFINES += QT_NODLL


# The test sources
SOURCES += \
	main.cpp \
    models/edbeeconfigtest.cpp


# manually add the sources that need testing
SOURCES += \
	..\edbee-app\models\edbeeconfig.cpp



HEADERS += \
	models/edbeeconfigtest.h

INCLUDEPATH += $$PWD/../edbee-app

## Extra dependencies
##====================
include(../vendor/qslog/QsLog.pri)


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
