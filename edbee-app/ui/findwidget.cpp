/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "findwidget.h"

#include <QAction>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>
#include <QLabel>
#include <QLineEdit>

#include "application.h"
#include "edbee/models/changes/selectiontextchange.h"
#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textsearcher.h"
#include "edbee/models/texteditorkeymap.h"
#include "edbee/texteditorwidget.h"
#include "edbee/util/regexp.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"
#include "QtAwesome.h"

#include "debug.h"


FindWidget::FindWidget(edbee::TextEditorWidget* parent)
    : QWidget(parent)
    , editorRef_( parent )
    , lineEditRef_(0)
{
    constructUI();

    /// when return is pressed we move to the given line
    connect(lineEditRef_,SIGNAL(returnPressed()), this, SLOT(enterPressed()) );

    // delete when editing is done
    connect(lineEditRef_,SIGNAL(editingFinished()), this, SLOT(deactivate()) );

    // create a close editor action
    QAction* closeAction = new QAction(lineEditRef_);
    closeAction->setShortcut( Qt::Key_Escape );
    connect( closeAction,SIGNAL(triggered()), this, SLOT(close()) );
    lineEditRef_->addAction(closeAction);

    // move the focus to the editor line
    lineEditRef_->setFocus();
    setFocusProxy(lineEditRef_);

}

void FindWidget::findNextWord()
{
    edbee::TextEditorController* controller = editorRef_->controller();
    edbee::TextSearcher* searcher = controller->textSearcher();
    searcher->setSearchTerm(lineEditRef_->text() );
    searcher->findNext( editorRef_ );
}

void FindWidget::findPrevWord()
{
    edbee::TextEditorController* controller = editorRef_->controller();
    edbee::TextSearcher* searcher = controller->textSearcher();
    searcher->setSearchTerm(lineEditRef_->text() );
    searcher->findPrev( editorRef_ );
}

void FindWidget::selectAllWords()
{
    edbee::TextEditorController* controller = editorRef_->controller();
    edbee::TextSearcher* searcher = controller->textSearcher();
    searcher->setSearchTerm(lineEditRef_->text() );
    searcher->selectAll( editorRef_ );
}


void FindWidget::activate()
{
    lineEditRef_->selectAll();
    lineEditRef_->setFocus();
}

void FindWidget::deactivate()
{
    editorRef_->setFocus();
}

void FindWidget::close()
{
    editorRef_->setFocus();
    hide();
}


/// Enter is pressed in the line-edit widget
void FindWidget::enterPressed()
{
    QString str = lineEditRef_->text().trimmed();

    findNextWord();
    //qlog_info() << "TODO: Implement find!" << str ;
}


void FindWidget::constructUI()
{

    QHBoxLayout* layout = new QHBoxLayout();// QBoxLayout::LeftToRight );
    layout->setMargin(7);
     setLayout( layout );



    QToolButton* toggleButton = new QToolButton(this);
    toggleButton->setFont( Application::instance()->iconFont() );
    toggleButton->setText( QChar(icon_repeat) );
    toggleButton->setToolTip(tr("Wrap Around"));
    toggleButton->setCheckable(true);
    layout->addWidget( toggleButton, 0 );

    toggleButton = new QToolButton(this);
    toggleButton->setFont( Application::instance()->iconFont() );
    toggleButton->setText( QChar(icon_asterisk) );
    toggleButton->setToolTip(tr("Regular Expressions"));
    toggleButton->setCheckable(true);
    layout->addWidget( toggleButton, 0 );

    toggleButton = new QToolButton(this);
    toggleButton->setFont( Application::instance()->iconFont() );
    toggleButton->setText( QChar(icon_font) );
    toggleButton->setToolTip(tr("Case Sensitive"));
    toggleButton->setCheckable(true);
    layout->addWidget( toggleButton, 0 );




    QLabel* label = new QLabel( tr("Find") );
    layout->addWidget( label, 0, Qt::AlignLeft );


    lineEditRef_ = new QLineEdit(this);
//    lineEditRef_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred);
//    lineEditRef_->setSizePolicy( QSizePolicy::M);
//    lineEditRef_->setMaximumWidth( 10000 );
    layout->addWidget( lineEditRef_, 1);//, Qt::AlignLeft);
//    layout->insertStretch(1);

    // add the action

    QToolButton* but = new QToolButton( this );
    but->setFont( Application::instance()->iconFont() );
    but->setText( QChar(icon_caret_left) ); //<
    but->setShortcut( editorRef_->controller()->keyMap()->getSequence("find_prev_match") );
    but->setToolTip( QString(tr("Find Previous (%1)")).arg(but->shortcut().toString()) );
    connect( but, SIGNAL(clicked()), SLOT(findPrevWord()) );
    layout->addWidget( but, 0  );

    but = new QToolButton( this );
    but->setFont( Application::instance()->iconFont() );
    but->setText( QChar(icon_caret_right) ); // >
    but->setShortcut( editorRef_->controller()->keyMap()->getSequence("find_next_match") );
    but->setToolTip( QString(tr("Find Next (%1)")).arg(but->shortcut().toString()) );
    connect( but, SIGNAL(clicked()), SLOT(findNextWord()) );
    layout->addWidget( but, 0 );


    but = new QToolButton( this );
    but->setFont( Application::instance()->iconFont() );
    but->setText( QChar(icon_th) ); // >
    but->setShortcut( editorRef_->controller()->keyMap()->getSequence("sel_all_matches") );
    but->setToolTip( QString(tr("Select All (%1)")).arg(but->shortcut().toString()) );
    connect( but, SIGNAL(clicked()), SLOT(selectAllWords()) );
    layout->addWidget( but, 0 );


//    setWindowFlags( Qt::Popup );
}

