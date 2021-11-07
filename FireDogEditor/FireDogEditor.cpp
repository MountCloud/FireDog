#include "FireDogEditor.h"
#include <QLabel>

#include <QFileDialog>
#include <QMessageBox>

#include "config.h"
#include "featurelibrary.h"

using namespace firedog;

//���캯��
FireDogEditor::FireDogEditor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    this->loadingDialog = new LoadingDialog(this);

    //���ñ���
    QString windowTitle = "FireDog Editor ";
    windowTitle.append(FIREDOG_EDITOR_VERSION);
    setWindowTitle(windowTitle);

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

    this->parseThread = new ParseThread();

    //��������-start==================================================================
    this->featureLibraryTableModel = new BigDataTableModel();

    QStringList featureLibraryTableModelTitle;
    featureLibraryTableModelTitle << "Name" << "Describe" << "Author";
    featureLibraryTableModel->setHeaders(featureLibraryTableModelTitle);

    ui.tableViewLibrary->setModel(this->featureLibraryTableModel);

    //���ɱ༭
    ui.tableViewLibrary->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableViewLibrary->setSelectionBehavior(QAbstractItemView::SelectRows);
    //һ�ι���һ������
    ui.tableViewLibrary->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui.tableViewLibrary->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //�ɵ����Ҽ��˵�
    ui.tableViewLibrary->setContextMenuPolicy(Qt::CustomContextMenu); 

    //�˵�
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

    //��������-end==================================================================

    //����-�������-start==================================================================
    this->featureLibraryInfoFeatureTableModel = new BigDataTableModel();

    QStringList featureLibraryInfoFeatureTableModelTitle;
    featureLibraryInfoFeatureTableModelTitle << "Key" << "Type" << "Content";
    featureLibraryInfoFeatureTableModel->setHeaders(featureLibraryInfoFeatureTableModelTitle);

    ui.tableViewLibraryInfoFeatures->setModel(this->featureLibraryInfoFeatureTableModel);
    //����-�������-end==================================================================


    //���¼�
    connect(ui.actionOpen, &QAction::triggered, this, &FireDogEditor::slots_openFile);
    connect(ui.pushButtonSearchName, &QPushButton::clicked, this, &FireDogEditor::slots_featureLibraryTableSearch);
    connect(ui.tableViewLibrary, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_featureTableOpenMenu(QPoint)));
    //�����߳�
    connect(this->parseThread, SIGNAL(parseBegin()), this, SLOT(slots_parseBinBegin()));
    connect(this->parseThread, SIGNAL(parseEnd(firedog::FeatureLibrary*,int)), this, SLOT(slots_parseBinEnd(firedog::FeatureLibrary*, int)));

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
    
    //���֮ǰ������
    if (this->featureLibrary!=NULL) {
        delete this->featureLibrary;
        this->featureLibrary = NULL;
        this->featureLibraryTableModel->removeRows(0, this->featureLibraryTableModel->rowCount());
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
        row.id = QString::number(i);

        tableRows.push_back(row);
    }

    this->featureLibraryTableModel->handleData(tableRows);
}

void FireDogEditor::slots_featureTableOpenMenu(QPoint pos) {
    auto index = ui.tableViewLibrary->indexAt(pos);
    featureLibraryTableMenu->exec(QCursor::pos()); // �˵����ֵ�λ��Ϊ��ǰ����λ��
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
}

void FireDogEditor::resizeEvent(QResizeEvent* event) {
    int width = this->width();
    int height = this->height();
    this->loadingDialog->setFixedSize(QSize(width, height));
}

