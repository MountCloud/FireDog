#include "FireDogEditor.h"
#include <QLabel>

#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>

#include "config.h"
#include "featurelibrary.h"
#include "rule/rule.h"

#include "partutil.h"
#include "matchthread.h"

using namespace firedog;

//���캯��
FireDogEditor::FireDogEditor(QWidget *parent)
    : QssMainWindow(parent)
{
    ui.setupUi(this);

    updateWindowTitle(false);

    this->loadingDialog = new LoadingDialog(this);

    //��Ӱ汾
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
    fireDogFeatureInfoView = new FireDogFeatureInfo(this);
    fireDogFeatureInfoView->setWindowTitle("Feature Editor");

    fireDogFeatureRuleInfoView = new FireDogFeatureRuleInfo(this);
    fireDogFeatureRuleInfoView->setWindowTitle("Feature Rule Editor");

	//tab��-end==================================================================
    //��������-start==================================================================
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
    featureLibraryTableMenuAddAction->setText(QString("Add New"));
    featureLibraryTableMenu->addAction(featureLibraryTableMenuAddAction);

    featureLibraryTableMenuDelAction = new QAction();
    featureLibraryTableMenuDelAction->setText(QString("Delete"));
    featureLibraryTableMenu->addAction(featureLibraryTableMenuDelAction);

    //��������-end==================================================================

    //����-�������-start==================================================================
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
    //����-�������-end==================================================================

    //����-����-start==================================================================
    this->featureLibraryInfoRuleTreeModel = new QStandardItemModel();
    ui.treeViewLibraryInfoRules->setModel(featureLibraryInfoRuleTreeModel);

	//�ɵ����Ҽ��˵�
	ui.treeViewLibraryInfoRules->setContextMenuPolicy(Qt::CustomContextMenu);
    //�˵�
    featureLibraryInfoRuleTreeMenu = new QMenu(ui.treeViewLibraryInfoRules);

    featureLibraryInfoRuleTreeMenuAddAction = new QAction();
    featureLibraryInfoRuleTreeMenuAddAction->setText(QString("Add Child"));
    featureLibraryInfoRuleTreeMenu->addAction(featureLibraryInfoRuleTreeMenuAddAction);

    featureLibraryInfoRuleTreeMenuEditAction = new QAction();
    featureLibraryInfoRuleTreeMenuEditAction->setText(QString("Edit"));
    featureLibraryInfoRuleTreeMenu->addAction(featureLibraryInfoRuleTreeMenuEditAction);

    featureLibraryInfoRuleTreeMenuDelAction = new QAction();
    featureLibraryInfoRuleTreeMenuDelAction->setText(QString("Delete"));
    featureLibraryInfoRuleTreeMenu->addAction(featureLibraryInfoRuleTreeMenuDelAction);
    //����-����-end==================================================================
	
    //ƥ��-�ļ��б�-start==================================================================
    this->matchingFilesTableModel = new BigDataTableModel();

	QStringList matchingFilesTableModelTitle;
    matchingFilesTableModelTitle << "Path" << "Size";
    matchingFilesTableModel->setHeaders(matchingFilesTableModelTitle);

	ui.tableViewFiles->setModel(this->matchingFilesTableModel);

	//�˵�
    matchingFilesTableMenu = new QMenu(ui.tableViewFiles);

    matchingFilesTableDelAction = new QAction();
    matchingFilesTableDelAction->setText(QString("Del"));
    matchingFilesTableMenu->addAction(matchingFilesTableDelAction);

	//���ɱ༭
	ui.tableViewFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableViewFiles->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableViewFiles->setSelectionBehavior(QAbstractItemView::SelectRows);
	//һ�ι���һ������
	ui.tableViewFiles->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	ui.tableViewFiles->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	//�ɵ����Ҽ��˵�
	ui.tableViewFiles->setContextMenuPolicy(Qt::CustomContextMenu);
	//�����п�
	//�����п�
	ui.tableViewFiles->setColumnWidth(0, 280);
	ui.tableViewFiles->setColumnWidth(1, 100);
	//ƥ��-�ļ��б�-end==================================================================

	//ƥ��-����б�-start==================================================================
	this->matchingResultTableModel = new BigDataTableModel();

	QStringList matchingResultTableTitle;
    matchingResultTableTitle << "Source" << "Feature Name" << "Feature Describe" << "Feature Author";
    matchingResultTableModel->setHeaders(matchingResultTableTitle);

	ui.tableViewSearchResult->setModel(this->matchingResultTableModel);

	//���ɱ༭
	ui.tableViewSearchResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableViewSearchResult->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableViewSearchResult->setSelectionBehavior(QAbstractItemView::SelectRows);
	//һ�ι���һ������
	ui.tableViewSearchResult->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	ui.tableViewSearchResult->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	//�ɵ����Ҽ��˵�
	ui.tableViewSearchResult->setContextMenuPolicy(Qt::CustomContextMenu);
	//�����п�
	//�����п�
	ui.tableViewSearchResult->setColumnWidth(0, 180);
	ui.tableViewSearchResult->setColumnWidth(1, 150);
    ui.tableViewSearchResult->setColumnWidth(2, 350);

	ui.tableViewSearchResult->setColumnWidth(4, 150);
    //ƥ��-����б�-end==================================================================

    //���¼�
    connect(ui.actionOpen, &QAction::triggered, this, &FireDogEditor::slots_openFile);
    connect(ui.actionSave, &QAction::triggered, this, &FireDogEditor::slots_saveFile);
    connect(ui.actionSaveTo, &QAction::triggered, this, &FireDogEditor::slots_saveToFile);
    connect(ui.actionHomePage, &QAction::triggered, this, &FireDogEditor::slots_openGit);
	connect(ui.actionReportIssue, &QAction::triggered, this, &FireDogEditor::slots_openIssue);
	connect(ui.actionAbout, &QAction::triggered, this, &FireDogEditor::slots_about);


    connect(ui.pushButtonSearchName, &QPushButton::clicked, this, &FireDogEditor::slots_featureLibraryTableSearch);
    //��������Ҽ��¼�
    connect(ui.tableViewLibrary, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_featureTableOpenMenu(QPoint)));
	//�����������Ҽ��¼�
	connect(ui.tableViewLibraryInfoFeatures, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_featureInfoTableOpenMenu(QPoint)));
	//��������������Ҽ��¼�
	connect(ui.treeViewLibraryInfoRules, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_featureInfoRuleTreeOpenMenu(QPoint)));
	//ƥ���ļ��б��Ҽ��¼�
	connect(ui.tableViewFiles, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_matchFilesTableOpenMenu(QPoint)));
	//����������¼�
    connect(ui.tableViewLibrary->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slots_selectFeatureTableEvent(const QModelIndex&, const QModelIndex&)));
    //�����߳�
    connect(this->parseThread, SIGNAL(parseBegin()), this, SLOT(slots_parseBinBegin()));
    connect(this->parseThread, SIGNAL(parseEnd(firedog::FeatureLibrary*,int)), this, SLOT(slots_parseBinEnd(firedog::FeatureLibrary*, int)));

    connect(this->featureLibraryTableMenuAddAction, &QAction::triggered, this, &FireDogEditor::slots_featureTableMenuAddEvent);
	connect(this->featureLibraryTableMenuDelAction, &QAction::triggered, this, &FireDogEditor::slots_featureTableMenuDelEvent);

	connect(this->featureLibraryInfoFeatureTableMenuAddAction, &QAction::triggered, this, &FireDogEditor::slots_featureInfoTableMenuAddEvent);
	connect(this->featureLibraryInfoFeatureTableMenuEditAction, &QAction::triggered, this, &FireDogEditor::slots_featureInfoTableMenuEditEvent);
	connect(this->featureLibraryInfoFeatureTableMenuDelAction, &QAction::triggered, this, &FireDogEditor::slots_featureInfoTableMenuDelEvent);

	connect(this->featureLibraryInfoRuleTreeMenuAddAction, &QAction::triggered, this, &FireDogEditor::slots_featureLibraryInfoRuleMenuAddEvent);
	connect(this->featureLibraryInfoRuleTreeMenuEditAction, &QAction::triggered, this, &FireDogEditor::slots_featureLibraryInfoRuleMenuEditEvent);
	connect(this->featureLibraryInfoRuleTreeMenuDelAction, &QAction::triggered, this, &FireDogEditor::slots_featureLibraryInfoRuleMenuDelEvent);

    connect(this->matchingFilesTableDelAction, &QAction::triggered, this, &FireDogEditor::slots_matchingFilesDeleteEvent);

    connect(ui.pushButtonLibraryInfoSave, &QPushButton::clicked, this, &FireDogEditor::slots_saveBtnClickEvent);

	connect(ui.pushButtonAddFiles, &QPushButton::clicked, this, &FireDogEditor::slots_matchingAddFilesBtnClickEvent);

    connect(ui.pushButtonMatch, &QPushButton::clicked, this, &FireDogEditor::slots_matchingBtnClick);
}


