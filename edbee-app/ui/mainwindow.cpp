/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "mainwindow.h"

#include <QApplication>
#include <QComboBox>
#include <QDropEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QFont>
#include <QHeaderView>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QSplitter>
#include <QStatusBar>

#include "edbee/io/textdocumentserializer.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/texteditorcommandmap.h"
#include "edbee/models/texteditorkeymap.h"
#include "edbee/models/textgrammar.h"
#include "edbee/models/textundostack.h"
#include "edbee/texteditorcommand.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/edbee.h"
#include "edbee/texteditorwidget.h"
#include "edbee/util/lineending.h"
#include "edbee/util/textcodec.h"
#include "edbee/views/components/texteditorcomponent.h"
#include "edbee/views/textrenderer.h"

#include "application.h"
#include "filetreesidewidget.h"
#include "findwidget.h"
#include "gotowidget.h"
#include "io/workspaceserializer.h"
#include "models/edbeeconfig.h"
#include "models/workspace.h"
#include "ui/windowmanager.h"

#include "debug.h"



/// A build in warning size to warn for big file sizes
static const int FileSizeWarning = 1024*1024*20;  // Larger then 20 MB give a warning!


/// Constructor for a main application windows
MainWindow::MainWindow(Workspace* workspace, QWidget* parent)
    : QMainWindow(parent)
    , fileTreeSideWidgetRef_(0)
    , tabWidgetRef_(0)
    , statusBarRef_(0)
    , grammarComboRef_(0)
    , lineEndingComboRef_(0)
    , encodingComboRef_(0)
    , workspaceRef_(0)
{
    constructActions();
    constructUI();
    constructMenu();
    connectSignals();
    setWorkspace( workspace );
}


/// The application destructor
MainWindow::~MainWindow()
{    
    qDeleteAll(actionMap_);
}


/// returns the number of tabs
int MainWindow::tabCount() const
{
    return tabWidgetRef_->count();
}


/// Returns the filenane open in the given tab
/// @param idx the tab index. When -1 is supplied the active tab is selected
/// @return the filename
QString MainWindow::tabFilename(int idx) const
{
    edbee::TextEditorWidget* widget = this->tabEditor(idx);
    if( widget ) {
        return widget->property("file").toString();
    }
    return QString();
}


/// This method returns the name displayed on the given tab
/// @param idx the tab index to retrieve the name for
/// @return the name on the given tab (or QString if no tab is available
QString MainWindow::tabName(int idx) const
{
    // get the active index if no tab is selected
    if( idx < 0 ) {
        // return a blank string
        idx = tabWidgetRef_->currentIndex();
        if( idx < 0 ) { return QString(); }
    }
    // simply returnt the text at the given tab index
    return tabWidgetRef_->tabText(idx);
}


/// Returns the editor widget at the given tab index
/// @param index the tab index (if no index is supplied the current tab is used)
/// @return the edbee::editor widget or 0 if not found
edbee::TextEditorWidget* MainWindow::tabEditor(int index) const
{
    if( index < 0 ) {
        index = tabWidgetRef_->currentIndex();
        if(index<0) { return 0; }
    }
    QWidget* widget = tabWidgetRef_->widget(index);
    return qobject_cast<edbee::TextEditorWidget*>(widget);
}


/// Returns the active tab index.
/// @return the active tab index or -1 if no tab was active
int MainWindow::activeTabIndex() const
{
    return tabWidgetRef_->currentIndex();
}


/// Changes the current active tab index to the given tab
/// @param idx the index of the new active tab
void MainWindow::setActiveTabIndex(int idx)
{
    if( idx >= tabCount()) { return; }
    tabWidgetRef_->setCurrentIndex(idx);
}


/// Returns the side tree widget
FileTreeSideWidget* MainWindow::fileTreeSideWidget() const
{
    return fileTreeSideWidgetRef_;
}


/// Sets the current workspace
/// @param workspace the assign this window to
void MainWindow::setWorkspace(Workspace* workpace)
{
    workspaceRef_ = workpace;
    fileTreeSideWidgetRef_->setWorkspace( workpace );
}


/// Returns the current workspace
Workspace* MainWindow::workspace() const
{
    return workspaceRef_;
}


/// This method checks if a given tab is modified
bool MainWindow::isModified() const
{
    // check all tabs if the content has been persisted
    for( int i=0,cnt=tabCount(); i<cnt; ++i ) {
        edbee::TextEditorWidget* widget = tabEditor(i);
        edbee::TextDocument* document = widget->textDocument();

        // persisted is a document flag that's turned to true when text is changed
        if( !document->isPersisted() ) {
            return true;
        }
    }
    // all tabs have been persisted, so nothing is modified
    return false;
}


