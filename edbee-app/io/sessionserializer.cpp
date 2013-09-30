/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "sessionserializer.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "application.h"
#include "ui/mainwindow.h"
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
    doc.setObject( QJsonObject::fromVariantMap( serializeApplication( edbeeApp() ) ) );

    // write it
    file.write( doc.toJson() );
    file.close();
    return true;
}

/// Loads the current state from the given file
/// @param fileName the filename to load the state from
/// @return true on success
bool SessionSerializer::loadState(const QString& fileName)
{
    return false;
}

/// Returns the last error message
QString SessionSerializer::errorMessage() const
{
    return errorMessage_;
}

/// This method serializes the application to a QVariantMap
QVariantMap SessionSerializer::serialize()
{
    QVariantMap result;

    return result;
}

/// Serializes the application
/// @param app the application to serialize
QVariantMap SessionSerializer::serializeApplication(Application* app)
{
    Q_UNUSED(app);
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

        /// (TODO: Add line/column scroll information of the given tab!)
        //  edbee::TextEditorWidget* widget = win->tabEditor(i);
        //  tab.insert("caret",)
        tabs.push_back(tab);
    }
    result.insert("tabs",tabs);
    return result;
}


