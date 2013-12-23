/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "workspace.h"

#include <QStringListModel>

#include "debug.h"


/// Returns the file-extension used for project filenames
QString Workspace::fileExtension()
{
    static QString extension(".edbee-workspace");
    return extension;
}


/// Returns the dialog filter
QString Workspace::fileDialogFilter()
{
    static QString filter( QObject::tr("edbee workspace files (*.edbee-workspace);;All files (*.*)"));
    return filter;
}


/// Constructs a blank project
Workspace::Workspace()
    : QObject(0)
{
}


/// Copies the new project
/// @param project the project to copy
Workspace::Workspace(const Workspace& workspace )
    : QObject(0)
{
    copyFrom( workspace );
}


/// A copy constructor with a pointer
/// @param project the project pointer. When this pointer is 0 the default constructor is used
Workspace::Workspace(const Workspace* workspace )
    : QObject(0)
{
    if( workspace ) {
        copyFrom( *workspace );
    }
}


/// The project
Workspace::~Workspace()
{
}


/// Fills this project from the data of another project
/// @param project the project to copy het from
void Workspace::copyFrom(const Workspace& workspace )
{
    filename_ = workspace.filename_;
}


/// Sets the filename of the project
void Workspace::setFilename( const QString& filename )
{
    filename_ = filename;
    if( !filename_.endsWith(fileExtension() ) ) {
        filename_ += fileExtension();
    }

    // currently the name is set to the basename of the project
    name_ = QFileInfo(filename).baseName();
}


/// Returns the current filename of the project
QString Workspace::filename() const
{
    return filename_;
}


/// returns the name of this workspace
QString Workspace::name() const
{
    return name_;
}


/// set the name of the project
void Workspace::setName(const QString& name)
{
    bool equal = name == name_;
    name_ = name;
    if( !equal ) {
        emit nameChanged(name_);
    }
}

