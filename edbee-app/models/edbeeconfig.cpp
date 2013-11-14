/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "edbeeconfig.h"

#include "edbee/io/jsonparser.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/util/cascadingqvariantmap.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/texteditorwidget.h"

#include "debug.h"


/// The edbee configuration constructor
EdbeeConfig::EdbeeConfig()
{
}


/// When destructing the config the cascading variant is destroyed
EdbeeConfig::~EdbeeConfig()
{
    qDeleteAll( configFileItemList_ );
}


/// Adds a config file to the edbee configuration
/// The configuration aren't loaded directly
/// @param fileName the name of the file
/// @param configFileMode the filemode of the config file (Autocreate, Optional or WarnIfMissing)
void EdbeeConfig::addFile(const QString& fileName, ConfigFileMode configFileMode )
{
    EdbeeConfigFileItem* cfi = new EdbeeConfigFileItem(fileName,configFileMode, this->configMap() );
    configFileItemList_.push_back(cfi);
    if( configFileMode == AutoCreate ) {
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
/// This method returns true on success, false on error
bool EdbeeConfig::loadConfig()
{
    // result handling
    bool result = true;

    // read all variant-maps from the config files
    foreach( EdbeeConfigFileItem* cfi, configFileItemList_ ) {
        cfi->setLoadMessage(QString()); // clear the load messsage

        // create the parser and if the file exist parse the content
        edbee::JsonParser parser;
        if( QFile::exists( cfi->file() ) ) {
            if( parser.parse( cfi->file() ) ) {
                cfi->configMap()->setQVariantMap( parser.result().toMap() );
            } else {
                cfi->setLoadMessage( parser.fullErrorMessage() );
                result = false;
            }
        } else {
            // if it isn't an optional file, add an error mesage
            if( cfi->mode() != Optional ) {
                cfi->setLoadMessage( QObject::tr("File %1 not found!").arg(cfi->file()) );
                result = false;
            }
        }
    }
    return result;
}


/// This method fills the editor-config with the configuration of this objec
void EdbeeConfig::fillEditorConfig(edbee::TextEditorConfig* config) const
{
    const edbee::CascadingQVariantMap* map = configMap();
    config->beginChanges();
    config->setCaretBlinkRate( map->intValue("caret_blink_rate",700) );
    config->setCaretWidth( map->intValue("caret_width",2) );
    config->setCharGroups( QStringList(map->stringValue("char_groups")) );          // this should use character groups
    config->setExtraLineSpacing( map->intValue("extra_line_spacing", 0 ));
    config->setIndentSize( map->intValue("indent_size",4) );
    config->setSmartTab( map->boolValue("smart_tab",true) );                        // returns the smarttab value
    //config->setLineSeperatorPen( );
    config->setShowCaretOffset( map->boolValue("show_caret_offset",true));
    config->setThemeName( map->stringValue("theme", "Monokai") );
    config->setUndoGroupPerSpace( map->boolValue("undo_group_per_space",true) );
    config->setUseLineSeparator( map->boolValue( "use_line_separator", false) );
    config->setUseTabChar( map->boolValue( "use_tab", true ));
    QFont font( map->stringValue("font","Monospace"), map->intValue("font_size",12) );
    config->setScrollPastEnd( map->boolValue("scroll_past_end",false) );
    config->setShowWhitespaceMode( map->stringValue("show_whitespace_mode","hide") );
    config->setFont(font);
    config->endChanges();
}


/// Applies the current config to the given widget
void EdbeeConfig::applyToWidget(edbee::TextEditorWidget* widget) const
{
    fillEditorConfig( widget->config() );
}


/// returns the qvariant map
edbee::CascadingQVariantMap* EdbeeConfig::configMap() const
{
    if( configFileItemList_.empty() ) return 0;
    return configFileItemList_.last()->configMap();
}


/// Retursn the number of files in this configuration
int EdbeeConfig::fileCount() const
{
    return configFileItemList_.size();
}


/// Returns the filename at the given index
/// @param idx the index of the file to retrieve
QString EdbeeConfig::file(int idx) const
{
    return configFileItemList_.at(idx)->file();
}


/// Returns the load message for the given file
/// The load message will be null-string if the load was successfull
/// @param idx the index of the message to retrieve
/// @return the message for the loading of the given file QString() if successful
QString EdbeeConfig::loadMessageForFile(int idx) const
{
    Q_ASSERT(idx < configFileItemList_.size() );
    return configFileItemList_.at(idx)->loadMessage();
}


/// Returns the autoreveal value
/// When auto-revealing is enabled the sidebar automaticly reveals the current tab
bool EdbeeConfig::autoReveal() const
{
    return configMap()->boolValue("auto_reveal",false);
}


//----------------------------------------------


/// Constructs an config item
/// @param file the configuration filename
/// @param mode the mode how to handle this file
/// @param parentConfigFile the parennt config file
EdbeeConfigFileItem::EdbeeConfigFileItem(const QString& file, EdbeeConfig::ConfigFileMode mode, edbee::CascadingQVariantMap* parentConfigMap )
    : mode_(mode)
    , file_(file)
    , configMap_(0)
{
    configMap_ = new edbee::CascadingQVariantMap( parentConfigMap );
}


/// The config file destrucotor
EdbeeConfigFileItem::~EdbeeConfigFileItem()
{
    delete configMap_;
}


/// returns the file mode of this item
EdbeeConfig::ConfigFileMode EdbeeConfigFileItem::mode() const
{
    return mode_;
}


/// Retursn the filename
QString EdbeeConfigFileItem::file() const
{
    return file_;
}


/// Sets the loading result message
/// @param str the message of this file
void EdbeeConfigFileItem::setLoadMessage(const QString& str)
{
    loadMessage_ = str;
}


/// Returns the error/load message. When the result is QString() there's no message
QString EdbeeConfigFileItem::loadMessage() const
{
    return loadMessage_;
}


/// Returns the configmap
edbee::CascadingQVariantMap *EdbeeConfigFileItem::configMap()
{
    return configMap_;
}
