/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QApplication>
#include <QFont>
#include <QStringList>

class EdbeeConfig;
class QtAwesome;
class WindowManager;
class Workspace;


/// The global application for the edbee editor
class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int& argc, char** argv);
    virtual ~Application();

    void initApplication();
    void shutdown();

    void loadState();
    void saveState();

    QtAwesome* qtAwesome() const;
    QFont iconFont( int size=12 ) const;

    QString appDataPath() const;
    QString appConfigPath() const;
    QString userDataPath() const;
    QString userConfigPath() const;
    QString lastSessionFilename() const;

    EdbeeConfig* config() const;
    WindowManager* windowManager() const;

    // os specific items
    bool isOSX();
    bool isX11();
    bool isWin();
    const char* osNameString();

    Workspace* workspace() const;
    void giveWorkspace( Workspace* workspace );
    void closeWorkspace();
    void addToRecentWorkspaceFilenameList( const QString& filename );
    void setRecentWorkspaceFilenameList( const QStringList& filenameList );
    QStringList recentWorkspaceFilenameList() const;
    void clearRecentWorkspaceFilenameList();

signals:
    /// This signal is fired with the recentWorkspaceFilenameList is changed
    void recentWorkspaceFilenameListChanged();


protected:
    bool event(QEvent* event);

    void registerCustomEditorCommands();


private:
    QString appDataPath_;           ///< The application data path
    QString userDataPath_;          ///< The user configuration path
    QtAwesome* qtAwesome_;          ///< The QtAwesome IconFont instance

    EdbeeConfig* config_;           ///< The main edbee configuration file
    WindowManager* windowManager_;  ///< The window manager
    Workspace* workspace_;          ///< The current workspace. At the moment only 1 workspace can be opened. In the future we might support multiple workspace

    QStringList recentWorkspaceFilenameList_;  ///< The recent workspace filename list
};


extern Application* edbeeApp();
