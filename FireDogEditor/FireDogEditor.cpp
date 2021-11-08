#include "FireDogEditor.h"
#include <QLabel>

#include <QFileDialog>
#include <QMessageBox>

#include "config.h"
#include "featurelibrary.h"
#include "rule/rule.h"

using namespace firedog;

//构造函数
FireDogEditor::FireDogEditor(QWidget *parent)
    : QssMainWindow(parent)
{
    ui.setupUi(this);


    this->loadingDialog = new LoadingDialog(this);

    //设置标题
    QString windowTitle = "FireDog Editor ";
    windowTitle.append(FIREDOG_EDITOR_VERSION);
    setWindowTitle(windowTitle);

    //添加版本
    QString fireDogVersion = FIREDOG_VERSION;
    QLabel* fireDogVersionLable = new QLabel("FireDog Version:" + fireDogVersion, this);

    QString fireDogFeatureLibraryVersion = FIREDOG_FEATURE_LIBRARY_VERSION;
    QLabel* fireDogFeatureLibraryVersionLable = new QLabel("Feature Library Version:" + fireDogFeatureLibraryVersion, this);

    statusBar()->addPermanentWidget(fireDogVersionLable);
    statusBar()->addPermanentWidget(fireDogFeatureLibraryVersionLable);

    //初始化
    init();
}

//初始化
void FireDogEditor::init() {

    QFile qss(":/qss/css/qss.css");
    if (qss.open(QIODevice::ReadOnly)) {
        QByteArray qssBytes = qss.readAll();
        //this->setStyleSheet(qssBytes);
        qApp->setStyleSheet(qssBytes);
        qApp->setFont(QFont("微软雅黑", 9));
    }

    this->parseThread = new ParseThread();

    //子窗口
    fireDogFeatureInfo = new FireDogFeatureInfo(this);
    fireDogFeatureInfo->setWindowTitle("Feature Editor");

    //特征库表格-start==================================================================
    this->featureLibraryTableModel = new BigDataTableModel();

    QStringList featureLibraryTableModelTitle;
    featureLibraryTableModelTitle << "Name" << "Describe" << "Author";
    featureLibraryTableModel->setHeaders(featureLibraryTableModelTitle);

    ui.tableViewLibrary->setModel(this->featureLibraryTableModel);

    //不可编辑
    ui.tableViewLibrary->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableViewLibrary->setSelectionBehavior(QAbstractItemView::SelectRows);
    //一次滚动一个像素
    ui.tableViewLibrary->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui.tableViewLibrary->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //可弹出右键菜单
    ui.tableViewLibrary->setContextMenuPolicy(Qt::CustomContextMenu); 
    //设置列宽
    ui.tableViewLibrary->setColumnWidth(0, 150);
    ui.tableViewLibrary->setColumnWidth(1, 290);
    ui.tableViewLibrary->setColumnWidth(2, 150);

    //菜单
    featureLibraryTableMenu = new QMenu(ui.tableViewLibrary);

    featureLibraryTableMenuAddAction = new QAction();
    featureLibraryTableMenuAddAction->setText(QString("Add"));
    featureLibraryTableMenu->addAction(featureLibraryTableMenuAddAction);

    featureLibraryTableMenuEditAction = new QAction();
    featureLibraryTableMenuEditAction->setText(QString("Edit"));
    featureLibraryTableMenu->addAction(featureLibraryTableMenuEditAction);

    featureLibraryTableMenuDelAction = new QAction();
    featureLibraryTableMenuDelAction->setText(QString("Delete"));
    featureLibraryTableMenu->addAction(featureLibraryTableMenuDelAction);

    //特征库表格-end==================================================================

    //详情-特征表格-start==================================================================
    this->featureLibraryInfoFeatureTableModel = new BigDataTableModel();

    QStringList featureLibraryInfoFeatureTableModelTitle;
    featureLibraryInfoFeatureTableModelTitle << "Key" << "Type" << "Content";
    featureLibraryInfoFeatureTableModel->setHeaders(featureLibraryInfoFeatureTableModelTitle);

    ui.tableViewLibraryInfoFeatures->setModel(this->featureLibraryInfoFeatureTableModel);

    //不可编辑
    ui.tableViewLibraryInfoFeatures->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableViewLibraryInfoFeatures->setSelectionBehavior(QAbstractItemView::SelectRows);
    //一次滚动一个像素
    ui.tableViewLibraryInfoFeatures->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui.tableViewLibraryInfoFeatures->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //可弹出右键菜单
    ui.tableViewLibraryInfoFeatures->setContextMenuPolicy(Qt::CustomContextMenu);
    //设置列宽
    ui.tableViewLibraryInfoFeatures->setColumnWidth(0, 100);
    ui.tableViewLibraryInfoFeatures->setColumnWidth(1, 100);
    ui.tableViewLibraryInfoFeatures->setColumnWidth(2, 385);
    //详情-特征表格-end==================================================================

    //详情-规则-start==================================================================
    this->featureLibraryInfoRuleTreeModel = new QStandardItemModel();
    ui.treeViewLibraryInfoRules->setModel(featureLibraryInfoRuleTreeModel);
    //详情-规则-end==================================================================


    //绑定事件
    connect(ui.actionOpen, &QAction::triggered, this, &FireDogEditor::slots_openFile);
    connect(ui.pushButtonSearchName, &QPushButton::clicked, this, &FireDogEditor::slots_featureLibraryTableSearch);
    //特征表格右键事件
    connect(ui.tableViewLibrary, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_featureTableOpenMenu(QPoint)));
    //特征表格点击事件
    connect(ui.tableViewLibrary->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slots_selectFeatureTableEvent(const QModelIndex&, const QModelIndex&)));
    //解析线程
    connect(this->parseThread, SIGNAL(parseBegin()), this, SLOT(slots_parseBinBegin()));
    connect(this->parseThread, SIGNAL(parseEnd(firedog::FeatureLibrary*,int)), this, SLOT(slots_parseBinEnd(firedog::FeatureLibrary*, int)));

}

