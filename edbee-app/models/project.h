/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>

class QStringListModel;

/// This class represents a single project
class Project
{

public:
    Project();
    Project( const Project& project );
    Project( const Project* project );
    virtual ~Project();

    void copyFrom( const Project& project );

    void setFilename( const QString& filename );
    QString filename() const;

    QStringListModel* rootPathList() const;

private:

    QString filename_;                 ///< The project filename
    QStringListModel* rootPathList_;   ///< The list of all root files
};

