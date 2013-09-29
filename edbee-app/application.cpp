/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "application.h"

#include <QAbstractNativeEventFilter>
#include <QDir>
#include <QFileOpenEvent>
#include <QMenu>
#include <QMessageBox>
#include <QStandardPaths>

#include "edbee/io/tmlanguageparser.h"
#include "edbee/edbee.h"
#include "edbee/views/texttheme.h"
#include "models/edbeeconfig.h"
#include "QtAwesome.h"
#include "ui/mainwindow.h"
#include "ui/windowmanager.h"

#include "debug.h"


/// constructs the main appication object
/// @param argc the application argument acount
/// @param argv the application arguments
Application::Application(int& argc, char** argv )
    : QApplication( argc, argv )
    , qtAwesome_(0)
    , config_(0)
{
    config_ = new EdbeeConfig();
    windowManager_ = new WindowManager();
}

/// destruct the 'owned' objects
Application::~Application()
{
    delete windowManager_;
    delete config_;
    delete qtAwesome_;
}


/// Initializes the application
void Application::initApplication()
{
    // Initialize the application paths
    #ifdef Q_OS_MAC
        appDataPath_    = applicationDirPath() + "/../Resources/";


    #else
        appDataPath_ = qApp->applicationDirPath() + "/data/";
    #endif
    userDataPath_   = QStandardPaths::writableLocation( QStandardPaths::DataLocation) + "/";

    // configure the edbee component to use the default paths
    edbee::Edbee* tm = edbee::Edbee::instance();
    tm->setKeyMapPath( QString("%1%2").arg(appDataPath_).arg("keymaps"));
    tm->setGrammarPath(  QString("%1%2").arg(appDataPath_).arg("syntaxfiles") );
    tm->setThemePath( QString("%1%2").arg(appDataPath_).arg("themes") );
    tm->init();   
    tm->autoShutDownOnAppExit();

    // make sure the user config path exists
//    QDir dir( userConfigPath() );
    QDir dir;
    dir.mkpath( userConfigPath() );

    // add the configuration paths to the edbeeconfig
    config()->addFile( QString("%1%2").arg(appConfigPath()).arg("default.json") );
    config()->addFile( QString("%1%2").arg(appConfigPath()).arg( QString("default.%1.json").arg(osNameString()) ), EdbeeConfig::Optional );
    config()->addFile( QString("%1%2").arg(userConfigPath()).arg("default.json"), EdbeeConfig::AutoCreate );
    config()->addFile( QString("%1%2").arg(userConfigPath()).arg( QString("default.%1.json").arg(osNameString()) ), EdbeeConfig::Optional );


    // load the configuration
    if( !config()->loadConfig() ) {

        /// build a nice error message
        QString messages;
        for( int i=0, cnt=config()->fileCount(); i<cnt; ++i ) {
            QString msg = config()->loadMessageForFile(i);
            if( !msg.isEmpty() ) {
                messages.append( tr("- %1: %2\n").arg(config()->file(i)).arg(msg) );
            }
        }
        // and show it
        if( !messages.isEmpty() ) {
            QString title = tr("Error loading configuration file(s)");
            QMessageBox::warning(0, title,QString("%1\n%2").arg(title).arg(messages) );
        }
    }

    // make qtawesome
    qtAwesome_ = new QtAwesome( this);
    qtAwesome_->initFontAwesome();

    // construct the first window and show it
    // (probably we should restore the last window state over here :)
    windowManager()->createWindow()->show();
}

/// thsi method shutsdown the application
void Application::shutdown()
{
}

/// returns the qtAwesome instance for the application's icons
QtAwesome *Application::qtAwesome() const
{
    return qtAwesome_;
}

/// Returns the (default) icon font
QFont Application::iconFont(int size) const
{
    return qtAwesome()->font(size);
}

/// Returns the application data path.
/// This is the path to fixed application resources
/// On Mac OS X this will be in the .app 'file'
QString Application::appDataPath() const
{
    return appDataPath_;
}

/// Returns tha full application coniguration path
QString Application::appConfigPath() const
{
    return QString("%1%2/").arg(appDataPath()).arg("config");
}

/// Returns the user config data path. This is the data path where user specific
/// settings are stored
QString Application::userDataPath() const
{
    return userDataPath_;
}

/// Returns the full user configuration path
QString Application::userConfigPath() const
{
    return QString("%1%2/").arg(userDataPath()).arg("config");
}


/// Returns the edbee configuration
EdbeeConfig* Application::config() const
{
    return config_;
}

/// Returns the application window manager
WindowManager* Application::windowManager() const
{
    return windowManager_;
}

/// This method returnst true if we're running on Mac OS X
/// Note: I could have used macro's but that will probably litter the application with #ifdefs
/// the current approach is much more flexible.
bool Application::isOSX()
{
#ifdef Q_OS_MAC
    return true;
#else
    return false;
#endif
}

/// This method reutrns true if we're running on a Unix X11 environment
bool Application::isX11()
{
#ifdef Q_OS_X11
    return true;
#else
    return false;
#endif
}

/// Returns true if the current environment is windows
bool Application::isWin()
{
#ifdef Q_OS_WIN32
    return true;
#else
    return false;
#endif
}

/// This method returns the operatingsystem string
/// that's can be used as prefix/postfix items in config-settings
const char *Application::osNameString()
{
#ifdef Q_OS_MAC
    return "osx";
#elif Q_OS_WIN32
    return "win";
#else
    return "x11";
#endif

}


bool Application::event(QEvent* event)
{
//    qlog_info()<< "event: " << event;
    switch (event->type())
    {
        case QEvent::FileOpen:
        {
            QString file = static_cast<QFileOpenEvent *>(event)->file();
            qlog_info() << "TODO: File open event: " <<file;
            //loadFile(static_cast<QFileOpenEvent *>(event)->file());
            return true;
        }
        default:
            return QApplication::event(event);
    }
}


/// A global function to access the edbee application quickly
/// returns the Application instance
Application* edbeeApp()
{
    return static_cast<Application *>(qApp);
}


