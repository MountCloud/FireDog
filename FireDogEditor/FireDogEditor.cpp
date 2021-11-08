#include "FireDogEditor.h"
#include <QLabel>

#include <QFileDialog>

#include "config.h"
#include "featurelibrary.h"
#include "rule/rule.h"

using namespace firedog;

//���캯��
FireDogEditor::FireDogEditor(QWidget *parent)
    : QssMainWindow(parent)
{
    ui.setupUi(this);


    this->loadingDialog = new LoadingDialog(this);

    //���ñ���
    QString windowTitle = "FireDog Editor ";
    windowTitle.append(FIREDOG_EDITOR_VERSION);
    setWindowTitle(windowTitle);

    //���Ӱ汾
    QString fireDogVersion = FIREDOG_VERSION;
    QLabel* fireDogVersionLable = new QLabel("FireDog Version:" + fireDogVersion, this);

    QString fireDogFeatureLibraryVersion = FIREDOG_FEATURE_LIBRARY_VERSION;
    QLabel* fireDogFeatureLibraryVersionLable = new QLabel("Feature Library Version:" + fireDogFeatureLibraryVersion, this);

    statusBar()->addPermanentWidget(fireDogVersionLable);
    statusBar()->addPermanentWidget(fireDogFeatureLibraryVersionLable);

    //��ʼ��
    init();
}

//��ʼ��
void FireDogEditor::init() {

    QFile qss(":/qss/css/qss.css");
    if (qss.open(QIODevice::ReadOnly)) {
        QByteArray qssBytes = qss.readAll();
        //this->setStyleSheet(qssBytes);
        qApp->setStyleSheet(qssBytes);
        qApp->setFont(QFont("΢���ź�", 9));
    }

    this->parseThread = new ParseThread();

    //�Ӵ���
    fireDogFeatureInfo = new FireDogFeatureInfo(this);
    fireDogFeatureInfo->setWindowTitle("Feature Editor");

    //���������-start==================================================================
    this->featureLibraryTableModel = new BigDataTableModel();

    QStringList featureLibraryTableModelTitle;
    featureLibraryTableModelTitle << "Name" << "Describe" << "Author";
    featureLibraryTableModel->setHeaders(featureLibraryTableModelTitle);

    ui.tableViewLibrary->setModel(this->featureLibraryTableModel);

	//���ɱ༭
	ui.tableViewLibrary->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableViewLibrary->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.tableViewLibrary->setSelectionBehavior(QAbstractItemView::SelectRows);
    //һ�ι���һ������
    ui.tableViewLibrary->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui.tableViewLibrary->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //�ɵ����Ҽ��˵�
    ui.tableViewLibrary->setContextMenuPolicy(Qt::CustomContextMenu); 
    //�����п�
    ui.tableViewLibrary->setColumnWidth(0, 150);
    ui.tableViewLibrary->setColumnWidth(1, 290);
    ui.tableViewLibrary->setColumnWidth(2, 150);

    //�˵�
    featureLibraryTableMenu = new QMenu(ui.tableViewLibrary);

    featureLibraryTableMenuAddAction = new QAction();
    featureLibraryTableMenuAddAction->setText(QString("Add"));
    featureLibraryTableMenu->addAction(featureLibraryTableMenuAddAction);

    featureLibraryTableMenuDelAction = new QAction();
    featureLibraryTableMenuDelAction->setText(QString("Delete"));
    featureLibraryTableMenu->addAction(featureLibraryTableMenuDelAction);

    //���������-end==================================================================

    //����-��������-start==================================================================
    this->featureLibraryInfoFeatureTableModel = new BigDataTableModel();

    QStringList featureLibraryInfoFeatureTableModelTitle;
    featureLibraryInfoFeatureTableModelTitle << "Key" << "Type" << "Content";
    featureLibraryInfoFeatureTableModel->setHeaders(featureLibraryInfoFeatureTableModelTitle);

    ui.tableViewLibraryInfoFeatures->setModel(this->featureLibraryInfoFeatureTableModel);

    //�˵�
    featureLibraryInfoFeatureTableMenu = new QMenu(ui.tableViewLibraryInfoFeatures);

    featureLibraryInfoFeatureTableMenuAddAction = new QAction();
    featureLibraryInfoFeatureTableMenuAddAction->setText(QString("Add"));
    featureLibraryInfoFeatureTableMenu->addAction(featureLibraryInfoFeatureTableMenuAddAction);

    featureLibraryInfoFeatureTableMenuEditAction = new QAction();
    featureLibraryInfoFeatureTableMenuEditAction->setText(QString("Edit"));
	featureLibraryInfoFeatureTableMenu->addAction(featureLibraryInfoFeatureTableMenuEditAction);

    featureLibraryInfoFeatureTableMenuDelAction = new QAction();
    featureLibraryInfoFeatureTableMenuDelAction->setText(QString("Delete"));
	featureLibraryInfoFeatureTableMenu->addAction(featureLibraryInfoFeatureTableMenuDelAction);

    //���ɱ༭
	ui.tableViewLibraryInfoFeatures->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableViewLibraryInfoFeatures->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.tableViewLibraryInfoFeatures->setSelectionBehavior(QAbstractItemView::SelectRows);
    //һ�ι���һ������
    ui.tableViewLibraryInfoFeatures->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui.tableViewLibraryInfoFeatures->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //�ɵ����Ҽ��˵�
    ui.tableViewLibraryInfoFeatures->setContextMenuPolicy(Qt::CustomContextMenu);
    //�����п�
    ui.tableViewLibraryInfoFeatures->setColumnWidth(0, 100);
    ui.tableViewLibraryInfoFeatures->setColumnWidth(1, 100);
    ui.tableViewLibraryInfoFeatures->setColumnWidth(2, 385);
    //����-��������-end==================================================================

    //����-����-start==================================================================
    this->featureLibraryInfoRuleTreeModel = new QStandardItemModel();
    ui.treeViewLibraryInfoRules->setModel(featureLibraryInfoRuleTreeModel);
    //����-����-end==================================================================


    //���¼�
    connect(ui.actionOpen, &QAction::triggered, this, &FireDogEditor::slots_openFile);
    connect(ui.pushButtonSearchName, &QPushButton::clicked, this, &FireDogEditor::slots_featureLibraryTableSearch);
    //���������Ҽ��¼�
    connect(ui.tableViewLibrary, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_featureTableOpenMenu(QPoint)));
	//������������Ҽ��¼�
	connect(ui.tableViewLibraryInfoFeatures, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_featureInfoTableOpenMenu(QPoint)));
    //�����������¼�
    connect(ui.tableViewLibrary->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slots_selectFeatureTableEvent(const QModelIndex&, const QModelIndex&)));
    //�����߳�
    connect(this->parseThread, SIGNAL(parseBegin()), this, SLOT(slots_parseBinBegin()));
    connect(this->parseThread, SIGNAL(parseEnd(firedog::FeatureLibrary*,int)), this, SLOT(slots_parseBinEnd(firedog::FeatureLibrary*, int)));

    connect(this->featureLibraryTableMenuAddAction, &QAction::triggered, this, &FireDogEditor::slots_featureTableMenuAddEvent);
	connect(this->featureLibraryTableMenuDelAction, &QAction::triggered, this, &FireDogEditor::slots_featureTableMenuDelEvent);

	connect(this->featureLibraryInfoFeatureTableMenuAddAction, &QAction::triggered, this, &FireDogEditor::slots_featureInfoTableMenuAddEvent);
	connect(this->featureLibraryInfoFeatureTableMenuEditAction, &QAction::triggered, this, &FireDogEditor::slots_featureInfoTableMenuEditEvent);
	connect(this->featureLibraryInfoFeatureTableMenuDelAction, &QAction::triggered, this, &FireDogEditor::slots_featureInfoTableMenuDelEvent);

}

