/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "workspaceserializer.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "application.h"
#include "edbee/edbee.h"
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
#include "models/workspacemanager.h"
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


/// Saves the workspace
/// A project is almost identical to the session state so saving the project here
/// seems to be ok for the moment
/// @param workspace the workspace to save
bool WorkspaceSerializer::saveWorkspace( Workspace* workspace )
{
    errorMessage_.clear();

    // open the device
    QFile file( workspace->filename() );
    if( !file.open( QIODevice::WriteOnly) ) {
        errorMessage_ = file.errorString();
        return false;
    }

    // serialize the data
    // serialize the data (into a json document)
    QJsonDocument doc;
    QVariantMap data = serializeWorkspace( workspace );
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
    Workspace* result = edbeeApp()->workspaceManager()->createWorkspace();
    result->setFilename( fileName );            // this need to be set before giving it to the workspace

    // load the data into the workspace
    // Unfortunately the current implementation depends on the window being available.
    // so the deserialization needs to take place after giving it to the application
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
    for( int i=0,cnt=wm->size(); i<cnt; ++i ) {
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

    // when there are no windows, make sure there's at least one for this workspace
    if( windows.size() == 0 ) {
        MainWindow* window = edbeeApp()->windowManager()->createWindow( workspace );
        window->show();
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
            tab.insert("active",1);
        }

        tabs.push_back(tab);
    }
    result.insert("tabs",tabs);

    // add the side-widget
    result.insert("sidebar", win->fileTreeSideWidget()->serialize() );

    return result;
}


/// deserializes the map to configure this main window
/// @param win the window to configure
/// @param map the map with the serialized data
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

    // update the side tree (this needs to be first, so the correct root is selected)
    win->fileTreeSideWidget()->deserialize( map.value("sidebar").toMap() );


    // reopen all existing tabs
    QVariantList tabs = map.value("tabs").toList();
    int activeIndex = 0;
    foreach( QVariant tabVar, tabs ) {
        QVariantMap tabMap = tabVar.toMap();
        QString file = tabMap.value("file").toString();
        bool active = tabMap.value("active").toInt() != 0;

        // open the file if it exists
        if( !file.isEmpty() && QFile::exists(file)) {
            if( active ) {
                activeIndex = win->tabCount();
            }

            // open the file and derialize the extra document information
            if( win->openFile( file ) ) {
                deserializeEditorTab( win->tabEditor( win->tabCount()-1), tabMap);
            }
        }
    }

    // now select the correct tab
    win->setActiveTabIndex(activeIndex);

    // update the tab state
    win->activeTabChanged();    // makes the tab update the state

}


/// Serializes the editor tab
/// @param widget the widget that needs to be serialized
/// @return the serialized variant map
QVariantMap WorkspaceSerializer::serializeEditorTab(edbee::TextEditorWidget* editor)
{
    QVariantMap result;

    // serialize the encoding
    edbee::TextDocument* doc = editor->textDocument();
    result.insert("encoding", doc->encoding()->name() );    // (Save the last encoding)

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


/// deserializes the editor tab settings
/// @param widget the widget to serialize the data to
/// @param map the map with options to deserialize
void WorkspaceSerializer::deserializeEditorTab(edbee::TextEditorWidget* editor, const QVariantMap& map)
{
    // retrieve some ed
    edbee::TextDocument* doc = editor->textDocument();
    edbee::Edbee* edbee = edbee::Edbee::instance();

    // select the correct textencoding (we do not restore the encoding)
//    edbee::TextCodec* codec = edbee->codecManager()->codecForName( map.value("encoding").toString() );
//    if( codec ) {
//        doc->setEncoding( codec );
//    }

    // select the grammar
    edbee::TextLexer* lexer = doc->textLexer();

    // TODO we need a cleaner way to do this
    edbee::GrammarTextLexer* grammarLexer = dynamic_cast<edbee::GrammarTextLexer*>(lexer) ;
    if( grammarLexer ){
        edbee::TextGrammar* grammar = edbee->grammarManager()->get( map.value("grammar").toString() );
        if( grammar ) {
            grammarLexer->setGrammar( grammar );
        }

    }

    // restore the scroll position
    int line = map.value("scroll").toInt();
    if( line > 0 && line < doc->lineCount() ) {
        // editor->scrollTopToLine( line );         // Yuck, somehow the scroll-dimensions aren't ready at this moment so changing scroll-position doesn't work
        QMetaObject::invokeMethod( editor, "scrollTopToLine", Qt::QueuedConnection, Q_ARG(int,line) );  // work-around is to post a scroll event
    }

    // restore the caret position
    // only 1 caret position is stored. Starting an editor with multi-carets is not something you'de expect!
    int caret = map.value("caret").toInt();
    if( caret >= 0 && caret <= doc->length() ) {
        editor->textSelection()->setRange(caret,caret);
    }
}


