/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "sessionserializer.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "application.h"
#include "edbee/io/jsonparser.h"
#include "models/project.h"
#include "ui/mainwindow.h"
#include "ui/filetreesidewidget.h"
#include "ui/windowmanager.h"


#include "debug.h"


/// initializes the session serializer
/// @param a reference to the app
SessionSerializer::SessionSerializer()
{
}


/// destructor
SessionSerializer::~SessionSerializer()
{
}


/// Saves the current state to the given file
/// @param fileName the filename to store the state
/// @return true on success
bool SessionSerializer::saveState(const QString& fileName)
{
    errorMessage_.clear();

    // open the device
    QFile file( fileName);
    if( !file.open( QIODevice::WriteOnly) ) {
        errorMessage_ = file.errorString();
        return false;
    }

    // serialize the data (into a json document)
    QJsonDocument doc;
    QVariantMap data = serializeApplication( edbeeApp() );
    doc.setObject( QJsonObject::fromVariantMap( data ) );

    // write the docuemnt
    file.write( doc.toJson() );
    file.close();
    return true;
}


/// Loads the current state from the given file
/// @param fileName the filename to load the state from
/// @return true on success
bool SessionSerializer::loadState(const QString& fileName)
{
    errorMessage_.clear();

    // serialize the data (into a json document)
    edbee::JsonParser parser;
    if( !parser.parse(fileName) ) {
        errorMessage_ = parser.errorMessage();
        return false;
    }
    QVariantMap map = parser.result().toMap();
    deserializeApplication( edbeeApp(), map );
    return true;
}


/// Saves the project
/// A project is almost identical to the session state so saving the project here
/// seems to be ok for the moment
/// @param fileName the filename to save the project
bool SessionSerializer::saveProject( Project* project )
{
    errorMessage_.clear();

    // open the device
    QFile file( project->filename() );
    if( !file.open( QIODevice::WriteOnly) ) {
        errorMessage_ = file.errorString();
        return false;
    }

    // serialize the data

    /// TODO Implement this


    return false;

}


/// Loads the project
/// @param fileName the filename to load the project
Project* SessionSerializer::loadProject()
{
    /// TODO Implement this
    return 0;
}


/// Returns the last error message
QString SessionSerializer::errorMessage() const
{
    return errorMessage_;
}


/// Serializes the application
/// @param app the application to serialize
QVariantMap SessionSerializer::serializeApplication(Application* app)
{
    QVariantMap result;

    // 'remember' all open files per window
    QVariantList windowList;
    WindowManager* wm = app->windowManager();
    for( int i=0,cnt=wm->windowCount(); i<cnt; ++i ) {
        MainWindow* window = wm->window(i);
        windowList.append( serializeMainWindow( window ));
    }
    result.insert("windows",windowList);
    return result;
}


/// Serializes the given main window
/// @param win the window to serialize
QVariantMap SessionSerializer::serializeMainWindow(MainWindow* win)
{
    QVariantMap result;

    // 'remember' the window dimensions
    QVariantMap winState;
    winState.insert("width",win->size().width());
    winState.insert("height",win->size().height());
    winState.insert("x",win->pos().x());
    winState.insert("y",win->pos().y());
    winState.insert("state", (int)win->windowState() );
    result.insert("window", winState );

    // 'remeber' all open files
    QVariantList tabs;
    for( int i=0,cnt=win->tabCount(); i<cnt; ++i ) {
        QVariantMap tab;
        tab.insert("file",win->tabFilename(i));
        if( win->activeTabIndex() == i ) {
            tab.insert("active",1);
        }

        /// (TODO: Add line/column scroll information of the given tab!)
        //  edbee::TextEditorWidget* widget = win->tabEditor(i);
        //  tab.insert("caret",)
        tabs.push_back(tab);
    }
    result.insert("tabs",tabs);

    // add the side-widget
    result.insert("sidebar", win->fileTreeSideWidget()->serialize() );

    return result;
}


/// Deserializes the given appliation state
/// @param app a reference to the application
/// @param map the map with serialized data
void SessionSerializer::deserializeApplication(Application* app, const QVariantMap& map)
{
    WindowManager* winManager = app->windowManager();
    QVariantList windows = map.value("windows").toList();
    foreach( QVariant winVar, windows ) {
        QVariantMap winMap = winVar.toMap();
        MainWindow* win = winManager->createWindow();
        deserializeMainWindow( win, winMap );
        win->show();
    }
}


/// deserializes the map to configure this main window
void SessionSerializer::deserializeMainWindow(MainWindow* win, const QVariantMap& map)
{
    // reposition all windows
    QVariantMap window = map.value("window").toMap();
    int width = window.value("width").toInt();
    int height = window.value("height").toInt();
    int x = window.value("x").toInt();
    int y = window.value("y").toInt();
    if( width > 0 && height > 0 ) {
        win->setGeometry(x,y,width,height);
    }

    // reopen all existing tabs
    QVariantList tabs = map.value("tabs").toList();
    int activeIndex = 0;
    foreach( QVariant tabVar, tabs ) {
        QVariantMap tabMap = tabVar.toMap();
        QString file = tabMap.value("file").toString();
        bool active = tabMap.value("active").toInt() != 0;
        if( !file.isEmpty() && QFile::exists(file)) {
            if( active ) {
                activeIndex = win->tabCount();
            }
            win->openFile( file );
        }
    }

    // now select the correct tab
    win->setActiveTabIndex(activeIndex);

    // update the side tree
    win->fileTreeSideWidget()->deserialize( map.value("sidebar").toMap() );
}


