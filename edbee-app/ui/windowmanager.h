/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QObject>

class MainWindow;
class Workspace;

/// Manages the windows the edbee app is using
class WindowManager : public QObject
{
    Q_OBJECT
public:
    explicit WindowManager(QObject *parent = 0);
    virtual ~WindowManager();

    MainWindow* createWindow(Workspace* workspace);
    MainWindow* activeWindow() const;

    int size() const;
    MainWindow* window( int idx ) const;
    QList<MainWindow*> windowsForWorkspace( Workspace* workspace ) const;

public slots:

    void closeAllForWorkspace( Workspace* workspace );
    bool closeAll();

protected slots:
    
    void windowClosed();

private:
    QList<MainWindow*> windowList_;     ///< the list of active windows    
};