void FireDogEditor::slots_openIssue() {
	QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/MountCloud/FireDog/issues")));
}

void FireDogEditor::slots_openGit() {
    QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/MountCloud/FireDog")));
}

void FireDogEditor::slots_about() {

}


void FireDogEditor::updateWindowTitle(bool isUpdated) {
    //���ñ���
    QString windowTitle = "FireDog Editor ";
	windowTitle.append(FIREDOG_EDITOR_VERSION);
	windowTitle.append(" ");
	windowTitle.append(" ");
	if (isUpdated) {
		windowTitle.append("*");
	}

	if (!this->openFilePath.isEmpty()) {
		windowTitle.append("[");
        QFile file(openFilePath);
        QFileInfo fileInfo(file);
        QString fileName = fileInfo.fileName();
        windowTitle.append(fileName);
		windowTitle.append("]");
    }
    else {
		windowTitle.append("[");
		windowTitle.append("Temporary file");
		windowTitle.append("]");
    }

    setWindowTitle(windowTitle);

}

void FireDogEditor::slots_saveFile() {
	if (this->featureLibrary == NULL || this->openFilePath.isEmpty()) {
		QssMessageBox::warn("Please open the file first.", this, "Warn");
        return;
    }
	//��ʼ����
	QFile file(this->openFilePath);

	int state = -1;
	std::string json = this->featureLibrary->toJson(&state);

	if (state != NO_ERROR) {
		QssMessageBox::warn("Save fail,Feature library convert to json fail!.", this, "Warn");
		return;
	}

	QString jsonstr = QString::fromUtf8(QByteArray::fromRawData(json.c_str(), json.size()));

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QssMessageBox::warn("File Open fail!.", this, "Warn");
		return;
	}
	QTextStream in(&file);
	in << jsonstr;
	file.flush();
	file.close();
    updateWindowTitle(false);
}

