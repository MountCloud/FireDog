#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FireDogEditor.h"

#include <QStandardItemModel>
#include <QMenu>

#include "loadingdialog.h"
#include "parsethread.h"
#include "bigdatatablemodel.h"

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

    //开始解析文件
    void slots_parseBinBegin();
    //解析文件结束
    void slots_parseBinEnd(firedog::FeatureLibrary* featureLibrary, int state);
    //特征库表格搜索
    void slots_featureLibraryTableSearch();
private:
    Ui::FireDogEditorClass ui;

    //特征库
    firedog::FeatureLibrary* featureLibrary = NULL;

    //特征库的表格model
    BigDataTableModel* featureLibraryTableModel = NULL;
    QMenu* featureLibraryTableMenu = NULL;
    QAction* featureLibraryTableMenuAddAction = NULL;
    QAction* featureLibraryTableMenuEditAction = NULL;
    QAction* featureLibraryTableMenuDelAction = NULL;


    //特征库详情特征表格model
    BigDataTableModel* featureLibraryInfoFeatureTableModel = NULL;

    //loading dialog
    LoadingDialog* loadingDialog = NULL;

    //解析线程
    ParseThread* parseThread = NULL;

    //初始化
    void init();

    void resizeEvent(QResizeEvent* event) override;

    //加载特征库列表
    void loadFeatureLibraryTable(QString search);
};