/// opens the given directory or the given file. Depending on the type it will open
/// a file in an editor window or it will open the directory in the sidebar
/// @param path the path to open
void MainWindow::openDirOrFile(const QString& path)
{
    QFileInfo fileInfo(path);
    if( fileInfo.exists() ) {
        if( fileInfo.isDir() ) {
            openDir( path );
        } else {
            openFile( path );
        }
    }
}


/// Opens the given dir in the side-tree window
void MainWindow::openDir(const QString& path)
{
    QFileInfo fileInfo(path);

    // file not found ??
    if( !fileInfo.exists() ) {
        QMessageBox::warning(this, tr("Folder not found"), tr("The folder could not be found!)") );
        return;
    }
    fileTreeSideWidgetRef_->setRootPath( path );
}


/// Opens the given file in the editor window
/// @param filename the filename to open
bool MainWindow::openFile(const QString& filename)
{
    QFileInfo fileInfo(filename);

    // file not found ??
    if( !fileInfo.exists() ) {
        QMessageBox::warning(this, tr("File not found"), tr("The file could not be found!)") );
        return false;
    }

    // file not readable?
    if( !fileInfo.isReadable() ) {
        QMessageBox::warning(this, tr("File not accessible"), tr("The file could not be read!)") );
        return false;
    }

    // safeguard for large files
    if( fileInfo.size() >= FileSizeWarning ) {
        if( QMessageBox::question(this, tr("Open very large file?"), tr("Warning file is larger then %1 MB. Open file?").arg(FileSizeWarning/1024/1024) ) != QMessageBox::Yes ) {
            return false;
        }
    }

    // open the file
    QFile file(filename);

    // create the widget and serialize the file
    edbee::TextEditorWidget* widget = createEditorWidget();
    edbee::TextDocumentSerializer serializer( widget->textDocument() );
    if( !serializer.load( &file ) ) {
        QMessageBox::warning(this, tr("Error opening file"), tr("Error opening file:\n%1").arg(serializer.errorString()) );
        delete widget;
        return false;
    }
    addEditorTab( widget, fileInfo.filePath() );
    return true;
}


///  Opens a file by showing a file dialog
void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if( !fileName.isEmpty() ) openFile(fileName);
}


/// Creates a new file (opens an empty tab)
void MainWindow::newFile()
{
    addEditorTab( createEditorWidget(), "" );
}


/// Adds an editor tab
/// @param edito the editor tab
/// @param fileName the filename of the file to edit
void MainWindow::addEditorTab(edbee::TextEditorWidget* editor, const QString& fileName )
{
    QFileInfo info( fileName );

    // set the state to 'persisted'
    editor->textDocument()->setPersisted();
    editor->setProperty("file",fileName);

    // detect the grammar
    editor->textDocument()->setLanguageGrammar( edbee::Edbee::instance()->grammarManager()->detectGrammarWithFilename(fileName) );

    // listen to the editor
    connect( editor->controller(), SIGNAL(updateStatusTextSignal(QString)), statusBar(), SLOT(showMessage(QString)) );
    connect( editor->textDocument()->textUndoStack(), SIGNAL(persistedChanged(bool)), this, SLOT(updatePersistedState())  );
    connect( editor, SIGNAL(focusIn(QWidget*)), SLOT(updateActions()) );
    connect( editor, SIGNAL(focusOut(QWidget*)), SLOT(updateActions()) );

    // Add the tab and read it
    int idx = tabWidgetRef_->addTab( editor, "-" );
    tabWidgetRef_->setCurrentIndex(idx);
    updateTabName(idx);
    editor->setFocus();
}


/// Closes the file with the given tab indec
/// @param idx the index of the tab to close (<0 means close the active tab)
/// @return true if the tab has been closed.
///       false means the save has been canceled or the save has failed
bool MainWindow::closeFileWithTabIndex(int idx)
{
    // retrieve the current tab index if <0 )
    if( idx < 0 ) {
        idx = tabWidgetRef_->currentIndex();
    }

    // is there an tab index ??
    if( idx >= 0 ) {
        edbee::TextEditorWidget* widget = tabEditor( idx );
        if( widget && !widget->textDocument()->isPersisted() ) {

            // retrieve the filename
            QString file = widget->property("file").toString();     // FIXME: We really need to improve the tab-widget
            QString tabname = tabName(idx);

            // show a different message if the file hasn't been saved
            QString msg;
            if( file.isEmpty() ) {
                msg  = tr("Tab '%1' hasn't been saved!\nDo you want to save the file?").arg( tabname );
            } else {
                msg = tr("File '%1' has been changed!\nDo you want to save the changes?").arg( file );
            }

            // show the standard message
            QMessageBox::StandardButton but = QMessageBox::question(this, tr("Save changes?"), msg, QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel );

            // save options has been chosen
            if( but == QMessageBox::Save ) {
                if( !saveFile() ) { return false; }

            // when the user doen't choose to disacard the file exit
            } else if( but != QMessageBox::Discard  ) {
                return false;
            }
        }

        // remove the tab
        tabWidgetRef_->removeTab(idx);
        updatePersistedState(); // maybe the persisted state has changed
        return true;
    }
    // this only happens when there's no active tab
    return false;
}


