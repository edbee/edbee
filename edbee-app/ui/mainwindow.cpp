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
#include "models/edbeeconfig.h"
#include "ui/windowmanager.h"

#include "debug.h"



/// A build in warning size to warn for big file sizes
static const int FileSizeWarning = 1024*1024*20;  // Larger then 20 MB give a warning!


/// Constructor for a main application windows
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , fileTreeSideWidgetRef_(0)
    , tabWidgetRef_(0)
    , statusBarRef_(0)
    , grammarComboRef_(0)
    , lineEndingComboRef_(0)
    , encodingComboRef_(0)
{
    constructActions();
    constructUI();
    constructMenu();
    connectSignals();
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
/// @param idx the tab index
/// @return the filename
QString MainWindow::tabFilename(int idx) const
{
    edbee::TextEditorWidget* widget = this->tabEditor(idx);
    if( widget ) {
        return widget->property("file").toString();
    }
    return QString();
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
void MainWindow::openFile(const QString& fileName)
{
    QFileInfo fileInfo(fileName);

    // file not found ??
    if( !fileInfo.exists() ) {
        QMessageBox::warning(this, tr("File not found"), tr("The file could not be found!)") );
        return;
    }

    // file not readable?
    if( !fileInfo.isReadable() ) {
        QMessageBox::warning(this, tr("File not accessible"), tr("The file could not be read!)") );
        return;
    }

    // safeguard for large files
    if( fileInfo.size() >= FileSizeWarning ) {
        if( QMessageBox::question(this, tr("Open very large file?"), tr("Warning file is larger then %1 MB. Open file?").arg(FileSizeWarning/1024/1024) ) != QMessageBox::Yes ) {
            return;
        }
    }

    // open the file
    QFile file(fileName);

    // create the widget and serialize the file
    edbee::TextEditorWidget* widget = createEditorWidget();
    edbee::TextDocumentSerializer serializer( widget->textDocument() );
    if( !serializer.load( &file ) ) {
        QMessageBox::warning(this, tr("Error opening file"), tr("Error opening file:\n%1").arg(serializer.errorString()) );
        delete widget;
        return;
    }
    addEditorTab( widget, fileInfo.filePath() );
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
/// @param idx the index of the tab to close
void MainWindow::closeFileWithTabIndex(int idx)
{
    if( idx < 0 ) {
        idx = tabWidgetRef_->currentIndex();
    }
    if( idx >= 0 ) {
        edbee::TextEditorWidget* widget = tabEditor( idx );
        if( widget && !widget->textDocument()->isPersisted() ) {
            QMessageBox::StandardButton but = QMessageBox::question(this, tr("Save changes?"), tr("File has been changed, do you want to save the changes?"), QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel );
            if( but == QMessageBox::Save ) {
                if( !saveFile() ) { return; }
            } else if( but != QMessageBox::Discard  ) {
                return;
            }
        }

        tabWidgetRef_->removeTab(idx);
        updatePersistedState(); // maybe the persisted state has changed
    }
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
        QMessageBox::warning(this, tr("Error saving file"), tr("Error savingfile:\n%1").arg(serializer.errorString()) );
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

/// opens a new window
void MainWindow::windowNew()
{
    edbeeApp()->windowManager()->createWindow()->show();
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

/// executes if an action is triggered
void MainWindow::editorActionTrigged()
{
     QAction* action = qobject_cast<QAction*>(sender());
     edbee::TextEditorWidget* widget = tabEditor();
     if( action && widget ) {
         edbee::TextEditorCommand* command = widget->commandMap()->get( action->data().toString() );
         command->execute( widget->controller() );
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

void MainWindow::showFindPopup()
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


void MainWindow::dropEvent(QDropEvent*event)
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


/// When the window is closed the close event need to handled
void MainWindow::closeEvent(QCloseEvent* event)
{
//    if (maybeSave()) {
//        writeSettings();
//        event->accept();
//    } else {
//        event->ignore();
//    }
    emit windowClosed();
    QMainWindow::closeEvent(event);
}


/// This method simply creates new editor. Applying all settings
edbee::TextEditorWidget* MainWindow::createEditorWidget()
{
    edbee::TextEditorWidget* result = new edbee::TextEditorWidget();
    edbeeApp()->config()->applyToWidget( result );
    return result;
}

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
    connect( action, SIGNAL(triggered()), SLOT(editorActionTrigged()) );
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



void MainWindow::constructActions()
{
    createAction( "file.new", tr("&New File"), QKeySequence::New, this, SLOT(newFile()) );
    createAction( "file.open", tr("&Open File"), QKeySequence::Open, this, SLOT(openFile()) );
    createAction( "file.close", tr("&Close File"), QKeySequence::Close, this, SLOT(closeFileWithTabIndex()) );
    createAction( "file.save", tr("&Save"), QKeySequence::Save, this, SLOT(saveFile()) );
    createAction( "file.save_as", tr("&Save As..."), QKeySequence::SaveAs, this, SLOT(saveFileAs()) );


    createAction( "find.find", tr("&Find..."), QKeySequence::Find, this, SLOT(showFindPopup()));
    createAction( "goto.line", tr("&Goto Line..."), QKeySequence( Qt::META + Qt::Key_G), this, SLOT(showGotoEntryPopup()) );

    createAction( "goto.prev_tab", tr("Previous Tab"), QKeySequence::PreviousChild, this, SLOT(gotoPrevTab()) );
    createAction( "goto.next_tab", tr("Next Tab"), QKeySequence::NextChild, this, SLOT(gotoNextTab()) );


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

    QList<edbee::TextGrammar*> grammars = gr->grammars();
//    qSort(grammars);
    foreach( edbee::TextGrammar *grammar, grammars ) {
        grammarComboRef_->addItem(grammar->displayName(), grammar->name());
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


/// BUilds the main menu of the application window
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


    QMenu* windowMenu = menuBar()->addMenu(("&Window"));
    windowMenu->addAction( action("win.minimize"));
    windowMenu->addAction( action("win.maximize"));
    windowMenu->addSeparator();
    windowMenu->addAction( action("win.fullscreen"));
    windowMenu->addSeparator();
    windowMenu->addAction( action("win.new"));     // I think it is, so we add those options here too :)
    windowMenu->addAction( action("win.close"));

}


void MainWindow::connectSignals()
{
    connect( tabWidgetRef_, SIGNAL(tabCloseRequested(int)), SLOT(closeFileWithTabIndex(int)) );
    connect( tabWidgetRef_, SIGNAL(currentChanged(int)), SLOT(activeTabChanged()) );

    connect( lineEndingComboRef_, SIGNAL(currentIndexChanged(int)), SLOT(lineEndingChanged()) );
    connect( encodingComboRef_, SIGNAL(currentIndexChanged(int)), SLOT(encodingChanged()) );
    connect( grammarComboRef_, SIGNAL(currentIndexChanged(int)), SLOT(grammarChanged()) );


    // tree menu actions
    connect( fileTreeSideWidgetRef_,SIGNAL(fileDoubleClicked(QString)), SLOT(openFile(QString)) );

}

