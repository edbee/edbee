/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "appstateserializer.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "edbee/io/jsonparser.h"

#include "application.h"
#include "io/workspaceserializer.h"
#include "models/workspace.h"
#include "models/workspacemanager.h"

#include "debug.h"

/// The application state serializer
AppStateSerializer::AppStateSerializer()
{
}


/// The appstate destructor serializer
AppStateSerializer::~AppStateSerializer()
{

}


/// Saves the current state to the given file
/// @param fileName the filename to store the state
/// @return true on success
bool AppStateSerializer::saveState(const QString& fileName)
{
    errorMessage_.clear();

    // open the device
    QFile file( fileName);
    if( !file.open( QIODevice::WriteOnly) ) {
        errorMessage_ = file.errorString();
        return false;
    }

    // serialize the complete copy of the current workspace (and the filename to open this workspace)
    QVariantMap data;
    /*
    Workspace* activeWorkspace = edbeeApp()->activeWorkspace();
    data.insert("workspace", serializeWorkspace( activeWorkspace  ) );
    data.insert("workspace-file", activeWorkspace->filename() );
    data.insert("recent-workspaces", edbeeApp()->workspaceManager()->recentWorkspaceFilenameList() );

    */

    // save the recent workspace list
    WorkspaceManager* workspaceManager = edbeeApp()->workspaceManager();
    data.insert("recent-workspaces", workspaceManager->recentWorkspaceFilenameList() );

    // save the current open workspaces (backed by filenames)
    QStringList openWorkspaceFilesList;
    for( int i=0,cnt=workspaceManager->size(); i<cnt; ++i ) {

        // add the open workspace to the open workspaces list
        Workspace* workspace = workspaceManager->workspace(i);
        if( !workspace->filename().isEmpty() ) {
            openWorkspaceFilesList.push_back( workspace->filename() );
        }
    }
    data.insert("open-workspace-files", openWorkspaceFilesList );

    // save the workspaces that are not backed by a file
    QVariantList blankWorkspaceList;
    for( int i=0,cnt=workspaceManager->size(); i<cnt; ++i ) {

        // add the open workspace to the open workspaces list
        Workspace* workspace = workspaceManager->workspace(i);
        if( workspace->filename().isEmpty() ) {
            WorkspaceSerializer workspaceIO;
            blankWorkspaceList.push_back( workspaceIO.serializeWorkspace( workspace ) );
        }
    }
    data.insert("open-blank-workspaces", blankWorkspaceList );


    // serialize the data (into a json document)
    QJsonDocument doc;
    doc.setObject( QJsonObject::fromVariantMap( data ) );

    // write the docuemnt
    file.write( doc.toJson() );
    file.close();

    return true;
}


/// Loads the current state from the given file
/// @param fileName the filename to load the state from
/// @return true on success
bool AppStateSerializer::loadState(const QString& fileName)
{
    errorMessage_.clear();
    WorkspaceManager* workspaceManager = edbeeApp()->workspaceManager();

    // serialize the data (into a json document)
    edbee::JsonParser parser;
    if( !parser.parse(fileName) ) {
        errorMessage_ = parser.errorMessage();
        return false;
    }

    // When the last opened workspace file exists re-open it
    // and don't use the workspace data available in the last session file
    QVariantMap map = parser.result().toMap();

    // load all workspaces from file
    QStringList openWorkspaceList = map.value("open-workspace-files").toStringList();
    foreach( QString workspaceFilename, openWorkspaceList ) {
        if(  QFile::exists( workspaceFilename) ) {
            WorkspaceSerializer workspaceIO;
            workspaceIO.loadWorkspace(workspaceFilename);
        }
    }

    // next dereriaze all open memory workspaces
    QVariantList blankWorkspaceList = map.value("open-blank-workspaces").toList();
    foreach( QVariant serializedWorkspace, blankWorkspaceList ) {
        WorkspaceSerializer workspaceIO;
        workspaceIO.deserializeWorkspace( workspaceManager->createWorkspace(), serializedWorkspace.toMap() );
    }

    // deserialize the recent workspace file list
    QStringList recentList = map.value("recent-workspaces").toStringList();
    if( !recentList.isEmpty() ) {
        workspaceManager->setRecentWorkspaceFilenameList( recentList );
    }
    return true;
}



/// Returns the last error message
QString AppStateSerializer::errorMessage() const
{
    return errorMessage_;
}
