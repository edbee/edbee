/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "application.h"

#include <QtCore>
#include <QtGui>
#include <QFileOpenEvent>
#include <QAbstractNativeEventFilter>

#include <QMenu>

#include "edbee/io/tmlanguageparser.h"
#include "edbee/edbee.h"
#include "edbee/views/texttheme.h"
#include "QtAwesome.h"

#include "debug.h"

static Application* inst;

Application::Application(int& argc, char** argv )
    : QApplication( argc, argv )
    , qtAwesome_(0)
{
    inst = this;
}

Application::~Application()
{
    delete qtAwesome_;
}

Application* Application::instance()
{
    return inst;
}


/// Initializes the application
void Application::initApplication()
{
    #ifdef Q_OS_MAC
        appDataPath_ = applicationDirPath() + "/../Resources/";
    #else
        appDataPath_ = qApp->applicationDirPath() + "/data/";
    #endif

    edbee::Edbee* tm = edbee::Edbee::instance();
    tm->setKeyMapPath( QString("%1%2").arg(appDataPath_).arg("keymaps"));
    tm->setGrammarPath(  QString("%1%2").arg(appDataPath_).arg("syntaxfiles") );
    tm->setThemePath( QString("%1%2").arg(appDataPath_).arg("themes") );
    tm->init();   
    tm->autoShutDownOnAppExit();


    qtAwesome_ = new QtAwesome( qApp );
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

