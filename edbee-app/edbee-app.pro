
# This example demonstrates the use of the edbee component as a library dependency

include($$PWD/edbee-app.pri)


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


