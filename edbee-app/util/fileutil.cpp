/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "fileutil.h"

#include <QDesktopServices>
#include <QFile>
#include <QPRocess>
#include <QUrl>

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


/// This method reveals the given path/file in the operating system file browser (Finder/Explorer)
/// We need to implement a linux version for this
/// There's no garantee this method will work, it just forks a process
void FileUtil::revealInOSFileBrowser(const QString& path)
{
    QStringList args;
    bool fallbackToOpenFolder = true;
#ifdef Q_OS_MAC
    args << "-e" << "tell application \"Finder\"";
    args << "-e" << QString("reveal POSIX file \"%1\"").arg(path);
    args << "-e" << "activate";
    args << "-e" << "end tell";
    fallbackToOpenFolder  = !QProcess::startDetached("osascript", args);

#elif defined(Q_OS_WIN)
    if (!QFileInfo(path).isDir()) {
        args <<  QLatin1String("/select,");
    }
    args << QDir::toNativeSeparators( path );
    fallbackToOpenFolder = !QProcess::startDetached("explorer", args);
#endif

    // when the open failed, simply open the folder of the given file
    // this can be done with standard QDesktopSerivces unctionality
    if( fallbackToOpenFolder ) {
        QString folder;
        QFileInfo fileInfo(path);
        if( fileInfo.isDir() ) {
            folder = path;
        } else {
            folder = fileInfo.path();
        }
        if( !folder.isEmpty() ) {
            QDesktopServices::openUrl( QUrl::fromLocalFile(folder) );
        }
    }

}
