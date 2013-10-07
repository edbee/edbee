/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QWidget>

class QComboBox;
class QFileSystemModel;
class QPushButton;
class QTreeView;

/// the side tree widget
class FileTreeSideWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileTreeSideWidget(QWidget *parent = 0);
    virtual ~FileTreeSideWidget();

signals:
    void fileDoubleClicked( const QString& fileName );
    
public slots:

    void fileTreeDoubleClicked( const QModelIndex& index );
    void fileTreeContextMenu( const QPoint& point );
    void setRootPath( const QString& rootPath="/" );
    void setRootPathByAction();

    void clearCurrentRootPath();
    void clearAllRootPaths();


private:

    void constructUI();
    void connectSignals();


private:
    QFileSystemModel* fileTreeModel_;   ///< The tree model
    QTreeView* fileTreeRef_;            ///< A reference to the tree view
    QComboBox* pathComboRef_;           ///< The current path
    QPushButton* trashButtonRef_;       ///< A close button
};
