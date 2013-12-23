/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QObject>
#include <QString>

class QStringListModel;



/// This class represents a single workspace
class Workspace : public QObject
{
Q_OBJECT

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

    QString name() const;
    void setName( const QString& name );

signals:
    /// this signal is emitted if the name of the workspace is changed
    void nameChanged( const QString& newName );


private:

    QString filename_;                      ///< The project filename
    QString name_;                          ///< The workspace name
};