/// saves the file
bool MainWindow::saveFile()
{
    edbee::TextEditorWidget* widget = tabEditor();
    edbee::TextDocument* doc = widget->textDocument();
    if( !widget ) { return false; }
    QString fileName = widget->property("file").toString();
    if( fileName.isEmpty() ) {
        return saveFileAs();
    }
    QFile file( fileName );

    edbee::TextDocumentSerializer serializer( widget->textDocument() );
    if( !serializer.save( &file ) ) {
        QMessageBox::warning(this, tr("Error saving file"), tr("Error saving file %1:\n%2").arg(fileName).arg(serializer.errorString()) );
        return false;
    }

    doc->setPersisted();
    return true;
}


/// Saves the file as a given name
bool MainWindow::saveFileAs()
{
    edbee::TextEditorWidget* widget = tabEditor();
    if( !widget ) { return false; }

    QString filename = QFileDialog::getSaveFileName( this, tr("Save file as") );
    if( filename.isEmpty() ) { return false; }
    widget->setProperty("file",filename);
    if( saveFile() ) {
        updateTabName();
        return true;
    }
    widget->setProperty("file","");
    return false;
}


/// Creates a blank workspace without filename.
/// At the moment it closes all other windows and files
void MainWindow::newWorkspace()
{
    qApp->setQuitOnLastWindowClosed(false);    // disable auto application quit on closing last window

    // close the current workspace
    edbeeApp()->closeWorkspace();

    // create a new workspace
    Workspace* workspace = new Workspace();
    edbeeApp()->giveWorkspace( workspace );
    edbeeApp()->windowManager()->createAndShowWindowIfEmpty();  // make sure at least one window is available

    // re-enable quit on close
    qApp->setQuitOnLastWindowClosed(true);
}


/// Opens the given workspace
/// @param file the file of the workspace
bool MainWindow::openWorkspace( const QString& fileName )
{
    /// TODO Implement this
    QFileInfo fileInfo(fileName);

    // file not found ??
    if( !fileInfo.exists() ) {
        QMessageBox::warning(this, tr("File not found"), tr("The file could not be found!)") );
        return false;
    }

    // file not readable?
    if( !fileInfo.isReadable() ) {
        QMessageBox::warning(this, tr("File not accessible"), tr("The file could not be read!)") );
        return false;
    }

    // close the current workspace
    edbeeApp()->setQuitOnLastWindowClosed(false);
    edbeeApp()->closeWorkspace();

    // create the widget and serialize the file
    WorkspaceSerializer serializer;
    serializer.loadWorkspace( fileName );

    // make sure there's always a single window
    edbeeApp()->windowManager()->createAndShowWindowIfEmpty();
    edbeeApp()->setQuitOnLastWindowClosed(true);

    return true;
}


/// This method opens the workspace by retrieving the filename
/// from the senders() QAction userdata
/// This method is used for the recent workspace list
bool MainWindow::openWorkspaceWithActionDataFilename()
{
    // we expect an action here
    QAction* action= qobject_cast<QAction*>(sender());
    if( action ) {
        QString filename = action->data().toString();
        return openWorkspace( filename );
    }
    return false;
}


/// open the workspace and shows a dialog
/// @returns false on error or if no file is selected
bool MainWindow::openWorkspace()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Openen Workspace"),   // title
        QString(),              // current path
        Workspace::fileDialogFilter()
    );
    if( !fileName.isEmpty() ) {
        return openWorkspace(fileName);
    }
    return false;
}


/// Saves the current workspace
/// @returns true on success false on error
bool MainWindow::saveWorkspace()
{
    // when no file has been given show a file-chooser
    if( workspaceRef_->filename().isEmpty() ) {
        return saveWorkspaceAs();
    }

    // serialize the workspace
    WorkspaceSerializer serializer;
    if( !serializer.saveWorkspace( workspaceRef_ ) ) {
        QMessageBox::warning(this, tr("Error saving workspace"), tr("Error saving workspace %1:\n%2").arg(workspaceRef_->filename()).arg(serializer.errorMessage()) );
        return false;
    }
    return true;
}


