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
    //���ļ�
    void slots_openFile();
    //�������Ҽ��˵�
    void slots_featureTableOpenMenu(QPoint pos);

    //��ʼ�����ļ�
    void slots_parseBinBegin();
    //�����ļ�����
    void slots_parseBinEnd(firedog::FeatureLibrary* featureLibrary, int state);
    //������������
    void slots_featureLibraryTableSearch();
private:
    Ui::FireDogEditorClass ui;

    //������
    firedog::FeatureLibrary* featureLibrary = NULL;

    //������ı��model
    BigDataTableModel* featureLibraryTableModel = NULL;
    QMenu* featureLibraryTableMenu = NULL;
    QAction* featureLibraryTableMenuAddAction = NULL;
    QAction* featureLibraryTableMenuEditAction = NULL;
    QAction* featureLibraryTableMenuDelAction = NULL;


    //�����������������model
    BigDataTableModel* featureLibraryInfoFeatureTableModel = NULL;

    //loading dialog
    LoadingDialog* loadingDialog = NULL;

    //�����߳�
    ParseThread* parseThread = NULL;

    //��ʼ��
    void init();

    void resizeEvent(QResizeEvent* event) override;

    //�����������б�
    void loadFeatureLibraryTable(QString search);
};
