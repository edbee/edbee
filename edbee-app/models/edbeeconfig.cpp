/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "edbeeconfig.h"

#include "edbee/util/cascadingqvariantmap.h"

#include "debug.h"


EdbeeConfig::EdbeeConfig()
    : configMap_(0)
{
}


/// When destructing the config the cascading variant is destroyed
EdbeeConfig::~EdbeeConfig()
{
    if( configMap_ ) {
        configMap_->deleteParents();
        delete configMap_;
    }
}

/// Adds a config file to the edbee configuration
/// The configuration aren't loaded directly
void EdbeeConfig::addFile(const QString& fileName)
{
    configFileList_.push_back( fileName );
    configMap_ = new edbee::CascadingQVariantMap(configMap_ );
}

/// returns the qvariant map
edbee::CascadingQVariantMap* EdbeeConfig::configMap()
{
    return configMap_;
}