/// Saves the current workspace as a given name
bool MainWindow::saveWorkspaceAs()
{
    // show the file dialog
    QFileDialog dlg(this, tr("Save workspace as"), QString(), Workspace::fileDialogFilter() );
    dlg.setWindowModality( Qt::WindowModal );
    dlg.setAcceptMode( QFileDialog::AcceptSave );
    if( !workspaceRef_->filename().isEmpty() ) {       // set the inital selection
        dlg.selectFile( workspaceRef_->filename() );
    }

    // show the dialog
    if( !dlg.exec() ) {
        return false;
    }

    // retrieve the selected filename
    QString filename =dlg.selectedFiles().first();
    if( filename.isEmpty() ) { return false; }

    QString oldFilename = workspaceRef_->filename();

    // create a new workspace
    workspaceRef_->setFilename( filename );
    if( saveWorkspace() ) {

        // add the new filename to the recent workspace filenamelist
        edbeeApp()->addToRecentWorkspaceFilenameList( workspaceRef_->filename() );
        return true;
    }
    workspaceRef_->setFilename( oldFilename );
    return false;
}


/// opens a new window
void MainWindow::windowNew()
{
    edbeeApp()->windowManager()->createWindow( edbeeApp()->workspace() )->show();
}


/// closes this window
void MainWindow::windowClose()
{
    close();
}


/// Updates the persisted state of the document
void MainWindow::updatePersistedState()
{
    bool persisted = true;
    for( int i=0; i < this->tabWidgetRef_->count(); ++i ) {
        edbee::TextEditorWidget* textEditor = tabEditor(i);
        if( textEditor ) {
            persisted = persisted && textEditor->textDocument()->isPersisted();
        }
    }
    setWindowModified(!persisted);
}


/// this method updates the tab-name
void MainWindow::updateTabName(int tabIndex)
{
    if( tabIndex < 0 ) { tabIndex = tabWidgetRef_->currentIndex(); }
    edbee::TextEditorWidget* widget = tabEditor(tabIndex);
    if( !widget ) { return; }
    QString filename = widget->property("file").toString();
    QString tabName("Untitled");
    if( !filename.isEmpty() ) {
        QFileInfo fileInfo(filename);
        tabName = fileInfo.fileName();
    }
    tabWidgetRef_->setTabText( tabIndex, tabName );

}


/// The active tab has change
/// This requires the updating of the ui controls
void MainWindow::activeTabChanged()
{
    edbee::TextEditorWidget* widget = tabEditor();
    if( widget ) {
        edbee::TextDocument* doc = widget->textDocument();

        // select the line type
        edbee::TextGrammar* grammar = doc->languageGrammar();
        this->grammarComboRef_->setCurrentText( grammar->displayName() );

        // select the correct encoding in the combobox
        edbee::TextCodec* codec = doc->encoding();
        encodingComboRef_->setCurrentText(codec->name());

        // select the correct line ending
        const edbee::LineEnding* lineEnding = doc->lineEnding();
        lineEndingComboRef_->setCurrentIndex( lineEnding->type() );

        // set the filename in the window menu
        QString filename = widget->property("file").toString();
        setWindowFilePath(filename);
        setWindowTitle( filename.isEmpty() ? qApp->applicationDisplayName() : tr("%1 - %2").arg(filename).arg(qApp->applicationDisplayName()) );
    }

    updateStateEditorActions();
}


/// Selects the next tab as the active file
void MainWindow::gotoNextTab()
{
    if( tabWidgetRef_->count() > 0 ) {
        int nextIndex = ( tabWidgetRef_->currentIndex() + 1 ) % tabWidgetRef_->count();
        tabWidgetRef_->setCurrentIndex( nextIndex );
    }
}


/// Selects the previous tab as active file
void MainWindow::gotoPrevTab()
{
    if( tabWidgetRef_->count() > 0 ) {
        int nextIndex = ( tabWidgetRef_->currentIndex() + tabWidgetRef_->count() - 1 ) % tabWidgetRef_->count();
        tabWidgetRef_->setCurrentIndex( nextIndex );
    }
}


/// Goes to the given file
/// If the file is open it activates the correct tab
/// If the file isn't open the file is opened in a new tab
/// @param file the filename to goto
void MainWindow::gotoFile(const QString& file)
{
    for( int i=0,cnt=tabCount(); i<cnt; ++i ) {
        if( tabFilename(i) == file ) {
            setActiveTabIndex(i);
            return;
        }
    }
    openFile( file );
}


/// This slot is called if the encoding is changed in the combo
/// The encoding setting of the document is changed
void MainWindow::encodingChanged()
{
    edbee::TextEditorWidget* widget = tabEditor();
    if( widget ) {
        edbee::TextDocument* doc = widget->textDocument();
        edbee::TextCodec* codec = edbee::Edbee::instance()->codecManager()->codecForName( encodingComboRef_->currentText().toLatin1() );
        if( codec ) {
            doc->setEncoding(codec);
        }
    }
}


