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
    //获取行数据
    BigDataTableRow getRowData(int nRow);
    //清空数据
    void clear();
public slots:
    //处理数据
    void handleData(const QVector<BigDataTableRow>& vecData);
signals:
    void signalRecvhandleData();
private:
    //表头
    QStringList horHeaderData;
    //加载数据
    QVector<BigDataTableRow> m_vecData;
};

#endif