/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QString>
#include <QVariantMap>

class Application;


/// The SessionSerializer is used to save/load the application state
/// The state is stored as a plain json file
///
/// This is to 'remember' the last opened state of the application, and can be used
/// in the future to store the application state in a seperate session file
///
/// I decided to not make a 'serializable' interface to offload the serialization complexibility
/// into this class. When serialization of certain objects gets complex seperate classes should be made
class SessionSerializer
{
public:
    SessionSerializer();
    virtual ~SessionSerializer();

    bool saveState( const QString& fileName );
    bool loadState( const QString& fileName );

protected:
    QVariantMap serialize();
    void unserialize( const QVariantMap& map );

    void serializeApplication( Application* app );


};