//打开文件
void FireDogEditor::slots_openFile() {
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Feature Library File."),
        "F:",
        tr("Feature Library(*.fdog)"));
    if (!filePath.isEmpty()) {
        qDebug(filePath.toStdString().c_str());
        this->parseThread->setFilePath(filePath);
        this->parseThread->start();
    }
}

//搜索
void FireDogEditor::slots_featureLibraryTableSearch() {
    QString search = ui.lineEditSearchName->text();
    loadFeatureLibraryTable(search);

    fireDogFeatureInfo->exec();

    QssMessageBox::tips("test tip", this, "Tips");
    QssMessageBox::warn("test warn", this, "Warn");
    QssMessageBox::error("test error", this, "Error");
    QssMessageBox::ask("test question", this, "Question");

}


void FireDogEditor::slots_parseBinBegin() {
    this->loadingDialog->show();
}

void FireDogEditor::slots_parseBinEnd(firedog::FeatureLibrary* featureLibrary, int state) {
    this->loadingDialog->loadingEnd();
    if (state != 0) {
        if (state == PARSE_ERROR_CODE_FILE_READ_FAIL) {
            QMessageBox::warning(this, "Error", "Feature library file read failed.",
                QMessageBox::Yes);
            return;
        }
        if (state == PARSE_ERROR_CODE_FILE_PARSE_FAIL) {
            QMessageBox::warning(this, "Error", "Feature library file format verification failed.",
                QMessageBox::Yes);
            return;
        }
        if (state == PARSE_ERROR_CODE_FILE_VERSION_CHECK_FAIL) {
            QMessageBox::warning(this, "Error", "Feature library file version verification failed.",
                QMessageBox::Yes);
            return;
        }
        QMessageBox::warning(this, "Error", "Feature library parse failed.",
            QMessageBox::Yes);
        return;
    }
    
    //清空之前的数据
    if (this->featureLibrary!=NULL) {
        delete this->featureLibrary;
        this->featureLibrary = NULL;
        if (this->featureLibraryTableModel->rowCount()>0) {
            this->featureLibraryTableModel->removeRows(0, this->featureLibraryTableModel->rowCount());
        }
    }

    //如果成功的话
    this->featureLibrary = featureLibrary;

    //清空搜索框
    ui.lineEditSearchName->setText("");

    //加载数据
    loadFeatureLibraryTable("");
}

void FireDogEditor::loadFeatureLibraryTable(QString search) {

    if (this->featureLibrary == NULL|| this->featureLibrary->items->size()==0) {
        return;
    }

    QVector<BigDataTableRow> tableRows;
    for (int i = 0; i < featureLibrary->items->size(); i++) {
        FeatureLibraryItem* item = featureLibrary->items->at(i);

        //搜索
        QString name = item->name.c_str();
        QString describe = item->describe.c_str();
        QString author = item->author.c_str();
        if (!search.isEmpty()) {
            if (!name.contains(search) && !describe.contains(search) && !author.contains(search)) {
                continue;
            }
        }

        BigDataTableRow row;
        row.contents << BigDataTableCol(name) << BigDataTableCol(describe) << BigDataTableCol(author);
        row.id = QString::number(i);

        tableRows.push_back(row);
    }

    this->featureLibraryTableModel->handleData(tableRows);
}

void FireDogEditor::slots_featureTableOpenMenu(QPoint pos) {
    auto index = ui.tableViewLibrary->indexAt(pos);
    featureLibraryTableMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
}

