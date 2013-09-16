/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "edbeeconfigtest.h"

#include "edbee/util/cascadingqvariantmap.h"
#include "models/edbeeconfig.h"

#include "debug.h"


void EdbeeConfigTest::testAddFile()
{
    EdbeeConfig config;

    testTrue( config.configMap() == 0 );

    config.addFile("filename1.txt");
    edbee::CascadingQVariantMap* rootMap = config.configMap();
    testTrue( rootMap != 0 );

    config.addFile("filename2.txt");
    edbee::CascadingQVariantMap* subMap = config.configMap();
    testTrue( subMap != 0  );
    testTrue( subMap != rootMap );
    testTrue( subMap->parent() == rootMap );
    testTrue( rootMap->parent() == 0 );
}
