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

#include "debug.h"

static Application* inst;

Application::Application(int& argc, char** argv )
    : QApplication( argc, argv )
    , qtAwesome_(0)
    , config_(0)
{
    inst = this;
    config_ = new EdbeeConfig();
}

Application::~Application()
{
    delete config_;
    delete qtAwesome_;
}

/// returns the application instance
Application* Application::instance()
{
    return inst;
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
    config()->addFile( QString("%1%2").arg(userConfigPath()).arg("default.json"), true );


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

}

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

bool Application::eventFilter(QObject* obj, QEvent* ev)
{
    Q_UNUSED(obj);
    Q_UNUSED(ev);
    return false;
}

