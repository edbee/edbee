/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "project.h"

#include <QStringListModel>

#include "debug.h"

/// Constructs a blank project
Project::Project()
    : rootPathList_(0)
{
    rootPathList_ = new QStringListModel( QStringList("/") );
}


/// Copies the new project
/// @param project the project to copy
Project::Project( const Project& project )
    : rootPathList_(0)
{
    copyFrom( project );
}


/// A copy constructor with a pointer
/// @param project the project pointer. When this pointer is 0 the default constructor is used
Project::Project( const Project* project )
    : rootPathList_(0)
{
    if( project ) {
        copyFrom( *project );
    }
}


/// The project
Project::~Project()
{
    delete rootPathList_;
}


/// Fills this project from the data of another project
/// @param project the project to copy het from
void Project::copyFrom( const Project& project )
{
    filename_ = project.filename_;
    delete rootPathList_;
    rootPathList_= new QStringListModel( project.rootPathList() );
}


/// Sets the filename of the project
void Project::setFilename( const QString& filename )
{
    filename_ = filename;
}


/// Returns the current filename of the project
QString Project::filename() const
{
    return filename_;
}


/// Returns the pointer to the root file list
/// @return a pointer to the string list
QStringListModel* Project::rootPathList() const
{
    return rootPathList_;
}
