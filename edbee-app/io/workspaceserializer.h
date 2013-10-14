/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>
#include <QVariantMap>

class Application;
class MainWindow;
class Workspace;

namespace edbee {
    class TextEditorWidget;
}


/// The WorkspaceSerializer is used to save/load the workspace state
/// The state is stored as a plain json file
///
/// This is to 'remember' the last opened state of the current workspace.
///
/// I decided to not make a 'serializable' interface to offload the serialization complexibility
/// into this class. When serialization of certain objects gets complex seperate classes should be made
class WorkspaceSerializer
{
public:
    WorkspaceSerializer();
    virtual ~WorkspaceSerializer();

    bool saveState( const QString& fileName );
    bool loadState( const QString& fileName );

    bool saveWorkspace( Workspace* project );
    Workspace* loadWorkspace( const QString& fileName );

    QString errorMessage() const;

protected:
    QVariantMap serializeWorkspace( Workspace* workspace );
    void deserializeWorkspace( Workspace* workspace, const QVariantMap& map);

    QVariantMap serializeMainWindow( MainWindow* win );
    void deserializeMainWindow( MainWindow* win, const QVariantMap& map);

    QVariantMap serializeEditorTab( edbee::TextEditorWidget* widget );

private:
    QString errorMessage_;                   ///< The last error message
};

