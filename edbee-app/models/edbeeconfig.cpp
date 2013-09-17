/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "edbeeconfig.h"

#include "edbee/io/jsonparser.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/util/cascadingqvariantmap.h"

#include "debug.h"


EdbeeConfig::EdbeeConfig()
{
}


/// When destructing the config the cascading variant is destroyed
EdbeeConfig::~EdbeeConfig()
{
    qDeleteAll( configMapList_);
}

/// Adds a config file to the edbee configuration
/// The configuration aren't loaded directly
void EdbeeConfig::addFile(const QString& fileName, bool createIfNotExists )
{
    configFileList_.push_back( fileName );
    configMapList_.push_back( new edbee::CascadingQVariantMap( configMap() ) );
    if( createIfNotExists ) {
        QFile file(fileName);
        if( !file.exists() ) {
            if( file.open( QIODevice::WriteOnly ) ) {
                file.write("{\n\n}\n");   // create an empty config file
                file.close();
            }
        }
    }
}

/// Loads the configuration fom the files and places them in the configmap
bool EdbeeConfig::loadConfig()
{
    Q_ASSERT( configFileList_.size() == configMapList_.size() );

    // result handling
    bool result = true;
    loadMessageList_.clear();

    // read all variant-maps from the config files
    for( int i=0, cnt=configFileList_.size(); i<cnt; ++i ) {
        edbee::JsonParser parser;
        if( parser.parse(configFileList_.at(i)) ) {
            configMapList_.at(i)->setQVariantMap( parser.result().toMap() );
            loadMessageList_.push_back( QString() );                            // we would like the same amount of result texts as files
        } else {
            loadMessageList_.push_back( parser.fullErrorMessage() );
            result = false;
        }
    }
    return result;
}

/// This method fills the editor-config with the configuration of this objec
void EdbeeConfig::fillEditorConfig(edbee::TextEditorConfig* config) const
{
    const edbee::CascadingQVariantMap* map = configMap();
    config->setCaretBlinkRate( map->intValue("caret_blink_rate",700) );
    config->setCaretWidth( map->intValue("caret_width",2) );
    config->setCharGroups( QStringList(map->stringValue("char_groups")) );          /// this should use character groups
    config->setExtraLineSpacing( map->intValue("extra_line_spacing", 0 ));
    config->setIndentSize( map->intValue("indent_size",4) );
    //config->setLineSeperatorPen( );
    config->setShowCaretOffset( map->boolValue("show_caret_offset",true));
    config->setThemeName( map->stringValue("theme", "Monokai") );
    config->setUndoGroupPerSpace( map->boolValue("undo_group_per_space",true) );
    config->setUseLineSeparator( map->boolValue( "use_line_separator", false) );
    config->setUseTabChar( map->boolValue( "use_tab", true ));
}




/// returns the qvariant map
edbee::CascadingQVariantMap* EdbeeConfig::configMap() const
{
    if( configMapList_.isEmpty() ) return 0;
    return configMapList_.last();
}

/// Retursn the number of files in this configuration
int EdbeeConfig::fileCount() const
{
    return configFileList_.size();
}

/// Returns the filename at the given index
QString EdbeeConfig::file(int idx) const
{
    return configFileList_.at(idx);
}

/// Returns the load message for the given file
/// The load message will be null-string if the load was successfull
QString EdbeeConfig::loadMessageForFile(int idx) const
{
    Q_ASSERT(idx < loadMessageList_.size() );
    return loadMessageList_.at(idx);
}
