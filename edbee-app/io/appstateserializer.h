/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>


/// This class is used for storing the application state
///
/// This is to 'remember' the last opened state of the current workspace.
class AppStateSerializer
{
public:
    AppStateSerializer();
    virtual ~AppStateSerializer();

    bool saveState( const QString& fileName );
    bool loadState( const QString& fileName );

    QString errorMessage() const;

private:
    QString errorMessage_;                   ///< The last error message

};
