/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "sessionserializer.h"

#include "application.h"

#include "debug.h"


/// initializes the session serializer
/// @param a reference to the app
SessionSerializer::SessionSerializer()
{
}

/// destructor
SessionSerializer::~SessionSerializer()
{
}

/// Saves the current state to the given file
/// @param fileName the filename to store the state
/// @return true on success
bool SessionSerializer::saveState(const QString& fileName)
{

    return false;
}

/// Loads the current state from the given file
/// @param fileName the filename to load the state from
/// @return true on success
bool SessionSerializer::loadState(const QString &fileName)
{
    return false;
}

/// This method serializes the application to a QVariantMap
QVariantMap SessionSerializer::serialize()
{
    QVariantMap result;

    return result;
}

/// Serializes the application
void SessionSerializer::serializeApplication(Application* app)
{
    Q_UNUSED(app);
}
