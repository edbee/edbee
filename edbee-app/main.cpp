/*
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QDir>

#include "QsLog.h"
#include "QsLogDest.h"

#include "application.h"

#include "debug.h"


int main(int argc, char* argv[])
{
    Application app(argc, argv);
    app.setApplicationName( "edbee" );
    app.setApplicationVersion( "0.1");
    app.setOrganizationName( "edbee" );
    app.setOrganizationDomain( "edbee.net" );
    app.setApplicationDisplayName("edbee - a sample application for the edbee component");

    // init the logging mechanism
    QsLogging::Logger& logger = QsLogging::Logger::instance();

//    const QString sLogPath(QDir(qApp->applicationDirPath()).filePath("log.txt"));
//    static QsLogging::DestinationPtr fileDestination(  QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
    static QsLogging::DestinationPtr debugDestination( QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination.get());
//    logger.addDestination(fileDestination.get());
    logger.setLoggingLevel(QsLogging::TraceLevel);

    // initialize the application (parsing syntax files, load configuration et)
    app.initApplication();

    // run the application
    int result = app.exec();

    // perform a shutdown and return the result
    app.shutdown();
    return result;
}
