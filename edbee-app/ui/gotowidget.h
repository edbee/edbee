/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QWidget>

class QLineEdit;

namespace edbee {
class TextEditorWidget;
}

/// A simple goto-text-widget
class GotoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GotoWidget(edbee::TextEditorWidget* parent = 0);

    
signals:

    void gotoLineColumnEntered( int line, int column );
    
protected slots:

    void enterPressed();

private:

    edbee::TextEditorWidget* editorRef_;  ///< The reference to the texteditor
    QLineEdit* lineEditRef_;              ///< The line text-editor
    
};


