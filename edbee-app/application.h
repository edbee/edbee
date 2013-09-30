/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QApplication>
#include <QFont>

class EdbeeConfig;
class QtAwesome;
class WindowManager;

/// The global application for the edbee editor
class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int& argc, char** argv);
    virtual ~Application();

    void initApplication();
    void shutdown();

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


protected:
    bool event(QEvent* event);


private:
    QString appDataPath_;           ///< The application data path
    QString userDataPath_;          ///< The user configuration path
    QtAwesome* qtAwesome_;          ///< The QtAwesome IconFont instance

    EdbeeConfig* config_;           ///< The main edbee configuration file
    WindowManager* windowManager_;  ///< The window manager
};


extern Application* edbeeApp();
