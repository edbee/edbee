/*
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QDir>

#include "QsLog.h"
#include "QsLogDest.h"

#include "application.h"
#include "ui/mainwindow.h"

#include "debug.h"


int main(int argc, char* argv[])
{
    Application qapp(argc, argv);
    qapp.setApplicationName( "edbee - A sample application for the edbee component" );
    qapp.setApplicationVersion( "0.1");
    qapp.setOrganizationName( "edbee" );
    qapp.setOrganizationDomain( "edbee.net" );


    // init the logging mechanism
    QsLogging::Logger& logger = QsLogging::Logger::instance();

//    const QString sLogPath(QDir(qApp->applicationDirPath()).filePath("log.txt"));
//    static QsLogging::DestinationPtr fileDestination(  QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
    static QsLogging::DestinationPtr debugDestination( QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination.get());
//    logger.addDestination(fileDestination.get());
    logger.setLoggingLevel(QsLogging::TraceLevel);

    // parse the syntax files
    qapp.initApplication();

    // show the main window
    MainWindow mainWindow;
    mainWindow.show();
    return qapp.exec();
}
