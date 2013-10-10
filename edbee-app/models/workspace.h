/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>

class QStringListModel;



/// This class represents a single workspace
class Workspace
{

public:
    static QString fileExtension();
    static QString fileDialogFilter();

    Workspace();
    Workspace( const Workspace& workspace );
    Workspace( const Workspace* workspace );
    virtual ~Workspace();

    void copyFrom( const Workspace& workspace );

    void setFilename( const QString& filename );
    QString filename() const;

private:

    QString filename_;                      ///< The project filename
};

