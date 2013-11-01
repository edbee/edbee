/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "fileutil.h"

#include <QFile>

#include "debug.h"


/// blank destructor.
FileUtil::FileUtil()
{
}

/// This method generates a new filename with the given pattern
/// @param path the path to create the file in
/// @param pattern the pattern of the filename to create. This pattern should contain %1. %1 recieves the file-number
/// @return a new non existing filename. It returns QString() if the given path doesnt' exist or isn't a directory!
QString FileUtil::generateNewFilename(const QString& path, const QString& pattern)
{
    QFileInfo pathInfo( path );

    // make sure the given path exists and is aq directory
    if( !pathInfo.exists() || !pathInfo.isDir() ) {
        return QString();
    }

    // next generate a new filename
    int idx = 1;
    while( true ) {

        // build the filename
        QFileInfo newFile( QString("%1/%2").arg(path).arg( pattern.arg(idx) ) );
        if( !newFile.exists() ) {
            return newFile.absoluteFilePath();
        }

        // next index if the file exists
        ++idx;
    }
    // unreachable
}
