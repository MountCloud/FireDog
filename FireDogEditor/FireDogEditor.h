#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FireDogEditor.h"


#include <QStandardItemModel>
#include <QMenu>


#define FIREDOG_EDITOR_VERSION "v1.0"

class FireDogEditor : public QMainWindow
{
    Q_OBJECT

public:
    FireDogEditor(QWidget *parent = Q_NULLPTR);
    ~FireDogEditor();

private slots:
    //打开文件
    void slots_openFile();
    //特征库右键菜单
    void slots_featureTableOpenMenu(QPoint pos);

private:
    Ui::FireDogEditorClass ui;

    //特征库的表格model
    QStandardItemModel* featureLibraryTableModel = NULL;
    QMenu* featureLibraryTableMenu = NULL;
    QAction* featureLibraryTableMenuAddAction = NULL;
    QAction* featureLibraryTableMenuEditAction = NULL;
    QAction* featureLibraryTableMenuDelAction = NULL;

    //初始化
    void init();
};