/// This slot is called if the line ending widget is chagned.
/// The line-ending type of the editor document is changed
void MainWindow::lineEndingChanged()
{
    edbee::TextEditorWidget* widget = tabEditor();
    if( widget ) {
        edbee::TextDocument* doc = widget->textDocument();
        edbee::LineEnding* lineEnding = edbee::LineEnding::get( lineEndingComboRef_->currentIndex() );
        if( lineEnding ) {
            doc->setLineEnding( lineEnding );
        }
    }
}


/// This slot is called if the grammar is changed
/// Here we forward the selected grammar to the textdocument of the editor
void MainWindow::grammarChanged()
{
    edbee::TextEditorWidget* widget = tabEditor();
    if( widget ) {
        edbee::TextDocument* doc = widget->textDocument();
        QString name = grammarComboRef_->itemData( grammarComboRef_->currentIndex() ).toString();
        edbee::TextGrammar* grammar = edbee::Edbee::instance()->grammarManager()->get(name);
        if( grammar) {
            doc->setLanguageGrammar( grammar );
        }
    }
}


/// executed when an editor action is triggered
void MainWindow::editorActionTriggered()
{
    // retrieve the action and the widget
    QAction* action = qobject_cast<QAction*>(sender());
    edbee::TextEditorWidget* widget = tabEditor();
    if( action && widget ) {

        // execute the command when it's triggered
        edbee::TextEditorCommand* command = widget->commandMap()->get( action->data().toString() );
        if( command ) {
            widget->controller()->executeCommand( command );
        }
    }
}


/// this method updates the action states for the current editor state.
void MainWindow::updateStateEditorActions()
{
    edbee::TextEditorWidget* widget = tabEditor();
    QHash<QString,QAction*>& am = actionMap_;
    bool focus = false;
    if( widget ) {
        focus = widget->hasFocus();
    }

    // default action is to enabled all actions
    foreach( QAction* action, this->actionMap_.values()) {
        action->setEnabled(focus);
    }

    am.value("undo")->setEnabled( focus && widget->textDocument()->textUndoStack()->canUndo());
    am.value("redo")->setEnabled( focus && widget->textDocument()->textUndoStack()->canRedo());

    // always enable these
    am.value("file.new")->setEnabled(true);
    am.value("file.open")->setEnabled(true);
}


/// thows the goto-entry popup
void MainWindow::showGotoEntryPopup()
{
    edbee::TextEditorWidget* widget = tabEditor();
    if( widget ) {
        GotoWidget* gotoWidget = new GotoWidget( widget );
        QPoint pos = widget->mapToGlobal( widget->pos() );
        gotoWidget->show();
        gotoWidget->move( pos.x() + widget->width()/2 - gotoWidget->width()/2, pos.y()  ); // center top of editor widget
    }
}


/// This method shows the find widget
void MainWindow::showFindWidget()
{
    edbee::TextEditorWidget* widget = tabEditor();
    if( widget ) {
        FindWidget* findWidget = (FindWidget*)widget->property("findWidget").value<void *>();
        if( !findWidget ) {
            findWidget = new FindWidget( widget );
            widget->layout()->addWidget(findWidget);

            // set the find widget
            QVariant v = qVariantFromValue((void *)findWidget);
            widget->setProperty("findWidget",v);
        }
        findWidget->show();
        findWidget->activate();
        widget->updateGeometryComponents();


//        QPoint pos = widget->mapToGlobal( widget->pos() );
//        findWidget->show();
//        findWidget->move( pos.x() + widget->width()/2 - findWidget->width()/2, pos.y()  ); // center top of editor widget
    }
}


/// This method updates the actions
void MainWindow::updateActions()
{
    updateStateEditorActions();

}


/// Updates the recent menu item list
void MainWindow::updateRecentWorkspaceMenuItems()
{
    // remove all existing menu items (perhaps, we should delete the actions)
    qDeleteAll(recentItemsMenuRef_->actions()); // I need to remove all actions, else they will stay alive till the end of the current window
    recentItemsMenuRef_->clear();

    // add all recent files
    QStringList recentList = edbeeApp()->recentWorkspaceFilenameList();
    foreach( QString filename, recentList ) {
        // create the action
        QAction* action = new QAction( filename, recentItemsMenuRef_ );
        action->setData( filename );
        connect( action, SIGNAL(triggered()), this, SLOT(openWorkspaceWithActionDataFilename()) );

        // add the action the recent menu list
        recentItemsMenuRef_->addAction( action );
    }

    /// Add the clear recent workspace list action
    recentItemsMenuRef_->addSeparator();
    QAction* clearListAction = new QAction( tr("Clear List"), recentItemsMenuRef_);
    connect( clearListAction, &QAction::triggered, edbeeApp(), &Application::clearRecentWorkspaceFilenameList );
    recentItemsMenuRef_->addAction( clearListAction );
}


