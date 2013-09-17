/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QApplication>
#include <QFont>

class EdbeeConfig;
class QtAwesome;

/// The global application for the edbee editor
class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int& argc, char** argv);
    virtual ~Application();

    static Application* instance();

    void initApplication();
    QtAwesome* qtAwesome() const;
    QFont iconFont( int size=12 ) const;

    QString appDataPath() const;
    QString appConfigPath() const;
    QString userDataPath() const;
    QString userConfigPath() const;

    EdbeeConfig* config() const;

protected:
    bool event(QEvent* event);
    bool eventFilter(QObject *obj, QEvent *ev);

private:
    QString appDataPath_;           ///< The application data path
    QString userDataPath_;          ///< The user configuration path
    QtAwesome* qtAwesome_;          ///< The QtAwesome IconFont instance

    EdbeeConfig* config_;           ///< The main edbee configuration file
};