//���ļ�
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

//����
void FireDogEditor::slots_featureLibraryTableSearch() {
    QString search = ui.lineEditSearchName->text();
    loadFeatureLibraryTable(search);

    //fireDogFeatureInfo->exec();

    //QssMessageBox::tips("test tip", this, "Tips");
    //QssMessageBox::warn("test warn", this, "Warn");
    //QssMessageBox::error("test error", this, "Error");
    //QssMessageBox::ask("test question", this, "Question");

}


void FireDogEditor::slots_parseBinBegin() {
    this->loadingDialog->show();
}

void FireDogEditor::slots_parseBinEnd(firedog::FeatureLibrary* featureLibrary, int state) {
    this->loadingDialog->loadingEnd();
    if (state != 0) {
        if (state == PARSE_ERROR_CODE_FILE_READ_FAIL) {
            QssMessageBox::warn("Feature library file read failed.",this,"Warn");
            return;
        }
		if (state == PARSE_ERROR_CODE_FILE_PARSE_FAIL) {
			QssMessageBox::warn("Feature library file format verification failed.", this, "Warn");
            return;
        }
		if (state == PARSE_ERROR_CODE_FILE_VERSION_CHECK_FAIL) {
			QssMessageBox::warn("Feature library file version verification failed.", this, "Warn");
            return;
		}
		QssMessageBox::warn("Feature library parse failed.", this, "Warn");
        return;
    }
    
    //���֮ǰ������
    if (this->featureLibrary!=NULL) {
        delete this->featureLibrary;
        this->featureLibrary = NULL;
        if (this->featureLibraryTableModel->rowCount()>0) {
            this->featureLibraryTableModel->removeRows(0, this->featureLibraryTableModel->rowCount());
        }
    }

    //����ɹ��Ļ�
    this->featureLibrary = featureLibrary;

    //���������
    ui.lineEditSearchName->setText("");

    //��������
    loadFeatureLibraryTable("");
}

