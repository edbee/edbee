/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "tabwidget.h"

#include <QTabBar>
#include <QWheelEvent>

#include "debug.h"

/// constructs the tab widget
TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    // add the defaults for the tab widget
    setTabsClosable(true);
    setTabShape(QTabWidget::Triangular);
    setDocumentMode(true);
    setMovable(true);
    setUsesScrollButtons(true);
    setFocusPolicy( Qt::NoFocus );

    // install the event filter
    tabBar()->installEventFilter(this);
}


/// We don't like wheel events with the tab bar
bool TabWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::Wheel && obj == tabBar() )
    {
        return true;
    }
    return false;
}
