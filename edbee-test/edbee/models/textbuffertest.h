/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "util/test.h"

namespace edbee {

class TextBufferTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:

    void testlineFromOffset();
    void testColumnFromOffsetAndLine();
    void testReplaceText();
    void testFindCharPosWithinRange();
    void testLine();

    void testReplaceIssue141();

};

} // edbee

DECLARE_TEST(edbee::TextBufferTest);
