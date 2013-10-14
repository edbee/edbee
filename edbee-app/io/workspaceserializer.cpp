/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "workspaceserializer.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "application.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textgrammar.h"
#include "edbee/models/textlexer.h"
#include "edbee/lexers/grammartextlexer.h"
#include "edbee/io/jsonparser.h"
#include "edbee/texteditorwidget.h"
#include "edbee/util/textcodec.h"
#include "edbee/views/textrenderer.h"
#include "edbee/views/textselection.h"

#include "models/workspace.h"
#include "ui/mainwindow.h"
#include "ui/filetreesidewidget.h"
#include "ui/windowmanager.h"


#include "debug.h"


/// initializes the session serializer
/// @param a reference to the app
WorkspaceSerializer::WorkspaceSerializer()
{
}


/// destructor
WorkspaceSerializer::~WorkspaceSerializer()
{
}


/// Saves the current state to the given file
/// @param fileName the filename to store the state
/// @return true on success
bool WorkspaceSerializer::saveState(const QString& fileName)
{
    errorMessage_.clear();

    // open the device
    QFile file( fileName);
    if( !file.open( QIODevice::WriteOnly) ) {
        errorMessage_ = file.errorString();
        return false;
    }

    // serialize the complete copy of the current workspace (and the filename to open thisw workspace)
    QVariantMap data;
    data.insert("workspace", serializeWorkspace( edbeeApp()->workspace() ) );
    data.insert("workspace-file", edbeeApp()->workspace()->filename() );

    // serialize the data (into a json document)
    QJsonDocument doc;
    doc.setObject( QJsonObject::fromVariantMap( data ) );

    // write the docuemnt
    file.write( doc.toJson() );
    file.close();
    return true;
}


/// Loads the current state from the given file
/// @param fileName the filename to load the state from
/// @return true on success
bool WorkspaceSerializer::loadState(const QString& fileName)
{
    errorMessage_.clear();

    // serialize the data (into a json document)
    edbee::JsonParser parser;
    if( !parser.parse(fileName) ) {
        errorMessage_ = parser.errorMessage();
        return false;
    }

    // When the last opened workspace file exists re-open it
    // and don't use the workspace data available in the last session file
    QVariantMap map = parser.result().toMap();
    QString workspaceFile = map.value("workspace-file").toString();
    if( !workspaceFile.isEmpty() && QFile::exists( workspaceFile ) ) {
qlog_info() << "Load workspace filename!!";
        loadWorkspace( workspaceFile );
    // else we restore the workspace stored in the last session
    } else {
qlog_info() << "Using state-workspace!!";
        deserializeWorkspace( edbeeApp()->workspace(), map.value("workspace").toMap() );
    }
    return true;
}


/// Saves the project
/// A project is almost identical to the session state so saving the project here
/// seems to be ok for the moment
/// @param fileName the filename to save the project
bool WorkspaceSerializer::saveWorkspace( Workspace* project )
{
    errorMessage_.clear();

    // open the device
    QFile file( project->filename() );
    if( !file.open( QIODevice::WriteOnly) ) {
        errorMessage_ = file.errorString();
        return false;
    }

    // serialize the data
    // serialize the data (into a json document)
    QJsonDocument doc;
    QVariantMap data = serializeWorkspace( edbeeApp()->workspace() );
    doc.setObject( QJsonObject::fromVariantMap( data ) );

    // write the docuemnt
    file.write( doc.toJson() );
    file.close();
    return true;

}


/// Loads the project
/// @param fileName the filename to load the project
/// @return the project object or 0 on error. The error-message can be recieved by caller errorMessage()
Workspace* WorkspaceSerializer::loadWorkspace( const QString& fileName )
{
    errorMessage_.clear();

    // serialize the data (into a json document)
    edbee::JsonParser parser;
    if( !parser.parse(fileName) ) {
        errorMessage_ = parser.errorMessage();
        return 0;
    }
    QVariantMap map = parser.result().toMap();


    // change the application workspace
    Workspace* result = new Workspace();
    edbeeApp()->giveWorkspace( result );
    result->setFilename( fileName );

    // load the data
    deserializeWorkspace( result, map );
    return result;
}


/// Returns the last error message
QString WorkspaceSerializer::errorMessage() const
{
    return errorMessage_;
}


/// Serializes the application
/// @paramapp the application to serialize
QVariantMap WorkspaceSerializer::serializeWorkspace(Workspace* workspace)
{
    QVariantMap result;

    // 'remember' all open files per window
    QVariantList windowList;
    WindowManager* wm = edbeeApp()->windowManager();
    for( int i=0,cnt=wm->windowCount(); i<cnt; ++i ) {
        MainWindow* window = wm->window(i);
        if( window->workspace() == workspace ) {
            windowList.append( serializeMainWindow( window ));
        }
    }
    result.insert("windows",windowList);
    return result;
}



/// Deserializes the given application state
/// @param app a reference to the application
/// @param map the map with serialized data
void WorkspaceSerializer::deserializeWorkspace(Workspace* workspace, const QVariantMap& map)
{
    WindowManager* winManager = edbeeApp()->windowManager();
    QVariantList windows = map.value("windows").toList();
    foreach( QVariant winVar, windows ) {
        QVariantMap winMap = winVar.toMap();
        MainWindow* win = winManager->createWindow( workspace );
        deserializeMainWindow( win, winMap );
        win->show();
    }
}


/// Serializes the given main window
/// @param win the window to serialize
QVariantMap WorkspaceSerializer::serializeMainWindow(MainWindow* win)
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
        QVariantMap tab = serializeEditorTab( win->tabEditor(i) );

        // add the file
        tab.insert("file",win->tabFilename(i));

        // add the active tab index
        if( win->activeTabIndex() == i ) {
            result.insert("active",1);
        }

        tabs.push_back(tab);
    }
    result.insert("tabs",tabs);

    // add the side-widget
    result.insert("sidebar", win->fileTreeSideWidget()->serialize() );

    return result;
}


/// deserializes the map to configure this main window
void WorkspaceSerializer::deserializeMainWindow(MainWindow* win, const QVariantMap& map)
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


/// Serializes the editor tab
/// @param widget the widget that needs to be serialized
/// @return the serialized variant map
QVariantMap WorkspaceSerializer::serializeEditorTab(edbee::TextEditorWidget* editor)
{
    QVariantMap result;

    // serialize the encoding
    edbee::TextDocument* doc = editor->textDocument();
    result.insert("encoding", doc->encoding()->name() );

    // add the active grammer
    edbee::TextLexer* lexer = doc->textLexer();

    // Yuck, this is dirty, we need a nice way to do this
    edbee::GrammarTextLexer* grammarLexer = dynamic_cast<edbee::GrammarTextLexer*>(lexer) ;
    if( grammarLexer ) {
        edbee::TextGrammar* grammar = grammarLexer->grammar();
        if( grammar ) {
            result.insert("grammar", grammar->name() );
        }
    }

    // add the scroll position
    result.insert("scroll", editor->textRenderer()->firstVisibleLine() );

    // add the active line and colum
    // we do not store alle ranges!! This is confusing when opening the file
    // only the first caret is stored!
    const edbee::TextRange& range = editor->textSelection()->range(0);
    result.insert("caret", range.caret() );

    return result;
}


