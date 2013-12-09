/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QTabWidget>


/// This class implements a tabwidget.
/// I've decided to use a seperate class because we're going to need some special QTabWidget features in the near future.
class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);


protected:
    bool eventFilter(QObject* obj, QEvent* event);
};
