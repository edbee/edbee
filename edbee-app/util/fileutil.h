/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>

/// A file util class, a collection of file-related operations
class FileUtil
{
public:
    FileUtil();

    QString generateNewFilename( const QString& path, const QString& pattern );
};
