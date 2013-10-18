/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "edbee/texteditorcommand.h"


/// Reveals the current document in the sidebar
class RevealInSidebarCommand : public edbee::TextEditorCommand
{
public:
    virtual void execute( edbee::TextEditorController* controller );
    virtual QString toString();

};

