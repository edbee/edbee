/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "revealinsidebarcommand.h"

#include "application.h"
#include "edbee/texteditorcontroller.h"
#include "ui/filetreesidewidget.h"
#include "ui/mainwindow.h"
#include "ui/windowmanager.h"

#include "debug.h"


/// Executes the reveal in side-bar command
void RevealInSidebarCommand::execute( edbee::TextEditorController* controller )
{
    Q_UNUSED(controller);

    // FIXME: This is probably not correct. It's better to find the MainWindow with the help of the current controller!
    // For the moment this solution is sufficient
    MainWindow* window = edbeeApp()->windowManager()->activeWindow();
    if( window ) {

        // FIXME: This needs to be different too. We should get the filename with the help of the given controller
        // This is also sufficient for now
        QString filename = window->tabFilename();
        if( !filename.isEmpty() ) {
            window->fileTreeSideWidget()->reveal( filename );
        }
    }
}


/// Returns the command name
QString RevealInSidebarCommand::toString()
{
    return "RevealInSideBar";
}
