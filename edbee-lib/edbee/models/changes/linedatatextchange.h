/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "edbee/models/textchange.h"

namespace edbee {

class TextDocument;
class TextLineData;

/// a class to handle line-data textchanges
class LineDataTextChange : public TextChange
{
public:
    LineDataTextChange(int line, int field);
    virtual ~LineDataTextChange();

    void giveLineData( TextLineData* lineData );

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* doc);

    virtual bool merge(TextDocument* document, TextChange* textChange );

    virtual void moveLine( int delta );

    virtual QString toString();

    int line() { return line_; }
    void setLine( int line ) { line_ = line; }

    int field() { return field_; }
    void setField( int field )  { field_ = field; }

private:
    void changeLineData( TextDocument* doc );

private:

    int line_;                      ///< The line number
    int field_;                     ///< The field index
    TextLineData* lineData_;        ///< The text-line data

};

} // edbee
