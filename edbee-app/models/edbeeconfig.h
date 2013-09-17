/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QStringList>

namespace edbee {
    class CascadingQVariantMap;
    class TextEditorConfig;
}


/// A general class to load the 'nested' configuration
/// from the config direction
class EdbeeConfig
{
public:
    explicit EdbeeConfig();
    virtual ~EdbeeConfig();

    void addFile( const QString& fileName, bool createIfNotExists=false );

    bool loadConfig();
    void fillEditorConfig( edbee::TextEditorConfig* config ) const;

    edbee::CascadingQVariantMap* configMap() const;

    int fileCount() const;
    QString file( int idx ) const;
    QString loadMessageForFile( int idx ) const;

private:

    QStringList configFileList_;                           ///< The configuration file names
    QList<edbee::CascadingQVariantMap*> configMapList_;    ///< The list of config-maps (internally these maps list to eachother)
    QStringList loadMessageList_;                          ///< A list with parse results

};
