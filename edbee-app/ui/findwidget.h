/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QWidget>

class QButton;
class QLineEdit;

namespace edbee {
class TextEditorWidget;
}

class FindWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindWidget(edbee::TextEditorWidget* parent);
    
signals:

public slots:

    void findNextWord();
    void findPrevWord();
    void selectAllWords();
    void activate();
    void deactivate();
    void close();


protected slots:

    void enterPressed();


private:
    void constructUI();

    edbee::TextEditorWidget* editorRef_;     ///< The reference to the texteditor
    QLineEdit* lineEditRef_;                 ///< The textfield to find
//    QAction* findActionRef_;          ///< Finds the next item

};

