#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FireDogEditor.h"

#include <QStandardItemModel>
#include <QMenu>

#include "FireDogFeatureInfo.h"
#include "FireDogFeatureRuleInfo.h"

#include "loadingdialog.h"
#include "parsethread.h"
#include "bigdatatablemodel.h"

#include "Qss/Qss.h"

#define FIREDOG_EDITOR_VERSION "v1.0"

class FireDogEditor : public QssMainWindow
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
	//特征库详情右键菜单
	void slots_featureInfoTableOpenMenu(QPoint pos);
	//特征库详情规则树右键菜单
	void slots_featureInfoRuleTreeOpenMenu(QPoint pos);

    //开始解析文件
    void slots_parseBinBegin();
    //解析文件结束
    void slots_parseBinEnd(firedog::FeatureLibrary* featureLibrary, int state);
    //特征库表格搜索
    void slots_featureLibraryTableSearch();
    //特征表格点击
    void slots_selectFeatureTableEvent(const QModelIndex& current, const QModelIndex& previous);
    
    //特征表格添加菜单点击
    void slots_featureTableMenuAddEvent();
	//特征表格删除菜单点击
	void slots_featureTableMenuDelEvent();

    //特征详情表格添加菜单点击
    void slots_featureInfoTableMenuAddEvent();
	//特征详情表格编辑菜单点击
	void slots_featureInfoTableMenuEditEvent();
	//特征详情表格删除菜单点击
	void slots_featureInfoTableMenuDelEvent();

	//特征详情规则树添加菜单点击
	void slots_featureLibraryInfoRuleMenuAddEvent();
	//特征详情规则树编辑菜单点击
	void slots_featureLibraryInfoRuleMenuEditEvent();
	//特征详情规则树删除菜单点击
	void slots_featureLibraryInfoRuleMenuDelEvent();

    //添加或者更新按钮
    void slots_saveBtnClickEvent();
private:
    Ui::FireDogEditorClass ui;

    FireDogFeatureInfo* fireDogFeatureInfoView;

    FireDogFeatureRuleInfo* fireDogFeatureRuleInfoView;

    //是否是添加，true是add，false是update
    bool isFeatureAdd = true;

    //特征库
    firedog::FeatureLibrary* featureLibrary = NULL;

    //特征库的表格model
    BigDataTableModel* featureLibraryTableModel = NULL;
    QMenu* featureLibraryTableMenu = NULL;
    QAction* featureLibraryTableMenuAddAction = NULL;
    QAction* featureLibraryTableMenuDelAction = NULL;

    //特征库详情特征表格model
    BigDataTableModel* featureLibraryInfoFeatureTableModel = NULL;
	QMenu* featureLibraryInfoFeatureTableMenu = NULL;
	QAction* featureLibraryInfoFeatureTableMenuAddAction = NULL;
    QAction* featureLibraryInfoFeatureTableMenuEditAction = NULL;
	QAction* featureLibraryInfoFeatureTableMenuDelAction = NULL;

    //规则树
    QStandardItemModel* featureLibraryInfoRuleTreeModel = NULL;
	QMenu* featureLibraryInfoRuleTreeMenu = NULL;
	QAction* featureLibraryInfoRuleTreeMenuAddAction = NULL;
	QAction* featureLibraryInfoRuleTreeMenuEditAction = NULL;
	QAction* featureLibraryInfoRuleTreeMenuDelAction = NULL;

    //loading dialog
    LoadingDialog* loadingDialog = NULL;

    //解析线程
    ParseThread* parseThread = NULL;

	//初始化
	void init();

    //分辨率变更事件
    void resizeEvent(QResizeEvent* event) override;

    //加载特征库列表
    void loadFeatureLibraryTable(QString search);

    //清空详情内容
    void clearInfoContent(bool isAdd);

    //规则转视图item
    QStandardItem* ruleToItem(mountcloud::Rule* rule);

    //从详情视图中获取特征Item
    firedog::FeatureLibraryItem* getInfoViewFeatureItem();

    //视图转规则
	mountcloud::Rule* itemToRule(QStandardItem* item);

};
