/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QObject>
#include <QList>
#include <QStringList>

class Workspace;

/// A general workspace manager.
/// This class is required for managing multiple workspaces at once.
class WorkspaceManager : public QObject
{
    Q_OBJECT
public:
    explicit WorkspaceManager(QObject *parent = 0);
    virtual ~WorkspaceManager();

    Workspace* createWorkspace();
    Workspace* activeWorkspace() const;
    int size() const;
    Workspace* workspace( int idx ) const;
    void giveWorkspace( Workspace* workspace);
    void closeWorkspace(Workspace* workspace);

    void addToRecentWorkspaceFilenameList( const QString& filename );
    void setRecentWorkspaceFilenameList( const QStringList& filenameList );
    QStringList recentWorkspaceFilenameList() const;

public slots:
    void clearRecentWorkspaceFilenameList();

signals:
    void workspaceAboutToClose( Workspace* workspace );

    /// This signal is fired with the recentWorkspaceFilenameList is changed
    void recentWorkspaceFilenameListChanged();


private:
    QList<Workspace*> workspaceList_;           ///< The list with all workspaces
    QStringList recentWorkspaceFilenameList_;   ///< A list of recent workspaces
    int nextWorkspaceNumber_;                   ///< A number to generate the next workspace number
};
