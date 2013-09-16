/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QObject>

#include "util/test.h"

class EdbeeConfigTest : public edbee::test::TestCase
{
Q_OBJECT
private slots:

    void testAddFile();
};


DECLARE_TEST(EdbeeConfigTest);