void FireDogEditor::slots_selectFeatureTableEvent(const QModelIndex& current, const QModelIndex& previous) {

    ui.pushButtonLibraryInfoSave->setText("Update");


    //clear
    if (this->featureLibraryInfoFeatureTableModel->rowCount() > 0) {
        this->featureLibraryInfoFeatureTableModel->removeRows(0, this->featureLibraryInfoFeatureTableModel->rowCount());
    }

    this->featureLibraryInfoRuleTreeModel->clear();


    int row = current.row();
    BigDataTableRow bigDataTableRow = this->featureLibraryTableModel->getRowData(row);
    int index = bigDataTableRow.id.toInt();

    FeatureLibraryItem* item = this->featureLibrary->items->at(index);
    QString name = item->name.c_str();
    QString author = item->author.c_str();
    QString describe = item->describe.c_str();

    ui.lineEditLibraryInfoName->setText(name);
    ui.lineEditLibraryInfoAuthor->setText(author);
    ui.textEditLibraryInfoDescribe->setText(describe);


    QVector<BigDataTableRow> tableRows;
    if (item->features != NULL) {
        for (int i = 0; i < item->features->size(); i++) {
            Feature* feature = item->features->at(i);
            QString key = feature->key.c_str();
            QString type = !feature->hex.empty() ? "hex" : "text";
            QString content = !feature->hex.empty() ? feature->hex.c_str() : feature->text.c_str();

            BigDataTableRow row;
            row.contents << BigDataTableCol(key) << BigDataTableCol(type) << BigDataTableCol(content);
            row.id = QString::number(i);

            tableRows.push_back(row);
        }
    }
    this->featureLibraryInfoFeatureTableModel->handleData(tableRows);
    
    if (item->rule != NULL) {
        QStandardItem* ruleItem = ruleToItem(item->rule);
        this->featureLibraryInfoRuleTreeModel->appendRow(ruleItem);
        ui.treeViewLibraryInfoRules->expandAll();
    }
}

QStandardItem* FireDogEditor::ruleToItem(mountcloud::Rule* rule) {
    QStandardItem* ruleitem = new QStandardItem();
    if (!rule->id.empty()) {
        ruleitem->setText(QString(rule->id.c_str()));
    }
    else {
        if (rule->ands != NULL) {
            ruleitem->setText(QStringLiteral("$and"));

            for (int i = 0; i < rule->ands->size(); i++) {
                mountcloud::Rule* andRule = rule->ands->at(i);
                QStandardItem* andRuleItem = ruleToItem(andRule);
                ruleitem->appendRow(andRuleItem);
            }
        }
        else if (rule->ors != NULL) {
            ruleitem->setText(QStringLiteral("$or"));

            for (int i = 0; i < rule->ors->size(); i++) {
                mountcloud::Rule* andRule = rule->ors->at(i);
                QStandardItem* andRuleItem = ruleToItem(andRule);
                ruleitem->appendRow(andRuleItem);
            }
        }
        else {
            ruleitem->setText(QStringLiteral("unknown"));
        }
    }
    return ruleitem;
}

void FireDogEditor::clearInfoContent(bool isAdd) {
    this->isAdd = isAdd;
    if (isAdd) {
        ui.pushButtonLibraryInfoSave->setText("Add");
    }
    else {
        ui.pushButtonLibraryInfoSave->setText("Update");
    }
    ui.lineEditLibraryInfoName->setText("");
    ui.lineEditLibraryInfoAuthor->setText("");
    ui.textEditLibraryInfoDescribe->setText("");

    QVector<BigDataTableRow> tableRows;
    this->featureLibraryInfoFeatureTableModel->handleData(tableRows);
}

FireDogEditor::~FireDogEditor() {
    if (featureLibraryTableModel!=NULL) {
        delete featureLibraryTableModel;
        featureLibraryTableModel = NULL;
    }

    if (featureLibraryTableMenu != NULL) {
        delete featureLibraryTableMenu;
        featureLibraryTableMenu = NULL;
    }

    if (featureLibraryTableMenuAddAction != NULL) {
        delete featureLibraryTableMenuAddAction;
        featureLibraryTableMenuAddAction = NULL;
    }

    if (featureLibraryTableMenuEditAction != NULL) {
        delete featureLibraryTableMenuEditAction;
        featureLibraryTableMenuEditAction = NULL;
    }

    if (featureLibraryTableMenuDelAction != NULL) {
        delete featureLibraryTableMenuDelAction;
        featureLibraryTableMenuDelAction = NULL;
    }

    if (featureLibraryInfoFeatureTableModel != NULL) {
        delete featureLibraryInfoFeatureTableModel;
        featureLibraryInfoFeatureTableModel = NULL;
    }

    if (loadingDialog != NULL) {
        delete loadingDialog;
        loadingDialog = NULL;
    }

    if (featureLibrary != NULL) {
        delete featureLibrary;
        featureLibrary = NULL;
    }

    if (featureLibraryInfoRuleTreeModel != NULL) {
        delete featureLibraryInfoRuleTreeModel;
        featureLibraryInfoRuleTreeModel = NULL;
    } 
}

void FireDogEditor::resizeEvent(QResizeEvent* event) {
    int width = this->width();
    int height = this->height();
    this->loadingDialog->setFixedSize(QSize(width, height));
}

