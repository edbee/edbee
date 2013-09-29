/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "windowmanager.h"

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
MainWindow* WindowManager::createWindow()
{
    MainWindow* result = new MainWindow();
    windowList_.push_back(result);
    connect( result, &MainWindow::windowClosed, this, &WindowManager::windowClosed );
    return result;
}

/// retursn the number of active windows
int WindowManager::windowCount() const
{
    return windowList_.size();
}

/// Returns the window at the given index
MainWindow* WindowManager::window(int idx) const
{
    Q_ASSERT(idx<windowCount());
    return windowList_.at(idx);
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