void FireDogEditor::loadFeatureLibraryTable(QString search) {

    if (this->featureLibrary == NULL|| this->featureLibrary->items->size()==0) {
        return;
    }

    QVector<BigDataTableRow> tableRows;
    for (int i = 0; i < featureLibrary->items->size(); i++) {
        FeatureLibraryItem* item = featureLibrary->items->at(i);

        //����
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

        tableRows.push_back(row);
    }

    this->featureLibraryTableModel->handleData(tableRows);
}

void FireDogEditor::slots_featureTableOpenMenu(QPoint pos) {

    
    int curRow = ui.tableViewLibrary->selectionModel()->currentIndex().row(); //ѡ����

    if (curRow==-1) {
        featureLibraryTableMenuDelAction->setEnabled(false);
    }
	else {
		featureLibraryTableMenuDelAction->setEnabled(true);
    }

    auto index = ui.tableViewLibrary->indexAt(pos);
    featureLibraryTableMenu->exec(QCursor::pos()); // �˵����ֵ�λ��Ϊ��ǰ����λ��
}

void FireDogEditor::slots_featureInfoTableOpenMenu(QPoint pos) {

    int curRow = ui.tableViewLibraryInfoFeatures->selectionModel()->currentIndex().row(); //ѡ����

	if (curRow == -1) {
        featureLibraryInfoFeatureTableMenuEditAction->setEnabled(false);
        featureLibraryInfoFeatureTableMenuDelAction->setEnabled(false);
	}
	else {
		featureLibraryInfoFeatureTableMenuEditAction->setEnabled(true);
		featureLibraryInfoFeatureTableMenuDelAction->setEnabled(true);
	}

	auto index = ui.tableViewLibraryInfoFeatures->indexAt(pos);
    featureLibraryInfoFeatureTableMenu->exec(QCursor::pos()); // �˵����ֵ�λ��Ϊ��ǰ����λ��
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

    FeatureLibraryItem* item = this->featureLibrary->items->at(row);
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

void FireDogEditor::slots_featureTableMenuAddEvent() {
    clearInfoContent(true);
}

void FireDogEditor::slots_featureTableMenuDelEvent() {
    int curRow = ui.tableViewLibrary->selectionModel()->currentIndex().row(); //ѡ����

	FeatureLibraryItem* item = this->featureLibrary->items->at(curRow);
	delete item;
	item = NULL;

	this->featureLibrary->items->erase(this->featureLibrary->items->begin() + curRow);

    this->featureLibraryTableModel->removeRow(curRow);

}

void FireDogEditor::slots_featureInfoTableMenuAddEvent() {
    firedog::Feature feature;
    bool state = this->fireDogFeatureInfo->updateFeature(&feature);
    if (state) {
		QString key = feature.key.c_str();
		QString type = !feature.hex.empty() ? "hex" : "text";
		QString content = !feature.hex.empty() ? feature.hex.c_str() : feature.text.c_str();

		BigDataTableRow row;
		row.contents << BigDataTableCol(key) << BigDataTableCol(type) << BigDataTableCol(content);

        this->featureLibraryInfoFeatureTableModel->addRow(row);
    }
}

void FireDogEditor::slots_featureInfoTableMenuEditEvent() {

}

void FireDogEditor::slots_featureInfoTableMenuDelEvent() {

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

    this->featureLibraryInfoRuleTreeModel->clear();
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

	if (featureLibraryInfoFeatureTableMenu != NULL) {
		delete featureLibraryInfoFeatureTableMenu;
        featureLibraryInfoFeatureTableMenu = NULL;
	}

	if (featureLibraryInfoFeatureTableMenuAddAction != NULL) {
		delete featureLibraryInfoFeatureTableMenuAddAction;
        featureLibraryInfoFeatureTableMenuAddAction = NULL;
	}

	if (featureLibraryInfoFeatureTableMenuEditAction != NULL) {
		delete featureLibraryInfoFeatureTableMenuEditAction;
        featureLibraryInfoFeatureTableMenuEditAction = NULL;
	}

	if (featureLibraryInfoFeatureTableMenuDelAction != NULL) {
		delete featureLibraryInfoFeatureTableMenuDelAction;
        featureLibraryInfoFeatureTableMenuDelAction = NULL;
	}
}

void FireDogEditor::resizeEvent(QResizeEvent* event) {
    int width = this->width();
    int height = this->height();
    this->loadingDialog->setFixedSize(QSize(width, height));
}