/// This method shows a custom editor context menu
void MainWindow::editorContextMenu()
{
    // retrieve the current controller and editor
    edbee::TextEditorWidget* editor = tabEditor();
    if( editor ) {
        edbee::TextEditorController* controller = tabEditor()->controller();

        // create the menu
        QMenu* menu = new QMenu();
        menu->addAction( controller->createAction( "cut", tr("Cut") ) );
        menu->addAction( controller->createAction( "copy", tr("Copy") ) );
        menu->addAction( controller->createAction( "paste", tr("Paste") ) );
        menu->addSeparator();
        menu->addAction( controller->createAction( "sel_all", tr("Select All") ) );

        // is a file coupled to the curent editor add 'reveal in sidebar'
        if( !tabFilename().isEmpty() ) {
            menu->addSeparator();
            menu->addAction( controller->createAction( "app.reveal-in-sidebar", tr("Reveal in sidebar") ) );
        }

        /// shows the contextmenu
        menu->exec( QCursor::pos() );

        // cleaup the contextmenu
        qDeleteAll( menu->actions() );
        delete menu;
    }
}


/// When a file or folder is dropped try to open it
void MainWindow::dropEvent(QDropEvent* event)
{
    // check for our needed mime type, here a file or a list of files
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (int i = 0; i < urlList.size() && i < 32; ++i) {
            openDirOrFile(urlList.at(i).toLocalFile());
        }
    }
}


/// A drag enter event is recieved
void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        event->accept();
    } else {
        event->ignore();
    }
}


/// When the window is closed check if a tab has been changed
/// ask for saving the changes
void MainWindow::closeEvent(QCloseEvent* event)
{
    // check if all files a clean
    for( int i=tabCount()-1; i>= 0; --i ) {
        // close the tab. When the tab isn't closed return and ignore the close evenet
        if( !closeFileWithTabIndex(i) ) {
            event->ignore();
            return;
        }
    }

    // emit a close event (this is required for the window-manager to know the window is closed)
    emit windowClosed();
    QMainWindow::closeEvent(event);
}


