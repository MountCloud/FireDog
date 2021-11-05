#include "FireDogEditor.h"
#include <QLabel>

#include <QFileDialog>
#include "config.h"

//���캯��
FireDogEditor::FireDogEditor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

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

    //��������-start==================================================================
    this->featureLibraryTableModel = new QStandardItemModel();
    ui.tableViewLibrary->setModel(this->featureLibraryTableModel);

    this->featureLibraryTableModel->setHorizontalHeaderItem(0, new QStandardItem("Name"));
    this->featureLibraryTableModel->setHorizontalHeaderItem(1, new QStandardItem("Describe"));
    this->featureLibraryTableModel->setHorizontalHeaderItem(2, new QStandardItem("Author"));

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


    //���¼�
    connect(ui.actionOpen, &QAction::triggered, this, &FireDogEditor::slots_openFile);
    
    connect(ui.tableViewLibrary, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_featureTableOpenMenu(QPoint)));


}

//���ļ�
void FireDogEditor::slots_openFile() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Feature Library File."),
        "F:",
        tr("Feature Library(*.fdog)"));

    qDebug(fileName.toStdString().c_str());
}

void FireDogEditor::slots_featureTableOpenMenu(QPoint pos) {
    auto index = ui.tableViewLibrary->indexAt(pos);
    featureLibraryTableMenu->exec(QCursor::pos()); // �˵����ֵ�λ��Ϊ��ǰ����λ��
    //if (index.isValid())
    //{
    //    featureLibraryTableMenu->exec(QCursor::pos()); // �˵����ֵ�λ��Ϊ��ǰ����λ��
    //}
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
}