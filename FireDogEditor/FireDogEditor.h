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
    //���ļ�
    void slots_openFile();
    //�������Ҽ��˵�
    void slots_featureTableOpenMenu(QPoint pos);

private:
    Ui::FireDogEditorClass ui;

    //������ı��model
    QStandardItemModel* featureLibraryTableModel = NULL;
    QMenu* featureLibraryTableMenu = NULL;
    QAction* featureLibraryTableMenuAddAction = NULL;
    QAction* featureLibraryTableMenuEditAction = NULL;
    QAction* featureLibraryTableMenuDelAction = NULL;

    //��ʼ��
    void init();
};
