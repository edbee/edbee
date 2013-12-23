/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "workspacemanager.h"

#include "application.h"
#include "ui/mainwindow.h"
#include "ui/windowmanager.h"
#include "workspace.h"

#include "debug.h"


/// constructs the workspace manager
WorkspaceManager::WorkspaceManager(QObject *parent)
    : QObject(parent)
    , nextWorkspaceNumber_(1)
{
}


//// Deletes all workspaces
WorkspaceManager::~WorkspaceManager()
{
    qDeleteAll(workspaceList_);
}


/// Creates a new workspace
/// @return the newly created workspace
Workspace* WorkspaceManager::createWorkspace()
{
    Workspace* workspace = new Workspace();
    workspace->setName( tr("Workspace %1").arg(nextWorkspaceNumber_++));
    giveWorkspace( workspace );
    return workspace;
}


/// Returns the active workspace. It's a bit scary, this method depends on the active window.
/// This is the most natural way to define the active workspace
/// @return the workspace that is active
Workspace* WorkspaceManager::activeWorkspace() const
{
    MainWindow* window = edbeeApp()->windowManager()->activeWindow();
    if( window ) {
        return window->workspace();
    }
    return 0;
}


/// Return the number of active workspace
int WorkspaceManager::size() const
{
    return workspaceList_.size();
}


/// Returns the workspace at the given index
Workspace*WorkspaceManager::workspace(int idx) const
{
    return workspaceList_.at(idx);
}


/// Gives the workspace to the application
/// replacing the other workspace
/// @param workspace the new workspace to use
void WorkspaceManager::giveWorkspace( Workspace* workspace )
{
    workspaceList_.push_back( workspace );
}


/// Closes the current workspace
/// @param workspace the workspace to close.
void WorkspaceManager::closeWorkspace( Workspace* workspace )
{
    emit workspaceAboutToClose( workspace );
    workspaceList_.removeOne( workspace );
    delete workspace;
}


/// Adds the given filename to the recent workspace filename list
/// @param filename the filename to add to the list
void WorkspaceManager::addToRecentWorkspaceFilenameList(const QString& filename)
{
    recentWorkspaceFilenameList_.removeOne( filename );
    recentWorkspaceFilenameList_.push_front( filename );
    // notify the list has changed
    emit recentWorkspaceFilenameListChanged();
}


/// changes the recent workspace filename list to the given stringlist
/// @param filenameList the list of new filename
void WorkspaceManager::setRecentWorkspaceFilenameList(const QStringList& filenameList)
{
    recentWorkspaceFilenameList_ = filenameList;
    emit recentWorkspaceFilenameListChanged();
}


/// Returns the list with all recent opened workspaces
QStringList WorkspaceManager::recentWorkspaceFilenameList() const
{
    return recentWorkspaceFilenameList_;
}


/// clears the recent workspace filename list
void WorkspaceManager::clearRecentWorkspaceFilenameList()
{
    recentWorkspaceFilenameList_.clear();
    // notify the list has changed
    emit recentWorkspaceFilenameListChanged();
}
