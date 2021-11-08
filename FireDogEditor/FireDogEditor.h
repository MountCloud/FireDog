#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FireDogEditor.h"

#include <QStandardItemModel>
#include <QMenu>

#include "FireDogFeatureInfo.h"

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
    //���ļ�
    void slots_openFile();
    //�������Ҽ��˵�
    void slots_featureTableOpenMenu(QPoint pos);
	//�����������Ҽ��˵�
	void slots_featureInfoTableOpenMenu(QPoint pos);

    //��ʼ�����ļ�
    void slots_parseBinBegin();
    //�����ļ�����
    void slots_parseBinEnd(firedog::FeatureLibrary* featureLibrary, int state);
    //������������
    void slots_featureLibraryTableSearch();
    //���������
    void slots_selectFeatureTableEvent(const QModelIndex& current, const QModelIndex& previous);
    
    //���������Ӳ˵����
    void slots_featureTableMenuAddEvent();
	//�������ɾ���˵����
	void slots_featureTableMenuDelEvent();

    //������������Ӳ˵����
    void slots_featureInfoTableMenuAddEvent();
	//����������༭�˵����
	void slots_featureInfoTableMenuEditEvent();
	//����������ɾ���˵����
	void slots_featureInfoTableMenuDelEvent();

private:
    Ui::FireDogEditorClass ui;

    FireDogFeatureInfo* fireDogFeatureInfo;

    //��ʼ��
    void init();

    //�Ƿ�����ӣ�true��add��false��update
    bool isAdd = true;

    //������
    firedog::FeatureLibrary* featureLibrary = NULL;

    //������ı��model
    BigDataTableModel* featureLibraryTableModel = NULL;
    QMenu* featureLibraryTableMenu = NULL;
    QAction* featureLibraryTableMenuAddAction = NULL;
    QAction* featureLibraryTableMenuDelAction = NULL;

    //�����������������model
    BigDataTableModel* featureLibraryInfoFeatureTableModel = NULL;
	QMenu* featureLibraryInfoFeatureTableMenu = NULL;
	QAction* featureLibraryInfoFeatureTableMenuAddAction = NULL;
    QAction* featureLibraryInfoFeatureTableMenuEditAction = NULL;
	QAction* featureLibraryInfoFeatureTableMenuDelAction = NULL;

    QStandardItemModel* featureLibraryInfoRuleTreeModel = NULL;

    //loading dialog
    LoadingDialog* loadingDialog = NULL;

    //�����߳�
    ParseThread* parseThread = NULL;

    //�ֱ��ʱ���¼�
    void resizeEvent(QResizeEvent* event) override;

    //�����������б�
    void loadFeatureLibraryTable(QString search);

    //�����������
    void clearInfoContent(bool isAdd);

    //����ת��ͼitem
    QStandardItem* ruleToItem(mountcloud::Rule* rule);

};