void FireDogEditor::slots_saveToFile() {
	if (this->featureLibrary == NULL || this->featureLibrary->items->size()==0) {
		QssMessageBox::warn("Please add the feature library first.", this, "Warn");
        return;
    }
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save To"),
		"",
		tr("Feature library Files (*.fdog)"));

	if (!fileName.isNull())
	{
        if (!fileName.toLower().endsWith(".fdog")) {
            fileName.append(".fdog");
        }
		//��ʼ����
        QFile file(fileName);
        if (file.exists()) {
            QMessageBox::StandardButton button = QssMessageBox::question(this, "Question", "Whether to overwrite this file?");
            if (button != QMessageBox::Ok && button != QMessageBox::Yes) {
                return;
            }
        }

        int state = -1;
        std::string json = this->featureLibrary->toJson(&state);

        if (state != NO_ERROR) {
			QssMessageBox::warn("Save fail,Feature library convert to json fail!.", this, "Warn");
			return;
        }

        QString jsonstr = QString::fromUtf8(QByteArray::fromRawData(json.c_str(), json.size()));

		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QssMessageBox::warn("File Open fail!.", this, "Warn");
			return;
		}
		QTextStream in(&file);
        in << jsonstr;
        file.flush();
		file.close();

		this->openFilePath = fileName;
		updateWindowTitle(false);
	}
}

//���ļ�
void FireDogEditor::slots_openFile() {
    //����Ѿ��д򿪻������ڱ༭��������
	if (this->featureLibrary != NULL) {
		QMessageBox::StandardButton botton = QMessageBox::question(this, "Question", "A file has been opened, do you want to continue?");
		if (botton != QMessageBox::Ok && botton != QMessageBox::Yes) {
			return;
		}
        delete this->featureLibrary;
        this->featureLibrary = NULL;
    }
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Feature Library File."),
        "F:",
        tr("Feature Library(*.fdog)"));
    if (!filePath.isEmpty()) {
        qDebug(filePath.toStdString().c_str());
        this->parseThread->setFilePath(filePath);
        this->parseThread->start();
        openFilePath = filePath;
    }
}