/// This method simply creates new editor. Applying all settings
edbee::TextEditorWidget* MainWindow::createEditorWidget()
{
    // create the widget and apply the configuration
    edbee::TextEditorWidget* result = new edbee::TextEditorWidget();
    edbeeApp()->config()->applyToWidget( result );

    // connect our custom contextmenu to the widget
    result->textEditorComponent()->setContextMenuPolicy( Qt::CustomContextMenu  );
    connect( result->textEditorComponent(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(editorContextMenu()) );
    return result;
}


/// Returns the action in the actionmap with the given name
/// @param name the name of the action to retrieve
/// @return the action with the given name or 0 if no action was found
QAction *MainWindow::action(const QString& name)
{
    return actionMap_.value(name);
}


//==[ construction ]===============================================================================


/// creates a text-editor action
void MainWindow::createEditorAction(const QString& id, const char* text )
{
    QAction* action = new QAction( tr(text), 0 );
    edbee::TextEditorKeyMap* map = edbee::Edbee::instance()->defaultKeyMap();
    action->setShortcut( map->get( id )->sequence() );
    action->setData( id );
    connect( action, SIGNAL(triggered()), SLOT(editorActionTriggered()) );
    actionMap_.insert(id,action);
}


/// creates an action and adds it to the actionmap
/// @param id the unique identifier for this action
/// @param text the text for this action
/// @param keySequence the shortcut key sequence
/// @param object the object that needs to recieve the events of the object
/// @param slot the that should recieve the event
void MainWindow::createAction(const QString& id, const QString& text, const QKeySequence& keySequence, QObject* object, const char* slot)
{
    QAction* action = new QAction( text, 0);
    action->setShortcut(keySequence);
    action->setData(id);
    connect( action, SIGNAL(triggered()), object, slot );
    actionMap_.insert(id,action);
}


/// Constructs all actions
void MainWindow::constructActions()
{
    createAction( "file.new", tr("&New File"), QKeySequence::New, this, SLOT(newFile()) );
    createAction( "file.open", tr("&Open File"), QKeySequence::Open, this, SLOT(openFile()) );
    createAction( "file.close", tr("&Close File"), QKeySequence::Close, this, SLOT(closeFileWithTabIndex()) );
    createAction( "file.save", tr("&Save"), QKeySequence::Save, this, SLOT(saveFile()) );
    createAction( "file.save_as", tr("&Save As..."), QKeySequence::SaveAs, this, SLOT(saveFileAs()) );


    createAction( "find.find", tr("&Find..."), QKeySequence::Find, this, SLOT(showFindWidget()));
    createAction( "goto.line", tr("&Goto Line..."), QKeySequence( Qt::META + Qt::Key_G), this, SLOT(showGotoEntryPopup()) );

    createAction( "goto.prev_tab", tr("Previous Tab"), QKeySequence::PreviousChild, this, SLOT(gotoPrevTab()) );
    createAction( "goto.next_tab", tr("Next Tab"), QKeySequence::NextChild, this, SLOT(gotoNextTab()) );

    createAction( "workspace.new", tr("&New Workspace"), QKeySequence(), this, SLOT(newWorkspace()) );
    createAction( "workspace.open", tr("&Open Workspace..."), QKeySequence(), this, SLOT(openWorkspace()) );
    createAction( "workspace.save", tr("Save Workspace"), QKeySequence(), this, SLOT(saveWorkspace()) );
    createAction( "workspace.save_as", tr("&Save Workspace As..."), QKeySequence(), this, SLOT(saveWorkspaceAs()) );

    createAction("win.new", tr("&New Window"), QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_N ), this, SLOT(windowNew() ) );
    createAction("win.close", tr("&Close Window"), QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_W ), this, SLOT(windowClose() ) );
    createAction("win.minimize",tr("&Minimize"), QKeySequence(Qt::CTRL + Qt::Key_M ), this, SLOT(showMinimized()) );
    createAction("win.maximize",tr("&Zoom"), QKeySequence(), this, SLOT(showMaximized()) );
    createAction("win.fullscreen",tr("Enter FullScreen"), QKeySequence::FullScreen, this, SLOT(showFullScreen()) );

    createEditorAction( "undo", "&Undo" );
    createEditorAction( "redo", "&Redo" );
    createEditorAction( "soft_undo", "Soft Undo" );
    createEditorAction( "soft_redo", "Soft Redo" );

    createEditorAction( "cut", "&Cut" );
    createEditorAction( "copy", "&Copy" );
    createEditorAction( "paste", "&Paste" );
}


/// constructing the base user interface
void MainWindow::constructUI()
{
    this->setWindowTitle( qApp->applicationDisplayName() );


    // statusbar
    QFont font = QFont(statusBar()->font().family(), 10 );
    statusBar()->setFont(font);
    statusBar()->showMessage(tr("Ready"));
    statusBar()->addPermanentWidget(constructGrammarCombo());
    statusBar()->addPermanentWidget(constructLineEndingCombo());
    statusBar()->addPermanentWidget(constructEncodingCombo());

    // the splitter
    QSplitter* splitter = new QSplitter(Qt::Horizontal);

    // create the tab widget
    tabWidgetRef_ = new QTabWidget();
    tabWidgetRef_->setTabsClosable(true);
    tabWidgetRef_->setTabShape(QTabWidget::Triangular);
    tabWidgetRef_->setDocumentMode(true);
    tabWidgetRef_->setMovable(true);
    tabWidgetRef_->setUsesScrollButtons(true);


    // build the splitter
    fileTreeSideWidgetRef_ = new FileTreeSideWidget();
    splitter->addWidget(fileTreeSideWidgetRef_);
    splitter->addWidget(tabWidgetRef_);
    splitter->setStretchFactor(0,0);
    splitter->setStretchFactor(1,1);
    splitter->setLineWidth(0);


//    splitter->setSizePolicy(0,QSizePolicy::Minimum);
//    splitter->setSizePolicy(1,QSizePolicy::Expanding);

    // add the splitter
    setCentralWidget(splitter);
    resize( 630, 400 );

    // we accept drops
    setAcceptDrops(true);
}


/// Constructs the grammar combobox with all loaded grammars
QComboBox* MainWindow::constructGrammarCombo()
{
    grammarComboRef_ = new QComboBox();
    grammarComboRef_->setMinimumWidth(100);
    edbee::TextGrammarManager* gr = edbee::Edbee::instance()->grammarManager();

    // retrieve all grammar names and sort them
    QStringList grammarNames = gr->grammarNames();
    grammarNames.sort( Qt::CaseInsensitive );

    // next add all grammars in the given list
    grammarNames.removeOne(""); // remove the plaint-text grammar
    grammarNames.push_front(""); // place, plaint text as the first language


    // next add all grammars
    foreach( QString name, grammarNames ) {
        edbee::TextGrammar* grammar = gr->get(name);
        if( grammar ) {
            grammarComboRef_->addItem(grammar->displayName(), grammar->name());
        }
    }
    return grammarComboRef_;
}


