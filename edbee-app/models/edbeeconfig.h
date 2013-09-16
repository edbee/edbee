/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QStringList>

namespace edbee {
    class CascadingQVariantMap;
}


/// A general class to load the 'nested' configuration
/// from the config direction
class EdbeeConfig
{
public:
    explicit EdbeeConfig();
    virtual ~EdbeeConfig();

    void addFile( const QString& fileName );

    edbee::CascadingQVariantMap *configMap();

private:

    QStringList configFileList_;                    ///< The configuration file names
    edbee::CascadingQVariantMap* configMap_;        ///< The build configmap

};