//����
void FireDogEditor::slots_featureLibraryTableSearch() {
    QString search = ui.lineEditSearchName->text();
    loadFeatureLibraryTable(search);

    //fireDogFeatureInfoView->exec();

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

    //���水ť�ͱ��浽��ť����
    ui.actionSave->setEnabled(true);
    ui.actionSaveTo->setEnabled(true);
    updateWindowTitle(false);
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

    if (this->isFeatureAdd) {
        this->featureLibraryTableMenuAddAction->setEnabled(false);
    }
	else {
		this->featureLibraryTableMenuAddAction->setEnabled(true);
    }
    
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

void FireDogEditor::slots_matchFilesTableOpenMenu(QPoint pos) {
    QModelIndexList selectList = ui.tableViewFiles->selectionModel()->selectedRows();
    if (selectList.isEmpty()) {
        matchingFilesTableDelAction->setEnabled(false);
    }
	else {
        matchingFilesTableDelAction->setEnabled(true);
    }

	auto index = ui.tableViewFiles->indexAt(pos);
    matchingFilesTableMenu->exec(QCursor::pos()); // �˵����ֵ�λ��Ϊ��ǰ����λ��
}

void FireDogEditor::slots_featureInfoRuleTreeOpenMenu(QPoint pos) {
    QModelIndex currIndex = ui.treeViewLibraryInfoRules->currentIndex();;
	int curRow = currIndex.row(); //ѡ����

    bool isKey = false;
    if (curRow != -1) {
        QStandardItem* item = this->featureLibraryInfoRuleTreeModel->itemFromIndex(currIndex);
        QString text = item->text();
        if (text != "$and" && text != "$or") {
            isKey = true;
        }
    }

    if (isKey || (this->featureLibraryInfoRuleTreeModel->rowCount() != 0 && curRow == -1)) {
        this->featureLibraryInfoRuleTreeMenuAddAction->setEnabled(false);
    }
    else {
        this->featureLibraryInfoRuleTreeMenuAddAction->setEnabled(true);
    }
    
  
	if (curRow == -1) {
        featureLibraryInfoRuleTreeMenuEditAction->setEnabled(false);
        featureLibraryInfoRuleTreeMenuDelAction->setEnabled(false);
	}
	else {
		featureLibraryInfoRuleTreeMenuEditAction->setEnabled(true);
		featureLibraryInfoRuleTreeMenuDelAction->setEnabled(true);
	}

	auto index = ui.treeViewLibraryInfoRules->indexAt(pos);
    featureLibraryInfoRuleTreeMenu->exec(QCursor::pos()); // �˵����ֵ�λ��Ϊ��ǰ����λ��
}

void FireDogEditor::slots_selectFeatureTableEvent(const QModelIndex& current, const QModelIndex& previous) {

    clearInfoContent(false);

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
    QMessageBox::StandardButton botton = QMessageBox::question(this, "Question", "Please confirm to delete?");
    if (botton != QMessageBox::Ok && botton != QMessageBox::Yes) {
        return;
    }
    
    clearInfoContent(true);
    int curRow = ui.tableViewLibrary->selectionModel()->currentIndex().row(); //ѡ����

	FeatureLibraryItem* item = this->featureLibrary->items->at(curRow);
	delete item;
	item = NULL;

	this->featureLibrary->items->erase(this->featureLibrary->items->begin() + curRow);

    this->featureLibraryTableModel->removeRow(curRow);
	//�������
	updateWindowTitle(true);

}

void FireDogEditor::slots_featureInfoTableMenuAddEvent() {
    firedog::Feature feature;

    QStringList existsKeys;

    int rowCount = this->featureLibraryInfoFeatureTableModel->rowCount();
    for (int i = 0; i < rowCount; i++) {
        BigDataTableRow row = this->featureLibraryInfoFeatureTableModel->getRowData(i);
        QString key = row.contents[0].content.toString();
        existsKeys << key;
    }

    bool state = this->fireDogFeatureInfoView->updateFeature(existsKeys,&feature);
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
    int curRow = ui.tableViewLibraryInfoFeatures->selectionModel()->currentIndex().row(); //ѡ����
    BigDataTableRow row = this->featureLibraryInfoFeatureTableModel->getRowData(curRow);
    QString key = row.contents[0].content.toString();
    QString type = row.contents[1].content.toString();
    QString content = row.contents[2].content.toString();

    firedog::Feature feature;
    feature.key = key.toStdString();
    if (type == "hex") {
        feature.hex = content.toStdString();
    }
    else {
        feature.text = content.toStdString();
    }

    QStringList existsKeys;

	int rowCount = this->featureLibraryInfoFeatureTableModel->rowCount();
	for (int i = 0; i < rowCount; i++) {
        if (i == curRow) {
            continue;
        }
		BigDataTableRow existrow = this->featureLibraryInfoFeatureTableModel->getRowData(i);
		QString key = existrow.contents[0].content.toString();
		existsKeys << key;
	}

    bool isok = this->fireDogFeatureInfoView->updateFeature(existsKeys ,&feature);
    if (isok) {
		QString key = feature.key.c_str();
		QString type = !feature.hex.empty() ? "hex" : "text";
		QString content = !feature.hex.empty() ? feature.hex.c_str() : feature.text.c_str();

		BigDataTableRow newrow;
        newrow.contents << BigDataTableCol(key) << BigDataTableCol(type) << BigDataTableCol(content);

        this->featureLibraryInfoFeatureTableModel->replaceRow(curRow, newrow);
    }
}

void FireDogEditor::slots_featureInfoTableMenuDelEvent() {
	QMessageBox::StandardButton botton = QMessageBox::question(this, "Question", "Please confirm to delete?");
	if (botton != QMessageBox::Ok && botton != QMessageBox::Yes) {
		return;
	}
    int curRow = ui.tableViewLibraryInfoFeatures->selectionModel()->currentIndex().row();
    this->featureLibraryInfoFeatureTableModel->removeRow(curRow);
}

void FireDogEditor::clearInfoContent(bool isFeatureAdd) {
    this->isFeatureAdd = isFeatureAdd;
    if (isFeatureAdd) {
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

void FireDogEditor::slots_featureLibraryInfoRuleMenuAddEvent() {

    bool ruleIsEmpty = this->featureLibraryInfoRuleTreeModel->rowCount() == 0;
    bool isRoot = false;
    if (ruleIsEmpty) {
        isRoot = true;
    }

    QString rule;
    bool isOk = this->fireDogFeatureRuleInfoView->updateRule(&rule, isRoot);
	if (isOk) {
		QStandardItem* ruleitem = new QStandardItem(rule);
        //����ǿյ�
        if (ruleIsEmpty) {
            featureLibraryInfoRuleTreeModel->appendRow(ruleitem);
        }
        else {
			QModelIndex currIndex = ui.treeViewLibraryInfoRules->currentIndex();;
            QStandardItem* selectItem = this->featureLibraryInfoRuleTreeModel->itemFromIndex(currIndex);
            selectItem->appendRow(ruleitem);
		}
		//���ѡ��
        ui.treeViewLibraryInfoRules->selectionModel()->clear();

		QModelIndex index = this->featureLibraryInfoRuleTreeModel->indexFromItem(ruleitem);
        ui.treeViewLibraryInfoRules->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void FireDogEditor::slots_featureLibraryInfoRuleMenuEditEvent() {
	QModelIndex currIndex = ui.treeViewLibraryInfoRules->currentIndex();;
	QStandardItem* selectItem = this->featureLibraryInfoRuleTreeModel->itemFromIndex(currIndex);
    
    bool isRoot = selectItem->parent() == NULL;

    QString rule = selectItem->text();
    bool isOk = this->fireDogFeatureRuleInfoView->updateRule(&rule, isRoot);
    if (isOk) {
        selectItem->setText(rule);
    }
}

void FireDogEditor::slots_featureLibraryInfoRuleMenuDelEvent() {
	QMessageBox::StandardButton botton = QMessageBox::question(this, "Question", "Please confirm to delete?");
	if (botton != QMessageBox::Ok && botton != QMessageBox::Yes) {
		return;
	}

	QModelIndex currIndex = ui.treeViewLibraryInfoRules->currentIndex();;
    this->featureLibraryInfoRuleTreeModel->removeRow(currIndex.row(),currIndex.parent());
}
void FireDogEditor::slots_saveBtnClickEvent() {

    this->loadingDialog->show();

    if (true) {
        return;
    }

	firedog::FeatureLibraryItem* item = getInfoViewFeatureItem();
    if (item==NULL) {
        return;
    }
    if (this->featureLibrary==NULL) {
        this->featureLibrary = new FeatureLibrary();
    }
	this->featureLibrary->items->push_back(item);

	//����
	QString name = item->name.c_str();
	QString describe = item->describe.c_str();
	QString author = item->author.c_str();

	BigDataTableRow row;
	row.contents << BigDataTableCol(name) << BigDataTableCol(describe) << BigDataTableCol(author);

    //���������
    if (this->isFeatureAdd) {
        if (this->featureLibrary == NULL) {
            this->featureLibrary = new firedog::FeatureLibrary();
		}
        this->featureLibraryTableModel->addRow(row);
    }
    //����Ǹ���
	else {
		int rowNum = ui.tableViewLibrary->currentIndex().row();
        //�滻���ɵ�����
        firedog::FeatureLibraryItem* olditem = this->featureLibrary->items->at(rowNum);
        //�ȸ�ֵ
        this->featureLibrary->items->at(rowNum) = item;
        //��ɾ�ڴ�
        delete olditem;
        olditem = NULL;

        this->featureLibraryTableModel->replaceRow(rowNum, row);
    }

    //���浽��ť������
	ui.actionSaveTo->setEnabled(true);
    //���ұ������
    updateWindowTitle(true);
}

firedog::FeatureLibraryItem* FireDogEditor::getInfoViewFeatureItem() {

	QString name = ui.lineEditLibraryInfoName->text();
	QString auth = ui.lineEditLibraryInfoAuthor->text();
	QString describe = ui.textEditLibraryInfoDescribe->toPlainText();

	int featureCount = this->featureLibraryInfoFeatureTableModel->rowCount();
	int ruleCount = this->featureLibraryInfoRuleTreeModel->rowCount();

	if (name.isEmpty()) {
		QssMessageBox::warn("Please enter [Name].", this, "Warn");
		return NULL;
	}

	if (auth.isEmpty()) {
		QssMessageBox::warn("Please enter [Auth].", this, "Warn");
		return NULL;
	}

	if (describe.isEmpty()) {
		QssMessageBox::warn("Please enter [Describe].", this, "Warn");
		return NULL;
	}

	if (featureCount == 0) {
		QssMessageBox::warn("Please add [Feature].", this, "Warn");
		return NULL;
	}

	if (ruleCount == 0) {
		QssMessageBox::warn("Please add [Rule].", this, "Warn");
		return NULL;
	}

    //У�����
    QStandardItem* rootItem = featureLibraryInfoRuleTreeModel->item(0);
    mountcloud::Rule* rule = itemToRule(rootItem);
    if (rule == NULL) {
        return NULL;
    }

    firedog::FeatureLibraryItem* result = new firedog::FeatureLibraryItem();
    result->name = name.toStdString();
    result->describe = describe.toStdString();
    result->author = auth.toStdString();

    //����
    for (int i = 0; i < featureCount; i++) {
        BigDataTableRow row = this->featureLibraryInfoFeatureTableModel->getRowData(i);
		QString key = row.contents[0].content.toString();
		QString type = row.contents[1].content.toString();
		QString content = row.contents[2].content.toString();

		Feature* feature = new Feature();
		feature->key = key.toStdString();
        if (type=="hex") {
            feature->hex = content.toStdString();
        }
		else {
			feature->text = content.toStdString();
        }

        result->features->push_back(feature);
    }

    //����
    result->rule = rule;

    return result;
}

mountcloud::Rule* FireDogEditor::itemToRule(QStandardItem* item) {
    if (item == NULL) {
        return NULL;
    }
    QString errorMessage;
    QString text = item->text();
    if (text.isEmpty()) {
        errorMessage = "The rule is wrong, please delete and add again.";
        goto fail;
    }

    bool isOperation = text == "$and" || text == "$or";

    //������ֻ�����߼����ʽ
    if (item->parent()==NULL) {
        if (!isOperation) {
			errorMessage = "The root rule can only be $and or $or.";
			goto fail;
        }
    }

    //������������нڵ�
    if (isOperation) {
        if (item->rowCount() == 0) {
			errorMessage = "The rule childs is empty.";
			goto fail;
        }
    }

    mountcloud::Rule* result = new mountcloud::Rule();

    if (isOperation) {
		int childCount = item->rowCount();
		for (int i = 0; i < childCount; i++) {
            QStandardItem* childItem = item->child(i);
            mountcloud::Rule*  childRule = itemToRule(childItem);
            if (childRule==NULL) {
                delete result;
                return NULL;
            }
            if(text=="$and"){
                if (result->ands == NULL) {
                    result->ands = new std::vector<mountcloud::Rule*>();
                }
                result->ands->push_back(childRule);
            }
            else {
				if (result->ors == NULL) {
					result->ors = new std::vector<mountcloud::Rule*>();
				}
				result->ors->push_back(childRule);
            }
		}
	}
    else {
        result->id = text.toStdString();
    }

    return result;

fail:
	ui.treeViewLibraryInfoRules->selectionModel()->clear();
	QModelIndex index = this->featureLibraryInfoRuleTreeModel->indexFromItem(item);
	ui.treeViewLibraryInfoRules->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    QssMessageBox::warn(errorMessage, this, "Warn");
	return NULL;
}


void FireDogEditor::slots_matchingAddFilesBtnClickEvent() {
    QStringList files = QFileDialog::getOpenFileNames(this,"Select Files","","");

    QStringList nowFiles;
    int count = this->matchingFilesTableModel->rowCount();
    if (count > 0) {
        for (int i = 0;i < this->matchingFilesTableModel->rowCount(); i++) {
            BigDataTableRow row = this->matchingFilesTableModel->getRowData(i);
            nowFiles << row.contents.at(0).content.toString();
        }
    }

    if (files.size() > 0) {
        for (int i = 0; i < files.size(); i++) {
            QString filePath = files.at(i);

            if (nowFiles.contains(filePath)) {
                continue;
            }

            QFile file(filePath);
            QString sizeStr = "UNKNOWN";
            if (file.exists()) {
                qint64 size = file.size();
                sizeStr = getFileSizeString(size);
            }
            BigDataTableRow row;
            row.contents << BigDataTableCol(filePath) << BigDataTableCol(sizeStr);

			this->matchingFilesTableModel->addRow(row);
        }
    }
}

QString FireDogEditor::getFileSizeString(const qint64& size) {
	int integer = 0;  //����λ
	int decimal = 0;  //С��λ��������λ
	char unit = 'B';
	qint64 standardSize = size;
	qint64 curSize = size;

	if (standardSize > 1024) {
		curSize = standardSize * 1000;
		curSize /= 1024;
		integer = curSize / 1000;
		decimal = curSize % 1000;
		standardSize /= 1024;
		unit = 'K';
		if (standardSize > 1024) {
			curSize = standardSize * 1000;
			curSize /= 1024;
			integer = curSize / 1000;
			decimal = curSize % 1000;
			standardSize /= 1024;
			unit = 'M';
			if (standardSize > 1024) {
				curSize = standardSize * 1000;
				curSize /= 1024;
				integer = curSize / 1000;
				decimal = curSize % 1000;
				unit = 'G';
			}
		}
    }
    else {
        integer = size;
    }

	QString dec = "0";
	if (0 <= decimal && decimal <= 9) {
		dec = dec + dec + QString::number(decimal);
	}

	if (10 <= decimal && decimal <= 99) {
		dec = "0" + QString::number(decimal);
	}

	if (100 <= decimal && decimal <= 999) {
		dec = QString::number(decimal);
	}

	return QString::number(integer) + "." + dec + unit;
}

void FireDogEditor::slots_matchingFilesDeleteEvent() {
	QModelIndexList selectList = ui.tableViewFiles->selectionModel()->selectedRows();
	for (int i = 0; i < selectList.size(); i++) {
		QModelIndex index = selectList.at(i);
		int row = index.row();
		matchingFilesTableModel->removeRow(row);
	}
}

void FireDogEditor::slots_matchingBtnClick() {
    if (this->featureLibrary == NULL || this->featureLibrary->items->size() == 0) {
		QssMessageBox::warn("Please add feature library first.", this, "Warn");
		return;
    }
    //std::vector<Part> parts = PartUtil::getParts(101, 10);
    QString text = ui.plainTextText->toPlainText();
    QString hex = ui.plainTextHex->toPlainText();

    text = text.trimmed();
    hex = hex.trimmed();

	QStringList files;
	int count = this->matchingFilesTableModel->rowCount();
	if (count > 0) {
		for (int i = 0; i < this->matchingFilesTableModel->rowCount(); i++) {
			BigDataTableRow row = this->matchingFilesTableModel->getRowData(i);
            files << row.contents.at(0).content.toString();
		}
	}

    if (text.isEmpty() && hex.isEmpty() && files.isEmpty()) {
        QssMessageBox::warn("Please enter text or hex or add a file.", this, "Warn");
        return;
    }

	this->loadingDialog->show();

    int threadNum = ui.spinBoxThreadNum->value();
    int ge100MSlice = ui.spinBox100MB->value();
    int ge1GSlice = ui.spinBox1GB->value();

    ui.pushButtonMatch->setEnabled(false);
    if (matchButlerThread != NULL) {
        disconnect(matchButlerThread, &MatchButlerThread::matchEnd, this, &FireDogEditor::matchEnd);
        delete matchButlerThread;
        matchButlerThread = NULL;
    }
    matchButlerThread = new MatchButlerThread(threadNum, this->featureLibrary);;
    connect(matchButlerThread, &MatchButlerThread::matchEnd, this, &FireDogEditor::matchEnd);

    //���text���ǿ�
    if (!text.isEmpty()) {
        MatchWork work;
        work.workType = WORK_TYPE_TEXT;
        work.content = text;
        matchButlerThread->addWork(work);
    }

    //���hex���ǿ�
    if (!hex.isEmpty()) {
		MatchWork work;
		work.workType = WORK_TYPE_HEX;
		work.content = hex;
		matchButlerThread->addWork(work);
    }

	//���files���ǿ�
    if (!files.isEmpty()) {

        qint64 fs100M = 1024 * 1024 * 100;
        qint64 fs1G = 1024 * 1024 * 1024;

        for (int i = 0; i < files.size(); i++) {
            QString filePath = files.at(i);
            QFile file(filePath);
            if (!file.exists()) {
                continue;
            }

            qint64 fileSize = file.size();
            if (fileSize < fs100M) {
                MatchWork work;
                work.workType = WORK_TYPE_FILE;
                work.content = filePath;
                matchButlerThread->addWork(work);
            }
            else{
                int partSize = 0;
                if (fileSize < fs1G) {
                    partSize = ge100MSlice;
                }
                else {
                    partSize = ge1GSlice;
                }
				std::vector<Part> parts = PartUtil::getParts(fileSize, partSize);
                for (int y = 0; y < parts.size(); y++) {
                    Part part = parts.at(y);
                    if (y != 0) {
                        //��ǰ�Ƶ�
                        part.start = part.start - 256;
                    }
                    MatchWork work;
                    work.workType = WORK_TYPE_PART;
                    work.content = filePath;
                    work.part = part;
                    matchButlerThread->addWork(work);
                }
            }
        }
    }

    //��ʼ����
    matchButlerThread->start();
}

void FireDogEditor::matchEnd(int worksize, int success, int error, int state) {
    this->loadingDialog->hide();
    ui.pushButtonMatch->setEnabled(true);
    QVector<BigDataTableRow> rows;
    if (state == 0) {
        QVector<HitFeature> hits = this->matchButlerThread->getHits();
        if (hits.size()>0) {
            for (int i = 0; i < hits.size(); i++) {
                HitFeature hit = hits.at(i);
                BigDataTableRow row;
                QString source;
                if (hit.workType == WORK_TYPE_TEXT) {
                    source = "text";
                }
                else if (hit.workType == WORK_TYPE_HEX) {
                    source = "hex";
                }
                else {
                    source = hit.content;
                }
                QString featureName = hit.name;
                QString featureDescribe = hit.describe;
                QString featureAuthor = hit.author;

                row.contents << BigDataTableCol(source) << BigDataTableCol(featureName) << BigDataTableCol(featureDescribe) << BigDataTableCol(featureAuthor);
                rows << row;
            }
        }
    }
    this->matchingResultTableModel->handleData(rows);
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

	if (matchingFilesTableModel != NULL) {
		delete matchingFilesTableModel;
        matchingFilesTableModel = NULL;
	}

	if (matchingFilesTableMenu != NULL) {
		delete matchingFilesTableMenu;
        matchingFilesTableMenu = NULL;
	}

	if (matchingFilesTableDelAction != NULL) {
		delete matchingFilesTableDelAction;
        matchingFilesTableDelAction = NULL;
	}

	if (matchingResultTableModel != NULL) {
		delete matchingResultTableModel;
        matchingResultTableModel = NULL;
	}
}

void FireDogEditor::resizeEvent(QResizeEvent* event) {
    int width = this->width();
    int height = this->height();
    this->loadingDialog->setFixedSize(QSize(width, height));
}

