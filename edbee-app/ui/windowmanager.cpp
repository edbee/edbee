/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "windowmanager.h"

#include "application.h"
#include "ui/mainwindow.h"

#include "debug.h"

/// the default window manager constructor
WindowManager::WindowManager(QObject *parent)
    : QObject(parent)
{
}


/// deletes all existing windows
WindowManager::~WindowManager()
{
    qDeleteAll( windowList_ );
}


/// Creates a main window
/// @param workspace the workpsace to create this window for
MainWindow* WindowManager::createWindow( Workspace* workspace )
{
    MainWindow* result = new MainWindow( edbeeApp()->workspace() );
    windowList_.push_back(result);
    connect( result, &MainWindow::windowClosed, this, &WindowManager::windowClosed );
    return result;
}


/// this method creates and shows a mainwindow if no windows are available
void WindowManager::createAndShowWindowIfEmpty()
{
    if( windowList_.isEmpty() ) {
        createWindow( edbeeApp()->workspace() )->show();
    }
}


/// retursn the number of active windows
int WindowManager::windowCount() const
{
    return windowList_.size();
}


/// Returns the window at the given index
/// @param idx the window index
/// @return the mainwindow at the given index
MainWindow* WindowManager::window(int idx) const
{
    Q_ASSERT(idx<windowCount());
    return windowList_.at(idx);
}


/// Closes all windows for the given workspace
/// @param workspace the workspace to close all windows for
/// @return true if all workspace windows have been closed. False if a window canceled the close operation
bool WindowManager::closeAllForWorkspace(Workspace* workspace)
{
    // clear the list
    for( int i=windowList_.size()-1; i>=0; --i ) {
        MainWindow* win = windowList_.at(i);

        // only close window if it from the same workspace
        if( win->workspace() == workspace ) {
            if( !win->close() ) { return false; }
        }
    }
    return true;
}


/// Closes all open windows and opens a blank window
/// @return true on success, false if a window prevented the close
bool WindowManager::closeAll()
{
    // clear the list
    foreach( MainWindow* win, windowList_ ) {
        if( !win->close() ) { return false; }
    }
    windowList_.clear();
    return true;
}


/// When a window closes itself it's removed from the window list and from memory
void WindowManager::windowClosed()
{
    MainWindow* mainWindow = qobject_cast<MainWindow*>( sender() );
    if( mainWindow ) {
        windowList_.removeOne(mainWindow);
        mainWindow->deleteLater();
    } else {
        qlog_warn() << "Recieved a windowClosed event from a non-window?!";
    }
}