/// Constucts the line eding combobox and fills it with all line ending types
QComboBox* MainWindow::constructLineEndingCombo()
{
    // add the line-endings
    lineEndingComboRef_ = new QComboBox();
    lineEndingComboRef_->setMinimumWidth(100);
    for( int i=0, cnt = edbee::LineEnding::typeCount(); i<cnt; ++i  ) {
        const edbee::LineEnding* ending = edbee::LineEnding::get(i);
        lineEndingComboRef_->addItem( QString("%1 (%2)").arg(ending->name()).arg(ending->escapedChars()), edbee::LineEnding::get(i)->name() );
    }
    return lineEndingComboRef_;
}


/// Constructs the encoding combobox
/// And fills the combobox with all encodings known to the codecManager
QComboBox* MainWindow::constructEncodingCombo()
{
    // add the line-endings
    encodingComboRef_ = new QComboBox();
    encodingComboRef_->setMinimumWidth(100);

    QList<edbee::TextCodec*> codecs = edbee::Edbee::instance()->codecManager()->codecList();
    foreach( edbee::TextCodec* codec, codecs ) {
        encodingComboRef_->addItem( codec->name() );
    }
    return encodingComboRef_;
}


/// Builds the main menu of the application window
void MainWindow::constructMenu()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction( action("file.new"));
    fileMenu->addAction( action("file.open"));
    fileMenu->addSeparator();
    fileMenu->addAction( action("file.save"));
    fileMenu->addAction( action("file.save_as") );
    fileMenu->addSeparator();
    fileMenu->addAction( action("file.close"));

    fileMenu->addSeparator();
    fileMenu->addAction( action("win.new"));     // Isn't the window menu a more logical place for this command?
    fileMenu->addAction( action("win.close"));

    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction( action("undo") );
    editMenu->addAction( action("redo") );
    editMenu->addSeparator();
    editMenu->addAction( action("soft_undo") );
    editMenu->addAction( action("soft_redo") );
    editMenu->addSeparator();
    editMenu->addAction( action("cut") );
    editMenu->addAction( action("copy") );
    editMenu->addAction( action("paste") );

    QMenu* findMenu = menuBar()->addMenu(("&Find"));
    findMenu->addAction( action("find.find"));

    QMenu* gotoMenu = menuBar()->addMenu(("&Goto"));
    gotoMenu->addAction( action("goto.line"));
    gotoMenu->addAction( action("goto.prev_tab"));
    gotoMenu->addAction( action("goto.next_tab"));

    // add the workspace menu items
    QMenu* workspaceMenu = menuBar()->addMenu("Workspace");
    workspaceMenu->addAction( action("workspace.new") );
    workspaceMenu->addSeparator();
    workspaceMenu->addAction( action("workspace.open") );
    recentItemsMenuRef_ = workspaceMenu->addMenu( tr("Open Recent") );
    workspaceMenu->addSeparator();
    workspaceMenu->addAction( action("workspace.save") );
    workspaceMenu->addAction( action("workspace.save_as") );

    // add the window menu items
    QMenu* windowMenu = menuBar()->addMenu(("&Window"));
    windowMenu->addAction( action("win.minimize"));
    windowMenu->addAction( action("win.maximize"));
    windowMenu->addSeparator();
    windowMenu->addAction( action("win.fullscreen"));
    windowMenu->addSeparator();
    windowMenu->addAction( action("win.new"));     // I think it is, so we add those options here too :)
    windowMenu->addAction( action("win.close"));

    // update the workspace menu items menu
    updateRecentWorkspaceMenuItems();

}


/// This method connects some signals between the user interface controls
void MainWindow::connectSignals()
{
    connect( tabWidgetRef_, SIGNAL(tabCloseRequested(int)), SLOT(closeFileWithTabIndex(int)) );
    connect( tabWidgetRef_, SIGNAL(currentChanged(int)), SLOT(activeTabChanged()) );

    connect( lineEndingComboRef_, SIGNAL(currentIndexChanged(int)), SLOT(lineEndingChanged()) );
    connect( encodingComboRef_, SIGNAL(currentIndexChanged(int)), SLOT(encodingChanged()) );
    connect( grammarComboRef_, SIGNAL(currentIndexChanged(int)), SLOT(grammarChanged()) );

    // tree menu actions
    connect( fileTreeSideWidgetRef_,SIGNAL(fileDoubleClicked(QString)), SLOT(gotoFile(QString)) );
    connect( edbeeApp(), &Application::recentWorkspaceFilenameListChanged,  this, &MainWindow::updateRecentWorkspaceMenuItems );
}

