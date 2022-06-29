#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FireDogEditor.h"

#include <QStandardItemModel>
#include <QMenu>

#include "FireDogFeatureInfo.h"
#include "FireDogFeatureRuleInfo.h"
#include "FireDogSwitchLanguage.h"
#include "FireDogAbout.h"

#include "loadingdialog.h"
#include "parsethread.h"
#include "bigdatatablemodel.h"

#include "Qss/Qss.h"

#define FIREDOG_EDITOR_VERSION "v2.1"
#define EXIT_CODE_REBOOT -123456789

//ƥ��ܼ�
class MatchButlerThread;
//ƥ�䵽������
class HitFeature;

class FireDogEditor : public QssMainWindow
{
    Q_OBJECT

public:
    FireDogEditor(QWidget *parent = Q_NULLPTR);
    ~FireDogEditor();

private slots:
    //���ļ�
    void slots_openFile();
    //�����ļ�
    void slots_saveFile();
    //�����ļ���
    void slots_saveToFile();
    //��������
    void slots_openIssue();
    //��git
    void slots_openGit();
	//��git
	void slots_openFireKylin();
    //�򿪹�����
    void slots_about();
    //��������
    void slots_settingLanguage();

    //�������Ҽ��˵�
    void slots_featureTableOpenMenu(QPoint pos);
	//�����������Ҽ��˵�
	void slots_featureInfoTableOpenMenu(QPoint pos);
	//����������������Ҽ��˵�
	void slots_featureInfoRuleTreeOpenMenu(QPoint pos);
	//ƥ���ļ��б��Ҽ��˵�
	void slots_matchFilesTableOpenMenu(QPoint pos);

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

	//���������������Ӳ˵����
	void slots_featureLibraryInfoRuleMenuAddEvent();
	//��������������༭�˵����
	void slots_featureLibraryInfoRuleMenuEditEvent();
	//�������������ɾ���˵����
	void slots_featureLibraryInfoRuleMenuDelEvent();

    //��ӻ��߸��°�ť
    void slots_saveBtnClickEvent();

    //ƥ��ҳ������ļ���ť
    void slots_matchingAddFilesBtnClickEvent();
	//ƥ���ļ��б�ɾ���˵����
	void slots_matchingFilesDeleteEvent();
    //ƥ�䰴ť����
    void slots_matchingBtnClick();

    void slots_searchMatchResultBtnClick();

    //ƥ�����
    void slots_matchEnd(int worksize, int success, int error, int state);

    //����
    void slots_reboot();
private:
    Ui::FireDogEditorClass ui;

    FireDogFeatureInfo* fireDogFeatureInfoView = NULL;

    FireDogFeatureRuleInfo* fireDogFeatureRuleInfoView = NULL;

    Ui::FireDogSwitchLanguageUi* fireDogSwitchLanguageUi = NULL;

    FireDogAbout* fireDogAbout = NULL;

	//loading dialog
	LoadingDialog* loadingDialog = NULL;

	//�����߳�
	ParseThread* parseThread = NULL;

    //���ļ���·��
    QString openFilePath;

    //�Ƿ�����ӣ�true��add��false��update
    bool isFeatureAdd = true;

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

    //������
    QStandardItemModel* featureLibraryInfoRuleTreeModel = NULL;
	QMenu* featureLibraryInfoRuleTreeMenu = NULL;
	QAction* featureLibraryInfoRuleTreeMenuAddAction = NULL;
	QAction* featureLibraryInfoRuleTreeMenuEditAction = NULL;
	QAction* featureLibraryInfoRuleTreeMenuDelAction = NULL;
    
    //�ļ����
    BigDataTableModel* matchingFilesTableModel = NULL;
    QMenu* matchingFilesTableMenu = NULL;
	QAction* matchingFilesTableDelAction = NULL;

	//ƥ�������
	BigDataTableModel* matchingResultTableModel = NULL;
    //match thread
    MatchButlerThread* matchButlerThread = NULL;
    //ƥ�䵽�Ľ��
    QVector<HitFeature> matchResultHits;

	//��ʼ��
	void init();
    //���´��ڱ���
    void updateWindowTitle(bool isUpdated);
    //�ֱ��ʱ���¼�
    void resizeEvent(QResizeEvent* event) override;
    //�����������б�
    void loadFeatureLibraryTable(QString search);
    //�����������
    void clearInfoContent(bool isAdd);
    //����ת��ͼitem
    QStandardItem* ruleToItem(mountcloud::Rule* rule);
    //��������ͼ�л�ȡ����Item
    firedog::FeatureLibraryItem* getInfoViewFeatureItem();
	//��ͼת����
	mountcloud::Rule* itemToRule(QStandardItem* item);
	//����ƥ�䵽�Ľ��
	void loadMatchResult(QString search);
    QString getFileSizeString(const qint64& size);

};
