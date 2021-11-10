#ifndef __FIREDOG_GUI_BIGDATATABLEMODEL_H__
#define __FIREDOG_GUI_BIGDATATABLEMODEL_H__

#include <QAbstractTableModel>
#include "bigdatatableitem.h"

class BigDataTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    BigDataTableModel(QObject* parent = nullptr);
    ~BigDataTableModel();
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    void setHeaders(const QList<QString>& headers);
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    //��ȡ������
    BigDataTableRow getRowData(int nRow);
    //�������
    void clear();
    //ɾ��һ��
    bool removeRow(int row, const QModelIndex& parent = QModelIndex());
    //ɾ������
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
    //׷��һ��
    void addRow(BigDataTableRow row);
    //����һ��
    void insertRow(int index,BigDataTableRow row);
    //�滻һ��
    void replaceRow(int index, BigDataTableRow row);
public slots:
    //��������
    void handleData(const QVector<BigDataTableRow>& vecData);
signals:
    void signalRecvhandleData();
private:
    //��ͷ
    QStringList horHeaderData;
    //��������
    QVector<BigDataTableRow> m_vecData;
};

#endif