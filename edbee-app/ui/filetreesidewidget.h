/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QWidget>

class QComboBox;
class QFileSystemModel;
class QPushButton;
class QStringListModel;
class QTreeView;
class Workspace;

/// the side tree widget
class FileTreeSideWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileTreeSideWidget(QWidget *parent = 0);
    virtual ~FileTreeSideWidget();

    void setWorkspace( Workspace* workspace );
    Workspace* workspace() const;

    QVariantMap serialize();
    void deserialize( const QVariantMap& map );

signals:
    void fileDoubleClicked( const QString& fileName );
    
public slots:

    void openFileItem( const QModelIndex& index );
    void fileTreeContextMenu( const QPoint& point );
    void setRootPath( const QString& rootPath="/" );
    void setRootPathByAction();

    void clearCurrentRootPath();
    void clearAllRootPaths();

    void reveal( const QString& filename );

    void startRenameItem( const QString& filename );
    void startRenameItemByAction();

    void createNewFileAndRename( const QString& pathname );
    void createNewFileAndRenameByAction();

    void createNewFolderAndRename( const QString& pathname );
    void createNewFolderAndRenameByAction();

    void deleteItem( const QString& pathname );
    void deleteItemByAction();

private:

    void constructUI();
    void connectSignals();


private:
    QFileSystemModel* fileTreeModel_;   ///< The tree model
    QTreeView* fileTreeRef_;            ///< A reference to the tree view
    QComboBox* pathComboRef_;           ///< The current path
    QPushButton* trashButtonRef_;       ///< A close button

    QStringListModel* rootPathList_;    ///< The list with all root pathnames

    Workspace* workspaceRef_;           ///< A reference to the project
};
