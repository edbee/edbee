/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QAction>
#include <QComboBox>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QLabel>
#include <QMenu>
#include <QTreeView>
#include <QVBoxLayout>


#include "filetreesidewidget.h"

#include "debug.h"


FileTreeSideWidget::FileTreeSideWidget(QWidget *parent)
    : QWidget(parent)
    , fileTreeModel_(0)
    , fileTreeRef_(0)
{
    constructUI();
    connectSignals();
}

FileTreeSideWidget::~FileTreeSideWidget()
{
    delete fileTreeModel_;
}


void FileTreeSideWidget::fileTreeDoubleClicked( const QModelIndex& index)
{
    // when clicking a file
    QFileInfo fileInfo = fileTreeModel_->fileInfo(index);
    if( !fileInfo.isFile() ) { return; }

    // open the file and add a tab
    emit fileDoubleClicked( fileInfo.absoluteFilePath() );
    //openFile( fileInfo.absoluteFilePath() );

}


void FileTreeSideWidget::fileTreeContextMenu(const QPoint& point)
{
    // for most widgets
    QPoint globalPos = fileTreeRef_->mapToGlobal(point);

    // get the file info
    QModelIndex index = fileTreeRef_->indexAt(point);

     // for QAbstractScrollArea and derived classes you would use:
     // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    // build the context menu
    QMenu menu;

    // valid item clicked
    if( index.isValid() ) {
        QFileInfo fileInfo = fileTreeModel_->fileInfo(index);

        if( fileInfo.isDir() ) {
            QAction* changeRootAction = new QAction(tr("Goto '%1'").arg(fileInfo.baseName()), &menu );
            changeRootAction->setData( fileInfo.absoluteFilePath() );
            connect(changeRootAction, SIGNAL(triggered()), this, SLOT(setRootPathByAction()) );
            menu.addAction(changeRootAction );
        }
        if( !menu.isEmpty() ) {
            menu.addSeparator();
        }
    }

    menu.addAction( tr("Goto '/'"), this, SLOT(setRootPath()) );

    menu.exec(globalPos);
}

/// When the rootPath is not given the root is used
void FileTreeSideWidget::setRootPath( const QString& rootPath )
{
    QModelIndex idx = fileTreeModel_->index( rootPath );
    fileTreeRef_->setRootIndex(idx);
    int index = pathComboRef_->findText(rootPath);
    if( index < 0 ) {
        pathComboRef_->addItem(rootPath);
        pathComboRef_->setCurrentIndex( pathComboRef_->count()-1 );
    } else {
        pathComboRef_->setCurrentIndex(index);
    }
}

/// Sets the root path by extacting the path from the QAction data
void FileTreeSideWidget::setRootPathByAction()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if( action ) {
        setRootPath( action->data().toString() );
    }
}


void FileTreeSideWidget::constructUI()
{
    QFont newFont = QFont(font().family(), 10 );
    setFont(newFont);


    // create the tree model
    fileTreeModel_ = new QFileSystemModel();
//    fileTreeModel_->setReadOnly(true);
    fileTreeModel_->setRootPath("/");
    fileTreeModel_->setFilter(QDir::Hidden|QDir::AllEntries|QDir::NoDotAndDotDot);
//    fileTreeModel_->setSorting( QDir::DirsFirst | QDir::IgnoreCase | QDir::Name );

    fileTreeRef_ = new QTreeView();
    fileTreeRef_->setContextMenuPolicy(Qt::CustomContextMenu);
    fileTreeRef_->setModel( fileTreeModel_ );


//    QHeaderView* hdr = fileTreeRef_->header();
//    hdr->setStretchLastSection(true);
//    hdr->setSortIndicator(0,Qt::AscendingOrder);
//    hdr->setSortIndicatorShown(true);
//    hdr->setSectionsClickable(true);

    QModelIndex index = fileTreeModel_->index(QDir::currentPath() );
    fileTreeRef_->expand(index);
    fileTreeRef_->scrollTo(index);
    fileTreeRef_->resizeColumnToContents(0);
    fileTreeRef_->setIndentation(15);


    pathComboRef_ = new QComboBox();
    pathComboRef_->addItem("/");
    pathComboRef_->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );


    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( pathComboRef_, 0 );
    layout->addWidget( fileTreeRef_, 1 );
    layout->setMargin(0);
    setLayout( layout);
}

void FileTreeSideWidget::connectSignals()
{
    connect( fileTreeRef_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(fileTreeDoubleClicked(QModelIndex)) );
    connect( fileTreeRef_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(fileTreeContextMenu(QPoint)) );
    connect( pathComboRef_, SIGNAL(currentIndexChanged(QString)), this, SLOT(setRootPath(QString)));
}

