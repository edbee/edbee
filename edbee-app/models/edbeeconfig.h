/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QStringList>

namespace edbee {
    class CascadingQVariantMap;
    class TextEditorConfig;
    class TextEditorWidget;
}


class EdbeeConfigFileItem;

/// A general class to load the 'nested' configuration
/// from the config direction
class EdbeeConfig
{
public:

    /// this enumeration is used to describe the configuration file mode handling
    enum ConfigFileMode {
        AutoCreate,         ///< Create the file if it doesn't exist, no warning
        Optional,           ///< Just skip the file if it doesn't exist
        WarnIfMissing       ///< Warn if the given file is missing
    };

    explicit EdbeeConfig();
    virtual ~EdbeeConfig();

    void addFile( const QString& fileName, ConfigFileMode configFileMode=WarnIfMissing );

    bool loadConfig();
    void fillEditorConfig( edbee::TextEditorConfig* config ) const;
    void applyToWidget( edbee::TextEditorWidget* widget ) const;

    edbee::CascadingQVariantMap* configMap() const;

    int fileCount() const;
    QString file( int idx ) const;
    QString loadMessageForFile( int idx ) const;

private:

    QList<EdbeeConfigFileItem*> configFileItemList_;       ///< A list with all config file items
};


/// An intenal class for representing a single config-file item
class EdbeeConfigFileItem
{
public:
    EdbeeConfigFileItem(const QString& file, EdbeeConfig::ConfigFileMode mode , edbee::CascadingQVariantMap *parentConfigMap);
    virtual ~EdbeeConfigFileItem();

    EdbeeConfig::ConfigFileMode mode() const;
    QString file() const;
    void setLoadMessage( const QString& str );
    QString loadMessage() const;

    edbee::CascadingQVariantMap* configMap();

private:

    EdbeeConfig::ConfigFileMode mode_;       ///< The configuration file mode
    QString file_;                           ///< The config file
    QString loadMessage_;                    ///< The configuration load message
    edbee::CascadingQVariantMap* configMap_; ///< The configuration map
};

