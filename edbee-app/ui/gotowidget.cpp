/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "edbee/texteditorcontroller.h"
#include "edbee/texteditorwidget.h"

#include "gotowidget.h"

#include "debug.h"


GotoWidget::GotoWidget(edbee::TextEditorWidget* parent)
    : QWidget(parent)
    , editorRef_(parent)
    , lineEditRef_(0)
{
    lineEditRef_ = new QLineEdit();

    QBoxLayout* boxLayout = new QBoxLayout( QBoxLayout::LeftToRight );
    boxLayout->setMargin(7);
    setLayout( boxLayout );
    boxLayout->addWidget( new QLabel( tr("Goto line[,column]")) );
    boxLayout->addWidget( lineEditRef_, 0, Qt::AlignHCenter );
    setWindowFlags(Qt::Popup);

    /// when return is pressed we move to the given line
    connect(lineEditRef_,SIGNAL(returnPressed()), this, SLOT(enterPressed()) );

    // delete when editing is done
    connect(lineEditRef_,SIGNAL(editingFinished()), this, SLOT(deleteLater()) );

    lineEditRef_->setFocus();
}


/// Enter is pressed in the line-edit widget
void GotoWidget::enterPressed()
{
    QString str = lineEditRef_->text().trimmed();
    QStringList items = str.split(",");
    int line = items.first().toInt();
    if( line > 0 ) { line -= 1; }
    int column = 0;
    if( items.size() > 1 ){
        column = items.last().toInt();
        if( column > 0 ) { column -= 1; }
    }

    edbee::TextEditorController* controller = editorRef_->controller();
    controller->moveCaretTo(line,column,false);
    controller->scrollCaretVisible();
}

